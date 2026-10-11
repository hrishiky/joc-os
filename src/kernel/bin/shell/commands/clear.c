#include "shell.h"

#include "vga_text.h"

void shell_command_clear(Shell_Arguments arguments) {
	vga_text_refresh_colors();
	vga_text_clear_screen();
	vga_text_cursor_shift(0, 0);
}
