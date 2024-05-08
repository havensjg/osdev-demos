#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <multiboot.h>
#include <pgalloc.h>

pgalloc_free_block_t *pgalloc_free_head = NULL;
pgalloc_free_block_t *pgalloc_free_tail = NULL;

pgalloc_free_block_t pgalloc_free_block_pool[PGALLOC_FREE_BLOCK_POOL_SIZE];

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