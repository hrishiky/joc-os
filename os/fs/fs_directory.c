#include "fs_directory.h"

#include "fs_data.h"
#include "fs_inode.h"
#include "fs_superblock.h"
#include "fs_system.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

#include "stdio.h"

fs_directory_block_cache_t block_cache;

extern fs_superblock_t superblock;

void fs_directory_init(void) {
	fs_directory_create_root();
	printf("created root directory\n");
}

void fs_directory_load(void) {
	fs_directory_cache_init();
	printf("block cache loaded\n");
}

void fs_directory_cache_init(void) {
	block_cache.block = malloc(superblock.bytes_per_block);
}

void fs_directory_cache_exit(void) {
	free(block_cache.block);
}

void fs_directory_cache_load_block(uint64_t block_index) {
	fs_data_read_block(block_index, block_cache.block);
	block_cache.index = block_index;
}

void fs_directory_create_root(void) {
	fs_inode_cache_entry_t* icache = fs_inode_create_forced(FS_INODE_TYPE_DIRECTORY, FS_ROOT_DIRECTORY_INODE);

	fs_directory_entry_t current_directory;
	fs_directory_entry_create(&current_directory, icache->index, FS_DIRECTORY_CURRENT_DIR_NAME);
	fs_directory_entry_add(icache->index, &current_directory);

	fs_directory_entry_t parent_directory;
	fs_directory_entry_create(&parent_directory, icache->index, FS_DIRECTORY_PARENT_DIR_NAME);
	fs_directory_entry_add(icache->index, &parent_directory);
}

bool fs_directory_create(char* path, uint64_t pwd_inode) {
	char name[strlen(path)];
	char split_path[strlen(path)];

	fs_path_split(path, name, split_path);

	uint64_t parent_inode = fs_path_to_inode(split_path, pwd_inode);

	if (parent_inode == FS_INODE_FAILURE) {
		return false;
	}

	fs_inode_cache_entry_t* icache = fs_inode_create(FS_INODE_TYPE_DIRECTORY);

	fs_directory_entry_t current_directory;
	fs_directory_entry_create(&current_directory, icache->index, FS_DIRECTORY_CURRENT_DIR_NAME);
	fs_directory_entry_add(icache->index, &current_directory);

	fs_directory_entry_t parent_directory;
	fs_directory_entry_create(&parent_directory, parent_inode, FS_DIRECTORY_PARENT_DIR_NAME);
	fs_directory_entry_add(icache->index, &current_directory);

	fs_directory_entry_t directory_entry;
	fs_directory_entry_create(&directory_entry, icache->index, name);
	fs_directory_entry_add(parent_inode, &directory_entry);

	return true;
}

void fs_directory_entry_create(fs_directory_entry_t* directory_entry, uint64_t inode_index, char* name) {
	size_t name_length = strlen(name);

	directory_entry->entry_size = sizeof(uint16_t) + sizeof(uint64_t) + sizeof(uint8_t) + name_length;
	directory_entry->padding_size = 0;
	directory_entry->inode_index = inode_index;
	directory_entry->name_length = name_length;
	memcpy((void*) name, (void*) directory_entry->name, name_length);
}

fs_directory_entry_wrapper_t fs_directory_entry_wrapper_create(uint64_t directory_inode_index) {
	fs_directory_entry_wrapper_t entry_wrapper;
	fs_inode_cache_entry_t* icache = fs_inode_get(directory_inode_index);

	entry_wrapper.entry = NULL;
	entry_wrapper.entry_index = 0;
	entry_wrapper.block_index = fs_inode_get_block(icache, 1);
	entry_wrapper.byte_offset = 0;
	entry_wrapper.blocks_read = 0;
	entry_wrapper.bytes_read = 0;
}

