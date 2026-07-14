#include "shell.h"

#include "dir.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

extern uint64_t cwd;

void shell_command_mkdir(Shell_Arguments arguments) {
	for (size_t i = 1; i < arguments.argc; i++) {
		if (!fs_directory_create(arguments.argv[i], cwd)) {
			printf("couldn't create directory '%s'\n", arguments.argv[i]);
		}
	}
}
