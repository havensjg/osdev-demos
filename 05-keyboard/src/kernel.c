#include <stdio.h>

#include <interrupt.h>
#include <keyboard.h>
#include <ps2.h>
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
void kernel_main(void) {
	/* Initialize terminal interface */
	terminal_initialize();

	/* Initialize IDT and re-map IRQs */
	interrupt_init();

	/* Initialize PS/2 controller and keyboard */
	ps2_init();
	keyboard_init();

	/* Reload IDT and enable interrupts */
	interrupt_load_idt();
	interrupt_enable();

	/* Print some things on the screen */
	printf("Enter some text:\n");

	/* Infinite loop waiting for and processing interrupts */
	while (1) {
		/* check if keyboard interrupt occurred */
		while (keyboard_update()) {
			uint8_t b = keyboard_read();
			
			/* if key released */
			if (b == 0xf0) {
				/* read the actual key code */
				b = keyboard_wait_read();
				printf("%x released\n",b);
			} else {
				printf("%x pressed\n",b);
			}
		}

		/* wait for next interrupt */
		interrupt_wait();
	}
}