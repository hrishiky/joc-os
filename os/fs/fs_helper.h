#ifndef FS_HELPER_H
#define FS_HELPER_H

#include "fs_superblock.h"
#include "stdint.h"

uint64_t fs_size_to_blocks(fs_superblock_t* superblock, uint64_t size);
uint64_t fs_block_to_sector(fs_superblock_t* superblock, uint64_t block);
uint64_t fs_size_to_sectors(uint64_t size);

#endif
