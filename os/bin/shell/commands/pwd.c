#include "shell.h"

#include "dir.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

extern size_t cwd;
extern char* cwd_path;

void shell_command_pwd(Shell_Arguments arguments) {
	printf("path: %s\n", cwd_path);
	printf("inode: %i\n", cwd);
}
