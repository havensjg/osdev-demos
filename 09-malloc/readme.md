Adapted from 08-pagealloc

Implements a memory allocation library for smaller amounts of data (8 to 3072 bytes). Provides the two standard C memory management functions: malloc and free.

### malloc

1. The request size is rounded up to the nearest 8 bytes.
2. If the request is larger than 3072 bytes (0.75 page), the request is forwarded to the page allocator.
3. The free list is searched for a free block of at least the requested size. First for a block of the exact size, then for the largest free block.
4. If a block of suitable size is not found, a new block is added to the end of the free list from the page allocator.
5. If the block would have 8 bytes or more remaining after the allocation with header, the free block is resized to the remaining bytes, and the block following is allocated.
6. If the block does not have at least 8 bytes left, the entire block is used to fill the request.

### free

1. If the pointer is on a page boundary, the page is returned to the page allocator.
2. If the block is adjacent to another free block, it is added to that block.
3. If the merged block is adjacent to another the following block is also merged.
4. If the block can't be merged, the block is added to the front of the free list 
5. If the block is then found to be an entire page after merging, the page is returned to the page allocator