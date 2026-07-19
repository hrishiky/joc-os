#ifndef FILE_H
#define FILE_H

#include "fs.h"

#define FILE_MODE_READ 'r'
#define FILE_MODE_WRITE 'w'

#define FILE_SEEK_START 0
#define FILE_SEEK_CURRENT 1
#define FILE_SEEK_END 2

typedef struct {
	uint8_t* block;
	size_t block_id;
	size_t block_index;
	bool dirty;

	size_t cursor;
	size_t inode;

	bool read;
	bool write;
} file_t;

bool file_create(char* path, size_t cwd, char* cwd_path);
bool file_delete(char* path, size_t cwd, char* cwd_path);

file_t* file_open(char* path, size_t cwd, char* cwd_path, char* mode);
void file_close(file_t* file);

bool file_seek(file_t* file, int64_t offset, uint8_t seek);
size_t file_tell(file_t* file);

size_t file_read(file_t* file, void* buffer, size_t count);
size_t file_write(file_t* file, void* buffer, size_t count);

#endif
