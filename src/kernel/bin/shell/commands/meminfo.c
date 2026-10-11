#include "shell.h"

#include "stdio.h"
#include "pmm.h"

extern Boot_Info* boot_info;

void shell_command_meminfo(Shell_Arguments arguments) {
	E820_Entry* memory_map = (E820_Entry*) boot_info->entries;

	printf("memory layout:\n");

	for (unsigned char i = 0; i < boot_info->entry_count; i++) {
		printf("\tregion %d: base = 0x%x; length = 0x%x; type = %d\n", i + 1, memory_map[i].base, memory_map[i].length, memory_map[i].type);
	}
}
