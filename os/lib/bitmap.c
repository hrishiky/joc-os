#include "bitmap.h"

#include "stdio.h"

#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
#include "ata.h"
#include "fs_system.h"

uint64_t bitmap_disk_bit_to_block(bitmap_disk_t* bitmap, uint64_t bit) {
	return bit / (bitmap->block_size * 8);
}

void bitmap_disk_init(bitmap_disk_t* bitmap, uint64_t start_sector, size_t sectors_per_block, size_t size) {
	bitmap->total_bits = size;
	bitmap->used_bits = 0;

	bitmap->start_sector = start_sector;
	bitmap->sectors_per_block = sectors_per_block;
	bitmap->block_size = sectors_per_block * FS_SECTOR_SIZE;
	bitmap->block_count = ((size / 8) + bitmap->block_size - 1) / (bitmap->block_size);

	uint64_t block_model[bitmap->block_size];

	for (size_t i = 0; i < (bitmap->block_size / 8); i++) {
		block_model[i] = 0;
	}

	for (uint64_t i = 0; i < (bitmap->block_count * bitmap->sectors_per_block) - 1; i++) {
		ata_write(start_sector + i, (void*) block_model, bitmap->block_size);
	}

	uint64_t extra_bits = size - ((bitmap->block_count - 1) * bitmap->block_size * 8);

	for (uint64_t i = 0; i < extra_bits; i++) {
		block_model[(i % (bitmap->block_size * 8)) / 64] &= ~(1ULL << (i % 64));
	}

	for (uint64_t i = extra_bits; i < (bitmap->block_size * 8); i++) {
		block_model[(i % (bitmap->block_size * 8)) / 64] |= (1ULL << (i % 64));
	}

	ata_write(start_sector + ((bitmap->block_count - 1) * bitmap->sectors_per_block), (void*) block_model, bitmap->block_size);
}

void bitmap_disk_load(bitmap_disk_t* bitmap, bool initialized, uint64_t start_sector, uint64_t sectors_per_block, uint64_t size) {
	if (!initialized) {
		bitmap->total_bits = size;
		bitmap->start_sector = start_sector;
		bitmap->sectors_per_block = sectors_per_block;
		bitmap->block_size = sectors_per_block * FS_SECTOR_SIZE;
		bitmap->block_count = ((size / 8) + bitmap->block_size - 1) / bitmap->block_size;

		bitmap->block = malloc(bitmap->block_size);
		bitmap->dirty = false;
		bitmap_disk_load_block(bitmap, 0);

		bitmap->used_bits = bitmap_disk_compute_used_bits(bitmap);
	} else {
		bitmap->block = malloc(bitmap->block_size);
		bitmap->dirty = false;
		bitmap_disk_load_block(bitmap, 0);
	}
}

size_t bitmap_disk_compute_used_bits(bitmap_disk_t* bitmap) {
	uint64_t loaded_block = bitmap->block_index;
	size_t used_bits = 0;

	used_bits += bitmap_disk_compute_used_bits_block(bitmap);

	for (uint64_t i = 0; i < loaded_block; i++) {
		bitmap_disk_load_block(bitmap, i);
		used_bits += bitmap_disk_compute_used_bits_block(bitmap);
	}

	for (uint64_t i = loaded_block + 1; i < bitmap->block_count; i++) {
		bitmap_disk_load_block(bitmap, i);
		used_bits += bitmap_disk_compute_used_bits_block(bitmap);
	}

	return used_bits;
}

size_t bitmap_disk_compute_used_bits_block(bitmap_disk_t* bitmap) {
	size_t used_bits_block = 0;
	uint64_t number;

	for (size_t i = 0; i < (bitmap->block_size / 8); i++) {
		number = bitmap->block[i];

		while (number) {
			number &= (number - 1);
			used_bits_block++;
		}
	}

	return used_bits_block;
}

void bitmap_disk_flush(bitmap_disk_t* bitmap) {
	if (bitmap->dirty) {
		ata_write(bitmap->start_sector + (bitmap->block_index * bitmap->sectors_per_block), (void*) bitmap->block, bitmap->block_size);
	}

	bitmap->dirty = false;
}

void bitmap_disk_load_block(bitmap_disk_t* bitmap, uint64_t block) {
	if (block >= bitmap->block_count) {
		return;
	}

	if (bitmap->dirty) {
		ata_write(bitmap->start_sector + (bitmap->block_index * bitmap->sectors_per_block), (void*) bitmap->block, bitmap->block_size);
	}

	ata_read(bitmap->start_sector + (block * bitmap->sectors_per_block), (void*) bitmap->block, bitmap->sectors_per_block);

	bitmap->block_index = block;
	bitmap->dirty = false;
}

void bitmap_disk_checked_load_block(bitmap_disk_t* bitmap, uint64_t bit) {
	uint64_t block = bitmap_disk_bit_to_block(bitmap, bit);

	if (bitmap->block_index != block) {
		bitmap_disk_load_block(bitmap, block);
	}
}

void bitmap_disk_set(bitmap_disk_t* bitmap, uint64_t bit) {
	if (bit >= bitmap->total_bits) {
		return;
	}

	if (bitmap->block[(bit % (bitmap->block_size * 8)) / 64] & (1ULL << (bit % 64))) {
		return;
	}

	bitmap_disk_checked_load_block(bitmap, bit);

	bitmap->block[(bit % (bitmap->block_size * 8)) / 64] |= (1ULL << (bit % 64));

	bitmap->used_bits++;
	bitmap->dirty = true;
}

void bitmap_disk_unset(bitmap_disk_t* bitmap, uint64_t bit) {
	if (bit >= bitmap->total_bits) {
		return;
	}

	if (!(bitmap->block[(bit % (bitmap->block_size * 8)) / 64] & (1ULL << (bit % 64)))) {
		return;
	}

	bitmap_disk_checked_load_block(bitmap, bit);

	bitmap->block[(bit % (bitmap->block_size * 8)) / 64] &= ~(1ULL << (bit % 64));

	bitmap->used_bits--;
	bitmap->dirty = true;
}

bool bitmap_disk_test(bitmap_disk_t* bitmap, uint64_t bit) {
	if (bit >= bitmap->total_bits) {
		return false;
	}

	bitmap_disk_checked_load_block(bitmap, bit);

	if (bitmap->block[(bit % (bitmap->block_size * 8)) / 64] & (1ULL << (bit % 64))) {
		return false;
	}

	return true;
}

uint64_t bitmap_disk_find_free_bit(bitmap_disk_t* bitmap) {
	if (bitmap->used_bits >= bitmap->total_bits) {
		return BITMAP_DISK_FAILURE;
	}

	uint64_t current_block = bitmap->block_index;
	uint64_t bit;

	for (uint64_t i = 0; i < bitmap->block_count; i++) {
		for (uint64_t j = 0; j < (bitmap->block_size / 8); j++) {
			if (bitmap->block[j] == 0xFFFFFFFFFFFFFFFF) {
				continue;
			}

			bit = (current_block * bitmap->block_size * 8) + (j * BITMAP_DISK_SECTOR_UINT64_COUNT) - 1;

			for (uint8_t k = 0; k < BITMAP_DISK_SECTOR_UINT64_COUNT; k++) {
				bit++;

				if (bitmap_disk_test(bitmap, bit)) {
					return bit;
				}
			}
		}

		current_block++;

		if (current_block >= bitmap->block_count) {
			current_block -= bitmap->block_count;
		}

		bitmap_disk_load_block(bitmap, current_block);
	}

	return BITMAP_DISK_FAILURE;
}
