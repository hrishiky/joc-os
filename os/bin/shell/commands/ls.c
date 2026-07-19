#include "shell.h"

#include "dir.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

extern size_t cwd;

void shell_command_ls(Shell_Arguments arguments) {
	if (arguments.argc == 1) {
		arguments.argv[1][0] = '\0';
		arguments.argc++;
	}

	for (size_t i = 1; i < arguments.argc; i++) {
		dir_t* dir = dir_open(arguments.argv[i], cwd);
		dirent_t* dirent;

		if (dir != NULL) {
			if (arguments.argc > 2) {
				printf("%s:  ", arguments.argv[i]);
			}

			while ((dirent = dir_read(dir)) != NULL) {
				if (dirent->type == FS_DIRECTORY_ENTRY_FREE) {
					continue;
				}

				for (uint8_t i = 0; i < dirent->name_length; i++) {
					printf("%c", dirent->name[i]);
				}

				if (dirent->type == FS_INODE_TYPE_DIRECTORY) {
					printf("/");
				}

				printf("  ");
			}

			printf("\n");
		} else {
			printf("invalid directory '%s'\n", arguments.argv[1]);
		}

		dir_close(dir);
	}
}
