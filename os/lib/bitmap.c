#include "bitmap.h"

#include "stdio.h"

#include "ata.h"
#include "fs_system.h"

uint64_t bitmap_disk_bit_to_block(bitmap_disk_t* bitmap, uint64_t bit) {
	return bit / (bitmap->block_sector_count * BITMAP_DISK_SECTOR_BIT_COUNT);
}

void bitmap_disk_init(bitmap_disk_t* bitmap, uint64_t start_sector, uint64_t block_sector_count, uint64_t size) {
	uint64_t block_count = (size + (block_sector_count * FS_SECTOR_SIZE) - 1) / (block_sector_count * FS_SECTOR_SIZE);

	bitmap->start_sector = start_sector;
	bitmap->block_sector_count = block_sector_count;
	bitmap->block_count = block_count;
	bitmap->total_bits = size;

	uint64_t sector_model[BITMAP_DISK_SECTOR_UINT64_COUNT];

	for (uint8_t i = 0; i < BITMAP_DISK_SECTOR_UINT64_COUNT; i++) {
		sector_model[i] = 0;
	}

	for (uint64_t i = 0; i < (block_count * block_sector_count) - 1; i++) {
		ata_write(start_sector + i, (void*) sector_model, FS_SECTOR_SIZE);
	}

	uint64_t last_sector_bits = size % BITMAP_DISK_SECTOR_BIT_COUNT;

	for (uint64_t i = 0; i < last_sector_bits; i++) {
		sector_model[(i / 64) % (block_sector_count * 512)] &= ~(1ULL << (i % 64));
	}

	for (uint64_t i = last_sector_bits; i < BITMAP_DISK_SECTOR_BIT_COUNT; i++) {
		sector_model[(i / 64) % (block_sector_count * 512)] |= (1ULL << (i % 64));
	}

	ata_write(start_sector + ((block_count -1) * block_sector_count), (void*) sector_model, FS_SECTOR_SIZE);
}

void bitmap_disk_load(bitmap_disk_t* bitmap, uint64_t start_sector, uint64_t block_sector_count, uint64_t size) {
	uint64_t block_count = (size + (block_sector_count * FS_SECTOR_SIZE) - 1) / (block_sector_count * FS_SECTOR_SIZE);

	bitmap->start_sector = start_sector;
	bitmap->block_sector_count = block_sector_count;
	bitmap->block_count = block_count;
	bitmap->total_bits = size;
}

void bitmap_disk_load_block(bitmap_disk_t* bitmap, uint64_t block) {
	if (block >= bitmap->block_count) {
		return;
	}

	if (bitmap->dirty) {
		ata_write(bitmap->start_sector + (bitmap->block_index * bitmap->block_sector_count), (void*) &bitmap->block, bitmap->block_sector_count);
	}

	ata_read(bitmap->start_sector + (block * bitmap->block_sector_count), (void*) &bitmap->block, bitmap->block_sector_count);

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

	bitmap_disk_checked_load_block(bitmap, bit);

	bitmap->block[(bit / 64) % (bitmap->block_sector_count * FS_SECTOR_SIZE)] |= (1ULL << (bit % 64));

	bitmap->used_bits++;
	bitmap->dirty = true;
}

void bitmap_disk_unset(bitmap_disk_t* bitmap, uint64_t bit) {
	if (bit >= bitmap->total_bits) {
		return;
	}

	bitmap_disk_checked_load_block(bitmap, bit);

	bitmap->block[(bit / 64) % (bitmap->block_sector_count * FS_SECTOR_SIZE)] &= ~(1ULL << (bit % 64));

	bitmap->used_bits--;
	bitmap->dirty = true;
}

bool bitmap_disk_test(bitmap_disk_t* bitmap, uint64_t bit) {
	if (bit >= bitmap->total_bits) {
		return false;
	}

	bitmap_disk_checked_load_block(bitmap, bit);

	if (bitmap->block[(bit / 64) % (bitmap->block_sector_count * FS_SECTOR_SIZE)] & (1ULL << (bit % 64))) {
		return true;
	}

	return false;
}

uint64_t bitmap_disk_find_free_bit(bitmap_disk_t* bitmap) {
	if (bitmap->used_bits >= bitmap->total_bits) {
		return BITMAP_DISK_FAILURE;
	}

	uint64_t current_block = bitmap->block_index;

	for (uint64_t i = 0; i < bitmap->block_count; i++) {
		for (uint64_t j = 0; j < (bitmap->block_sector_count / 512) / 8; j++) {
			if (bitmap->block[j] == 0xFFFFFFFFFFFFFFFF) {
				continue;
			}

			for (uint8_t k = 0; k < BITMAP_DISK_SECTOR_UINT64_COUNT; k++) {
				uint64_t bit = (j * BITMAP_DISK_SECTOR_UINT64_COUNT) + k;

				if (bitmap_disk_test(bitmap, bit)) {
					return bit;
				}
			}
		}

		if (current_block++ > bitmap->block_count) {
			current_block -= bitmap->block_count;
		}

		bitmap_disk_load_block(bitmap, current_block);
	}

	return BITMAP_DISK_FAILURE;
}
