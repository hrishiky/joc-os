#include "asm_wrappers.h"

void outb(unsigned char data, unsigned short port) {
	__asm__ volatile (
		"outb %0, %1"
		:
		: "a"(data), "d"(port)
	);
}

void outw(unsigned short data, unsigned short port) {
	__asm__ volatile (
		"outw %0, %1"
		:
		: "a"(data), "d"(port)
	);
}


unsigned char inb(unsigned short port) {
	unsigned char data;

	__asm__ volatile (
	"inb %1, %0"
		: "=a"(data)
		: "d"(port)
	);

	return data;
}

void rep_insw(unsigned short* buffer, unsigned short count, unsigned short port) {
	__asm__ volatile (
		"rep insw"
		: "+D"(buffer), "+c"(count)
		: "d"(port)
		: "memory"
	);
}

void halt(void) {
	__asm__ volatile ("hlt");
}
