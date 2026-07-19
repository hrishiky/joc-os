#include "shell.h"

#include "stdio.h"

void shell_command_print(Shell_Arguments arguments) {
	for (int i = 1; i < arguments.argc; i++) {
		printf("%s ", arguments.argv[i]);
	}
}
