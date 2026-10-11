#include "elf.h"

void elf_check_header(void* elf) {
	Elf64_Ehdr* ehdr = (Elf64_Ehdr*)elf;

	if (ehdr->e_ident[0] != 0x7F ||
	    ehdr->e_ident[1] != 'E' ||
	    ehdr->e_ident[2] != 'L' ||
	    ehdr->e_ident[3] != 'F' ) {
		__asm__("hlt");
	}

	if (ehdr->e_ident[4] != ELF_CLASS64) {
		__asm__("hlt");
	}

	if (ehdr->e_ident[5] != ELF_LITTLE_ENDIAN) {
		__asm__("hlt");
	}

	if (ehdr->e_ident[6] != ELF_VERSION) {
		__asm__("hlt");
	}

	if (ehdr->e_type != ELF_ET_EXEC &&
	    ehdr->e_type != ELF_ET_DYN) {
		__asm__("hlt");
	}

	if (ehdr->e_machine != ELF_EM_X86_64) {
		__asm__("hlt");
	}

	if (ehdr->e_ehsize != ELF_EHDR_SIZE) {
		__asm__("hlt");
	}

	if (ehdr->e_phentsize != ELF_PHDR_SIZE) {
		__asm__("hlt");
	}

	if (ehdr->e_phnum < ELF_PHDR_MINIMUM_COUNT) {
		__asm__("hlt");
	}
}

void elf_load_program_headers(void* elf) {
	Elf64_Ehdr* ehdr = (Elf64_Ehdr*) elf;
	Elf64_Phdr* phdrs = (Elf64_Phdr*)((unsigned char*) elf + ehdr->e_phoff);

	for (unsigned short i = 0; i < ehdr->e_phnum; i++) {
		if (phdrs[i].p_type != ELF_PT_LOAD) {
			continue;
		}

		unsigned char* segment_data = (unsigned char*)elf + phdrs[i].p_offset;
		unsigned char* segment_destination = (unsigned char*)phdrs[i].p_vaddr;

		for (unsigned long j = 0; j < phdrs[i].p_filesz; j++ ) {
			segment_destination[j] = segment_data[j];
		}

		if (phdrs[i].p_memsz > phdrs[i].p_filesz) {
			for (unsigned long k = phdrs[i].p_filesz; k < phdrs[i].p_memsz; k++) {
				segment_destination[k] = 0;
			}
		}
	}
}

void elf_jump_entry_point(void* elf, void* boot_info) {
	Elf64_Ehdr* ehdr = (Elf64_Ehdr*) elf;

	void (*entry_point)(void*) = (void(*)(void*)) ehdr->e_entry;

	entry_point(boot_info);
}
