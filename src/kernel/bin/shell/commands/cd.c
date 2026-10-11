#include "shell.h"

#include "dir.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

extern size_t cwd;
extern char* cwd_path;

void shell_command_cd(Shell_Arguments arguments) {
	if (arguments.argc != 2) {
		printf("usage: %s [DIRECTORY PATH]\n", arguments.argv[0]);
		return;
	}

	if (dir_change(arguments.argv[1], &cwd, cwd_path)) {
		return;
	}

	printf("invalid directory '%s'\n", arguments.argv[1]);
}
