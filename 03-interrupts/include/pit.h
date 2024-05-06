#pragma once

/* Variable that becomes 1 when the PIT interrupt occurs */
extern volatile int pit_occurred;

/* IRQ 0 ISR (see irq.s) */
void irq0_wrap(void);

/* Initialize the Programmable Interrupt Timer (PIT) */
void pit_init(void);