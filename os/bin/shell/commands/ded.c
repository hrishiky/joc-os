#include "shell.h"

#include "stdio.h"
#include "vga_text.h"

extern char vga_text_background_color;

void shell_command_ded(Shell_Arguments arguments) {
	printf("dedicated to:\n");
	printf("\tTerry Davis\n");
	printf("\tYe\n");

	printf("\n");

	printf("fun guys:\n");
	printf("\t");
	vga_text_print_character_color(0x02, VGA_TEXT_COLOR_MAGENTA, vga_text_background_color);
	printf(" Jobey\n");

	printf("\t");
	vga_text_print_character_color(0x06, VGA_TEXT_COLOR_LIGHT_BLUE, vga_text_background_color);
	printf(" Rowan\n");

	printf("\t");
	vga_text_print_character_color(0xFE, VGA_TEXT_COLOR_LIGHT_MAGENTA, vga_text_background_color);
	printf(" Harman\n");

	printf("\t");
	vga_text_print_character_color(0xE2, VGA_TEXT_COLOR_LIGHT_CYAN, vga_text_background_color);
	printf(" Foster\n");

	printf("\t");
	vga_text_print_character_color(0x81, VGA_TEXT_COLOR_BLUE, vga_text_background_color);
	printf(" Zakaria\n");

	printf("\t");
	vga_text_print_character_color(0x0E, VGA_TEXT_COLOR_LIGHT_RED, vga_text_background_color);
	printf(" Damian\n");
}
