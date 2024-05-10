#include <stddef.h>
#include <stdio.h>

#include <heap.h>
#include <pgalloc.h>

heap_entry_t *heap_free_head = NULL;
heap_entry_t *heap_free_tail = NULL;

/* Free memory */
void free(void *ptr) {
    /* Check if it is a full page allocation */
    uint32_t addr = (uint32_t) ptr;
    if ((addr & 0xfff) == 0) {
        pgfree(ptr);
        return;
    }

    int merged = 0;
    heap_entry_t *blk = (heap_entry_t *)(ptr - sizeof(heap_entry_t));
    heap_entry_t *merged_blk = heap_free_head;
    uint32_t blk_begin = (uint32_t) ((void *) blk);
    uint32_t blk_end = blk_begin + sizeof(heap_entry_t) + blk->size;
    
    /* Don't merge blocks across pages */
    if ((blk_begin & 0xfff) != 0) {
        /* Check if after a free block, if so merge */
        while (merged_blk != NULL) {
            /* Calculate bounds of merged_blk */
            uint32_t merged_blk_begin = (uint32_t) ((void *) merged_blk);
            uint32_t merged_blk_end = merged_blk_begin + sizeof(heap_entry_t) + merged_blk->size;

            /* blk is after merged_blk */
            if (blk_begin == merged_blk_end) {
                printf("free: adjacent after\n");
                merged_blk->size += sizeof(heap_entry_t) + blk->size;
                blk = merged_blk;
                merged = 1;
                break;
            }

            merged_blk = merged_blk->next;
        }
    }

    merged_blk = heap_free_head;
    blk_begin = (uint32_t) ((void *) blk);
    blk_end = blk_begin + sizeof(heap_entry_t) + blk->size;

    /* Don't merge blocks across pages */
    if ((blk_end & 0xfff) != 0) {
        /* Check if before a free block, if so merge */
        while (merged_blk != NULL) {
            /* Calculate bounds of merged_blk */
            uint32_t merged_blk_begin = (uint32_t) ((void *) merged_blk);

            /* blk is before merged_blk */
            if (blk_end == merged_blk_begin) {
                printf("free: adjacent before\n");
                blk->size += sizeof(heap_entry_t) + merged_blk->size;
                heap_remove_free_block(merged_blk);
                merged = 1;
                break;
            }

            merged_blk = merged_blk->next;
        }
    }

    /* Wasn't merged? Add to the front of the list */
    if (!merged) {
        printf("free: block not merged, adding to the list\n");
        heap_add_free_block_front(blk);
    }

    /* Check if the merged block is a full page. If so, return to the page allocator */
    addr = (uint32_t) ((void *)blk);
    if (((addr & 0xfff) == 0) && (blk->size = (4096 - sizeof(heap_entry_t)))) {
        heap_remove_free_block(blk);
        pgfree(blk);
    }
}

/* Add a free block to the back of the list */
void heap_add_free_block_back(heap_entry_t *blk) {
    if (heap_free_head == NULL) {
        heap_free_head = blk;
    } else {
        heap_free_tail->next = blk;
    }
    heap_free_tail = blk;
}

/* Add a free block to the front of the list */
void heap_add_free_block_front(heap_entry_t *blk) {
    if (heap_free_head == NULL) {
        heap_free_tail = blk;
    }
    blk->next = heap_free_head;
    heap_free_head = blk;
}

/* Print diagnostic information about the heap */
void heap_print_diagnostics() {
    printf(" heap free list: <");

    heap_entry_t *ent = heap_free_head;
    while (ent != NULL) {
        printf("%p:%d",ent->next,ent->size);
        if (ent->next != NULL) {
            printf(",");
        }

        ent = ent->next;
    }

    printf(">\n");
}

/* Remove a block from the free block list */
void heap_remove_free_block(heap_entry_t *blk) {
    /* Protect from blk = NULL */
    if (blk == NULL) {
        printf("heap_remove_free_block: tried to remove NULL from free list\n");
        return;
    }

    /* Removing the head is a special case */
    if (blk == heap_free_head) {
        heap_free_head = blk->next;
        if (heap_free_head == NULL) {
            heap_free_tail = NULL;
        }
    } else {
        heap_entry_t *prev = heap_free_head;

        /* Search for the previous block */
        while (prev != NULL) {
            if (prev->next == blk) {
                break;
            }
        }

        /* Does the previous block actually exist? Can't remove a block if its not in the list */
        if (prev != NULL) {
            prev->next = blk->next;
        } else {
            return;
        }

        /* Removing the tail is a special case */
        if (heap_free_tail == blk) {
            heap_free_tail = prev;
        }
    }
}

/* Allocate memory */
void *malloc(size_t size) {
    /* Is size 0? */
    if (size == 0) {
        printf("malloc: size was zero\n");
        return NULL;
    }

    /* Round up size to nearest 8 bytes */
    if ((size % 8) != 0) {
        size = (size + 8) & ~0x7;
        printf("malloc: rounding up to %d bytes\n",size);
    }

    /* If the request is >3072 bytes, use page allocator instead. */
    if (size >= 3072) {
        size_t pages = size / 4096;

        /* Round up pages */
        if ((size % 4096) != 0) {
            pages++;
        }

        printf("malloc: using pgalloc for size %d (%d pages)\n",size,pages);
        return pgalloc(pages);
    }

    /* First search for an exact size fit */
    heap_entry_t *blk = heap_free_head;
    while (blk != NULL) {
        if (blk->size == size) {
            /* blk now has a pointer to the entry of the exact size */
            printf("malloc: found block of exact size\n");
            break;
        }

        blk = blk->next;
    }

    /* If an exact fit wasn't found, search for the largest free block */
    if (blk == NULL && heap_free_head != NULL) {
        heap_entry_t *largest = heap_free_head;

        blk = heap_free_head;
        while (blk != NULL) {

            /* a larger block was found */
            if (blk->size > largest->size) {
                largest = blk;
            }

            blk = blk->next;
        }

        /* Does block actually exist? */
        if (largest != NULL) {
            printf("malloc: largest block found was %d bytes\n",largest->size);

            /* Must still check that this block can fulfill the request */
            if (largest->size >= size) {
                printf("malloc: using largest block\n");
                blk = largest;
            } else {
                printf("malloc: largest block is not large enough\n");
                blk = NULL;
            }
        }
    }

    /* If there's still no block that can fulfill the request, get a new one from the page allocator */
    if (blk == NULL) {
        printf("malloc: expanding from pgalloc\n");
        blk = (heap_entry_t *) pgalloc(1);

        if (blk == NULL) {
            printf("malloc: could not obtain a page to fill request\n");
            return NULL;
        }

        blk->next = NULL;
        blk->size = 4096 - sizeof(heap_entry_t);

        /* Add to the list */
        heap_add_free_block_back(blk);
    }

    /* Can this block be split? */
    uint32_t total_size = size + 8;
    uint32_t remaining = blk->size - total_size;
    if (remaining >= 8) {
        printf("malloc: splitting the block (r=%d)\n",remaining);

        /* Shrink the block */
        blk->size = remaining;

        /* Get pointer to the block just allocated */
        void *blkraw = (void *) blk;
        blkraw += sizeof(heap_entry_t) + remaining;
        blk = (heap_entry_t *) blkraw;
        blk->next = NULL;
        blk->size = size;
    } else {
        /* Block cannot be split, remove from list */
        printf("malloc: removing block from list\n");
        heap_remove_free_block(blk);
    }

    /* Return the pointer */
    void *ptr = (void *) blk;
    return ptr + sizeof(heap_entry_t);
}