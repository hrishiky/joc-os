#include "shell.h"

#include "stdio.h"
#include "stdint.h"

extern Shell_Command commands[];

void shell_command_help(Shell_Arguments arguments) {
	printf("commands:");

	for (size_t i = 0; i < SHELL_COMMANDS_COUNT; i++) {
		if (i % 8 == 0) {
			printf("\n");
		} else {
			printf("   ");
		}

		printf("%s", commands[i].command);
	}
}
