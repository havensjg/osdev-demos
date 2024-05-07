#pragma once

#include <stddef.h>
#include <stdint.h>

/* Erases a character from the screen and backs up the cursor*/
void terminal_backspace(void);

/* Initialize the terminal output */
void terminal_initialize(void);

/* Scroll the terminal by one line */
void terminal_scroll(void);

/* Set the color of the next characters to be printed */
void terminal_set_color(uint8_t color);

/* Set the position of the cursor */
void terminal_set_cursor(unsigned int x, unsigned int y);

/* Print one character and update cursor */
void terminal_putchar(char c);

/* Put the character at the given position with the given color */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

/* Write a string of a given size */
void terminal_write(const char* data, size_t size);

/* Write a null-terminated string */
void terminal_writestring(const char* data);