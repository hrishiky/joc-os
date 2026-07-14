#include "fs_data.h"

#include "fs_superblock.h"
#include "fs_helper.h"
#include "fs_system.h"
#include "stdio.h"
#include "stdint.h"
#include "ata.h"
#include "bitmap.h"

extern fs_superblock_t superblock;

bitmap_disk_t data_bitmap;

void fs_data_init(void) {
	bitmap_disk_init(
		&data_bitmap,
		fs_block_to_sector(&superblock, superblock.data_bitmap_start),
		superblock.block_sector_count,
		superblock.data_blocks_block_count
	);

	bitmap_disk_load(
		&data_bitmap,
		true,
		fs_block_to_sector(&superblock, superblock.data_bitmap_start),
		superblock.block_sector_count,
		superblock.data_blocks_block_count
	);
}

void fs_data_load(bool initialized) {
	bitmap_disk_flush(&data_bitmap);

	bitmap_disk_load(
		&data_bitmap,
		initialized,
		fs_block_to_sector(&superblock, superblock.data_bitmap_start),
		superblock.block_sector_count,
		superblock.data_blocks_block_count
	);
}

uint64_t fs_data_alloc_block(void) {
	uint64_t block = bitmap_disk_find_free_bit(&data_bitmap);

	if (block == BITMAP_DISK_FAILURE) {
		return FS_DATA_FAILURE;
	}

	bitmap_disk_set(&data_bitmap, block);

	return block;
}

void fs_data_free_block(uint64_t index) {
	bitmap_disk_unset(&data_bitmap, index);
}

void fs_data_read_block(uint64_t block, void* buffer) {
	ata_read(fs_block_to_sector(&superblock, superblock.data_blocks_start + block), buffer, superblock.block_sector_count);
}

void fs_data_write_block(uint64_t block, void* buffer) {
	ata_write(fs_block_to_sector(&superblock, superblock.data_blocks_start + block), buffer, superblock.bytes_per_block);
}

uint64_t fs_data_write_free_block(void* buffer) {
	uint64_t block = bitmap_disk_find_free_bit(&data_bitmap);

	if (block == BITMAP_DISK_FAILURE) {
		return FS_DATA_FAILURE;
	}

	ata_write(fs_block_to_sector(&superblock, superblock.data_blocks_start + block), buffer, superblock.bytes_per_block);

	return block;
}
