#include "shell.h"

#include "stdio.h"
#include "stdint.h"
#include "pmm.h"

extern unsigned int pmm_memory_size;
extern unsigned int pmm_used_blocks;
extern unsigned int pmm_max_blocks;
extern unsigned int* pmm_memory_map;
extern Boot_Info* boot_info;

void shell_command_pmminfo(Shell_Arguments arguments) {
	printf("physical memory size: %d KiB\n", pmm_memory_size / 8192);
	printf("pmm block size: %d b; %d/%d blocks used, %d blocks free\n", PMM_BLOCK_SIZE, pmm_used_blocks, pmm_max_blocks, pmm_max_blocks - pmm_used_blocks);
	printf("pmm bitmap start: 0x%x\n", (uint64_t) pmm_memory_map);
}
