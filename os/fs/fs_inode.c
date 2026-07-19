#include "fs_inode.h"

#include "fs_system.h"
#include "fs_superblock.h"
#include "fs_data.h"
#include "fs_helper.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "ata.h"
#include "bitmap.h"
#include "math.h"

// convert all to inode input instead of icache

extern fs_superblock_t superblock;

bitmap_disk_t inode_bitmap;
fs_inode_cache_entry_t inode_cache[FS_INODE_CACHE_SIZE];
uint64_t inode_cache_clock_hand;

void fs_inode_init(void) {
	bitmap_disk_init(
		&inode_bitmap,
		fs_block_to_sector(&superblock, superblock.inode_bitmap_start),
		superblock.block_sector_count,
		superblock.inode_max_count
	);

	bitmap_disk_load(
		&inode_bitmap,
		true,
		fs_block_to_sector(&superblock, superblock.inode_bitmap_start),
		superblock.block_sector_count,
		superblock.inode_max_count
	);

	bitmap_disk_set(&inode_bitmap, FS_ROOT_DIRECTORY_INODE);
	bitmap_disk_flush(&inode_bitmap);
}

void fs_inode_load(bool initialized) {
	bitmap_disk_load(
		&inode_bitmap,
		initialized,
		fs_block_to_sector(&superblock, superblock.inode_bitmap_start),
		superblock.block_sector_count,
		superblock.inode_max_count
	);

	for (uint64_t i = 0; i < FS_INODE_CACHE_SIZE; i++) {
		if (inode_cache[i].valid) {
			fs_inode_cache_entry_flush(i);
		}

		inode_cache[i].valid = false;
	}

	inode_cache_clock_hand = 0;
}

void fs_inode_cache_entry_flush(uint64_t index) {
	if (!inode_cache[index].dirty) {
		return;
	}

	uint64_t inode_number = inode_cache[index].index;

	uint64_t inode_block = inode_number / superblock.inodes_per_block;
	uint64_t inode_index = inode_number % superblock.inodes_per_block;
	uint64_t inode_sector = fs_block_to_sector(&superblock, superblock.inode_table_start + inode_block);

	uint8_t buffer[superblock.bytes_per_block];
	fs_inode_t* buf = (fs_inode_t*) buffer;
	ata_read(inode_sector, (void*) buffer, superblock.block_sector_count);
	buf[inode_index] = inode_cache[index].inode;
	ata_write(inode_sector, (void*) buffer, superblock.bytes_per_block);

	inode_cache[index].dirty = false;
}

int64_t fs_inode_cache_entry_evict(void) {
	if (!inode_cache[inode_cache_clock_hand].valid) {
		return -1;
	}

	if (!inode_cache[inode_cache_clock_hand].reference) {
		fs_inode_cache_entry_flush(inode_cache_clock_hand);
		inode_cache[inode_cache_clock_hand].valid = false;
		inode_cache_clock_hand++;

		return inode_cache_clock_hand - 1;
	}

	inode_cache[inode_cache_clock_hand].reference = false;
	inode_cache_clock_hand++;

	if (inode_cache_clock_hand >= FS_INODE_CACHE_SIZE) {
		inode_cache_clock_hand = 0;
	}

	return -1;
}

uint64_t fs_inode_cache_evict(void) {
	for (uint64_t i = 0; i < FS_INODE_CACHE_SIZE; i++) {
		int64_t index = fs_inode_cache_entry_evict();

		if (index != -1) {
			return index;
		}
	}

	fs_inode_cache_entry_flush(inode_cache_clock_hand);
	inode_cache[inode_cache_clock_hand].valid = false;

	inode_cache_clock_hand++;

	if (inode_cache_clock_hand >= FS_INODE_CACHE_SIZE) {
		inode_cache_clock_hand -= FS_INODE_CACHE_SIZE;
		return FS_INODE_CACHE_SIZE - 1;
	}

	return inode_cache_clock_hand - 1;
}

uint64_t fs_inode_cache_add(fs_inode_t inode, uint64_t index) {
	fs_inode_cache_entry_t entry;
	entry.inode = inode;
	entry.index = index;
	entry.reference = 1;
	entry.dirty = false;
	entry.valid = true;

	uint64_t cache_index = fs_inode_cache_evict();
	inode_cache[cache_index] = entry;

	return cache_index;
}

fs_inode_cache_entry_t* fs_inode_cache_search(uint64_t index) {
	for (uint64_t i = 0; i < FS_INODE_CACHE_SIZE; i++) {
		if (!inode_cache[i].valid) {
			continue;
		}

		if (inode_cache[i].index == index) {
			return &inode_cache[i];
		}
	}

	return NULL;
}

fs_inode_cache_entry_t* fs_inode_create_forced(uint8_t type, uint64_t index) {
	fs_inode_t inode;

	for (size_t i = 0; i < FS_INODE_TABLE_POINTER_COUNT; i++) {
		inode.blocks[i] = FS_DATA_FAILURE;
	}

	inode.type = type;
	inode.size = 0;
	inode.block_count = 0;

	uint64_t cache_index = fs_inode_cache_add(inode, index);
	bitmap_disk_set(&inode_bitmap, index);

	return &inode_cache[cache_index];
}

