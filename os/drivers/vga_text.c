#include "vga_text.h"

#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "asm_wrappers.h"

char* vga = (char*)0xB8000;

long vga_text_cursor_x = 0;
long vga_text_cursor_y = 0;

uint8_t vga_text_foreground_color = VGA_TEXT_FOREGROUND_COLOR_DEFAULT;
uint8_t vga_text_background_color = VGA_TEXT_BACKGROUND_COLOR_DEFAULT;

void vga_text_blinking_cursor_enable(unsigned char start, unsigned char end) {
	outb(0x0A, 0x3D4);

	outb((inb(0x3D5) & 0xC0) | start, 0x3D5);

	outb(0x0B, 0x3D4);
	outb((inb(0x3D5) & 0xE0) | end, 0x3D5);
}

void vga_text_blinking_cursor_disable(void) {
	outb(0x0A, 0x3D4);
	outb(0x20, 0x3D5);
}

void vga_text_blinking_cursor_shift(void) {
	unsigned long position = vga_text_cursor_y * VGA_TEXT_COLUMNS + vga_text_cursor_x;

	outb(0x0F, 0x3D4);
	outb((unsigned int) (position & 0xFF), 0x3D5);

	outb(0x0E, 0x3D4);
	outb((unsigned int) ((position >> 8) & 0xFF), 0x3D5);
}

void vga_text_scroll(long count) {
	memcpy((void*) vga, (void*) vga + count * (VGA_TEXT_COLUMNS * 2), (unsigned long) (VGA_TEXT_ROWS - count) * (VGA_TEXT_COLUMNS * 2));
	vga_text_clear_segment((VGA_TEXT_ROWS - count) * (VGA_TEXT_COLUMNS * 2), ((VGA_TEXT_ROWS - count) * (VGA_TEXT_COLUMNS * 2)) + count * (VGA_TEXT_COLUMNS * 2));
}

void vga_text_cursor_rectify(void) {
	if (vga_text_cursor_x > VGA_TEXT_COLUMNS_MAXIMUM) {
		unsigned long overflow = vga_text_cursor_x / VGA_TEXT_COLUMNS;
		vga_text_cursor_x -= VGA_TEXT_COLUMNS * overflow;
		vga_text_cursor_y += overflow;
	}

	if (vga_text_cursor_x < VGA_TEXT_COLUMNS_MINIMUM) {
		unsigned long overflow = (vga_text_cursor_x / VGA_TEXT_COLUMNS) + 1;
		vga_text_cursor_x -= VGA_TEXT_COLUMNS * overflow;
		vga_text_cursor_y += overflow;
	}

	if (vga_text_cursor_y > VGA_TEXT_ROWS_MAXIMUM) {
		unsigned long overflow = vga_text_cursor_y - VGA_TEXT_ROWS_MAXIMUM;
		vga_text_scroll(overflow);
		vga_text_cursor_y = VGA_TEXT_ROWS_MAXIMUM;
	}

	if (vga_text_cursor_y < VGA_TEXT_ROWS_MINIMUM) {
		vga_text_scroll(vga_text_cursor_y);
		vga_text_cursor_y = VGA_TEXT_ROWS_MINIMUM;
	}
}

void vga_text_cursor_shift(long x, long y) {
	vga_text_cursor_x = x;
	vga_text_cursor_y = y;

	vga_text_cursor_rectify();

	vga_text_blinking_cursor_shift();
}

void vga_text_reset_screen(void) {
	unsigned long long* buffer = (unsigned long long*) vga;

	for (unsigned short i = 0; i < VGA_TEXT_BUFFER_SIZE / 8; i++) {
		buffer[i] = 0;
	}

	vga_text_cursor_shift(0, 0);
}

void vga_text_clear_screen(void) {
	for (unsigned short i = 0; i < VGA_TEXT_BUFFER_SIZE; i += 2) {
		vga[i] = ' ';
	}

	vga_text_cursor_shift(0, 0);
}

void vga_text_clear_segment(unsigned short start, unsigned short end) {
	for (unsigned short i = start; i < end; i += 2) {
		vga[i] = ' ';
		vga[i + 1] = (vga_text_background_color << 4) | (vga_text_foreground_color & 0x0F);
	}
}

