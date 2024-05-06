#include <interrupt.h>
#include <pic.h>
#include <pit.h>

/* Variable that becomes 1 when the PIT interrupt occurs */
volatile int pit_occurred = 0;

/* Initialize the Programmable Interrupt Timer (PIT) */
void pit_init(void) {
    /* install the handler for the irq (see irq.s) */
    interrupt_install_irq(0, irq0_wrap);

    /* unmask IRQ 0 */
    pic_unmask_irq(0);
}

/* IRQ 0 Handler */
void pit_irq(void) {
    /* set flag */
    pit_occurred = 1;

    /* signal EOI */
    pic_eoi(0);
}