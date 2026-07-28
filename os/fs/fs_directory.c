#include "fs_directory.h"

#include "fs_data.h"
#include "fs_inode.h"
#include "fs_superblock.h"
#include "fs_system.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

// posix portable filename set - a-z 0-9 . _ -
// switch create and find functions to dirtory inode and name
// make path functions take in absolute path only char* path

extern fs_superblock_t superblock;

size_t directory_entries_per_block;

void fs_directory_init(void) {
	directory_entries_per_block = superblock.bytes_per_block / sizeof(fs_directory_entry_t);
	fs_directory_create_root();
}

void fs_directory_load(bool initialized) {
	if (!initialized) {
		directory_entries_per_block = superblock.bytes_per_block / sizeof(fs_directory_entry_t);
	}
}

void fs_directory_create_root(void) {
	fs_inode_cache_entry_t* icache = fs_inode_create_forced(FS_INODE_TYPE_DIRECTORY, FS_ROOT_DIRECTORY_INODE);

	fs_directory_entry_add(FS_ROOT_DIRECTORY_INODE, fs_directory_entry_create(FS_ROOT_DIRECTORY_INODE, FS_INODE_TYPE_DIRECTORY, FS_DIRECTORY_CURRENT_DIR_NAME));
	fs_directory_entry_add(FS_ROOT_DIRECTORY_INODE, fs_directory_entry_create(FS_ROOT_DIRECTORY_INODE, FS_INODE_TYPE_DIRECTORY, FS_DIRECTORY_PARENT_DIR_NAME));
}

uint8_t fs_directory_create(char* path, uint64_t cwd) {
	uint64_t parent_inode = fs_path_to_parent_inode(path, cwd);

	if (parent_inode == FS_INODE_FAILURE) {
		return false;
	}

	char name[strlen(path) + 1];
	fs_path_to_name(path, name);

	fs_directory_entry_t temp_entry = fs_directory_entry_find(parent_inode, name);

	if (temp_entry.type != FS_DIRECTORY_FAILURE) {
		return FS_DIRECTORY_ENTRY_EXISTS;
	}

	fs_inode_cache_entry_t* icache = fs_inode_create(FS_INODE_TYPE_DIRECTORY);

	fs_directory_entry_add(icache->index, fs_directory_entry_create(icache->index, FS_INODE_TYPE_DIRECTORY, FS_DIRECTORY_CURRENT_DIR_NAME));
	fs_directory_entry_add(icache->index, fs_directory_entry_create(parent_inode, FS_INODE_TYPE_DIRECTORY, FS_DIRECTORY_PARENT_DIR_NAME));

	fs_directory_entry_add(parent_inode, fs_directory_entry_create(icache->index, FS_INODE_TYPE_DIRECTORY, name));

	return true;
}

uint64_t fs_directory_find(char* path, uint64_t cwd) {
	uint64_t parent_inode = fs_path_to_parent_inode(path, cwd);

	if (parent_inode == FS_INODE_FAILURE) {
		return FS_INODE_FAILURE;
	}

	char name[strlen(path) + 1];
	fs_path_to_name(path, name);

	fs_directory_entry_t entry = fs_directory_entry_find(parent_inode, name);

	return entry.inode;
}

fs_directory_entry_t fs_directory_entry_create(uint64_t inode, uint8_t type, char* name) {
	fs_directory_entry_t directory_entry;
	directory_entry.inode = inode;
	directory_entry.type = type;
	directory_entry.name_length = strlen(name);
	memcpy((void*) directory_entry.name, (void*) name, directory_entry.name_length);

	return directory_entry;
}

