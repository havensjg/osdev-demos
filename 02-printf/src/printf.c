#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <terminal.h>

const char *HEX_LOWERCASE = "0123456789abcdef";
const char *HEX_UPPERCASE = "0123456789ABCDEF";

int putchar(int ic) {
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
	return ic;
}

static int print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return 0;
	return 1;
}

/* Print a decimal number*/
static int print_number(unsigned int v) {
    /* print the digits into the buffer in reverse order */
    char buf[16];
    for (int i=0;i<16;i++) {
        if (v > 0) {
            buf[i] = '0' + (v % 10);
        } else {
            buf[i] = 0;
        }
        v /= 10;
    }

    /* reverse the digits */
    int l = strlen(buf);
    int h = l / 2;
    for (int i=0;i<h;i++) {
        char tmp = buf[i];
        buf[i] = buf[l-1-i];
        buf[l-1-i] = tmp;
    }

    print(buf,l);
    return l;
}

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

    char c;
    const char *hex_chars = 0;
	int written = 0;
 
	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;
 
		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}
 
		format++;

		// print based on specifier:
		switch (*format) {
			case 'd':
			case 'i': // signed decimal integer
                int d = va_arg(parameters, int);
                
                /* if negative, put the minus out front */
                if (d < 0) {
                    d = -d;
                    putchar('-');
                    written++;
                }

                written += print_number(d);
                break;
			case 'u': // unsigned decimal integer
                unsigned int u = va_arg(parameters, unsigned int);

                written += print_number(u);
				break;
			case 'o': 
                // unsigned octal
				break;
			case 'p': // pointer
			case 'x': // unsigned hex
				hex_chars = HEX_LOWERCASE;
				__attribute__((fallthrough));
			case 'X': // unsigned hex uppercase
				if (!hex_chars) hex_chars = HEX_UPPERCASE;
				uint32_t v = va_arg(parameters, uint32_t);
				int s = 32;
                uint32_t v2 = 0;

                do {
                    s-=4;
                    v2 = v >> s;
                    v2 &= 0xf;
                } while (v2 == 0 && s>=0); // skip leading zeros
                if (s < 0) print(hex_chars,1); // 0
                while (s >= 0) {
                    uint32_t v2 = v >> s;
                    v2 &= 0xf;
                    print(hex_chars+v2,1);
                    written++;
                    s-=4;
                }
				break;
			case 'f':
				// decimal float lowercase
				break;
			case 'F':
				// decimal float uppercase
				break;
			case 'e':
				// scientific lowercase
				break;
			case 'E':
				// scientific uppercase
				break;
			case 'g':
				// shortest e or f
				break;
			case 'G':
				// shortest E or F
				break;
			case 'a':
				// hex float lowercase
				break;
			case 'A':
				// hex float uppercase
				break;
			case 'c': // char
				c = (char) va_arg(parameters, int /* char promotes to int */);
				if (!maxrem) {
					// TODO: Set errno to EOVERFLOW.
					return -1;
				}
				if (!print(&c, sizeof(c)))
					return -1;
				written++;
				break;
			case 's': // string
				const char* str = va_arg(parameters, const char*);
				size_t len = strlen(str);
				if (!print(str, len))
					return -1;
				written += len;
				break;
			case 'n':
				// put char count into argument of signed int
				break;
		}
		format++;
	}

	va_end(parameters);
	return written;
}