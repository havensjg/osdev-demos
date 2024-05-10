#include <stdio.h>
#include <stdlib.h>

#include <heap.h>
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

	/* Print initial state */
	heap_print_diagnostics();
	pgalloc_print_diagnostics();

	/* Allocate some bytes */
	void *p1 = malloc(64);
	printf(" malloc(64) gave %p\n",p1);
	heap_print_diagnostics();

	void *p2 = malloc(16);
	printf(" malloc(16) gave %p\n",p2);
	heap_print_diagnostics();

	void *p3 = malloc(1234);
	printf(" malloc(1234) gave %p\n",p3);
	heap_print_diagnostics();

	void *pbig = malloc(12345);
	printf(" malloc(12345) gave %p\n",pbig);
	heap_print_diagnostics();
	pgalloc_print_diagnostics();
	
	/* Free one in between */
	free(p2);
	heap_print_diagnostics();

	/* Free the first one */
	free(p1);
	heap_print_diagnostics();

	/* Free the last one */
	free(p3);
	heap_print_diagnostics();
	pgalloc_print_diagnostics();

	/* Free the big one */
	free(pbig);
	heap_print_diagnostics();
	pgalloc_print_diagnostics();

	/* After this, the heap and page allocators should be just like they were when they were initialized */
}