#include <io.h>
#include <pic.h>

/* Tell the PIC that the OS is done servicing the interrupt */
void pic_eoi(int irq) {
	if (irq >= 8) {
		outb(PIC2_COMMAND, PIC_EOI);
	}

	outb(PIC1_COMMAND, PIC_EOI);
}

/* Re-map the interrupt vector bases of the two Programmable Interrupt Controllers (PICs) */
void pic_remap(uint8_t master_base, uint8_t slave_base) {
	for (int i=0; i<16; i++) {
		pic_eoi(i);
	}
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, master_base);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, slave_base);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

    /* mask all interrupts. will unmask the needed interrupts later */
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

/* Unmask external IRQ */
void pic_unmask_irq(int irq) {
    uint16_t port = PIC1_DATA;

    /* master or slave PIC? */
    if (irq >= 8) {
        /* make sure slave is unmasked */
        pic_unmask_irq(2);

        /* adjust so the slave gets updated */
        port = PIC2_DATA;
        irq -= 8;
    }

    /* clear the bit */
    uint8_t value = inb(port) & ~(1 << irq);
    outb(port, value);
}