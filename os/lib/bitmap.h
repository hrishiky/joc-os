#ifndef BITMAP_H
#define BITMAP_H

#include "stdint.h"
#include "stdbool.h"


#define BITMAP_DISK_SECTOR_UINT64_COUNT 64
#define BITMAP_DISK_SECTOR_BIT_COUNT 512 * 8

#define BITMAP_DISK_FAILURE UINT64_MAX


typedef struct {
	uint64_t* bitmap;
	uint64_t size;
} bitmap_memory_t;

typedef struct {
	uint64_t* block;
	bool dirty;
	uint64_t block_index;

	size_t total_bits;
	size_t used_bits;

	uint64_t start_sector;
	size_t sectors_per_block;
	size_t block_size;
	size_t block_count;
} bitmap_disk_t;


uint64_t bitmap_disk_bit_to_block(bitmap_disk_t* bitmap, uint64_t bit);

void bitmap_disk_init(bitmap_disk_t* bitmap, uint64_t start_sector, uint64_t sectors_per_block, uint64_t size);
void bitmap_disk_load(bitmap_disk_t* bitmap, bool initialized, uint64_t start_sector, uint64_t sectors_per_block, uint64_t size);

size_t bitmap_disk_compute_used_bits(bitmap_disk_t* bitmap);
size_t bitmap_disk_compute_used_bits_block(bitmap_disk_t* bitmap);

void bitmap_disk_flush(bitmap_disk_t* bitmap);
void bitmap_disk_load_block(bitmap_disk_t* bitmap, uint64_t block);
void bitmap_disk_checked_load_block(bitmap_disk_t* bitmap, uint64_t bit);

void bitmap_disk_set(bitmap_disk_t* bitmap, uint64_t bit);
void bitmap_disk_unset(bitmap_disk_t* bitmap, uint64_t bit);
bool bitmap_disk_test(bitmap_disk_t* bitmap, uint64_t bit);

uint64_t bitmap_disk_find_free_bit(bitmap_disk_t* bitmap);

#endif
