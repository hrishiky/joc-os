#ifndef PMM_H
#define PMM_H

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE
#define PMM_LOW_BLOCK_COUNT 128

typedef struct {
        unsigned long base;
        unsigned long length;
        unsigned int type;
        unsigned int acpi_null;
} __attribute__((packed)) E820_Entry;

typedef struct {
        unsigned int entry_count;
        unsigned long entries;
} __attribute__((packed)) Boot_Info;

void pmm_memory_map_set (unsigned int bit);
void pmm_memory_map_unset (unsigned int bit);
int pmm_memory_map_test(unsigned int bit);

int pmm_memory_map_find_free_block(void);
int pmm_memory_map_find_low_free_block(void);
int pmm_memory_map_find_free_region(unsigned int page_count);

void pmm_init(void* boot_info_pointer);

void pmm_init_region(unsigned long base, unsigned int size);
void pmm_deinit_region(unsigned long base, unsigned int size);

void* pmm_alloc_block(void);
void pmm_free_block(void* block_pointer);

#endif
