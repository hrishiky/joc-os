#include "stdio.h"

#include "idt.h"
#include "fs.h"
#include "shell.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"

void main(void* boot_info) {
	idt_init();

	pmm_init(boot_info);

	vmm_init();

	heap_init();

	fs_init();

	shell_main();

	printf("\nkernel halting\n");
	__asm__("hlt");
}
