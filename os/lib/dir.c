#include "dir.h"

#include "stdlib.h"
#include "fs.h"

dir_t* opendir(char* dirname, uint64_t cwd) {
	uint64_t inode = fs_path_to_inode(dirname, cwd);

	if (inode == FS_INODE_FAILURE) {
		return NULL;
	}

	dir_t* dirp = malloc(sizeof(dir_t));

	dirp->inode = inode;
	dirp->index = 0;

	return dirp;
}

dirent_t* readdir(dir_t* dirp) {
	fs_directory_entry_get(&dirp->dirent, dirp->inode, dirp->index);

	if (dirp->dirent.type == FS_DIRECTORY_FAILURE) {
		return NULL;
	}

	dirp->index++;

	return &dirp->dirent;
}

void closedir(dir_t* dirp) {
	free(dirp);
}
