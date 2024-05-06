#pragma once

#include <stddef.h>
#include <stdint.h>

void terminal_initialize(void);
void terminal_set_color(uint8_t color);
void terminal_set_cursor(unsigned int x, unsigned int y);
void terminal_putchar(char c);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);