void fs_directory_entry_get(fs_directory_entry_t* entry, uint64_t directory_inode, uint64_t index) {
	fs_inode_cache_entry_t* icache = fs_inode_get(directory_inode);

	size_t entry_count = ((icache->inode.size / superblock.bytes_per_block) * directory_entries_per_block) + ((icache->inode.size % superblock.bytes_per_block) / sizeof(fs_directory_entry_t));

	if (index >= entry_count) {
		entry->type = FS_DIRECTORY_FAILURE;
		return;
	}

	uint8_t block_buffer[superblock.bytes_per_block];
	fs_directory_entry_t* block = (fs_directory_entry_t*) block_buffer;

	uint64_t block_index = index / directory_entries_per_block;
	uint64_t entry_index = index % directory_entries_per_block;
	uint64_t data_block = fs_inode_get_block(icache, block_index);

	fs_data_read_block(data_block, (void*) block);

	*entry = block[entry_index];
}

fs_directory_entry_t fs_directory_entry_find(uint64_t directory_inode, char* name) {
	fs_inode_cache_entry_t* icache = fs_inode_get(directory_inode);
	fs_directory_entry_t entry;
	char entry_name[FS_DIRECTORY_ENTRY_NAME_SIZE];

	uint8_t block_buffer[superblock.bytes_per_block];
	fs_directory_entry_t* block = (fs_directory_entry_t*) block_buffer;

	size_t entry_count = ((icache->inode.size / superblock.bytes_per_block) * directory_entries_per_block) + ((icache->inode.size % superblock.bytes_per_block) / sizeof(fs_directory_entry_t));
	uint64_t block_index;
	uint64_t entry_index;
	uint64_t data_block;

	for (size_t i = 0; i < entry_count; i++) {
		block_index = i / directory_entries_per_block;
		entry_index = i % directory_entries_per_block;

		if (entry_index == 0) {
			data_block = fs_inode_get_block(icache, block_index);
			fs_data_read_block(data_block, (void*) block_buffer);
		}

		entry = block[entry_index];
		memcpy((void*) entry_name, (void*) entry.name, entry.name_length);
		entry_name[entry.name_length] = '\0';

		if (strcmp(name, entry_name)) {
			return entry;
		}
	}

	entry.type = FS_DIRECTORY_FAILURE;

	return entry;
}

void fs_directory_entry_add(uint64_t directory_inode, fs_directory_entry_t directory_entry) {
	fs_inode_cache_entry_t* icache = fs_inode_get(directory_inode);

	uint8_t block_buffer[superblock.bytes_per_block];
	fs_directory_entry_t* block = (fs_directory_entry_t*) block_buffer;

	size_t entry_count = ((icache->inode.size / superblock.bytes_per_block) * directory_entries_per_block) + ((icache->inode.size % superblock.bytes_per_block) / sizeof(fs_directory_entry_t));
	uint64_t block_index;
	uint64_t entry_index;
	uint64_t data_block;

	if (entry_count == 0) {
		fs_inode_set_size(icache, icache->inode.size + sizeof(fs_directory_entry_t));

		data_block = fs_inode_get_block(icache, 0);

		fs_data_read_block(data_block, (void*) block);
		block[0] = directory_entry;
		fs_data_write_block(data_block, (void*) block);

		return;
	}

	for (size_t i = 0; i < entry_count; i++) {
		block_index = i / directory_entries_per_block;
		entry_index = i % directory_entries_per_block;
		data_block = fs_inode_get_block(icache, block_index);

		fs_data_read_block(data_block, (void*) block);

		if (entry_index == 0) {
			fs_data_read_block(data_block, (void*) block);
		}

		if (block[entry_index].type == FS_DIRECTORY_ENTRY_FREE) {
			block[entry_index] = directory_entry;
			fs_data_write_block(data_block, (void*) block);
			return;
		}
	}

	fs_inode_set_size(icache, icache->inode.size + sizeof(fs_directory_entry_t));

	block_index = entry_count / directory_entries_per_block;
	entry_index = entry_count % directory_entries_per_block;
	data_block = fs_inode_get_block(icache, block_index);

	if (entry_index == 0) {
		fs_inode_set_size(icache, icache->inode.size + (superblock.bytes_per_block - (directory_entries_per_block * sizeof(fs_directory_entry_t))));
		fs_data_read_block(data_block, (void*) block);
	}

	fs_data_read_block(data_block, (void*) block);
	block[entry_index] = directory_entry;
	fs_data_write_block(data_block, (void*) block);
}

