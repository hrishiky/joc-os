#include "shell.h"
#include "stdio.h"
#include "stdint.h"

#include "fs.h"

void shell_command_temp(Shell_Arguments arguments) {
	fs_directory_create("/foo", FS_ROOT_DIRECTORY_INODE);
}
