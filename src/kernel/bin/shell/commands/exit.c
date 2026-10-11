#include "shell.h"

#include "fs.h"

void shell_command_exit(Shell_Arguments arguments) {
	fs_exit();
}
