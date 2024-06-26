#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <multiboot.h>
#include <pgalloc.h>

/* Free block linked list */
pgalloc_free_block_t *pgalloc_free_head = NULL;
pgalloc_free_block_t *pgalloc_free_tail = NULL;

/* Pool of free block list entries */
pgalloc_free_block_t pgalloc_free_block_pool[PGALLOC_FREE_BLOCK_POOL_SIZE];

/* List of allocations longer than 1 page */
pgalloc_multipage_t pgalloc_multipage_list[PGALLOC_MULTIPAGE_LIST_SIZE];

/* Allocate pages. Returns NULL if not successful */
void *pgalloc(size_t pages) {
    /* Check argument */
    if (pages < 1) {
        printf("pgalloc: tried to allocate less than 1 page\n");
        return NULL;
    }

    /* Get first free block */
    pgalloc_free_block_t *blk = pgalloc_free_head;

    /* Is this block valid? If not, there's none in the list */
    if (blk == NULL) {
        printf("pgalloc: no free pages!\n");
        return NULL;
    }
    
    /* Check for a block of suitable size*/
    while (blk != NULL && blk->len < pages) {
        blk = blk->next;
    }

    /* Check that the block is valid */
    if (blk == NULL) {
        printf("pgalloc: no blocks large enough to fill request\n");
        return NULL;
    }

    /* Allocate from the end of this block */
    uint32_t block_end = blk->base + (blk->len * 0x1000);
    uint32_t allocation = block_end - (pages * 0x1000);
    blk->len -= pages;

    /* Add to the list of multi-page allocations if necessary */
    if (pages > 1) {
        int r = pgalloc_register_multipage(allocation, pages);
        if (r) {
            /* Undo allocation */
            blk->len += pages;

            printf("pgalloc: unable to register multipage allocation\n");
            return NULL;
        }
    }

    return (void *) allocation;
}

/* Add a block of free memory to the pool. Length in pages. Returns non-zero if not successful */
int pgalloc_add_free_block(uint32_t base, uint32_t len) {
    pgalloc_free_block_t *blk = pgalloc_alloc_free_block();

    /* Check that the list entry was allocated */
    if (blk == NULL) {
        printf("pgalloc_add_free_block: unable to allocate list entry\n");
        return 1;
    }

    /* Initialize list entry */
    blk->base = base;
    blk->len = len;
    blk->next = NULL;

    /* Add to list */
    if (pgalloc_free_head == NULL) {
        pgalloc_free_head = blk;
    } else {
        pgalloc_free_tail->next = blk;
    }
    pgalloc_free_tail = blk;

    return 0;
}

/* Allocate a free block list entry from the pool. Returns NULL if not successful */
pgalloc_free_block_t *pgalloc_alloc_free_block() {
    /* Search for an unallocated block */
    for (int i=0; i<PGALLOC_FREE_BLOCK_POOL_SIZE; i++) {
        pgalloc_free_block_t *blk = pgalloc_free_block_pool + i;

        /* Use this block if it's free */
        if (blk->pool_status == PGALLOC_BLOCK_FREE) {
            blk->pool_status = PGALLOC_BLOCK_INUSE;
            return blk;
        }
    }

    /* No free blocks */
    printf("pgalloc_alloc_free_block: no list entries left in pool\n");
    return NULL;
}

/* Free a free block list entry */
void pgalloc_free_free_block(pgalloc_free_block_t *blk) {
    blk->pool_status = PGALLOC_BLOCK_FREE;
}

