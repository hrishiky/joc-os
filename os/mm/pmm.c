#include "pmm.h"
#include "string.h"
#include "vga_text.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"

Boot_Info* boot_info;
bool pmm_init_mode;
extern unsigned char kernel_end;

unsigned int pmm_memory_size = 0;
unsigned int pmm_used_blocks = 0;
unsigned int pmm_max_blocks = 0;
unsigned int* pmm_memory_map = 0;

void pmm_memory_map_set(unsigned int bit) {
	unsigned int word = bit / 32;
	unsigned int mask = 1u << (bit % 32);
	pmm_memory_map[word] |= mask;
}

void pmm_memory_map_unset(unsigned int bit) {
	unsigned int word = bit / 32;
	unsigned int mask = 1u << (bit % 32);
	pmm_memory_map[word] &= ~mask;
}

int pmm_memory_map_test(unsigned int bit) {
	unsigned int word = bit / 32;
	unsigned int mask = 1u << (bit % 32);
	return (pmm_memory_map[word] & mask) ? 1 : 0;
}

int pmm_memory_map_find_free_block(void) {
	for (unsigned int i = 0; i < pmm_max_blocks / 32; i++) {
		if (pmm_memory_map[i] == 0xFFFFFFFF) {
			continue;
		}

		for (unsigned int j = 0; j < 32; j++) {
			int bit = (i * 32) + j;

			if (!(pmm_memory_map_test(bit))) {
				return bit;
			}
		}
	}

	return -1;
}

int pmm_memory_map_find_low_free_block(void) {
	for (unsigned int i = 0; i < PMM_LOW_BLOCK_COUNT; i++) {
		if (pmm_memory_map[i] == 0xFFFFFFFF) {
			continue;
		}

		for (unsigned int j = 0; j < 32; j++) {
			int bit = (i * 32) + j;

			if (!(pmm_memory_map_test(bit))) {
				return bit;
			}
		}
	}

	return -1;
}

int pmm_memory_map_find_free_region(unsigned int page_count) {
	unsigned int region_start = 0;
	unsigned char region_start_found = 0;
	unsigned int free_pages = 0;

	for (unsigned int i = 0; i < pmm_max_blocks; i++) {
		if (pmm_memory_map[i] == 0xFFFFFFFF) {
			continue;
		}

		region_start_found = 0;

		for (unsigned int j = 0; j < 32; j++) {
			int bit = (i * 32) + j;

			if (!pmm_memory_map_test(bit)) {
				region_start = bit;
				region_start_found = 1;
				break;
			}
		}

		if (!region_start_found) {
			continue;
		}

		free_pages = 0;

		for (unsigned int j = 0; j < page_count; j++) {
			if (pmm_memory_map_test(region_start + j)) {
				break;
			}

			free_pages++;
		}

		if (free_pages == page_count) {
			return region_start;
		}
	}

	return -1;
}

void pmm_init(void* boot_info_pointer) {
	printf("pmm init starting\n");

	boot_info = (Boot_Info*) boot_info_pointer;
	E820_Entry* memory_map = (E820_Entry*) boot_info->entries;

	unsigned long memory_size = 0;

	for (unsigned int i = 0; i < boot_info->entry_count; i++) {
		if (memory_map[i].type == 1) {
			memory_size += memory_map[i].length;
		}
	}

	pmm_memory_size = memory_size;
	pmm_memory_map = (unsigned int*) ((unsigned long) &kernel_end);
	pmm_max_blocks = pmm_memory_size / PMM_BLOCK_SIZE;
	pmm_used_blocks = pmm_max_blocks;

	memset(pmm_memory_map, 0xF, (pmm_max_blocks / PMM_BLOCKS_PER_BYTE));

	for (unsigned int i = 0; i < boot_info->entry_count; i++) {
		if (memory_map[i].type == 1) {
			pmm_init_region(memory_map[i].base, memory_map[i].length);
		}
	}

	pmm_memory_map_set(0);

	for (int i = 0; i < 2560; i++) {
		pmm_memory_map_set(i);
		pmm_used_blocks++;
	}

	printf("pmm init done\n\n");
}

void pmm_init_region(unsigned long base, unsigned int size) {
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	while (blocks > 0) {
		pmm_memory_map_unset(align++);
		pmm_used_blocks--;
		blocks--;
	}
}

void pmm_deinit_region(unsigned long base, unsigned int size) {
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	while (blocks > 0) {
		pmm_memory_map_set(align++);
		pmm_used_blocks++;
		blocks--;
	}
}

void* pmm_alloc_block(void) {
	int frame = 0;

	if (pmm_max_blocks - pmm_used_blocks <= 0) {
		return 0;
	}

	if (pmm_init_mode) {
		frame = pmm_memory_map_find_low_free_block();
	} else {
		frame = pmm_memory_map_find_free_block();
	}

	if (frame == -1) {
		return 0;
	}

	pmm_memory_map_set(frame);

	unsigned long address = frame * PMM_BLOCK_SIZE;
	pmm_used_blocks++;

	return (void*) address;
}

void pmm_free_block(void* block_pointer) {
	unsigned long address = (unsigned long) block_pointer;
	int frame = address / PMM_BLOCK_SIZE;

	pmm_memory_map_unset(frame);

	pmm_used_blocks--;
}
