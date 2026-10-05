#include "stdio.h"

#include "idt.h"
#include "fs.h"
#include "shell.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"

void main(void* boot_info) {
	printf("kernel loaded\n");

	// __asm__ ("hlt");
	idt_init();

	// __asm__ ("hlt");
	pmm_init(boot_info);

	// __asm__ ("hlt");
	vmm_init();

	// __asm__ ("hlt");
	heap_init();

	// __asm__ ("hlt");
	fs_init();

	// __asm__ ("hlt");
	shell_main();

	printf("\nkernel halting\n");
	__asm__("hlt");
}
