#ifndef FS_H
#define FS_H

#include "fs_system.h"
#include "fs_helper.h"
#include "fs_superblock.h"
#include "fs_data.h"
#include "fs_inode.h"
#include "fs_directory.h"

void fs_init(void);
void fs_exit(void);

#endif
