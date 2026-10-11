#ifndef DIR_H
#define DIR_H

#include "fs.h"
#include "stdbool.h"

typedef fs_directory_entry_t dirent_t;

typedef struct {
	dirent_t dirent;
	size_t inode;
	size_t index;
} dir_t;

dir_t* dir_open(char* path, size_t cwd);
dirent_t* dir_read(dir_t* dir);
void dir_close(dir_t* dir);

bool dir_make(char* path, size_t cwd, char* cwd_path, bool forced);
bool dir_remove(char* path, size_t cwd);

bool dir_change(char* path, size_t* cwd, char* cwd_path);

#endif

