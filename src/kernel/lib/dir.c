#include "dir.h"

#include "fs.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// dir operations on files should not be possible; open, change

dir_t* dir_open(char* path, size_t cwd) {
	uint64_t inode = fs_path_to_inode(path, cwd);

	if (inode == FS_INODE_FAILURE) {
		return NULL;
	}

	dir_t* dir = malloc(sizeof(dir_t));

	dir->inode = inode;
	dir->index = 0;

	return dir;
}

dirent_t* dir_read(dir_t* dir) {
	fs_directory_entry_get(&dir->dirent, dir->inode, dir->index);

	if (dir->dirent.type == FS_DIRECTORY_FAILURE) {
		return NULL;
	}

	dir->index++;

	return &dir->dirent;
}

void dir_close(dir_t* dir) {
	free(dir);
}

bool dir_make(char* path, size_t cwd, char* cwd_path, bool forced) {
	if (!forced) {
		return fs_directory_create(path, cwd);
	}

	char absolute_path[strlen(path) + strlen(cwd_path) + 1];
	fs_path_normalize(path, cwd_path, absolute_path);

	size_t parent_dir_count = fs_path_delimiter_count(absolute_path);
	char parent_path[strlen(absolute_path) + 1];

	for (size_t i = 1; i < parent_dir_count; i++) {
		fs_path_to_parent_path(absolute_path, parent_path, parent_dir_count - i - 1);

		if (!fs_directory_create(parent_path, cwd)) {
			return false;
		}
	}

	return fs_directory_create(absolute_path, cwd);
}

bool dir_remove(char* path, size_t cwd) {
	dir_t* dir;
	dirent_t* dirent;
	size_t inode;

	dir = dir_open(path, cwd);

	if (dir == NULL) {
		return false;
	}

	inode = dir->inode;

	fs_inode_cache_entry_t* icache = fs_inode_get(inode);
	size_t parent_inode = fs_path_to_parent_inode(path, cwd);

	char name[strlen(path) + 1];
	fs_path_to_name(path, name);

	if (strcmp(name, FS_DIRECTORY_CURRENT_DIR_NAME) || strcmp(name, FS_DIRECTORY_PARENT_DIR_NAME)) {
		return false;
	}

	while ((dirent = dir_read(dir)) != NULL) {
		if (dirent->name_length > FS_DIRECTORY_PARENT_DIR_NAME_LENGTH) {
			return false;
		}

		if (dirent->name_length == FS_DIRECTORY_PARENT_DIR_NAME_LENGTH) {
			if (!strncmp(dirent->name, FS_DIRECTORY_PARENT_DIR_NAME, FS_DIRECTORY_PARENT_DIR_NAME_LENGTH)) {
				return false;
			}
		} else {
			if (!strncmp(dirent->name, FS_DIRECTORY_CURRENT_DIR_NAME, FS_DIRECTORY_CURRENT_DIR_NAME_LENGTH)) {
				return false;
			}
		}
	}

	dir_close(dir);

	fs_inode_destroy(icache);
	fs_directory_entry_del(parent_inode, name);

	return true;
}

bool dir_change(char* path, size_t* cwd, char* cwd_path) {
	uint64_t inode = fs_path_to_inode(path, *cwd);

	if (inode == FS_INODE_FAILURE) {
		return false;
	}

	*cwd = inode;

	char buffer[strlen(cwd_path) + strlen(path) + 1];
	fs_path_normalize(path, cwd_path, buffer);
	strcpy(cwd_path, buffer);

	return true;
}

