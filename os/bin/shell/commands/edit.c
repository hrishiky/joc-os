#include "shell.h"

#include "editor.h"
#include "stdio.h"
#include "stdint.h"

extern size_t cwd;
extern char* cwd_path;

void shell_command_edit(Shell_Arguments arguments) {
	if (arguments.argc != 2) {
		printf("usage: %s [FILE PATH]\n", arguments.argv[0]);
		return;
	}

	editor_main(arguments.argv[1], cwd, cwd_path);
}
