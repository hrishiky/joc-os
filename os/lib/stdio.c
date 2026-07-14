#include "stdio.h"

#include "stdlib.h"
#include "string.h"
#include "vga_text.h"

#include <stdarg.h>

void printf(char* fstring, ...) {
	va_list args;
	va_start(args, fstring);

	for (unsigned short i = 0; i < strlen(fstring); i++) {
		if (fstring[i] != '%') {
			vga_text_print_character(fstring[i]);
			continue;
		}

		switch (fstring[i + 1]) {
			case '%':
				vga_text_print_character('%');
				break;

			case 'd':
			case 'i':
				long long signed_integer = va_arg(args, long long);
				vga_text_print_integer(signed_integer);
				break;

			case 'u':
				unsigned long long unsigned_integer = va_arg(args, unsigned long long);
				vga_text_print_unsigned_integer(unsigned_integer);
				break;

			case 'p':
				void* pointer = va_arg(args, void*);
				vga_text_print_hex((unsigned long long) pointer);
				break;

			case 'x':
				unsigned long long hex_integer = va_arg(args, unsigned long long);
				vga_text_print_hex(hex_integer);
				break;

			case 'f':
				double number = va_arg(args, double);
				// handle
				break;

			case 'c':
				int character = va_arg(args, int);
				vga_text_print_character((char) character);
				break;

			case 's':
				char* string = va_arg(args, char*);
				vga_text_print(string);
				break;

		}

		i++;

	}

	va_end(args);
}
