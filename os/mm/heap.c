#include "heap.h"

#include "pmm.h"
#include "vmm.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

heap_free_block_t* heap_free_list[HEAP_MAX_ORDER + 1];

uint8_t heap_size_to_order(uint64_t size) {
	uint8_t order = 0;
	size += sizeof(heap_header_t);
	uint64_t block_size = HEAP_BASE_BLOCK_SIZE;

	while (block_size <= size &&
	       order <= HEAP_MAX_ORDER) {
		order++;
		block_size <<= 1;
	}

	return order;
}

uint64_t heap_order_to_size(uint8_t order) {
	return 1ULL << (order + HEAP_BASE_BLOCK_ORDER);
}

void heap_init(void) {
	printf("heap init start\n");

	uint64_t block_start = HEAP_START;
	uint64_t heap_remaining = HEAP_SIZE;

	printf("allocating heap size: %d KiB\n", heap_remaining / 8192);

	for (uint8_t order = HEAP_MAX_ORDER; order >= 0; order--) {
		if (heap_remaining == 0 || heap_remaining < HEAP_BASE_BLOCK_SIZE) {
			break;
		}

		uint64_t block_size = 1ULL << (order + HEAP_BASE_BLOCK_ORDER);

		while (heap_remaining >= block_size && (block_start % block_size) == 0) {
			heap_add_block(order, (void*) block_start);
			block_start += block_size;
			heap_remaining -= block_size;

			printf("block assiged at address: %p; block size assigned: %d KiB (order %d); heap remaining: %d KiB\n", (void*) block_start, block_size / 8192, order, heap_remaining / 8192);
		}
	}

	printf("heap init done\n\n");
}

void heap_add_block(uint8_t order, void* block_start) {
	if (order > HEAP_MAX_ORDER) {
		return;
	}

	heap_free_block_t* block = (heap_free_block_t*) block_start;
	block->next = (uint64_t*) heap_free_list[order];
	heap_free_list[order] = block;
}

void heap_del_block(uint8_t order, void* block_start) {
	if (order > HEAP_MAX_ORDER) {
		return;
	}

	heap_free_block_t* block = (heap_free_block_t*) block_start;
	heap_free_block_t* current_block = heap_free_list[order];
	heap_free_block_t* previous_block;
	uint8_t list_index = 0;

	while (current_block != NULL) {
		if (block != current_block) {
			previous_block = current_block;
			current_block = (heap_free_block_t*) current_block->next;
			list_index++;

			continue;
		}

		if (list_index == 0) {
			heap_free_list[order] = (heap_free_block_t*) block->next;
		} else if (block->next == 0) {
			previous_block->next = 0;
		} else {
			previous_block->next = (uint64_t*) current_block->next;
		}

		return;
	}

	return; // error
}

void heap_split_block(uint8_t block_order, uint8_t target_order, void* block) {
	while (block_order > target_order) {
		heap_del_block(block_order, block);
		block_order--;
		heap_add_block(block_order, block);
		heap_add_block(block_order, (void*) ((uint8_t*) block + heap_order_to_size(block_order)));
	}
}

void* heap_alloc_block(uint8_t order) {
	if (!heap_free_list[order]) {
		return NULL;
	}

	void* block = (void*) heap_free_list[order];
	heap_del_block(order, block);

	page_table_t* cr3 = (page_table_t*) vmm_get_cr3();

	for (uint64_t i = 0; i < heap_order_to_size(order) / PMM_BLOCK_SIZE; i++) {
		vmm_map_page(pmm_alloc_block(), (void*) ((uint8_t*) block + (i * PMM_BLOCK_SIZE)), cr3);
	}

	heap_header_t* header = (heap_header_t*) block;
	header->order = order;

	return (void*) ((uint8_t*) block + sizeof(heap_header_t));
}

void* heap_alloc(size_t size) {
	if (size == 0 || size > heap_order_to_size(HEAP_MAX_ORDER)) {
		return NULL;
	}

	uint8_t original_order = heap_size_to_order(size);
	uint8_t free_block_order = original_order;

	while (free_block_order <= HEAP_MAX_ORDER) {
		if (heap_free_list[free_block_order] != 0) {
			break;
		}

		free_block_order++;
	}

	if (!heap_free_list[free_block_order]) {
		return NULL;
	}

	if (free_block_order != original_order) {
		heap_split_block(free_block_order, original_order, heap_free_list[free_block_order]);
	}

	return heap_alloc_block(original_order);
}

void heap_free(void* block) {
	if (block == NULL) {
		return;
	}

	uint8_t block_order = ((heap_header_t*) ((uint8_t*) block - sizeof(heap_header_t)))->order;

	heap_add_block(block_order, (void*) ((uint8_t*) block - sizeof(heap_header_t)));

	for (uint8_t order = block_order; order <= HEAP_MAX_ORDER; order++) {
		heap_free_block_t* current_block = heap_free_list[order];
		bool merged = false;

		while (current_block != NULL) {
			if ((uint64_t) current_block != ((uint64_t) block ^ heap_order_to_size(order))) {
				current_block = (heap_free_block_t*) current_block->next;
				continue;
			}

			heap_del_block(order, block);
			heap_del_block(order, (void*) ((uint8_t*) block + heap_order_to_size(order)));
			heap_add_block(order + 1, block);
			merged = true;

			break;
		}

		if (!merged) {
			break;
		}
	}
}

