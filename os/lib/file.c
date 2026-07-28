#include "file.h"

#include "fs.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

extern fs_superblock_t superblock;

bool file_create(char* path, size_t cwd, char* cwd_path) {
	char absolute_path[strlen(path) + strlen(cwd_path) + 1];
	fs_path_normalize(path, cwd_path, absolute_path);

	char name[strlen(absolute_path) + 1];
	fs_path_to_name(absolute_path, name);

	uint64_t parent_inode = fs_path_to_parent_inode(path, cwd);

	if (parent_inode == FS_INODE_FAILURE) {
		return false;
	}

	fs_directory_entry_t entry = fs_directory_entry_find(parent_inode, name);

	if (entry.type != FS_DIRECTORY_FAILURE) {
		return true;
	}

	fs_inode_cache_entry_t* icache = fs_inode_create(FS_INODE_TYPE_FILE);

	fs_directory_entry_add(parent_inode, fs_directory_entry_create(icache->index, FS_INODE_TYPE_FILE, name));

	return true;
}

bool file_delete(char* path, size_t cwd, char* cwd_path) {
	char absolute_path[strlen(path) + strlen(cwd_path) + 1];
	fs_path_normalize(path, cwd_path, absolute_path);

	char name[strlen(absolute_path) + 1];
	fs_path_to_name(absolute_path, name);

	uint64_t inode = fs_path_to_inode(absolute_path, cwd);
	uint64_t parent_inode = fs_path_to_parent_inode(absolute_path, cwd);

	if (inode == FS_INODE_FAILURE || parent_inode == FS_INODE_FAILURE) {
		return false;
	}

	fs_directory_entry_t temp_entry = fs_directory_entry_find(parent_inode, name);

	if (temp_entry.type == FS_DIRECTORY_FAILURE) {
		return false;
	}

	fs_directory_entry_del(parent_inode, name);

	fs_inode_cache_entry_t* icache = fs_inode_get(inode);

	fs_inode_destroy(icache);

	return true;
}

file_t* file_open(char* path, size_t cwd, char* cwd_path, char* mode) {
	file_t* file = malloc(sizeof(file_t));

	size_t mode_length = strlen(mode);

	if (mode_length == 0) {
		free(file);
		return NULL;
	}

	file->read = false;
	file->write = false;

	for (size_t i = 0; i < strlen(mode); i++) {
		switch (mode[i]) {
			case FILE_MODE_READ:
				file->read = true;
				break;

			case FILE_MODE_WRITE:
				file->write = true;
				break;

			default:
				free(file);
				return NULL;
		}
	}

	char absolute_path[strlen(path) + strlen(cwd_path) + 1];
	fs_path_normalize(path, cwd_path, absolute_path);

	char name[strlen(absolute_path) + 1];
	fs_path_to_name(absolute_path, name);

	size_t parent_inode = fs_path_to_parent_inode(path, cwd);
	fs_directory_entry_t entry = fs_directory_entry_find(parent_inode, name);

	if (entry.type == FS_DIRECTORY_FAILURE) {
		if (file->write) {
			if (!file_create(path, cwd, cwd_path)) {
				free(file);
				return NULL;
			}

			entry = fs_directory_entry_find(parent_inode, name);
		} else {
			return NULL;
		}
	}

	file->block = malloc(superblock.bytes_per_block);
	file->block_index = FS_DATA_FAILURE;
	file->dirty = false;
	file->cursor = 0;
	file->inode = entry.inode;

	file_seek(file, 0, FILE_SEEK_START);

	return file;
}

void file_close(file_t* file) {
	if (file->dirty) {
		fs_data_write_block(file->block_id, (void*) file->block);
	}

	free(file->block);
	free(file);
}

bool file_seek(file_t* file, int64_t offset, uint8_t seek) {
	fs_inode_cache_entry_t* icache = fs_inode_get(file->inode);
	size_t file_size = icache->inode.size;

	switch (seek) {
		case FILE_SEEK_START:
			if (offset < 0) {
				return false;
			}

			file->cursor = offset;
			break;

		case FILE_SEEK_CURRENT:
			if (file->cursor + offset < 0) {
				return false;
			}

			file->cursor += offset;
			break;

		case FILE_SEEK_END:
			if (file->cursor + icache->inode.size < 0) {
				return false;
			}

			file->cursor += icache->inode.size;
			break;

		default:
			return false;
	}
}

size_t file_tell(file_t* file) {
	return file->cursor;
}

size_t file_read(file_t* file, void* buffer, size_t count) {
	if (file->read == false) {
		return 0;
	}

	fs_inode_cache_entry_t* icache = fs_inode_get(file->inode);

	if (file->cursor > icache->inode.size) {
		return 0;
	}

	size_t bytes_read = 0;
	uint8_t* buf = (uint8_t*) buffer;

	if (file->cursor + count > icache->inode.size) {
		count -= (file->cursor + count) - icache->inode.size;
	}

	for (size_t i = 0; i < count; i++) {
		size_t block_cursor = file->cursor % superblock.bytes_per_block;

		if (block_cursor == 0) {
			if (file->dirty) {
				fs_data_write_block(file->block_id, (void*) file->block);
			}

			file->block_index++;
			file->block_id = fs_inode_get_block(icache, file->block_index);
			fs_data_read_block(file->block_id, file->block);
		}

		buf[i] = file->block[block_cursor];
		file->cursor++;
		bytes_read++;
	}

	return bytes_read;
}

size_t file_write(file_t* file, void* buffer, size_t count) {
	if (file->write == false) {
		return 0;
	}

	fs_inode_cache_entry_t* icache = fs_inode_get(file->inode);

	size_t bytes_written = 0;
	uint8_t* buf = (uint8_t*) buffer;

	if (file->cursor + count > icache->inode.size) {
		size_t extra_size = (file->cursor + count) - icache->inode.size;
		size_t extra_blocks = (extra_size + superblock.bytes_per_block - 1) / superblock.bytes_per_block;

		fs_inode_alloc_blocks(icache, extra_blocks);

		; // 0 out new blocks
	}

	for (size_t i = 0; i < count; i++) {
		size_t block_cursor = file->cursor % superblock.bytes_per_block;

		if (block_cursor == 0) {
			if (file->dirty) {
				fs_data_write_block(file->block_id, (void*) file->block);
			}

			file->block_index++;
			file->block_id = fs_inode_get_block(icache, file->block_index);
			fs_data_read_block(file->block_id, file->block);
		}

		file->block[block_cursor] = buf[i];
		file->dirty = true;

		file->cursor++;
		bytes_written++;

	}

	icache->inode.size += bytes_written;

	return bytes_written;
}
