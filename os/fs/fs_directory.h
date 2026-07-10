#ifndef FS_DIRECTORY_H
#define FS_DIRECTORY_H

#include "stdint.h"
#include "stdbool.h"


#define FS_DIRECTORY_CURRENT_DIR_NAME "."
#define FS_DIRECTORY_PARENT_DIR_NAME ".."


typedef struct {
	uint16_t entry_size;
	uint16_t padding_size;
	uint64_t inode_index;
	uint8_t name_length;
	char name[];
} fs_directory_entry_t;

typedef struct {
	fs_directory_entry_t* entry;
	uint64_t entry_index;
	uint64_t block_index;
	uint64_t byte_offset;
	uint64_t blocks_read;
	uint64_t bytes_read;
} fs_directory_entry_wrapper_t;

typedef struct {
	uint8_t* block;
	uint64_t index;
} fs_directory_block_cache_t;


void fs_directory_init(void);
void fs_directory_load(void);

void fs_directory_cache_init(void);
void fs_directory_cache_exit(void);
void fs_directory_cache_load_block(uint64_t block_index);

void fs_directory_create_root(void);
bool fs_directory_create(char* path, uint64_t pwd_inode);

void fs_directory_entry_create(fs_directory_entry_t* directory_entry, uint64_t inode_index, char* name);
fs_directory_entry_wrapper_t fs_directory_entry_wrapper_create(uint64_t directory_inode_index);

void fs_directory_entry_traverse(uint64_t directory_inode_index, fs_directory_entry_wrapper_t* state);

fs_directory_entry_wrapper_t fs_directory_entry_find_inode_index(uint64_t directory_inode_index, uint64_t entry_inode_index);
fs_directory_entry_wrapper_t fs_directory_entry_find_name(uint64_t directory_inode_index, char* name);

void fs_directory_entry_add(uint64_t directory_inode_index, fs_directory_entry_t* directory_entry);
bool fs_directory_entry_remove(uint64_t directory_inode_index, uint64_t directory_entry_index);

void fs_path_split(char* path, char* name_buffer, char* path_buffer);
uint64_t fs_path_to_inode(char* path, uint64_t pwd);

#endif
