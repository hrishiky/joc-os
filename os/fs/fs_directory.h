#ifndef FS_DIRECTORY_H
#define FS_DIRECTORY_H

#include "stdint.h"
#include "stdbool.h"


#define FS_DIRECTORY_ENTRY_NAME_SIZE 54

#define FS_DIRECTORY_FAILURE UINT8_MAX
#define FS_DIRECTORY_ENTRY_FREE (UINT8_MAX - 1)

#define FS_DIRECTORY_CURRENT_DIR_NAME "."
#define FS_DIRECTORY_PARENT_DIR_NAME ".."


typedef struct {
	uint64_t inode;
	uint8_t type;
	uint8_t name_length;
	char name[FS_DIRECTORY_ENTRY_NAME_SIZE];
} __attribute__((packed)) fs_directory_entry_t;


void fs_directory_init(void);
void fs_directory_init(void);
void fs_directory_load(bool initialized);

void fs_directory_create_root(void);
bool fs_directory_create(char* path, uint64_t cwd);
uint64_t fs_directory_find(char* path, uint64_t cwd);

fs_directory_entry_t fs_directory_entry_create(uint64_t inode, uint8_t type, char* name);
fs_directory_entry_t fs_directory_entry_find(uint64_t directory_inode, char* name);
void fs_directory_entry_get(fs_directory_entry_t* entry, uint64_t directory_inode, uint64_t index);
void fs_directory_entry_add(uint64_t directory_inode, fs_directory_entry_t directory_entry);
bool fs_directory_entry_del(uint64_t directory_inode, char* name);
void fs_directory_entry_print(fs_directory_entry_t* entry);

size_t fs_path_find_last_delimiter(char* path);
void fs_path_to_name(char* path, char* buffer);
void fs_path_to_parent_path(char* path, char* buffer);

uint64_t fs_path_to_inode(char* path, uint64_t cwd);
uint64_t fs_path_to_parent_inode(char* path, uint64_t cwd);

#endif
