#include <stdio.h>

#include <multiboot.h>
#include <pgalloc.h>
#include <terminal.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

multiboot_info_t *multiboot_info;
 
void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();

	/* Print something on the screen */
	printf("Hello, kernel World!\n");

	/* Initialize the page allocator */
	int r = pgalloc_init();
	if (r) {
		printf("Unable to initialize page allocator!\n");
		return;
	}

	/* Test the page allocator */
	void *page1 = pgalloc(1);
	printf("Allocated 1 page: %p\n",page1);
	pgalloc_print_diagnostics();

	void *page2 = pgalloc(1);
	printf("Allocated 1 page: %p\n",page2);
	pgalloc_print_diagnostics();

	void *page3 = pgalloc(10);
	printf("Allocated 10 pages: %p\n",page3);
	pgalloc_print_diagnostics();

	void *page4 = pgalloc(1);
	printf("Allocated 1 page: %p\n",page4);
	pgalloc_print_diagnostics();

	/* Free non-contiguous allocations. Each should create a new entry in the free block list. */
	pgfree(page1);
	printf("Freed %x\n",page1);
	pgalloc_print_diagnostics();
	pgfree(page3);
	printf("Freed %x\n",page3);
	pgalloc_print_diagnostics();

	/* Now allocate a page and see where it ends up */
	void *page5 = pgalloc(1);
	printf("Allocated 1 page: %p\n",page5);
	pgalloc_print_diagnostics();

	/* Free page2, which is now between two freed fragments. pgfree should merge them together. */
	pgfree(page2);
	printf("Freed %x\n",page2);
	pgalloc_print_diagnostics();
}