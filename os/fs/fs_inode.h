#ifndef FS_INODE_H
#define FS_INODE_H

#include "stdint.h"
#include "stdbool.h"

#define FS_INODE_CACHE_SIZE 128

#define FS_INODE_NAME_MAX_LENGTH 16
#define FS_INODE_TABLE_POINTER_COUNT 8
#define FS_INODE_INDIRECT_TABLE_POINTER_COUNT 64

#define FS_INODE_TYPE_FILE 0
#define FS_INODE_TYPE_DIRECTORY 1

#define FS_INODE_TABLE_CUTOFF FS_INODE_TABLE_POINTER_COUNT + 1
#define FS_INODE_FIRST_INDIRECT_TABLE_CUTOFF FS_INODE_TABLE_CUTOFF + FS_INODE_INDIRECT_TABLE_POINTER_COUNT
#define FS_INODE_SECOND_INDIRECT_TABLE_CUTOFF FS_INODE_FIRST_INDIRECT_TABLE_CUTOFF + pow(FS_INODE_INDIRECT_TABLE_POINTER_COUNT, 2)
#define FS_INODE_THIRD_INDIRECT_TABLE_CUTOFF FS_INODE_SECOND_INDIRECT_TABLE_CUTOFF + pow(FS_INODE_INDIRECT_TABLE_POINTER_COUNT, 3)

#define FS_INODE_FAILURE UINT64_MAX


typedef struct {
	uint64_t blocks[FS_INODE_INDIRECT_TABLE_POINTER_COUNT];
} fs_data_indirect_table_t;

typedef struct {
	uint8_t type;
	size_t size;
	uint64_t block_count;
	uint64_t blocks[FS_INODE_TABLE_POINTER_COUNT];
	uint64_t first_indirect_table;
	uint64_t second_indirect_table;
	uint64_t third_indirect_table;
} __attribute__((packed)) fs_inode_t;

typedef struct {
	fs_inode_t inode;
	uint64_t index;
	uint8_t reference;
	bool dirty;
	bool valid;
} __attribute__((packed)) fs_inode_cache_entry_t;


void fs_inode_init(void);
void fs_inode_load(bool initialized);

void fs_inode_cache_entry_flush(uint64_t index);
int64_t fs_inode_cache_entry_evict(void);
uint64_t fs_inode_cache_evict(void);
uint64_t fs_inode_cache_add(fs_inode_t inode, uint64_t index);
fs_inode_cache_entry_t* fs_inode_cache_search(uint64_t index);

fs_inode_cache_entry_t* fs_inode_create_forced(uint8_t type, uint64_t index);
fs_inode_cache_entry_t* fs_inode_create(uint8_t type);
void fs_inode_destroy(fs_inode_cache_entry_t* icache);

fs_inode_t fs_inode_table_get(uint64_t inode_table_block, uint64_t table_index);
fs_inode_cache_entry_t* fs_inode_get(uint64_t index);

bool fs_inode_set_size(fs_inode_cache_entry_t* icache, size_t size);
uint64_t fs_inode_get_block(fs_inode_cache_entry_t* icache, uint64_t table_index);
void fs_inode_set_block(fs_inode_cache_entry_t* icache, uint64_t table_index, uint64_t data_index);

bool fs_inode_alloc_blocks(fs_inode_cache_entry_t* icache, size_t count);
bool fs_inode_free_blocks(fs_inode_cache_entry_t* icache, size_t count);

#endif