void vga_text_print_character(char character) {
	unsigned char attribute = (vga_text_background_color << 4) | (vga_text_foreground_color & 0x0F);

	switch (character) {
		case '\n':
			vga_text_cursor_y++;
			vga_text_cursor_shift(0, vga_text_cursor_y);

			return;

		case '\t':
			unsigned char spaces = vga_text_cursor_x % VGA_TEXT_TAB_LENGTH;

			if (vga_text_cursor_x + (4 - spaces) > VGA_TEXT_COLUMNS_MAXIMUM) {
				vga_text_print_character('\n');
			} else {
				for (unsigned char i = 0; i < 4 - spaces; i++) {
					vga_text_print_character(' ');
				}
			}

			return;

		case '\b':
			vga_text_cursor_x--;
			vga_text_cursor_shift(vga_text_cursor_x, vga_text_cursor_y);

			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x)] = ' ';
			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x) + 1] = attribute;

			return;

		default:
			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x)] = character;
			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x) + 1] = attribute;

			vga_text_cursor_x++;
			vga_text_cursor_shift(vga_text_cursor_x, vga_text_cursor_y);

			return;
	}
}

void vga_text_print_character_color(char character, unsigned char foreground_color, unsigned char background_color) {
	unsigned char attribute = (background_color << 4) | (foreground_color & 0x0F);

	switch (character) {
		case '\n':
			vga_text_cursor_y++;
			vga_text_cursor_shift(0, vga_text_cursor_y);

			return;

		case '\b':
			vga_text_cursor_x--;
			vga_text_cursor_shift(vga_text_cursor_x, vga_text_cursor_y);

			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x)] = ' ';
			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x) + 1] = attribute;

			return;

		default:
			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x)] = character;
			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x) + 1] = attribute;

			vga_text_cursor_x++;
			vga_text_cursor_shift(vga_text_cursor_x, vga_text_cursor_y);

			return;
	}
}

void vga_text_print_position(char* string, unsigned char x, unsigned char y) {
	for (unsigned short i = 0; string[i] != '\0'; i++) {
		if (string[i] == '\n') {
			x = 0;
			y++;
			continue;
		}

		if (x >= VGA_TEXT_COLUMNS) {
			x -= VGA_TEXT_COLUMNS;
			y++;
		} else if (y >= VGA_TEXT_ROWS) {
 			y -= VGA_TEXT_ROWS;
		}

		vga[2 * ((y * VGA_TEXT_COLUMNS) + x)] = string[i];
		vga[2 * ((y * VGA_TEXT_COLUMNS) + x) + 1] = VGA_TEXT_BW;
		x++;
	}
}

void vga_text_print(char* string) {
	for (unsigned long i = 0; string[i] != '\0'; i++) {
		vga_text_print_character(string[i]);
	}
}

void vga_text_print_color(char* string, unsigned char foreground_color, unsigned char background_color) {
	for (unsigned long i = 0; string[i] != '\0'; i++) {
		vga_text_print_character_color(string[i], foreground_color, background_color);
	}
}

void vga_text_print_integer(long long number) {
	char number_string[255];

	ll_to_str(number, number_string, 10);

	vga_text_print(number_string);
}

void vga_text_print_unsigned_integer(unsigned long long number) {
	char number_string[255];

	ull_to_str(number, number_string, 10);

	vga_text_print(number_string);
}

void vga_text_print_hex(unsigned long long number) {
	char number_string[255];

	ull_to_str(number, number_string, 16);

	vga_text_print(number_string);
}

void vga_text_change_colors(unsigned char foreground_color, unsigned char background_color) {
	if (foreground_color > 0xF ||
		background_color > 0xF) {
		return;
	}

	vga_text_foreground_color = foreground_color;
	vga_text_background_color = background_color;
}

void vga_text_refresh_colors(void) {
	unsigned char attribute = (vga_text_background_color << 4) | (vga_text_foreground_color & 0x0F);

	for (unsigned short i = 0; i < VGA_TEXT_BUFFER_SIZE; i += 2) {
		vga[1 + i] = attribute;
	}
}