fs_inode_cache_entry_t* fs_inode_create(uint8_t type) {
	uint64_t index = bitmap_disk_find_free_bit(&inode_bitmap);

	if (index == BITMAP_DISK_FAILURE) {
		return NULL;
	}

	fs_inode_t inode;

	for (size_t i = 0; i < FS_INODE_TABLE_POINTER_COUNT; i++) {
		inode.blocks[i] = FS_DATA_FAILURE;
	}

	inode.type = type;
	inode.size = 0;
	inode.block_count = 0;

	uint64_t cache_index = fs_inode_cache_add(inode, index);
	bitmap_disk_set(&inode_bitmap, index);

	return &inode_cache[cache_index];
}

void fs_inode_destroy(fs_inode_cache_entry_t* icache) {
	fs_inode_free_blocks(icache, icache->inode.block_count);
	bitmap_disk_unset(&inode_bitmap, icache->index);

	icache->inode.type = 0;
	icache->inode.size = 0;
	icache->inode.block_count = 0;

	icache->reference = 0;
	icache->dirty = true;

	size_t cache_index = icache - inode_cache;
	fs_inode_cache_entry_flush(cache_index);

	icache->valid = false;
}

fs_inode_t fs_inode_table_get(uint64_t inode_table_block, uint64_t table_index) {
	uint8_t buffer[superblock.bytes_per_block];

	ata_read(fs_block_to_sector(&superblock, superblock.inode_table_start + inode_table_block), (void*) buffer, superblock.block_sector_count);

	return ((fs_inode_t*) buffer)[table_index];
}

fs_inode_cache_entry_t* fs_inode_get(uint64_t index) {
	fs_inode_cache_entry_t* cache_entry = fs_inode_cache_search(index);

	if (cache_entry) {
		cache_entry->reference = 1;
		return cache_entry;
	}

	uint64_t inode_table_block = index / superblock.inodes_per_block;
	uint64_t inode_table_block_index = index % superblock.inodes_per_block;

	fs_inode_t inode = fs_inode_table_get(inode_table_block, inode_table_block_index);

	uint64_t cache_index = fs_inode_cache_add(inode, index);

	return &inode_cache[cache_index];
}

bool fs_inode_set_size(fs_inode_cache_entry_t* icache, size_t size) {
	uint64_t old_block_count = (icache->inode.size + superblock.bytes_per_block - 1) / superblock.bytes_per_block;
	uint64_t new_block_count = (size + superblock.bytes_per_block - 1) / superblock.bytes_per_block;

	int64_t change = new_block_count - old_block_count;

	if (change > 0) {
		if (!fs_inode_alloc_blocks(icache, change)) {
			return false;
		}
	} else {
		if (!fs_inode_free_blocks(icache, change * -1)) {
			return false;
		}
	}

	icache->inode.size = size;

	icache->reference = 1;
	icache->dirty = true;

	return true;
}

uint64_t fs_inode_get_block(fs_inode_cache_entry_t* icache, uint64_t index) {
	if (index >= icache->inode.block_count) {
		return FS_DATA_FAILURE;
	}

	if (index >= FS_INODE_THIRD_INDIRECT_TABLE_CUTOFF) {
		return FS_DATA_FAILURE;
	}

	fs_data_indirect_table_t* buffer = (fs_data_indirect_table_t*) malloc(sizeof(fs_data_indirect_table_t));
	uint64_t data_block;

	if (index < FS_INODE_TABLE_CUTOFF) {
		data_block = icache->inode.blocks[index];

	} else if (index < FS_INODE_FIRST_INDIRECT_TABLE_CUTOFF) {
		uint64_t first_indirect_index = index - FS_INODE_TABLE_POINTER_COUNT;
		fs_data_read_block(icache->inode.first_indirect_table, (void*) buffer);

		data_block = buffer->blocks[first_indirect_index];

	} else if (index < FS_INODE_SECOND_INDIRECT_TABLE_CUTOFF) {
		uint64_t second_indirect_index = (index - FS_INODE_TABLE_POINTER_COUNT) / FS_INODE_INDIRECT_TABLE_POINTER_COUNT;
		uint64_t first_indirect_index = index - FS_INODE_TABLE_POINTER_COUNT - (second_indirect_index * FS_INODE_INDIRECT_TABLE_POINTER_COUNT);
		fs_data_read_block(icache->inode.second_indirect_table, (void*) buffer);

		uint64_t first_indirect_table = buffer->blocks[second_indirect_index];
		fs_data_read_block(first_indirect_table, (void*) buffer);

		data_block = buffer->blocks[first_indirect_index];

	} else {
		uint64_t third_indirect_index = (index - FS_INODE_TABLE_POINTER_COUNT) / pow(FS_INODE_INDIRECT_TABLE_POINTER_COUNT, 2);
		uint64_t second_indirect_index = (index - FS_INODE_TABLE_POINTER_COUNT - (third_indirect_index * pow(FS_INODE_INDIRECT_TABLE_POINTER_COUNT, 2))) / FS_INODE_INDIRECT_TABLE_POINTER_COUNT;
		uint64_t first_indirect_index = index - FS_INODE_TABLE_POINTER_COUNT - (second_indirect_index * FS_INODE_INDIRECT_TABLE_POINTER_COUNT) - (third_indirect_index * pow(FS_INODE_INDIRECT_TABLE_POINTER_COUNT, 2));
		fs_data_read_block(icache->inode.third_indirect_table, (void*) buffer);
		fs_data_read_block(buffer->blocks[third_indirect_index], (void*) buffer);

		uint64_t first_indirect_table = buffer->blocks[second_indirect_index];
		fs_data_read_block(first_indirect_table, (void*) buffer);

		data_block = buffer->blocks[first_indirect_index];
	}

	free(buffer);

	icache->reference = 1;

	return data_block;
}

