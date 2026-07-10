#include "fs_superblock.h"

#include "fs_system.h"
#include "fs_helper.h"
#include "fs_inode.h"
#include "stdio.h"
#include "ata.h"

fs_superblock_t superblock;

void fs_superblock_init(size_t sectors_per_block, size_t disk_sector_count, size_t inode_count) { // validate values like disk sector count (fs system size)
	superblock = fs_superblock_calculate(sectors_per_block, disk_sector_count, inode_count);
	superblock.magic_number = FS_SYSTEM_MAGIC_NUMBER;

	ata_write(superblock.start_sector, (void*) &superblock, sizeof(fs_superblock_t));
}

bool fs_superblock_load(bool print) {
	ata_read(FS_SYSTEM_START_SECTOR, (void*) &superblock, fs_size_to_sectors(sizeof(fs_superblock_t)));

	if (print == true) {
		printf("magic number: %x | ", superblock.magic_number);
		printf("start sector: %i | ", superblock.start_sector);
		printf("sectors per block: %i | ", superblock.block_sector_count);
		printf("block count: %i | ", superblock.block_count);
		printf("bytes per sector: %i | ", superblock.bytes_per_sector);
		printf("bytes per block: %i | ", superblock.bytes_per_block);

		printf("superblock start: %i | ", superblock.superblock_start);
		printf("superblock block count: %i | ", superblock.superblock_block_count);

		printf("inodes per block: %i | ", superblock.inodes_per_block);
		printf("inode max count: %i | ", superblock.inode_max_count);
		printf("inode bitmap start: %i | ", superblock.inode_bitmap_start);
		printf("inode bitmap block count: %i | ", superblock.inode_bitmap_block_count);
		printf("inode table start: %i | ", superblock.inode_table_start);
		printf("inode table block count: %i | ", superblock.inode_table_block_count);

		printf("data bitmap start: %i | ", superblock.data_bitmap_start);
		printf("data bitmap block count: %i | ", superblock.data_bitmap_block_count);
		printf("data blocks start: %i | ", superblock.data_blocks_start);
		printf("data blocks block count: %i", superblock.data_blocks_block_count);

		printf("\n");
	}

	if (superblock.magic_number == FS_SYSTEM_MAGIC_NUMBER) {
		return true;
	}

	return false;
}

fs_superblock_t fs_superblock_calculate(size_t sectors_per_block, size_t disk_sector_count, size_t inode_count) {
	fs_superblock_t superblock;

	superblock.start_sector = FS_SYSTEM_START_SECTOR;
	superblock.block_sector_count = sectors_per_block;
	superblock.block_count = disk_sector_count / sectors_per_block;
	superblock.bytes_per_sector = FS_SECTOR_SIZE;
	superblock.bytes_per_block = superblock.block_sector_count * superblock.bytes_per_sector;

	superblock.superblock_start = superblock.start_sector;
	superblock.superblock_block_count = (sizeof(fs_superblock_t) + superblock.bytes_per_block - 1) / superblock.bytes_per_block;

	superblock.inodes_per_block = superblock.bytes_per_block / sizeof(fs_inode_t);
	superblock.inode_max_count = inode_count;
	superblock.inode_table_block_count = (superblock.inode_max_count + superblock.inodes_per_block - 1) / superblock.inodes_per_block;
	superblock.inode_bitmap_start = superblock.superblock_start + superblock.superblock_block_count + 1;
	superblock.inode_bitmap_block_count = (superblock.inode_table_block_count + 7) / 8;
	superblock.inode_table_start = superblock.inode_bitmap_start + superblock.inode_bitmap_block_count + 1;

	size_t remaining_blocks = superblock.block_count - (superblock.inode_table_start + superblock.inode_table_block_count);
	remaining_blocks -= remaining_blocks % 9;

	superblock.data_bitmap_start = superblock.inode_table_start + superblock.inode_table_block_count + 1;
	superblock.data_bitmap_block_count = remaining_blocks / 9;
	superblock.data_blocks_start = superblock.data_bitmap_start + superblock.data_bitmap_block_count + 1;
	superblock.data_blocks_block_count = superblock.data_bitmap_block_count * 8;

	return superblock;
}
