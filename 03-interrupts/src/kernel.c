#include <interrupt.h>
#include <stdio.h>
#include <terminal.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* 
	Kernel entry point.

	Expected initial state:
		- Interrupts disabled
*/
void kernel_main(void) 
{
	const char *str = "Hello world";

	/* Initialize terminal interface */
	terminal_initialize();

	/* Initialize IDT */
	interrupt_init();

	/* Print some things on the screen */
	printf("Hello, kernel World!\n");
}