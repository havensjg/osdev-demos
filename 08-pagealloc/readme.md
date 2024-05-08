Adapted from 07-multiboot

Uses the memory map information from the multiboot bootloader to initialize a page allocator. The page allocator works by maintaining a linked-list of all free blocks of memory. A list of multi-page allocations is also maintained, which keeps track of the size of each multi-page allocation.

Whenever pages are allocated, they are allocated from the end of the first free block of suitable size. The count of pages in the free block is decremented by the number of pages requested. If more than one page are requested, an entry is added into the multi-page allocation list, recording the number of pages allocated. If a free block no longer contains any pages, it is removed from the list.

Whenever pages are freed, if the pages are directly after a free block, that block's page count is incremented by the allocation size retrieved from the multi-page allocation list. If the pages are directly before a free block, the base address is decreased to match the base address of the page and its page count increased by the allocation size. If two free blocks are found to be adjacent after the pages are freed, they are merged.