#include "stdio.h"
#include "fs_inode.h"
#include "fs_data.h"

bool test_file_1(bool print, size_t file_block_count) {
	// bool print = true;
	// size_t file_block_count = 10;

	fs_inode_cache_entry_t* icache = fs_inode_create(FS_INODE_TYPE_FILE);

	if (print) {
		printf("file inode: %i\n", icache->index);
	}

	uint64_t size = file_block_count * superblock.bytes_per_block;

	uint64_t buf[size / 8];
	uint64_t readbuf[size / 8];

	if (!fs_inode_alloc_blocks(icache, file_block_count)) {
		if (print) {
			printf("failed allocation\n");
		}
	}

	fs_inode_set_size(icache, size);

	for (uint64_t i = 0; i < file_block_count; i++) {
		printf("file block %i: %i ", i, fs_inode_get_block(icache, i));

		if (fs_inode_get_block(icache, i) == FS_DATA_FAILURE) {
			printf("(fs_data_failure)");
		}

		printf("\n");
	}

	for (uint64_t i = 0; i < size / 8; i++) {
		buf[i] = i;
	}

	for (uint64_t i = 0; i < file_block_count; i++) {
		fs_data_write_block(fs_inode_get_block(icache, i), (void*) ((uint8_t*) buf + (i * superblock.bytes_per_block)));
	}

	for (uint64_t i = 0; i < file_block_count; i++) {
		fs_data_read_block(fs_inode_get_block(icache, i), (void*) ((uint8_t*) readbuf + (i * superblock.bytes_per_block)));
	}

	for (uint64_t i = 0; i < size / 8; i++) {
		if (buf[i] != readbuf[i]) {
			if (print) {
				printf("\ntest failed\n");
			}

			return false;
			// return;
		}
	}

	// delete file

	if (print) {
		printf("\ntest passed\n");
	}

	return true;
}
