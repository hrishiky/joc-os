#include "stdio.h"
#include "fs_inode.h"
#include "fs_data.h"

#include FILE_BLOCK_COUNT 10

bool test_file_1(bool print) {
	fs_inode_cache_entry_t* icache = fs_inode_create(FS_INODE_TYPE_FILE, "fun");

	uint64_t free_blocks[FILE_BLOCK_COUNT];
	uint64_t size = FILE_BLOCK_COUNT * superblock.bytes_per_block;

	uint64_t buf[size / 8];
	uint64_t readbuf[size / 8];

	for (uint64_t i = 0; i < FILE_BLOCK_COUNT; i++) {
		free_blocks[i] = fs_data_alloc_block();

		if (free_blocks[i] == FS_DATA_FAILURE) {
			if (print) {
				printf("couldnt allocate file blocks\n");
			}

			return false;
		}
	}

	fs_inode_add_blocks(icache, free_blocks, FILE_BLOCK_COUNT);
	fs_inode_set_size(icache, size);

	for (uint64_t i = 0; i < size / 8; i++) {
		buf[i] = i;
	}

	for (uint64_t i = 0; i < FILE_BLOCK_COUNT; i++) {
		fs_data_write_block(free_blocks[i], (void*) ((uint8_t*) buf + (i * superblock.bytes_per_block)));
	}

	for (uint64_t i = 0; i < FILE_BLOCK_COUNT; i++) {
		fs_data_read_block(free_blocks[i], (void*) ((uint8_t*) readbuf + (i * superblock.bytes_per_block)));
	}

	for (uint64_t i = 0; i < size / 8; i++) {
		if (buf[i] != readbuf[i]) {
			if (print) {
				printf("test failed\n");
			}

			return false;
		}
	}

	// delete file

	if (print) {
		printf("test passed\n");
	}

	return true;
}
