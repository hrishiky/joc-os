#include "shell.h"

#include "file.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

extern uint64_t cwd;
extern char* cwd_path;

void shell_command_rm(Shell_Arguments arguments) {
	if (arguments.argc < 1) {
		printf("usage: %s [DIRECTORY NAME] ...", arguments.argv[0]);
		return;
	}

	for (size_t i = 1; i < arguments.argc; i++) {
		if (!file_delete(arguments.argv[i], cwd, cwd_path)) {
			printf("couldn't delete file '%s'\n", arguments.argv[i]);
		}
	}
}
