#include "shell.h"

#include "dir.h"
#include "stdio.h"
#include "stdint.h"

extern size_t cwd;

void shell_command_rmdir(Shell_Arguments arguments) {
	if (arguments.argc < 1) {
		printf("usage: %s [DIRECTORY NAME] ...", arguments.argv[0]);
		return;
	}

	for (size_t i = 1; i < arguments.argc; i++) {
		if (!dir_remove(arguments.argv[i], cwd)) {
			printf("couldn't delete directory '%s'\n", arguments.argv[i]);
		}
	}
}
