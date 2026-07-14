#ifndef DIR_H
#define DIR_H

#include "fs.h"

typedef fs_directory_entry_t dirent_t;

typedef struct {
	;
} file_t;

typedef struct {
	dirent_t dirent;
	uint64_t inode;
	uint64_t index;
} dir_t;

dir_t* opendir(char* dirname, uint64_t cwd);
dirent_t* readdir(dir_t* dirp);
void closedir(dir_t* dirp);

#endif