/* Initialize the page allocator. Returns non-zero if not successful */
int pgalloc_init(void) {
    /* Get and adjust bounds of kernel. These symbols are from the linker script */
    uint32_t kstart = (uint32_t) &kernel_start;
    uint32_t kend = (uint32_t) &kernel_end;

    kstart &= ~0xfff;

    if ((kend & 0xfff) != 0) {
        kend = (kend & ~0xfff) + 0x1000;
    }
    kend--;

    /* Initialize free block pool */
    for (int i=0; i<PGALLOC_FREE_BLOCK_POOL_SIZE; i++) {
        pgalloc_free_block_pool[i].pool_status = PGALLOC_BLOCK_FREE;
    }

    /* Initialize multipage allocation list */
    for (int i=0; i<PGALLOC_MULTIPAGE_LIST_SIZE; i++) {
        pgalloc_multipage_list[i].base = 0;
        pgalloc_multipage_list[i].pages = 0;
    }

    /* Add free blocks from the bootloader */
    int map_size = multiboot_info->mmap_length / sizeof(multiboot_memory_map_t);
    for (int i=0;i<map_size;i++) {
        /* Is the block available? */
        if (multiboot_info->mmap_addr[i].type != MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }

        /* Does the block start after 32-bit space? Not interested */
        uint64_t addr = multiboot_info->mmap_addr[i].addr;
        if ((addr >> 32) != 0) {
            continue;
        }

        /* Does the block end after 32-bit space? Adjust the length */
        uint64_t len = multiboot_info->mmap_addr[i].len;
        uint64_t block_end = addr + len - 1;
        if ((block_end >> 32) != 0) {
            /* Size of block after the 64-bit boundary */
            uint64_t unwanted = 0x100000000 - (block_end + 1);
            
            /* Subtract the unwanted memory from the block */
            len -= unwanted;
            block_end = addr + len - 1;
        }

        /* Is the block entirely under 2M? */
        if (block_end < 0x200000) {
            continue;
        }

        /* Does the block contain memory under 2M? */
        if (addr < 0x200000) {
            addr = 0x200000;
            len = block_end - addr + 1;
        }

        /* Does the block include the kernel? Adjust the block */
        if (addr <= kstart && block_end > kend) {
            addr = kend + 1;
            len = block_end - addr + 1;
        }

        /* Print the finalized block information */
        uint32_t addr32 = (uint32_t) addr;
        uint32_t len32 = (uint32_t) len;
        printf("pgalloc_init: adding block starting at %x of length %x to the free pool\n", addr32, len32);

        /* Add the block to the free blocks list */
        uint32_t pages = len32 >> 12;
        int r = pgalloc_add_free_block(addr32, pages);
        if (r) {
            printf("pgalloc_init: unable to add free block to pool\n");
            return 1;
        }
    }

    return 0;
}

/* Print current state of the page allocator system */
void pgalloc_print_diagnostics(void) {
    printf(" free blocks: <");

    pgalloc_free_block_t *blk = pgalloc_free_head;
    while (blk != NULL) {
        printf("%x:%d",blk->base,blk->len);
        if (blk->next != NULL) {
            printf(",");
        }

        blk = blk->next;
    }
    printf(">\n");
}

/* Register a multi-page allocation in the list of multi-page allocations */
int pgalloc_register_multipage(uint32_t base, uint32_t pages) {
    /* Search for an empty entry in the list */
    for (int i=0; i<PGALLOC_MULTIPAGE_LIST_SIZE; i++) {
        /* If pages=0 then the list entry is free */
        if (pgalloc_multipage_list[i].base == 0) {
            pgalloc_multipage_list[i].base = base;
            pgalloc_multipage_list[i].pages = pages;
            return 0;
        }
    }

    printf("pgalloc_register_multipage: no free entries in the multipage list\n");
    return 1;
}

