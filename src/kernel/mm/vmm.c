#include "vmm.h"
#include "pmm.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"

extern unsigned int pmm_memory_size;
extern bool pmm_init_mode;
bool vmm_init_mode;

enum {
        PTE_PRESENT = 0x1ULL,
        PTE_READ_WRITE = 0x2ULL,
        PTE_USER_SUPERVISOR = 0x4ULL,
        PTE_WRITE_THROUGH = 0x8ULL,
        PTE_CACHE_DISABLE = 0x10ULL,
        PTE_ACCESSED = 0x20ULL,
        PTE_DIRTY = 0x40ULL,
        PTE_PAGE_ATTRIBUTE_TABLE = 0x80ULL,
        PTE_GLOBAL = 0x100ULL,
        PTE_AVAILABLE_1 = 0xE00ULL,
        PTE_FRAME = 0x000FFFFFFFFFF000ULL,
        PTE_AVAILABLE_2 = 0x7F0000000000000ULL,
        PTE_PROTECTION_KEY = 0x7800000000000000ULL,
        PTE_EXECUTE_DISABLE = 0x8000000000000000ULL
} PTE_FLAGS;

enum {
        PDE_PRESENT = 0x1ULL,
        PDE_READ_WRITE = 0x2ULL,
        PDE_USER_SUPERVISOR = 0x4ULL,
        PDE_WRITE_THROUGH = 0x8ULL,
        PDE_CACHE_DISABLE = 0x10ULL,
        PDE_ACCESSED = 0x20ULL,
        PDE_AVAILABLE_1 = 0x40ULL,
        PDE_PAGE_SIZE = 0x80ULL,
        PDE_AVAILABLE_2 = 0xF00ULL,
        PDE_ADDRESS = 0x000FFFFFFFFFF000ULL,
        PDE_AVAILABLE_3 = 0x7FF0000000000000ULL,
        PDE_EXECUTE_DISABLE = 0x8000000000000000ULL
} PDE_FLAGS;

enum {
        PDPTE_PRESENT = 0x1ULL,
        PDPTE_READ_WRITE = 0x2ULL,
        PDPTE_USER_SUPERVISOR = 0x4ULL,
        PDPTE_WRITE_THROUGH = 0x8ULL,
        PDPTE_CACHE_DISABLE = 0x10ULL,
        PDPTE_ACCESSED = 0x20ULL,
        PDPTE_AVAILABLE_1 = 0x40ULL,
        PDPTE_PAGE_SIZE = 0x80ULL,
        PDPTE_AVAILABLE_2 = 0xF00ULL,
        PDPTE_ADDRESS = 0x000FFFFFFFFFF000ULL,
        PDPTE_AVAILABLE_3 = 0x7FF0000000000000ULL,
        PDPTE_EXECUTE_DISABLE = 0x8000000000000000ULL
} PDPTE_FLAGS;

enum {
        PML4E_PRESENT = 0x1ULL,
        PML4E_READ_WRITE = 0x2ULL,
        PML4E_USER_SUPERVISOR = 0x4ULL,
        PML4E_WRITE_THROUGH = 0x8ULL,
        PML4E_CACHE_DISABLE = 0x10ULL,
        PML4E_ACCESSED = 0x20ULL,
        PML4E_AVAILABLE_1 = 0x40ULL,
        PML4E_PAGE_SIZE = 0x80ULL,
        PML4E_AVAILABLE_2 = 0xF00ULL,
        PML4E_ADDRESS = 0x000FFFFFFFFFF000ULL,
        PML4E_AVAILABLE_3 = 0x7FF0000000000000ULL,
        PML4E_EXECUTE_DISABLE = 0x8000000000000000ULL
} PML4E_FLAGS;


uint64_t vmm_pte_get_frame(uint64_t entry) {
	return entry & PTE_FRAME;
}

void vmm_pte_set_frame(uint64_t* entry, uint64_t address) {
	*entry = (*entry & ~PTE_FRAME) | (address & PTE_FRAME);
}

bool vmm_pte_is_present(uint64_t entry) {
	return (entry & PTE_PRESENT) != 0;
}

uint64_t vmm_pde_get_address(uint64_t entry) {
	return entry & PDE_ADDRESS;
}

void vmm_pde_set_address(uint64_t* entry, uint64_t address) {
	*entry = (*entry & ~PDE_ADDRESS) | (address & PDE_ADDRESS);
}

bool vmm_pde_is_present(uint64_t entry) {
	return (entry & PDE_PRESENT) != 0;
}

uint64_t vmm_pdpte_get_address(uint64_t entry) {
	return entry & PDPTE_ADDRESS;
}

void vmm_pdpte_set_address(uint64_t* entry, uint64_t address) {
	*entry = (*entry & ~PDPTE_ADDRESS) | (address & PDPTE_ADDRESS);
}

bool vmm_pdpte_is_present(uint64_t entry) {
	return (entry & PDPTE_PRESENT) != 0;
}

