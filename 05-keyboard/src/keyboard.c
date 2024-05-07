#include <stdio.h>

#include <interrupt.h>
#include <keyboard.h>
#include <pic.h>
#include <ps2.h>

/* Maps key codes from scan code set 2 to ASCII */
static const char KEYBOARD_ASCII[128] = {
//  0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '`',  0x00, // 0x00
    0x00, 0x00, 0x00, 0x00, 0x00, 'Q',  '1',  0x00, 0x00, 0x00, 'Z',  'S',  'A',  'W',  '2',  0x00, // 0x10
    0x00, 'C',  'X',  'D',  'E',  '4',  '3',  0x00, 0x00, ' ',  'V',  'F',  'T',  'R',  '5',  0x00, // 0x20
    0x00, 'N',  'B',  'H',  'G',  'Y',  '6',  0x00, 0x00, 0x00, 'M',  'J',  'U',  '7',  '8',  0x00, // 0x30
    0x00, ',',  'K',  'I',  'O',  '0',  '9',  0x00, 0x00, '.',  '/',  'L',  ';',  'P',  '-',  0x00, // 0x40
    0x00, 0x00, '\'', 0x00, '[',  '=',  0x00, 0x00, 0x00, 0x00, '\n', ']',  0x00, '\\', 0x00, 0x00, // 0x50
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x60
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 0x70
};

volatile int keyboard_irq_occurred = 0;
uint8_t keyboard_code_queue[KEYBOARD_QUEUE_SIZE];
int keyboard_code_queue_read = 0;
int keyboard_code_queue_write = 0;

/* IRQ1 ISR */
void irq1_wrap(void);

/* Put a character in the keyboard code queue*/
void keyboard_code_queue_put(uint8_t code) {
    int next_write = keyboard_code_queue_write + 1;

    /* wrap the next write pointer if its past the end */
    if (next_write == KEYBOARD_QUEUE_SIZE) {
        next_write = 0;
    }

    /* can't tell if it's full or empty when the pointers are the same. want to leave 1 byte empty */
    if (next_write == keyboard_code_queue_read) {
        printf("keyboard overflow %x\n",code);
    }

    /* put into the queue */
    keyboard_code_queue[keyboard_code_queue_write] = code;
    keyboard_code_queue_write = next_write;
}

/* Get the number of codes in the buffer */
int keyboard_code_queue_size(void) {
    if (keyboard_code_queue_read > keyboard_code_queue_write) {
        /* length - (read - write) */
        return KEYBOARD_QUEUE_SIZE - (keyboard_code_queue_read - keyboard_code_queue_write);
    } else {
        /* write - read */
        return keyboard_code_queue_write - keyboard_code_queue_read;
    }
}

/* Issue command to the keyboard, no extra bytes sent or received */
void keyboard_command(uint8_t cmd) {
    ps2_write_device(0, cmd);
}

/* Initialize the PS/2 keyboard */
void keyboard_init(void) {
    /* Make sure a PS/2 controller is present and working */
    if (ps2_ports >= 1) {
        /* Install an interrupt handler */
        interrupt_install_irq(1, irq1_wrap);

        /* Enable IRQ1 in the PIC */
        pic_unmask_irq(1);

        /* Enable IRQs */
        ps2_enable_irq(PS2_CCB_P1_IRQ);

        /* Enable the device */
        ps2_command(PS2_P1_ENABLE);

        /* Reset the device */
        keyboard_command(KEYBOARD_RESET);

        /* Start scanning */
        keyboard_command(KEYBOARD_ENABLE_SCAN);
    }
}

/* IRQ1 handler */
void keyboard_irq(void) {
    keyboard_irq_occurred = 1;
    pic_eoi(1);
}

/* Read the key code from the keyboard queue */
uint8_t keyboard_read(void) {
    /* read == write means the queue is empty */
    if (keyboard_code_queue_read == keyboard_code_queue_write) {
        printf("keyboard underflow\n");
        return 0xff;
    }

    /* read from the queue */
    uint8_t c = keyboard_code_queue[keyboard_code_queue_read];

    /* wrap the pointer if it passes the end */
    keyboard_code_queue_read++;
    if (keyboard_code_queue_read == KEYBOARD_QUEUE_SIZE) {
        keyboard_code_queue_read = 0;
    }

    return c;
}

/* Convert a keycode to ASCII. Returns 0 when no ASCII code maps to the key */
char keyboard_to_ascii(uint8_t code) {
    if (code < 0x80) {
        return KEYBOARD_ASCII[code];
    } else {
        return 0x00;
    }
}

/* Check if an interrupt occurred and handle it. Returns 1 if there are codes in the queue to read */
int keyboard_update(void) {
    if (keyboard_irq_occurred) {
        /* read all available bytes and place into queue */
        while (ps2_read_ready()) {
            keyboard_code_queue_put(ps2_read_data());
        }

        /* clear the flag */
        keyboard_irq_occurred = 0;
    }

    /* returns 1 (true) if there are characters in the queue */
    return (keyboard_code_queue_size() > 0);
}

/* Wait for the keyboard to send a byte and read it when its available */
uint8_t keyboard_wait_read(void) {
    /* wait for keyboard to send next code */
    while (!keyboard_update()) {
        /* this is fine for a simple example like this, but the CPU should do other things while waiting */
        interrupt_wait();
    }
    
    /* read the actual key code */
    return keyboard_read();
}