bool fs_directory_entry_del(uint64_t directory_inode, char* name) {
	fs_inode_cache_entry_t* icache = fs_inode_get(directory_inode);

	uint8_t block_buffer[superblock.bytes_per_block];
	fs_directory_entry_t* block = (fs_directory_entry_t*) block_buffer;

	size_t entry_count = ((icache->inode.size / superblock.bytes_per_block) * directory_entries_per_block) + ((icache->inode.size % superblock.bytes_per_block) / sizeof(fs_directory_entry_t));
	uint64_t block_index;
	uint64_t entry_index;
	uint64_t data_block;

	for (size_t i = 0; i < entry_count; i++) {
		block_index = i / directory_entries_per_block;
		entry_index = i % directory_entries_per_block;
		data_block = fs_inode_get_block(icache, block_index);

		if (entry_index == 0) {
			fs_data_read_block(data_block, (void*) block);
		}

		if (strncmp(name, block[entry_index].name, block[entry_index].name_length)) {
			block[entry_index].inode = FS_INODE_FAILURE;
			block[entry_index].type = FS_DIRECTORY_ENTRY_FREE;
			block[entry_index].name_length = 0;
			block[entry_index].name[0] = '\0';
			fs_data_write_block(data_block, (void*) block);
			return true;
		}
	}

	return false;
}

void fs_directory_entry_print(fs_directory_entry_t* entry) {
	printf("inode: %i | type: ", entry->inode);

	if (entry->type == FS_INODE_TYPE_DIRECTORY) {
		printf("directory | ", entry->type);
	} else if (entry->type == FS_INODE_TYPE_FILE) {
		printf("file | ", entry->type);
	} else if (entry->type == FS_DIRECTORY_ENTRY_FREE) {
		printf("empty | ", entry->type);
	} else if (entry->type == FS_DIRECTORY_FAILURE) {
		printf("DIRECTORY FAILURE | ", entry->type);
	} else {
		printf("unknown (%i) | ", entry->type);
	}

	printf("name length: %i | name: ", entry->name_length);

	for (uint64_t i = 0; i < entry->name_length; i++) {
		printf("%c", entry->name[i]);
	}

	printf("\n");
}

void fs_path_normalize(char* path, char* cwd_path, char* buffer) {
	size_t buffer_index = strlen(cwd_path);
	strcpy(buffer, cwd_path);

	size_t path_length = strlen(path);
	size_t path_index = 0;

	if (path_length == 0) {
		return;
	}

	char path_segment[path_length + 1];
	size_t path_segment_index = 0;

	bool in_root_directory = false;

	if (strcmp(buffer, "/")) {
		in_root_directory = true;
	}

	if (path[0] == '/') {
		buffer[0] = '/';
		buffer[1] = '\0';
		path_index = 1;
		buffer_index = 1;
		in_root_directory = true;
	}

	while (1) {
		if (path[path_index] == FS_PATH_DELIMITER || path[path_index] == '\0') {
			path_segment[path_segment_index] = '\0';

			if (strcmp(path_segment, FS_DIRECTORY_PARENT_DIR_NAME)) {
				if (in_root_directory) {
					path_segment_index = 0;
					path_index++;

					if (path[path_index] == '\0') {
						return;
					}

					continue;
				}

				fs_path_to_parent_path(buffer, buffer, 0);
				buffer_index = strlen(buffer);

				if (buffer_index == 1) {
					in_root_directory = true;
				}

			} else if (path_segment_index != 0 && !strcmp(path_segment, FS_DIRECTORY_CURRENT_DIR_NAME)) {
				if (!in_root_directory) {
					buffer[buffer_index] = '/';
					buffer_index++;
				} else {
					in_root_directory = false;
				}

				memcpy((void*) &buffer[buffer_index], (void*) path_segment, path_segment_index + 1);
				buffer_index = strlen(buffer);
			}

			if (path[path_index] == '\0') {
				return;
			}

			path_segment_index = 0;
			path_index++;
			continue;
		}

		path_segment[path_segment_index] = path[path_index];
		path_segment_index++;
		path_index++;
	}
}

