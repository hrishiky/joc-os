#ifndef FS_DATA_H
#define FS_DATA_H

#include "stdbool.h"
#include "stdint.h"

#define FS_DATA_BITMAP_CACHE_SIZE 64

#define FS_DATA_FAILURE UINT64_MAX

typedef struct {
 	uint64_t block[FS_DATA_BITMAP_CACHE_SIZE];
	uint64_t index;
	bool dirty;
} fs_data_bitmap_cache_t;

typedef struct {
	uint16_t entry_size;
	uint16_t padding_size;
	uint64_t inode_index;
	uint8_t filename_length;
	char filename[];
} fs_data_directory_entry_t;

void fs_data_init(void);
void fs_data_load(bool initialized);

uint64_t fs_data_alloc_block(void);
void fs_data_free_block(uint64_t index);

void fs_data_read_block(uint64_t block, void* buffer);
void fs_data_write_block(uint64_t block, void* buffer);

#endif