uint64_t vmm_pml4e_get_address(uint64_t entry) {
	return entry & PML4E_ADDRESS;
}

void vmm_pml4e_set_address(uint64_t* entry, uint64_t address) {
	*entry = (*entry & ~PML4E_ADDRESS) | (address & PML4E_ADDRESS);
}

bool vmm_pml4e_is_present(uint64_t entry) {
	return (entry & PML4E_PRESENT) != 0;
}

void vmm_entry_add_flag(uint64_t* entry, uint64_t flag_mask) {
	*entry |= flag_mask;
}

void vmm_entry_clear_flag(uint64_t* entry, uint64_t flag_mask) {
	*entry &= ~flag_mask;
}

uint64_t vmm_physical_to_virtual(uint64_t physical_address) {
	if (vmm_init_mode) {
		return physical_address;
	}

	if (physical_address < 0x1000000) {
		return physical_address;
	}

	return physical_address + VMM_PHYSMAP_OFFSET - 0x1000000;
}

void vmm_flush_tlb_entry(uint64_t address) {
	__asm__ volatile ("cli");

	__asm__ volatile (
		"invlpg (%0)"
		:
		: "r" (address)
		: "memory"
	);

	__asm__ volatile ("sti");
}

uint64_t vmm_get_cr3(void) {
	uint64_t value;
	__asm__ volatile (
		"mov %%cr3, %0"
		: "=r" (value)
	);
	return value;
}

void vmm_set_cr3(uint64_t physical_address) {
	__asm__ volatile (
		"mov %0, %%cr3"
		:
		: "r" (physical_address)
		: "memory"
	);
}

bool vmm_alloc_page(uint64_t* entry) {
	void* block = pmm_alloc_block();

	if (!block) {
		return false;
	}

	vmm_pte_set_frame(entry, (uint64_t) block);
	vmm_entry_add_flag(entry, PTE_PRESENT);

	return true;
}

bool vmm_free_page(uint64_t* entry, uint64_t virtual_address) {
	uint64_t address = vmm_pte_get_frame(*entry);

	if (!address) {
		return false;
	}

	pmm_free_block((void*) address);
	*entry = 0;
	vmm_flush_tlb_entry(virtual_address);

	return true;
}

bool vmm_map_page(void* phys_addr, void* virt_addr, page_table_t* pml4) {
	uint64_t physical_address = (uint64_t) phys_addr;
	uint64_t virtual_address = (uint64_t) virt_addr;

	uint64_t pml4_index = ((uint64_t) virtual_address >> 39) & 0x1FF;
	uint64_t pml4_entry = pml4->entries[pml4_index];

	if (!vmm_pml4e_is_present(pml4_entry)) {
		void* new_pdpt_physical = pmm_alloc_block();

		if (!new_pdpt_physical) {
			return false;
		}

		uint64_t new_pdpt_virtual = vmm_physical_to_virtual((uint64_t) new_pdpt_physical);
		memset((void*) new_pdpt_virtual, 0, sizeof(page_table_t));

		vmm_pml4e_set_address(&pml4_entry, (uint64_t) new_pdpt_physical);
		vmm_entry_add_flag(&pml4_entry, PML4E_PRESENT);
		vmm_entry_add_flag(&pml4_entry, PML4E_READ_WRITE);

		pml4->entries[pml4_index] = pml4_entry;
	}

	uint64_t pdpt_index = (virtual_address >> 30) & 0x1FF;
	uint64_t pdpt_address = vmm_pml4e_get_address(pml4_entry);
	page_table_t* pdpt = (page_table_t*) vmm_physical_to_virtual((uint64_t) pdpt_address);
	uint64_t pdpt_entry = pdpt->entries[pdpt_index];

	if (!vmm_pdpte_is_present(pdpt_entry)) {
		void* new_pd_physical = pmm_alloc_block();

		if (!new_pd_physical) {
			return false;
		}

		uint64_t new_pd_virtual = vmm_physical_to_virtual((uint64_t) new_pd_physical);
		memset((void*) new_pd_virtual, 0, sizeof(page_table_t));

		vmm_pdpte_set_address(&pdpt_entry, (uint64_t) new_pd_physical);
		vmm_entry_add_flag(&pdpt_entry, PDPTE_PRESENT);
		vmm_entry_add_flag(&pdpt_entry, PDPTE_READ_WRITE);

		pdpt->entries[pdpt_index] = pdpt_entry;
	}

	uint64_t pd_index = (virtual_address >> 21) & 0x1FF;
	uint64_t pd_address = vmm_pdpte_get_address(pdpt_entry);
	page_table_t* pd = (page_table_t*) vmm_physical_to_virtual((uint64_t) pd_address);
	uint64_t pd_entry = pd->entries[pd_index];

	if (!vmm_pde_is_present(pd_entry)) {
		void* new_pt_physical = pmm_alloc_block();

		if (!new_pt_physical) {
			return false;
		}

		uint64_t new_pt_virtual = vmm_physical_to_virtual((uint64_t) new_pt_physical);
		memset((void*) new_pt_virtual, 0, sizeof(page_table_t));

		vmm_pde_set_address(&pd_entry, (uint64_t) new_pt_physical);
		vmm_entry_add_flag(&pd_entry, PDE_PRESENT);
		vmm_entry_add_flag(&pd_entry, PDE_READ_WRITE);

		pd->entries[pd_index] = pd_entry;
	}

	uint64_t pt_index = (virtual_address >> 12) & 0x1FF;
	uint64_t pt_address = vmm_pde_get_address(pd_entry);
	page_table_t* pt = (page_table_t*) vmm_physical_to_virtual((uint64_t) pt_address);
	uint64_t pt_entry = pt->entries[pt_index];

	if (!vmm_pte_is_present(pt_entry)) {
		vmm_pte_set_frame(&pt_entry, (uint64_t) physical_address);
		vmm_entry_add_flag(&pt_entry, PTE_PRESENT);
		vmm_entry_add_flag(&pt_entry, PTE_READ_WRITE);

		pt->entries[pt_index] = (uint64_t) pt_entry;
	} else {
		vmm_pte_set_frame(&pt_entry, (uint64_t) physical_address);

		pt->entries[pt_index] = (uint64_t) pt_entry;
	}

	return true;
}

