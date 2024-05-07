#pragma once

#include <stdint.h>

/* Keyboard commands */
#define KEYBOARD_LEDS          0xED
#define KEYBOARD_ECHO          0xEE
#define KEYBOARD_SCAN_CODE_SET 0xF0
#define KEYBOARD_IDENTIFY      0xF2
#define KEYBOARD_TYPEMATIC     0xF3
#define KEYBOARD_ENABLE_SCAN   0xF4
#define KEYBOARD_DISABLE_SCAN  0xF5
#define KEYBOARD_SET_DEFAULTS  0xF6
#define KEYBOARD_ALL_TYPEMATIC 0xF7
#define KEYBOARD_ALL_MAKEREL   0xF8
#define KEYBOARD_ALL_MAKE      0xF9
#define KEYBOARD_ALL_TM_MR     0xFA
#define KEYBOARD_KEY_TYPEMATIC 0xFB
#define KEYBOARD_KEY_MAKEREL   0xFC
#define KEYBOARD_KEY_MAKE      0xFD
#define KEYBOARD_RESEND        0xFE
#define KEYBOARD_RESET         0xFF

/* Key codes for non-printable characters */
#define KEY_F9              0x01
#define KEY_F5              0x03
#define KEY_F3              0x04
#define KEY_F1              0x05
#define KEY_F2              0x06
#define KEY_F12             0x07
#define KEY_F10             0x09
#define KEY_F8              0x0A
#define KEY_F6              0x0B
#define KEY_F4              0x0C
#define KEY_TAB             0x0D
#define KEY_LEFT_ALT        0x11
#define KEY_LEFT_SHIFT      0x12
#define KEY_LEFT_CTRL       0x14
#define KEY_CAPS_LOCK       0x58
#define KEY_RIGHT_SHIFT     0x59
#define KEY_BACKSPACE       0x66
#define KEY_ESC             0x76
#define KEY_NUM_LOCK        0x77
#define KEY_F11             0x78
#define KEY_SCROLL_LOCK     0x7E
#define KEY_F7              0x83

#define KEYBOARD_QUEUE_SIZE 16

/* Put a character in the keyboard code queue*/
void keyboard_code_queue_put(uint8_t code);

/* Get the number of codes in the buffer */
int keyboard_code_queue_size(void);

/* Issue command to the keyboard, no extra bytes sent or received */
void keyboard_command(uint8_t cmd);

/* Initialize the PS/2 keyboard */
void keyboard_init(void);

/* Read the key code from the keyboard queue */
uint8_t keyboard_read(void);

/* Convert a keycode to ASCII. Returns 0 when no ASCII code maps to the key */
char keyboard_to_ascii(uint8_t code);

/* Check if an interrupt occurred and handle it. Returns 1 if there are codes in the queue to read */
int keyboard_update(void);

/* Wait for the keyboard to send a byte and read it when its available */
uint8_t keyboard_wait_read(void);