#pragma once

#include <stdint.h>

/* Dimensions of the text mode screen*/
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

/* VGA IO Ports */
#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA  0x3D5

/* CRTC register indices */
#define VGA_CRTC_REG_CURSOR_POS_HIGH 0x0E
#define VGA_CRTC_REG_CURSOR_POS_LOW  0x0F

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

/* Create a VGA text-mode attribute byte */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

/* Create a VGA text-mode character-attribute pair */
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}