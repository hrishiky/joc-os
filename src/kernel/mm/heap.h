#ifndef HEAP_H
#define HEAP_H

#include "stdint.h"
#include "pmm.h"


#define HEAP_SIZE 0x1000000
#define HEAP_START 0xFFFF800001000000
#define HEAP_END (HEAP_START + HEAP_SIZE)
#define HEAP_MAX_ORDER 12
#define HEAP_BASE_BLOCK_SIZE PMM_BLOCK_SIZE
#define HEAP_BASE_BLOCK_ORDER 12


typedef struct {
	uint64_t* next;
} heap_free_block_t;

typedef struct {
	uint8_t order;
} heap_header_t;


uint8_t heap_size_to_order(uint64_t size);
uint64_t heap_order_to_size(uint8_t order);

void heap_init(void);

void heap_add_block(uint8_t order, void* block_start);
void heap_del_block(uint8_t order, void* block_start);
void heap_split_block(uint8_t block_order, uint8_t target_order, void* block);
void* heap_alloc_block(uint8_t order);

void* heap_alloc(size_t size);
void heap_free(void* block);

#endif