size_t fs_path_find_last_delimiter(char* path) {
	size_t path_length = strlen(path);
	size_t path_index = path_length - 1;

	if (path[path_index] == FS_PATH_DELIMITER) {
		path_index--;
		path_length--;
	}

	for (size_t i = 0; i < path_length; i++) {
		if (path[path_index] == FS_PATH_DELIMITER) {
			return path_index;
		}

		path_index--;
	}

	return FS_DIRECTORY_FAILURE;
}

size_t fs_path_delimiter_count(char* path) {
	size_t delimiter_count = 0;

	for (size_t i = 0; i < strlen(path); i++) {
		if (path[i] == FS_PATH_DELIMITER) {
			delimiter_count++;
		}
	}

	return delimiter_count;
}

void fs_path_to_name(char* path, char* buffer) {
	size_t name_start = fs_path_find_last_delimiter(path);

	if (name_start == FS_DIRECTORY_FAILURE) {
		size_t path_length = strlen(path);
		memcpy((void*) buffer, (void*) path, path_length);
		buffer[path_length] = '\0';
		return;
	}

	name_start++;
	size_t path_length = strlen(path);
	size_t name_length = path_length - name_start;

	if (path[path_length - 1] == FS_PATH_DELIMITER) {
		name_length--;
	}

	memcpy((void*) buffer, (void*) &path[name_start], name_length);
	buffer[name_length] = '\0';
}

void fs_path_to_parent_path(char* path, char* buffer, size_t index) {
	size_t path_end = FS_DIRECTORY_FAILURE;
	strcpy(buffer, path);

	for (size_t i = 0; i < index + 1; i++) {
		path_end = fs_path_find_last_delimiter(buffer);

		if (path_end == FS_DIRECTORY_FAILURE) {
			buffer[0] = '\0';
			return;
		}

		memcpy((void*) buffer, (void*) path, path_end);
		buffer[path_end] = '\0';
	}

	if (path_end == 0) {
		buffer[0] = '/';
		buffer[1] = '\0';
	}
}

uint64_t fs_path_to_inode(char* path, uint64_t cwd) {
	if (strlen(path) == 0) {
		return cwd;
	}

	uint64_t current_inode = cwd;
	fs_directory_entry_t temp_entry;

	size_t path_index = 0;

	char path_segment[strlen(path) + 1];
	size_t path_segment_index = 0;

	if (path[0] == '/') {
		current_inode = FS_ROOT_DIRECTORY_INODE;
		path_index++;
	}

	while (1) {
		if (path[path_index] == FS_PATH_DELIMITER || path[path_index] == '\0') {
			path_segment[path_segment_index] = '\0';

			if (path_segment_index == 0) {
				if (path[path_index - 1] == '/') {
					return current_inode;
				}

				path_segment_index = 0;
				path_index++;
				continue;
			}

			temp_entry = fs_directory_entry_find(current_inode, path_segment);

			if (temp_entry.type == FS_DIRECTORY_FAILURE) {
				return FS_INODE_FAILURE;
			}

			current_inode = temp_entry.inode;

			if (path[path_index] == '\0') {
				break;
			}

			path_segment_index = 0;
			path_index++;
			continue;
		}

		path_segment[path_segment_index] = path[path_index];
		path_segment_index++;
		path_index++;
	}

	return current_inode;
}

uint64_t fs_path_to_parent_inode(char* path, uint64_t cwd) {
	char parent_path[strlen(path) + 1];
	fs_path_to_parent_path(path, parent_path, 0);

	uint64_t inode = fs_path_to_inode(parent_path, cwd);

	if (inode == FS_DIRECTORY_FAILURE) {
		return FS_INODE_FAILURE;
	}

	return inode;
}
