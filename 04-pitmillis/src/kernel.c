#include <interrupt.h>
#include <pit.h>
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
void kernel_main(void) {
	/* Initialize terminal interface */
	terminal_initialize();

	/* Initialize IDT and re-map IRQs */
	interrupt_init();

	/* Initialize Programmable Interrupt Timer */
	pit_init(8000);

	/* Reload IDT and enable interrupts */
	interrupt_load_idt();
	interrupt_enable();

	/* Print some things on the screen */
	printf("Hello, kernel World!\n");

	/* Counters */
	int eighths = 0;
	int millis = 0;

	/* Infinite loop waiting for and processing interrupts */
	while (1) {
		/* check if PIT interrupt occurred */
		if (pit_occurred) {
			/* clear the flag */
			pit_occurred = 0;

			/* increment count */
			eighths++;

			/* roll over to millis */
			if (eighths == 8) {
				millis++;
				eighths = 0;
			}

			/* every second */
			if (millis % 1000 == 0 && eighths == 0) {
				printf("Seconds: %d\n",millis / 1000);
			}
		}

		/* wait for next interrupt */
		interrupt_wait();
	}
}