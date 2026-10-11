#include "fs.h"

#include "stdio.h"

#include "fs_superblock.h"
#include "fs_helper.h"
#include "fs_system.h"
#include "fs_inode.h"
#include "fs_data.h"
#include "fs_directory.h"
#include "stdbool.h"

extern fs_superblock_t superblock;

void fs_init(void) {
	printf("fs init starting\n");

	bool initialized = true;

	if (!fs_superblock_init(initialized, FS_SYSTEM_BLOCK_SECTOR_COUNT, FS_SYSTEM_SECTOR_COUNT, FS_SYSTEM_INODE_COUNT)) {
		initialized = false;
	}

	printf("loading fs superblock... ");
	fs_superblock_init(initialized, FS_SYSTEM_BLOCK_SECTOR_COUNT, FS_SYSTEM_SECTOR_COUNT, FS_SYSTEM_INODE_COUNT);
	fs_superblock_print();
	printf("done\n");

	printf("loading fs data handling... ");
	fs_data_init(initialized);
	printf("done\n");

	printf("loading fs inode handling... ");
	fs_inode_init(initialized);
	printf("done\n");

	printf("loading fs directory handling... ");
	fs_directory_init(initialized);
	printf("done\n");

	printf("fs init done\n\n");
}

void fs_exit(void) {
	printf("fs exit starting\n");

	fs_superblock_exit();
	fs_data_exit();
	fs_inode_exit();
	fs_directory_exit();

	printf("fs exit done\n\n");
}
