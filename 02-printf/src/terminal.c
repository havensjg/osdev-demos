/* standard C headers */
#include <string.h>

/* driver headers */
#include <io.h>
#include <terminal.h>
#include <vga.h>

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_set_color(uint8_t color) 
{
	terminal_color = color;
}

void terminal_set_cursor(unsigned int x, unsigned int y) {
	uint16_t pos = y * VGA_WIDTH + x;
	
	outb(VGA_CRTC_INDEX, VGA_CRTC_REG_CURSOR_POS_LOW);
	outb(VGA_CRTC_DATA, (uint8_t)(pos & 0xff));
	outb(VGA_CRTC_INDEX, VGA_CRTC_REG_CURSOR_POS_HIGH);
	outb(VGA_CRTC_DATA, (uint8_t)((pos >> 8) & 0xff));
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) 
{
	/* handle \n (newline) specially */
	if (c == '\n') {
		/* reset cursor back to left side of the new line */
		terminal_column = 0;
		terminal_row++;
	} else {
		/* put character on screen */
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

		/* wrap to next line */
		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			
			/* wrap around to top */
			if (++terminal_row == VGA_HEIGHT)
				terminal_row = 0;
		}
	}

	/* move cursor to position of the next character */
	terminal_set_cursor(terminal_column, terminal_row);
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}