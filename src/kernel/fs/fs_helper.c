#include "fs_helper.h"

#include "stdint.h"
#include "fs_superblock.h"
#include "fs_system.h"


uint64_t fs_size_to_blocks(fs_superblock_t* superblock, uint64_t size) {
	return (size + superblock->bytes_per_block - 1) / superblock->bytes_per_block;
}

uint64_t fs_block_to_sector(fs_superblock_t* superblock, uint64_t block) {
	return superblock->start_sector + (block * superblock->block_sector_count);
}

uint64_t fs_size_to_sectors(uint64_t size) {
	return (size + FS_SECTOR_SIZE - 1) / FS_SECTOR_SIZE;
}
