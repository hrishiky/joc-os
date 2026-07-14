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
	printf("fs init start\n");

	bool initialized = false;

	if (!fs_superblock_load(false)) {
		initialized = true;

		printf("fs superblock not found\n");
		printf("fs initialization started\n");

		printf("initializing fs superblock... ");
		fs_superblock_init(FS_SYSTEM_BLOCK_SECTOR_COUNT, FS_SYSTEM_SECTOR_COUNT, FS_SYSTEM_INODE_COUNT);
		printf("done\n");

		printf("initializing fs data handling... ");
		fs_data_init();
		printf("done\n");

		printf("initializing fs inode handling... ");
		fs_inode_init();
		printf("done\n");

		printf("initializing fs directory handling... ");
		fs_directory_init();
		printf("done\n");

		printf("fs initialization done\n");
	}

	printf("fs loading started\n");

	printf("loading fs superblock... ");
	fs_superblock_load(false);
	printf("done\n");

	printf("loading fs data handling... ");
	fs_data_load(initialized);
	printf("done\n");

	printf("loading fs inode handling... ");
	fs_inode_load(initialized);
	printf("done\n");

	printf("loading fs directory handling... ");
	fs_directory_load(initialized);
	printf("done\n");

	printf("fs loading done\n");

	printf("fs init done\n");
}
