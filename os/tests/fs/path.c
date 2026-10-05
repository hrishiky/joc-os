#include "stdio.h"
#include "stdbool.h"

#include "fs.h"

void tests_fs_path_1(bool print, char* path) {
	// char* path = "./bar/fun/foo";
	// bool print = true;

	uint64_t delim = fs_path_find_last_delimiter(path);

	if (print) {
		printf("%s\n", path);
		for (uint64_t i = 0; i < delim; i++) {
			printf(" ");
		}

		printf("^\n");

		printf("path[%i]: %c\n", delim, path[delim]);

	}
}
