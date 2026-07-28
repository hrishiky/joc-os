#include "shell.h"

#include "file.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

extern size_t cwd;
extern char* cwd_path;

#define BUFFER_SIZE 511

void shell_command_out(Shell_Arguments arguments) {
	if (arguments.argc < 2) {
		printf("usage: %s [FILE PATH] ...", arguments.argv[0]);
		return;
	}

	uint8_t* buffer = malloc(BUFFER_SIZE + 1);
	void* buf = (void*) buffer;

	buffer[BUFFER_SIZE + 1] = '\0';

	for (size_t i = 1; i < arguments.argc; i++) {
		file_t* file = file_open(arguments.argv[i], cwd, cwd_path, "r");

		if (file == NULL) {
			printf("couldn't open file '%s'\n", arguments.argv[i]);
			continue;
		}

		while (file_read(file, buf, BUFFER_SIZE) != 0) {
			printf("%s", buffer);
		}

		printf("\n");
	}

	free(buffer);
}
