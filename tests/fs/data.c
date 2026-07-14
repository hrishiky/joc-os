#include "stdio.h"
#include "fs_inode.h"
#include "fs_data.h"

void test_file_1(size_t file1_size, size_t file2_size) {
	// size_t file1_size = 10;
	// size_t file2_size = 5;

	uint64_t file1_blocks[file1_size];
	uint64_t file2_blocks[file2_size];

	printf("file 1 (%i blocks):\n", file1_size);
	for (uint64_t i = 0; i < file1_size; i++) {
		file1_blocks[i] = fs_data_alloc_block();
		printf("%i ", file1_blocks[i]);
	}

	printf("\n\n");

	printf("file 2 (%i blocks):\n", file2_size);
	for (uint64_t i = 0; i < file2_size; i++) {
		file2_blocks[i] = fs_data_alloc_block();
		printf("%i ", file2_blocks[i]);
	}

	printf("\n\n");

	printf("freeing blocks: ");
	for (uint64_t i = 0; i < file1_size; i++) {
		printf("%i ", file1_blocks[i]);
		fs_data_free_block(file1_blocks[i]);
	}

	for (uint64_t i = 0; i < file2_size; i++) {
		printf("%i ", file2_blocks[i]);
		fs_data_free_block(file2_blocks[i]);
	}

	printf("\n\n");

	printf("file 1 (%i blocks):\n", file1_size);
	for (uint64_t i = 0; i < file1_size; i++) {
		file1_blocks[i] = fs_data_alloc_block();
		printf("%i ", file1_blocks[i]);
	}

	printf("\n\n");

	printf("file 2 (%i blocks):\n", file2_size);
	for (uint64_t i = 0; i < file2_size; i++) {
		file2_blocks[i] = fs_data_alloc_block();
		printf("%i ", file2_blocks[i]);
	}
}