void fs_directory_entry_traverse(uint64_t directory_inode_index, fs_directory_entry_wrapper_t* state) {
	fs_inode_cache_entry_t* icache = fs_inode_get(directory_inode_index);
	fs_directory_entry_t* new_entry;
	bool new_block = false;

	if (state->entry->entry_size + state->bytes_read >= icache->inode.size) {
		state->entry = NULL;
		return;
	}

	if (state->entry->entry_size + state->byte_offset >= superblock.bytes_per_block) {
		uint64_t next_block = fs_inode_get_block(icache, state->blocks_read + 1);
		fs_directory_cache_load_block(next_block);

		state->block_index = next_block;
		state->blocks_read++;
		state->byte_offset = 0;

		new_block = true;
	}

	if (state->block_index != block_cache.index) {
		uint64_t block = fs_inode_get_block(icache, state->blocks_read);
		fs_directory_cache_load_block(block);
	}

	if (!new_block) {
		state->byte_offset += state->entry->entry_size;
	}

	new_entry = (fs_directory_entry_t*) &block_cache.block[state->byte_offset];

	memcpy((void*) state->entry, (void*) new_entry, new_entry->entry_size);
	state->bytes_read += state->entry->entry_size;

	state->entry_index++;
}

fs_directory_entry_wrapper_t fs_directory_entry_find_inode_index(uint64_t directory_inode_index, uint64_t entry_inode_index) {
	fs_directory_entry_wrapper_t entry_wrapper = fs_directory_entry_wrapper_create(directory_inode_index);
	fs_directory_entry_traverse(directory_inode_index, &entry_wrapper);

	while (entry_wrapper.entry != NULL) {
		if (entry_wrapper.entry->inode_index == entry_inode_index) {
			return entry_wrapper;
		}

		fs_directory_entry_traverse(directory_inode_index, &entry_wrapper);
	}

	entry_wrapper.entry = NULL;

	return entry_wrapper;
}

fs_directory_entry_wrapper_t fs_directory_entry_find_name(uint64_t directory_inode_index, char* name) {
	fs_directory_entry_wrapper_t entry_wrapper = fs_directory_entry_wrapper_create(directory_inode_index);
	fs_directory_entry_traverse(directory_inode_index, &entry_wrapper);

	while (entry_wrapper.entry != NULL) {
		if (strcmp(entry_wrapper.entry->name, name)) {
			return entry_wrapper;
		}

		fs_directory_entry_traverse(directory_inode_index, &entry_wrapper);
	}

	entry_wrapper.entry = NULL;

	return entry_wrapper;
}

void fs_directory_entry_add(uint64_t directory_inode_index, fs_directory_entry_t* directory_entry) { // hanging here
	fs_inode_cache_entry_t* icache = fs_inode_get(directory_inode_index);
	fs_directory_entry_wrapper_t entry_wrapper = fs_directory_entry_wrapper_create(directory_inode_index);
	fs_directory_entry_traverse(directory_inode_index, &entry_wrapper);

	while (entry_wrapper.entry != NULL) {
		if (entry_wrapper.entry->padding_size >= directory_entry->entry_size) {
			memcpy((void*) &block_cache.block[entry_wrapper.byte_offset + entry_wrapper.entry->entry_size], (void*) directory_entry, directory_entry->entry_size);
			fs_data_write_block(entry_wrapper.block_index, (void*) block_cache.block);

			return;
		}

		fs_directory_entry_traverse(directory_inode_index, &entry_wrapper);
	}

	if (entry_wrapper.byte_offset + directory_entry->entry_size > superblock.bytes_per_block) {
		uint64_t padding = superblock.bytes_per_block - (entry_wrapper.byte_offset + entry_wrapper.entry->entry_size);
		entry_wrapper.entry->padding_size += padding;
		entry_wrapper.entry->entry_size += padding;
		memcpy((void*) &block_cache.block[entry_wrapper.byte_offset], (void*) entry_wrapper.entry, entry_wrapper.entry->entry_size);
		fs_data_write_block(entry_wrapper.block_index, (void*) block_cache.block);

		fs_inode_set_size(icache, icache->inode.size + padding + directory_entry->entry_size);
		uint64_t new_block = fs_inode_get_block(icache, entry_wrapper.blocks_read + 1);
		fs_directory_cache_load_block(new_block);

		memcpy((void*) &block_cache.block[0], (void*) directory_entry, directory_entry->entry_size);
		fs_data_write_block(new_block, (void*) block_cache.block);
	} else {
		fs_inode_set_size(icache, icache->inode.size + directory_entry->entry_size);
		memcpy((void*) &block_cache.block[entry_wrapper.byte_offset + entry_wrapper.entry->entry_size], (void*) directory_entry, directory_entry->entry_size);
		fs_data_write_block(entry_wrapper.block_index, (void*) block_cache.block);
	}
}

