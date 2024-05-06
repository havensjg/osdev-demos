#pragma once

#include <stdint.h>

/* PIC IO addresses */
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

/* ICW 1 */
#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

/* ICW 4 */
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

/* PIC commands */
#define PIC_EOI		0x20		/* End-of-interrupt command code */

/* Tell the PIC that the OS is done servicing the interrupt */
void pic_eoi(int irq);

/* Re-map the interrupt vector bases of the two Programmable Interrupt Controllers (PICs) */
void pic_remap(uint8_t master_base, uint8_t slave_base);

/* Unmask IRQ */
void pic_unmask_irq(int irq);