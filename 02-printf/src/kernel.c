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
 
void kernel_main(void) 
{
	const char *str = "Hello world";

	/* Initialize terminal interface */
	terminal_initialize();

	/* Print some things on the screen */
	printf("Hello, kernel World!\n");
	printf("The number 64 in decimal and hex: %u %x\n",64,64);
	printf("The number 64 in octal: %o\n",64);
	printf("The number -64: %d\n",-64);
	printf("A string from somewhere: %s\n",str);
	printf("The pointer to the string above: %p\n",str);
	printf("A character: %c\n",'\x02'); // should show up as a smiley face
}