bool fs_directory_entry_remove(uint64_t directory_inode_index, uint64_t directory_entry_index) {
	fs_inode_cache_entry_t* icache = fs_inode_get(directory_inode_index);
	fs_directory_entry_wrapper_t entry_wrapper = fs_directory_entry_wrapper_create(directory_inode_index);
	fs_directory_entry_wrapper_t last_entry_wrapper;

	for (uint64_t i = 0; i < directory_entry_index - 1; i++) {
		fs_directory_entry_traverse(directory_inode_index, &entry_wrapper);

		if (entry_wrapper.entry == NULL) {
			return false;
		}
	}

	last_entry_wrapper = entry_wrapper;
	fs_directory_entry_traverse(directory_inode_index, &entry_wrapper);

	if (entry_wrapper.entry == NULL) {
		return false;
	}

	if (entry_wrapper.bytes_read + entry_wrapper.entry->entry_size >= icache->inode.size) {
		if (entry_wrapper.byte_offset == 0) {
			; // delete entry, free block, handle size
		} else {
			; // delete entry, handle size
		}
	} else {
		if (last_entry_wrapper.block_index != entry_wrapper.block_index) {
			; //move next entry to start of block and add padding
		} else {
			last_entry_wrapper.entry->padding_size += entry_wrapper.entry->entry_size;
			memset((void*) &block_cache.block[entry_wrapper.byte_offset], 0, entry_wrapper.entry->entry_size);
		}
	}

	return true;
}

void fs_path_split(char* path, char* name_buffer, char* path_buffer) {
	size_t path_length = strlen(path);
	size_t last_delimiter_offset;

	for (size_t i = 0; i < path_length; i++) {
		if (path[i] == FS_PATH_DELIMITER) {
			last_delimiter_offset = i;
		}
	}

	for (size_t i = 0; i < last_delimiter_offset; i++) {
		path_buffer[i] = path[i];
	}

	path_buffer[last_delimiter_offset] = '\0';

	for (size_t i = last_delimiter_offset + 1; i < path_length; i++) {
		name_buffer[i] = path[i];
	}

	name_buffer[path_length - last_delimiter_offset] = '\0';
}

uint64_t fs_path_to_inode(char* path, uint64_t pwd_inode) {
	fs_directory_entry_wrapper_t temp_entry_wrapper;

	size_t path_index;
	char* path_segment = malloc(strlen(path));
	size_t path_segment_index;

	uint64_t current_inode;

	switch (path[0]) {
		case '.':
			if (path[1] == '.') {
				current_inode = (fs_directory_entry_find_name(pwd_inode, "..")).entry->inode_index;
				path_index = 3;
			} else {
				current_inode = pwd_inode;
				path_index = 2;
			}
		case '/':
			current_inode = FS_ROOT_DIRECTORY_INODE;
			path_index = 1;
		default:
			current_inode = pwd_inode;
			path_index = 0;
	}

	while (path[path_index] != '\0') {
		path_segment_index = 0;

		while (path[path_index] != FS_PATH_DELIMITER) {
			if (path[path_index] == '\0') {
				break;
			}

			path_segment[path_segment_index] = path[path_index];
			path_segment_index++;
			path_index++;
		}

		path_segment[path_segment_index + 1] = '\0';

		temp_entry_wrapper = fs_directory_entry_find_name(current_inode, path_segment);

		if (temp_entry_wrapper.entry == NULL) {
			return FS_INODE_FAILURE;
		}

		current_inode = temp_entry_wrapper.entry->inode_index;
	}

	free(path_segment);

	return current_inode;
}
