#include "shell.h"

#include "dir.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

extern size_t cwd;
extern char* cwd_path;

void shell_command_mkdir(Shell_Arguments arguments) {
	if (arguments.argc < 2) {
		printf("usage: %s [DIRECTORY PATH] ...", arguments.argv[0]);
		return;
	}

	for (size_t i = 1; i < arguments.argc; i++) {
		if (!dir_make(arguments.argv[i], cwd, cwd_path, true)) {
			printf("couldn't create directory '%s'\n", arguments.argv[i]);
		}
	}
}
