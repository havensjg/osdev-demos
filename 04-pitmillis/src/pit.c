#include <interrupt.h>
#include <io.h>
#include <pic.h>
#include <pit.h>

/* Variable that becomes 1 when the PIT interrupt occurs */
volatile int pit_occurred = 0;

/* Initialize the Programmable Interrupt Timer (PIT) */
void pit_init(uint32_t freq) {
    /* set mode 2 on channel 0, lo/hi byte access, binary count */
    outb(PIT_CMD, PIT_CH0 | PIT_ACC_LOHI | PIT_MODE_2 | PIT_BINARY);

    /* calculate reload value */
    uint16_t count = 1193182 / freq;
    outb(PIT0_DATA, count & 0xff);
    outb(PIT0_DATA, count >> 8);

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