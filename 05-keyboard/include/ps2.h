#pragma once

#include <stdint.h>

/* PS2 controller ports */
#define PS2_DATA   0x60
#define PS2_CMD    0x64
#define PS2_STATUS 0x64

/* PS2 controller commands */
#define PS2_RD_RAM(x)  0x20+x
#define PS2_RD_CCB     PS2_RD_RAM(0)
#define PS2_WR_RAM(x)  0x60+x
#define PS2_WR_CCB     PS2_WR_RAM(0)
#define PS2_P2_DISABLE 0xA7
#define PS2_P2_ENABLE  0xA8
#define PS2_P2_TEST    0xA9
#define PS2_POST       0xAA
#define PS2_P1_POST    0xAB
#define PS2_DIAG_DUMP  0xAC
#define PS2_P1_DISABLE 0xAD
#define PS2_P1_ENABLE  0xAE
#define PS2_RD_COP     0xD0
#define PS2_WR_COP     0xD1
#define PS2_WR_P1_BUF  0xD2
#define PS2_WR_P2_OBUF 0xD3
#define PS2_WR_P2_IBUF 0xD4

/* PS2 contoller configuration byte fields */
#define PS2_CCB_P1_IRQ       0x01
#define PS2_CCB_P2_IRQ       0x02
#define PS2_CCB_SYSTEM       0x04
#define PS2_CCB_P1_CLK       0x10
#define PS2_CCB_P2_CLK       0x20
#define PS2_CCB_P1_TRANSLATE 0x40

/* PS/2 status register fields */
#define PS2_STATUS_OBUF_FULL 0x01
#define PS2_STATUS_IBUF_FULL 0x02
#define PS2_STATUS_SYSTEM    0x04
#define PS2_STATUS_CD        0x08
#define PS2_STATUS_TIMEOUT   0x40
#define PS2_STATUS_PARITY    0x80

/* PS/2 POST command responses */
#define PS2_POST_GOOD 0x55
#define PS2_POST_BAD  0xFC

/* Count of PS/2 ports on the system */
extern int ps2_ports;

/* Issues a command to the controller with no extra byte and no response expected */
void ps2_command(uint8_t cmd);

/* Enable IRQs for a port. Use PS2_CCB_P1_IRQ for port 1 and PS2_CCB_P2_IRQ for port 2 */
void ps2_enable_irq(uint8_t irq);

/* Initialize PS/2 controller */
void ps2_init(void);

/* Read a byte from the data port */
uint8_t ps2_read_data(void);

/* Checks if a byte is available to be read with ps2_read_data */
int ps2_read_ready(void);

/* Read the status register */
uint8_t ps2_read_status(void);

/* Issue command to the controller that expects a result */
uint8_t ps2_request(uint8_t cmd);

/* Wait for a byte to be available and read it */
uint8_t ps2_wait_read(void);

/* Wait for the buffer to be ready and write a byte */
void ps2_wait_write(uint8_t data);

/* Issue command to the controller with a second byte */
void ps2_write(uint8_t cmd, uint8_t data);

/* Write a byte to the data port */
void ps2_write_data(uint8_t data);

/* Write a byte to a device */
void ps2_write_device(int dev, uint8_t data);

/* Check that the controller is ready to receive a byte */
int ps2_write_ready(void);