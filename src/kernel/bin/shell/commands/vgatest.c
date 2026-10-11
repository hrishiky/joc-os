#include "shell.h"
#include "vga_text.h"
#include "stdio.h"
#include "stdint.h"

extern long vga_text_cursor_x;
extern long vga_text_cursor_y;

void shell_command_vgatest(Shell_Arguments arguments) {
	// take input for character to use in color
	// fix bug with newline when running command on last row

	printf("  ");

	for (uint8_t j = 0; j < 16; j++) {
		printf("%x ", j);
	}

	printf("     ");

	for (uint8_t j = 0; j < 16; j++) {
		printf("%x ", j);
	}

	printf("\n");

	for (uint8_t i = 0; i < 16; i++) {
		printf("%x ", i);

		uint8_t x_pos, y_pos;

		for (uint8_t j = 0; j < 16; j++) {
			vga_text_cursor_shift(vga_text_cursor_x + 1, vga_text_cursor_y);
			x_pos = vga_text_cursor_x;
			y_pos = vga_text_cursor_y;
			vga_text_cursor_shift(vga_text_cursor_x - 1, vga_text_cursor_y);

			vga_text_print_character(i * 16 + j);

			vga_text_cursor_x = x_pos;
			vga_text_cursor_y = y_pos;

			vga_text_print_character(' ');
		}

		printf("   ");
		printf("%x ", i);

		for (uint8_t k = 0; k < 16; k++) {
			vga_text_print_character_color(4, i, k);
			vga_text_print_character(' ');
		}

		printf("\n");
	}
}