void fs_inode_set_block(fs_inode_cache_entry_t* icache, uint64_t table_index, uint64_t data_index) {
	if (table_index >= FS_INODE_THIRD_INDIRECT_TABLE_CUTOFF) {
		return;
	}

	if (data_index > superblock.data_blocks_block_count) {
		return;
	}

	fs_data_indirect_table_t* buffer = (fs_data_indirect_table_t*) malloc(sizeof(fs_data_indirect_table_t));

	if (table_index < FS_INODE_TABLE_CUTOFF) {
		icache->inode.blocks[table_index] = data_index;

	} else if (table_index < FS_INODE_FIRST_INDIRECT_TABLE_CUTOFF) {
		uint64_t first_indirect_index = table_index - FS_INODE_TABLE_POINTER_COUNT;

		fs_data_read_block(icache->inode.first_indirect_table, (void*) buffer);
		buffer->blocks[first_indirect_index] = data_index;
		fs_data_write_block(icache->inode.first_indirect_table, (void*) buffer);

	} else if (table_index < FS_INODE_SECOND_INDIRECT_TABLE_CUTOFF) {
		uint64_t second_indirect_index = (table_index - FS_INODE_TABLE_POINTER_COUNT) / FS_INODE_INDIRECT_TABLE_POINTER_COUNT;
		uint64_t first_indirect_index = table_index - FS_INODE_TABLE_POINTER_COUNT - (first_indirect_index * FS_INODE_INDIRECT_TABLE_POINTER_COUNT);

		fs_data_read_block(icache->inode.second_indirect_table, (void*) buffer);

		uint64_t first_indirect_table = buffer->blocks[second_indirect_index];

		fs_data_read_block(first_indirect_table, (void*) buffer);
		buffer->blocks[first_indirect_index] = data_index;
		fs_data_write_block(first_indirect_table, (void*) buffer);

	} else {
		uint64_t third_indirect_index = (table_index - FS_INODE_TABLE_POINTER_COUNT) / pow(FS_INODE_INDIRECT_TABLE_POINTER_COUNT, 2);
		uint64_t second_indirect_index = (table_index - FS_INODE_TABLE_POINTER_COUNT - (second_indirect_index * pow(FS_INODE_INDIRECT_TABLE_POINTER_COUNT, 2))) / FS_INODE_INDIRECT_TABLE_POINTER_COUNT;
		uint64_t first_indirect_index = table_index - FS_INODE_TABLE_POINTER_COUNT - (first_indirect_index * FS_INODE_INDIRECT_TABLE_POINTER_COUNT) - (second_indirect_index * pow(FS_INODE_INDIRECT_TABLE_POINTER_COUNT, 2));

		fs_data_read_block(icache->inode.third_indirect_table, (void*) buffer);
		fs_data_read_block(buffer->blocks[third_indirect_index], (void*) buffer);

		uint64_t first_indirect_table = buffer->blocks[second_indirect_index];

		fs_data_read_block(first_indirect_table, (void*) buffer);
		buffer->blocks[first_indirect_index] = data_index;
		fs_data_write_block(first_indirect_table, (void*) buffer);
	}

	free(buffer);

	icache->reference = 1;
	icache->dirty = true;
}

bool fs_inode_alloc_blocks(fs_inode_cache_entry_t* icache, size_t count) {
	if (count == 0) {
		return true;
	}

	uint64_t new_block;

	for (size_t i = 0; i < count; i++) {
		new_block = fs_data_alloc_block();

		if (new_block == FS_DATA_FAILURE) {
			fs_data_free_block(new_block);
			return false;
		}

		fs_inode_set_block(icache, icache->inode.block_count + i, new_block);
	}

	icache->inode.block_count += count;

	icache->reference = 1;
	icache->dirty = true;

	return true;
}

bool fs_inode_free_blocks(fs_inode_cache_entry_t* icache, size_t count) {
	if (count == 0) {
		return true;
	}

	if (icache->inode.block_count < count) {
		return false;
	}

	icache->inode.block_count -= count;

	for (size_t i = 0; i < count; i++) {
		fs_data_free_block(fs_inode_get_block(icache, icache->inode.block_count + i));
		fs_inode_set_block(icache, icache->inode.block_count + i, FS_DATA_FAILURE);
	}

	icache->reference = 1;
	icache->dirty = true;

	return true;
}

