#include <stdint.h>

#include <io.h>
#include <ps2.h>

/* Count of PS/2 ports on the system */
int ps2_ports = 0;

/* Issues a command to the controller with no extra byte and no response expected */
void ps2_command(uint8_t cmd) {
    outb(PS2_CMD, cmd);
}

/* Enable IRQs for a port. Use PS2_CCB_P1_IRQ for port 1 and PS2_CCB_P2_IRQ for port 2 */
void ps2_enable_irq(uint8_t irq) {
    /* read the controller configuration byte */
    uint8_t ccb = ps2_request(PS2_RD_CCB);

    /* mask the input to only the two irq bits */
    irq &= (PS2_CCB_P1_IRQ | PS2_CCB_P2_IRQ);

    /* set those bits */
    ccb |= irq;

    /* write back the register */
    ps2_write(PS2_WR_CCB, ccb);
}

/* Initialize PS/2 controller */
void ps2_init(void) {
    /* Disable the PS/2 devices so they don't send stuff during initialization. They will be re-enabled as needed */
    ps2_command(PS2_P1_DISABLE);
    ps2_command(PS2_P2_DISABLE);

    /* Flush output buffer, get rid of any bytes sent before now */
    while (ps2_read_ready()) {
        ps2_read_data();
    }

    /* Set controller configuration byte */
    uint8_t ccb = ps2_request(PS2_RD_CCB);

    ccb &= ~(PS2_CCB_P1_IRQ | PS2_CCB_P2_IRQ | PS2_CCB_P1_TRANSLATE);
    ps2_write(PS2_WR_CCB, ccb);

    /* Detect number of ports. This bit should be 1 if the port is truly disabled. If it is 0, then port 2 doesn't actually exist */
    if (ccb & PS2_CCB_P2_CLK) {
        ps2_ports = 2;
    } else {
        ps2_ports = 1;
    }

    /* Perform controller self test */
    uint8_t post = ps2_request(PS2_POST);
    if (post != PS2_POST_GOOD) {
        /* PS/2 failed post for some reason. Report 0 ports to indicate that it isn't present */
        ps2_ports = 0;
    }

    /* Write CCB again, some hardware reset the PS/2 controller during POST */
    ps2_write(PS2_WR_CCB, ccb);

    /* Now, the both ports are disabled and ready to be enabled and reset by their respective drivers */
}

/* Read a byte from the data port */
uint8_t ps2_read_data(void) {
    return inb(PS2_DATA);
}

/* Checks if a byte is available to be read with ps2_read_data */
int ps2_read_ready(void) {
    uint8_t status = ps2_read_status();

    if (status & PS2_STATUS_OBUF_FULL) {
        return 1;
    } else {
        return 0;
    }
}

/* Read the status register */
uint8_t ps2_read_status(void) {
    return inb(PS2_STATUS);
}

/* Issue command to the controller that expects a result */
uint8_t ps2_request(uint8_t cmd) {
    ps2_command(cmd);

    return ps2_wait_read();
}

/* Wait for a byte to be available and read it */
uint8_t ps2_wait_read(void) {
    /* wait for the byte */
    while (!ps2_read_ready()) {
        io_wait();
    }

    /* read */
    return ps2_read_data();
}

/* Wait for the buffer to be ready and write a byte */
void ps2_wait_write(uint8_t data) {
    /* wait for the byte */
    while (!ps2_write_ready()) {
        io_wait();
    }

    /* write */
    ps2_write_data(data);
}

/* Issue command to the controller with a second byte */
void ps2_write(uint8_t cmd, uint8_t data) {
    ps2_command(cmd);
    ps2_wait_write(data);
}

/* Write a byte to the data port */
void ps2_write_data(uint8_t data) {
    outb(PS2_DATA, data);
}

/* Write a byte to a device */
void ps2_write_device(int dev, uint8_t data) {
    /* validate argument, only 0 and 1 are accepted */
    if (dev > 1 || dev < 0) {
        return;
    }

    /* instruct the controller that the next byte is for second port*/
    if (dev == 1) {
        ps2_command(PS2_WR_P2_IBUF);
    }

    /* write to the device */
    ps2_wait_write(data);
}

/* Check that the controller is ready to receive a byte */
int ps2_write_ready(void) {
    uint8_t status = ps2_read_status();

    if (status & PS2_STATUS_IBUF_FULL) {
        return 0;
    } else {
        return 1;
    }
}