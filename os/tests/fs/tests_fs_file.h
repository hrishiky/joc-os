#ifndef TEST_FS_FILE_H
#define TEST_FS_FILE_H

#include "stdbool.h"

bool test_file(void);

bool test_file_1(bool print, size_t file_block_count);
void test_file_2(size_t file1_size, size_t file2_size);

#endif