/* Free a page allocation */
void pgfree(void *ptr) {
    uint32_t pages = 1;
    uint32_t base = (uint32_t) ptr;

    /* Check if it's multipage. If it's not in the list, it's one page */
    for (int i=0; i<PGALLOC_MULTIPAGE_LIST_SIZE; i++) {
        /* Check if base matches */
        if (pgalloc_multipage_list[i].base == base) {
            /* Retrieve page count */
            pages = pgalloc_multipage_list[i].pages;
            
            /* Mark as unused */
            pgalloc_multipage_list[i].base = 0;
            pgalloc_multipage_list[i].pages = 0;
            break;
        }
    }

    pgalloc_free_block_t *blk = pgalloc_free_head;

    /* Shortcut: this allocation is right at the end of a free block */
    /* Note: when this loop exits, blk should be NULL if the shortcut wasn't taken, and will hold the pointer to the expanded block if the shortcut was taken */
    while (blk != NULL) {
        uint32_t block_end = blk->base + (blk->len * 0x1000);

        /* The block to free is on the end of this free block */
        if (base == block_end) {
            printf("pgfree: adjacent after\n");
            blk->len += pages;
            /* leave blk with the pointer to the expanded block */
            break;
        } else {
            /* next in list */
            blk = blk->next;
        }
    }

    /* Shortcut: this allocation is right at the beginning of a free block */
    if (blk == NULL) {
        while (blk != NULL) {
            uint32_t block_end = base + (pages * 0x1000);

            /* The block to free is at the beginning of this free block */
            if (blk->base == block_end) {
                printf("pgfree: adjacent before\n");
                blk->base = base;
                blk->len += pages;

                /* leave blk with the pointer to the expanded block */
                break;
            }
        }
    }

    /* Not either shortcut case: make a new entry in the free block list */
    if (blk == NULL) {
        printf("pgfree: non-adjacent\n");

        /* New free block list entry */
        blk = pgalloc_alloc_free_block();
        if (blk == NULL) {
            printf("pgfree: unable to allocate free block list entry\n");
            return;
        }

        /* Fill in the list entry */
        blk->base = base;
        blk->len = pages;

        /* Add to the front of the list. Due to how pgalloc works, this will give priority to previously used memory, which is usually closer in size than the free blocks created in pgalloc_init */
        if (pgalloc_free_tail == NULL) {
            pgalloc_free_tail = blk;
        }
        blk->next = pgalloc_free_head;
        pgalloc_free_head = blk;
    }

    /* Merge if this makes the block adjacent to any others */
    pgalloc_free_block_t *absorber = pgalloc_free_head;

    while (absorber != NULL) {
        /* Makes no sense for a block to absorb itself */
        if (absorber == blk) {
            absorber = absorber->next;
            continue;
        }

        /* Some bounds calculations */
        uint32_t absorber_begin = absorber->base;
        uint32_t absorber_end = absorber->base + (absorber->len * 0x1000);
        uint32_t block_begin = blk->base;
        uint32_t block_end = blk->base + (blk->len * 0x1000);

        /* Changed block is before the absorber block */
        int absorbed = 0;
        if (absorber_begin == block_end) {
            printf("pgfree: absorb before\n");
            absorber->base = blk->base;
            absorber->len = blk->len + absorber->len;
            absorbed = 1;
        }

        /* Changed block is after the absorber block */
        if ((absorbed == 0) && (absorber_end == block_begin)) {
            printf("pgfree: absorb after\n");
            absorber->len += blk->len;
            absorbed = 1;
        }

        /* Handle absorption */
        if (absorbed) {
            /* Removing the head of the list is a special case */
            if (blk == pgalloc_free_head) {
                pgalloc_free_head = blk->next;
            } else {
                /* Find the previous block to the absorbed block */
                pgalloc_free_block_t *prev = pgalloc_free_head;
                while (prev != NULL) {
                    /* The next block of this block is blk */
                    if(prev->next == blk) {
                        break;
                    }

                    prev = prev->next;
                }

                /* Something weird happened and the absorbed block is not in the list? */
                if (prev == NULL) {
                    printf("pgfree: absorbed block is not in the list\n");
                    return;
                }

                /* Remove from list */
                prev->next = blk->next;
            }

            /* Free absorbed blk */
            pgalloc_free_free_block(blk);

            break;
        }
        
        absorber = absorber->next;
    }
}