#include "shell.h"

#include "vga_text.h"

void shell_command_color(Shell_Arguments arguments) { // color refresh changes custom color text to new colors; preserve custom colored text
	if (arguments.argc != 3) {
		vga_text_print("usage: color [FOREGROUND_COLOR] [BACKGROUND_COLOR]\n");

		return;
	}

	vga_text_change_colors(strtol(arguments.argv[1], 0), strtol(arguments.argv[2], 0));
	vga_text_refresh_colors();
}
