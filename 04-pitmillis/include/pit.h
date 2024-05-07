#pragma once

#include <stdint.h>

/* PIT IO ports */
#define PIT0_DATA 0x40
#define PIT1_DATA 0x41
#define PIT2_DATA 0x42
#define PIT_CMD   0x43

/* PIT_CMD channel field */
#define PIT_CH0   0x00
#define PIT_CH1   0x40
#define PIT_CH2   0x80
#define PIT_CH_RB 0xC0

/* PIT_CMD access field */
#define PIT_ACC_LATCH 0x00
#define PIT_ACC_LO    0x10
#define PIT_ACC_HI    0x20
#define PIT_ACC_LOHI  0x30

/* PIT_CMD mode field */
#define PIT_MODE_0 0x00 // interrupt on terminal count
#define PIT_MODE_1 0x02 // hardware retriggerable one-shot
#define PIT_MODE_2 0x04 // rate generator
#define PIT_MODE_3 0x06 // square wave generator
#define PIT_MODE_4 0x08 // software triggered strobe
#define PIT_MODE_5 0x0A // hardware triggered strobe

/* PIT_CMD binary/BCD */
#define PIT_BINARY 0
#define PIT_BCD    1

/* Variable that becomes 1 when the PIT interrupt occurs */
extern volatile int pit_occurred;

/* IRQ 0 ISR (see irq.s) */
void irq0_wrap(void);

/* Initialize the Programmable Interrupt Timer (PIT) */
void pit_init(uint32_t freq);