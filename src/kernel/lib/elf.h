#ifndef ELF_H
#define ELF_H

#define ELF_CLASS64 2
#define ELF_LITTLE_ENDIAN 1
#define ELF_VERSION 1
#define ELF_ET_EXEC 2
#define ELF_ET_DYN 3
#define ELF_EM_X86_64 62
#define ELF_EHDR_SIZE 64
#define ELF_PHDR_SIZE 56
#define ELF_PT_LOAD 1
#define ELF_IDENT_SIZE 16

#define ELF_PHDR_MINIMUM_COUNT 1

#define ELF_ERROR_MESSAGE_INVALID_HEADER "elf header is invalid"


typedef struct __attribute__((packed)) {
	unsigned char e_ident[ELF_IDENT_SIZE];
	unsigned short e_type;
	unsigned short e_machine;
	unsigned int e_version;
	unsigned long e_entry;
	unsigned long e_phoff;
	unsigned long e_shoff;
	unsigned int e_flags;
	unsigned short e_ehsize;
	unsigned short e_phentsize;
	unsigned short e_phnum;
	unsigned short e_shentsize;
	unsigned short e_shnum;
	unsigned short e_shstrndx;
} Elf64_Ehdr;

typedef struct __attribute__((packed)) {
	unsigned int p_type;
	unsigned int p_flags;
	unsigned long p_offset;
	unsigned long p_vaddr;
	unsigned long p_paddr;
	unsigned long p_filesz;
	unsigned long p_memsz;
	unsigned long p_align;
} Elf64_Phdr;


void elf_check_header(void* elf);
void elf_load_program_headers(void* elf);
void elf_jump_entry_point(void* elf);

#endif
