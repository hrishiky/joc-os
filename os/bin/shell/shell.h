#ifndef SHELL_H
#define SHELL_H

#include "keyboard.h"
#include "vga_text.h"
#include "string.h"

#define SHELL_COMMANDS_COUNT 12
#define SHELL_COMMAND_LENGTH_MAXIMUM 1024
#define SHELL_COMMAND_DELIMITER ' '
#define SHELL_ARGC_MAXIMUM 64
#define SHELL_ARGV_MAXIMUM 128

#define SHELL_COMMAND_HISTORY_COUNT 32

typedef struct {
	int argc;
	char argv[SHELL_ARGC_MAXIMUM][SHELL_ARGV_MAXIMUM];
} Shell_Arguments;

typedef struct {
	char* command;
	void (*function_pointer)(Shell_Arguments);
} Shell_Command;

void shell_print_prompt(void);
void shell_print_spacing(void);
char* shell_get_input(void);
Shell_Arguments shell_input_parse(char* input);
void shell_run_command(Shell_Arguments arguments);
void shell_main(void);

#endif
