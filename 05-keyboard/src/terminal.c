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

/* Erases a character from the screen and backs up the cursor*/
void terminal_backspace(void) {
	/* at the beginning of a row? */
	if (terminal_column == 0) {
		/* at the top left? can't backspace anymore */
		if (terminal_row == 0) {
			return;
		} else {
			/* not on the first row */
			terminal_row--;
			terminal_column = VGA_WIDTH - 1;
		}
	} else {
		/* in the middle of a row */
		terminal_column--;
	}

	/* erase the character */
	terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);

	/* move cursor to its new position */
	terminal_set_cursor(terminal_column, terminal_row);
}

/* Initialize the terminal output */
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

/* Scrolls terminal up by one line */
void terminal_scroll(void) {
	/* Scroll up by one line */
	memmove(terminal_buffer, terminal_buffer + VGA_WIDTH, (VGA_HEIGHT-1)*VGA_WIDTH*2);

	/* Fill in the line at the bottom */
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		const size_t index = (VGA_HEIGHT-1) * VGA_WIDTH + x;
		terminal_buffer[index] = vga_entry(' ', terminal_color);
	}
	
	/* Adjust the row position */
	terminal_row--;
}

/* Set the color of the next characters to be printed */
void terminal_set_color(uint8_t color) 
{
	terminal_color = color;
}

/* Set the position of the cursor */
void terminal_set_cursor(unsigned int x, unsigned int y) {
	uint16_t pos = y * VGA_WIDTH + x;
	
	outb(VGA_CRTC_INDEX, VGA_CRTC_REG_CURSOR_POS_LOW);
	outb(VGA_CRTC_DATA, (uint8_t)(pos & 0xff));
	outb(VGA_CRTC_INDEX, VGA_CRTC_REG_CURSOR_POS_HIGH);
	outb(VGA_CRTC_DATA, (uint8_t)((pos >> 8) & 0xff));
}

/* Put the character at the given position with the given color */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

/* Print one character and update cursor */
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

	/* scroll if necessary */
	while (terminal_row >= VGA_HEIGHT) {
		terminal_scroll();
	}

	/* move cursor to position of the next character */
	terminal_set_cursor(terminal_column, terminal_row);
}

/* Write a string of a given size */
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

/* Write a null-terminated string */
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}