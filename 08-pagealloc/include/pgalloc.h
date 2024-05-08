#pragma once

#include <stdint.h>

/* Number of entries in the free block pool */
#define PGALLOC_FREE_BLOCK_POOL_SIZE 64

/* Status of free block in the pool */
enum e_pgalloc_block_status {
    PGALLOC_BLOCK_FREE,
    PGALLOC_BLOCK_INUSE
};

typedef enum e_pgalloc_block_status pgalloc_block_status_t;

/* Free block list entry */
struct s_pgalloc_free_block {
    pgalloc_block_status_t pool_status;
    uint32_t base;
    uint32_t len;
    struct s_pgalloc_free_block *next;
};

typedef struct s_pgalloc_free_block pgalloc_free_block_t;

/* Boundaries of the kernel. Their location is determined by the linker script and correspond to the first byte of the kernel and the first byte after the kernel, respectively. Not intended to be accessed. */
extern int kernel_start;
extern int kernel_end;

/* Add a block of free memory to the pool. Length in pages. Returns non-zero if not successful */
int pgalloc_add_free_block(uint32_t base, uint32_t len);

/* Allocate a free block list entry from the pool. Returns NULL if not successful */
pgalloc_free_block_t *pgalloc_alloc_free_block();

/* Initialize the page allocator. Returns non-zero if not successful */
int pgalloc_init(void);