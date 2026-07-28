#include "shell.h"

#include "vga_text.h"
#include "stdlib.h"
#include "stdint.h"

extern uint8_t shell_foreground_color;
extern uint8_t shell_background_color;

void shell_command_color(Shell_Arguments arguments) { // color refresh changes custom color text to new colors; preserve custom colored text
	if (arguments.argc != 3) {
		vga_text_print("usage: color [FOREGROUND_COLOR] [BACKGROUND_COLOR]\n");

		return;
	}

	uint64_t foreground_color = str_to_ull(arguments.argv[1], 0);
	uint64_t background_color = str_to_ull(arguments.argv[2], 0);

	if (foreground_color > VGA_TEXT_COLOR_CODE_MAXIMUM || background_color > VGA_TEXT_COLOR_CODE_MAXIMUM) {
		return;
	}

	uint8_t cur_fg_color = shell_foreground_color;
	uint8_t cur_bg_color = shell_background_color;

	if (foreground_color <= VGA_TEXT_COLOR_MAXIMUM) {
		shell_foreground_color = foreground_color;
	} else {
		if (foreground_color == VGA_TEXT_COLOR_CODE_BACKGROUND) {
			shell_foreground_color = cur_bg_color;
		}
	}

	if (background_color <= VGA_TEXT_COLOR_MAXIMUM) {
		shell_background_color = background_color;
	} else {
		if (background_color == VGA_TEXT_COLOR_CODE_FOREGROUND) {
			shell_background_color = cur_fg_color;
		}
	}

	vga_text_change_colors(shell_foreground_color, shell_background_color);
	vga_text_refresh_colors();
}
