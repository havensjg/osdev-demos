#pragma once

#include <stdint.h>

/* Linked list entry that resides right before each block of memory */
struct s_heap_entry {
    struct s_heap_entry *next;
    uint32_t size;
};

typedef struct s_heap_entry heap_entry_t;

/* Add a free block to the back of the list */
void heap_add_free_block_back(heap_entry_t *blk);

/* Add a free block to the front of the list */
void heap_add_free_block_front(heap_entry_t *blk);

/* Print diagnostic information about the heap */
void heap_print_diagnostics();

/* Remove a block from the free block list */
void heap_remove_free_block(heap_entry_t *blk);