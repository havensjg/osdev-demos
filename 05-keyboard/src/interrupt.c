#include <gdt.h>
#include <interrupt.h>
#include <pic.h>

/* Interrupt Descriptor Table */
idt_entry_t interrupt_idt[INTERRUPT_MAX + 1];

/* CPU loads IDTR from here, holds current location and size of the IDT */
idtr_t interrupt_idtr;

/* Initialize IDT */
void interrupt_init(void) {
    /* fill tables with default isr handlers */
    for (int i=0;i<=INTERRUPT_MAX;i++) {
        interrupt_set(i,GDT_KERNEL_CS,interrupt_dummy_isr,IDT_PRESENT | IDT_32BIT_INTERRUPT);
    }

    /* load idt */
    interrupt_idtr.offset = interrupt_idt;
    interrupt_idtr.size = sizeof(interrupt_idt);
    interrupt_load_idt();

    /* re-map PIC */
    pic_remap(INTERRUPT_IRQ_BASE, INTERRUPT_IRQ_BASE + 8);
}

/* Install external IRQ handler */
void interrupt_install_irq(int irq, void *handler) {
    interrupt_set(INTERRUPT_IRQ_BASE + irq, GDT_KERNEL_CS, handler, IDT_PRESENT | IDT_32BIT_INTERRUPT);
}

/* 
    Installs an ISR into the IDT 

    num - vector in table (0 to INTERRUPT_MAX - 1)
    sel - GDT selector for the ISR code
    off - pointer to the ISR
    attr - attributes of the IDT entry. consists of a gate type or'd with a privilege level or'd with IDT_PRESENT (if present) or nothing if entry is not present
        gate types:
        IDT_TASK_GATE
        IDT_16BIT_INTERRUPT
        IDT_16BIT_TRAP
        IDT_32BIT_INTERRUPT 
        IDT_32BIT_TRAP

        privilege levels:
        DPL0
        DPL1
        DPL2
        DPL3

*/
void interrupt_set(uint8_t num, uint16_t sel, void *off, uint8_t attr) {
    uint32_t loff = (uint32_t) off;

    interrupt_idt[num].attributes = attr;
    interrupt_idt[num].offset_hi = loff >> 16;
    interrupt_idt[num].offset_lo = loff & 0xffff;
    interrupt_idt[num].selector = sel;
    interrupt_idt[num].zero = 0;
}