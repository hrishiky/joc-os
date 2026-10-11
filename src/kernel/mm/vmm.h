#ifndef VMM_H
#define VMM_H

#include "stdint.h"
#include "stdbool.h"

#define ENTRY_COUNT 512
#define VMM_PHYSMAP_OFFSET 0xFFFF800000000000

#define VMM_IDENTITYMAP_START 0x0
#define VMM_IDENTITYMAP_END 0x1000000
#define VMM_PHYSMAP_START 0x1000000

typedef struct {
	uint64_t entries[ENTRY_COUNT];
}__attribute__((packed)) page_table_t;

uint64_t vmm_pte_get_frame(uint64_t entry);
void vmm_pte_set_frame(uint64_t* entry, uint64_t address);
bool vmm_pte_is_present(uint64_t entry);

uint64_t vmm_pde_get_address(uint64_t entry);
void vmm_pde_set_address(uint64_t* entry, uint64_t address);
bool vmm_pde_is_present(uint64_t entry);

uint64_t vmm_pdpte_get_address(uint64_t entry);
void vmm_pdpte_set_address(uint64_t* entry, uint64_t address);
bool vmm_pdpte_is_present(uint64_t entry);

uint64_t vmm_pml4e_get_address(uint64_t entry);
void vmm_pml4e_set_address(uint64_t* entry, uint64_t address);
bool vmm_pml4e_is_present(uint64_t entry);

void vmm_entry_add_flag(uint64_t* entry, uint64_t flag_mask);
void vmm_entry_clear_flag(uint64_t* entry, uint64_t flag_mask);

uint64_t vmm_physical_to_virtual(uint64_t physical_address);
void vmm_flush_tlb_entry(uint64_t address);
uint64_t vmm_get_cr3(void);
void vmm_set_cr3(uint64_t physical_address);

bool vmm_alloc_page(uint64_t* entry);
bool vmm_free_page(uint64_t* entry, uint64_t virtual_address);
bool vmm_map_page(void* phys_addr, void* virt_addr, page_table_t* pml4);
bool vmm_unmap_page(uint64_t address, page_table_t* pml4);
void vmm_init(void);

#endif