bool vmm_unmap_page(uint64_t address, page_table_t* pml4) {
	uint64_t pml4_index = (address >> 39) & 0x1FF;
	uint64_t pml4_entry = pml4->entries[pml4_index];

	if (!vmm_pml4e_is_present(pml4_entry)) {
		return false;
	}

	uint64_t pdpt_index = (address >> 30) & 0x1FF;
	uint64_t pdpt_address = vmm_pml4e_get_address(pml4_entry);
	page_table_t* pdpt = (page_table_t*) vmm_physical_to_virtual((uint64_t) pdpt_address);
	uint64_t pdpt_entry = pdpt->entries[pdpt_index];

	if (!vmm_pdpte_is_present(pdpt_entry)) {
		return false;
	}

	uint64_t pd_index = (address >> 21) & 0x1FF;
	uint64_t pd_address = vmm_pdpte_get_address(pdpt_entry);
	page_table_t* pd = (page_table_t*) vmm_physical_to_virtual((uint64_t) pd_address);
	uint64_t pd_entry = pd->entries[pd_index];

	if (!vmm_pde_is_present(pd_entry)) {
		return false;
	}

	uint64_t pt_index = (address >> 12) & 0x1FF;
	uint64_t pt_address = vmm_pde_get_address(pd_entry);
	page_table_t* pt = (page_table_t*) vmm_physical_to_virtual((uint64_t) pt_address);
	uint64_t pt_entry = pt->entries[pt_index];

	if (!vmm_pte_is_present(pt_entry)) {
		return false;
	}

	uint64_t temp_entry = pt->entries[pt_entry];

	if (!vmm_free_page(&temp_entry, address)) {
		return false;
	}

	return true;
}

void vmm_init(void) {
	printf("vmm init starting\n");

	pmm_init_mode = true;
	vmm_init_mode = true;

	uint64_t old_pml4_physical = vmm_get_cr3();
	page_table_t* old_pml4 = (page_table_t*) vmm_physical_to_virtual(old_pml4_physical);

	void* new_pml4_physical = pmm_alloc_block();

	if (!new_pml4_physical) {
		printf("could not allocate pml4, halting\n");
		__asm__ volatile ("hlt");
	}

	page_table_t* new_pml4 = (page_table_t*) vmm_physical_to_virtual((uint64_t) new_pml4_physical);
	memset(new_pml4, 0, sizeof(page_table_t));

	printf("identity mapping address range 0x%x -> 0x%x\n", VMM_IDENTITYMAP_START, VMM_IDENTITYMAP_END);

	for (uint64_t physical_address = VMM_IDENTITYMAP_START; physical_address < VMM_IDENTITYMAP_END; physical_address += PMM_BLOCK_SIZE) {
		vmm_map_page((void*) physical_address, (void*) physical_address, new_pml4);
	}

	printf("mapping physmap address range 0x%x -> 0x%x to 0x%x -> 0x%x\n", VMM_PHYSMAP_START, pmm_memory_size, VMM_PHYSMAP_START + VMM_PHYSMAP_OFFSET, pmm_memory_size + VMM_PHYSMAP_OFFSET);

	for (uint64_t physical_address = VMM_PHYSMAP_START; physical_address < pmm_memory_size; physical_address += PMM_BLOCK_SIZE) {
		uint64_t virt = physical_address + VMM_PHYSMAP_OFFSET - VMM_PHYSMAP_START;
		vmm_map_page((void*) physical_address, (void*) virt, new_pml4);
	}

	vmm_set_cr3((uint64_t) new_pml4_physical);

	printf("new pml4 set\n");

	pmm_init_mode = false;
	vmm_init_mode = false;

	printf("vmm init done\n\n");
}
