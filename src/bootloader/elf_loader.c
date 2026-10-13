#include "ata.h"
#include "elf.h"

#define KERNEL_START 0x26
#define KERNEL_SIZE 500
#define KERNEL_ADDRESS 0x200000

#define BOOT_INFO_ADDRESS 0x0000000000002000ULL

#define UCHAR_MAX 255

typedef struct __attribute__((packed)) {
	unsigned long base;
	unsigned long length;
	unsigned int type;
	unsigned int acpi_null;
} E820_Entry;

typedef struct __attribute__((packed)) {
	unsigned int entry_count;
	unsigned long entries;
} Boot_Info;

__attribute__((section(".start")))
void main(void) {
	Boot_Info *boot_info = (Boot_Info*) BOOT_INFO_ADDRESS;

	unsigned int lba = KERNEL_START;
	unsigned int count = KERNEL_SIZE;
	unsigned short* buffer = (unsigned short*) KERNEL_ADDRESS;

	unsigned int full_count = count / UCHAR_MAX;
	unsigned int remaining_count = count % UCHAR_MAX;

	if (full_count > 0) {
		for (unsigned int i = 0; i < full_count; i++) {
			ata_prepare_read(lba, UCHAR_MAX);
			ata_read(buffer, UCHAR_MAX);

			lba += UCHAR_MAX;
			buffer += UCHAR_MAX * 256;
		}
	}

	ata_prepare_read(lba, remaining_count);
	ata_read(buffer, remaining_count);

	void* elf = (void*)0x200000;

	elf_check_header(elf);
	elf_load_program_headers(elf);
	elf_jump_entry_point(elf, (void*) boot_info);
}
