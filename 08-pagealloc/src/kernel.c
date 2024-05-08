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

	/* TODO: Test the page allocator */
}