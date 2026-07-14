#include "shell.h"

#include "commands.h"
#include "stdio.h"
#include "stdint.h"
#include "keyboard.h"
#include "vga_text.h"
#include "string.h"
#include "fs.h"

// move info and test commands under one command, use command line arguments to specify which (info heap, info pmm, etc.)
// keep spaces during parsing when using "" or ''


extern unsigned char vga_text_cursor_x;
extern unsigned char vga_text_cursor_y;

Shell_Command commands[] = {
	{ "help", &shell_command_help },
	{ "echo", &shell_command_echo },
	{ "clear", &shell_command_clear },
	{ "reboot", &shell_command_reboot },
	{ "color", &shell_command_color },
	{ "halt", &shell_command_halt },
	{ "pmminfo", &shell_command_pmminfo },
	{ "heapinfo", &shell_command_heapinfo },
	{ "vgatest", &shell_command_vgatest },
	{ "meminfo", &shell_command_meminfo },
	{ "ded", &shell_command_ded },
	{ "temp", &shell_command_temp },
	{ "ls", &shell_command_ls },
	{ "mkdir", &shell_command_mkdir }
};

uint16_t shell_command_history_start;
Shell_Arguments shell_command_history[SHELL_COMMAND_HISTORY_COUNT];

uint8_t shell_foreground_color;
uint8_t shell_background_color;

uint8_t shell_input_start_x;
uint8_t shell_input_start_y;

uint64_t cwd;

void shell_print_prompt(void) {
	printf("$ ");

	shell_input_start_x = vga_text_cursor_x;
	shell_input_start_y = vga_text_cursor_y;
}

void shell_print_spacing(void) {
	if (vga_text_cursor_x == 0) {
		if (vga_text_cursor_y == 0) {
			return;
		} else {
			printf("\n");
		}
	} else {
		printf("\n\n");
	}
}

char* shell_get_input(void) {
	static char input[SHELL_COMMAND_LENGTH_MAXIMUM];
	memset(input, 0, SHELL_COMMAND_LENGTH_MAXIMUM);

	unsigned short index = 0;

	while (1) {
		if (index == SHELL_COMMAND_LENGTH_MAXIMUM - 1) {
			input[index] = '\0';
			return input;
		}

		char character = keyboard_buffer_read();

		if (character == -1) {
			continue;
		}

		if (character == '\n') {
			vga_text_print_character('\n');
			input[index] = '\0';
			return input;
		}

		if (character == '\b') {
			if (vga_text_cursor_x == shell_input_start_x &&
			    vga_text_cursor_y == shell_input_start_y) {
				continue;
			}

			if (index > 0) {
				index--;
				input[index] = '\0';
				vga_text_print_character('\b');
				continue;
			}
		}

		vga_text_print_character(character);
		input[index++] = character;
		input[index] = '\0';
	}
}

Shell_Arguments shell_input_parse(char* input) {
	Shell_Arguments arguments;
	arguments.argc = 0;

	for (unsigned short i = 0; i < SHELL_ARGC_MAXIMUM; i++) {
		arguments.argv[i][0] = '\0';
	}

	unsigned short argv_index = 0;
	unsigned long i = 0;

	while (input[i] == SHELL_COMMAND_DELIMITER) {
		i++;
	}

	for (; i < strlen(input); i++) {
		if (input[i] == SHELL_COMMAND_DELIMITER) {
			while (input[i + 1] == SHELL_COMMAND_DELIMITER) {
				i++;
			}

			if (argv_index > 0) {
				arguments.argv[arguments.argc][argv_index] = '\0';
				arguments.argc++;
				argv_index = 0;

				if (arguments.argc >= SHELL_ARGC_MAXIMUM) {
					return arguments;
				}
			}

			continue;
		}

		if (argv_index < SHELL_ARGV_MAXIMUM - 1) {
			arguments.argv[arguments.argc][argv_index++] = input[i];
			arguments.argv[arguments.argc][argv_index] = '\0';
		}
	}

	if (argv_index > 0) {
		arguments.argv[arguments.argc][argv_index] = '\0';
		arguments.argc++;
	}

	return arguments;
}


void shell_command_history_add(Shell_Arguments arguments) {
	if (shell_command_history_start >= SHELL_COMMAND_HISTORY_COUNT) {
		shell_command_history_start -= SHELL_COMMAND_HISTORY_COUNT;
	}

	shell_command_history[shell_command_history_start++] = arguments;
}

void shell_run_command(Shell_Arguments arguments) {
	if (strcmp(arguments.argv[0], "\0")) {
		return;
	}

	for (unsigned short i = 0; i < SHELL_COMMANDS_COUNT; i++) {
		if (strcmp(commands[i].command, arguments.argv[0])) {
			shell_command_history_add(arguments);
			commands[i].function_pointer(arguments);
			return;
		}
	}

	printf("invalid command '%s'\n", arguments.argv[0]);
	printf("use 'help' to see valid commands");
}

void shell_main(void) {
	shell_command_history_start = 0;

	shell_background_color = VGA_TEXT_COLOR_BLACK;
	shell_foreground_color = VGA_TEXT_COLOR_WHITE;

	vga_text_clear_screen();
	vga_text_blinking_cursor_enable(0, 15);

	cwd = FS_ROOT_DIRECTORY_INODE;

	while (1) {
		shell_print_prompt();
		char* input = shell_get_input();
		Shell_Arguments arguments = shell_input_parse(input);
		shell_run_command(arguments);
		shell_print_spacing();
	}
}
