#include "stdlib.h"
#include "stdio.h"
#include "shell.h"
#include "heap.h"

#include "string.h"

extern heap_free_block_t* heap_free_list[];

void shell_command_heapinfo(Shell_Arguments arguments) {
	// char* num = arguments.argv[1];
	// heap_alloc(strtol(num, 10));

	printf("heap range: 0x%x -> 0x%x; heap size: %u KiB\n\n", HEAP_START, HEAP_END, HEAP_SIZE / 8192);
	printf("heap status:");

	for (uint8_t order = 0; order <= HEAP_MAX_ORDER; order++) {
		printf("\n\tblocks of order %d: ", order);

		heap_free_block_t* current_block = heap_free_list[order];
		uint8_t count = 0;

		if (current_block != NULL) {
			printf("%p", (void*) current_block);
			current_block = (heap_free_block_t*) current_block->next;
			count++;
		}

		while (current_block != NULL) {
			printf(" -> %p", (void*) current_block);
			current_block = (heap_free_block_t*) current_block->next;
			count++;
		}

		if (count == 0) {
			printf("none");
		} else if (count == 1) {
			printf("; 1 block");
		} else {
			printf("; %d blocks", count);
		}
	}
}
