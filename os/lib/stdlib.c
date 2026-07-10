#include "stdlib.h"

#include "stdint.h"
#include "string.h"
#include "heap.h"

void* malloc(size_t size) {
	return heap_alloc(size);
}

void free(void* ptr) {
	heap_free(ptr);
}

char* itoa_ll(long long number, char* buffer, unsigned char base) {
	char* base_lookup = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (base < 2 ||
	    base > 36) {
		return buffer;
	}

	if (number == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return (char*) buffer;
	}

	unsigned char digit;
	unsigned short i = 0;
	unsigned char negative = 0;

	if (number < 0) {
		negative = 1;
		number *= -1;
	}

	while (number != 0) {
		digit = number % base;
		buffer[i] = base_lookup[digit];
		number /= base;
		i++;
	}

	if (negative) {
		buffer[i++] = '-';
	}

	buffer[i] = '\0';
	strrev(buffer);

	return buffer;
}

char* itoa_ull(unsigned long long number, char* buffer, unsigned char base) {
	char* base_lookup = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (base < 2 ||
	    base > 36) {
		return buffer;
	}

	if (number == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return (char*) buffer;
	}

	unsigned char digit;
	unsigned short i = 0;

	while (number != 0) {
		digit = number % base;
		buffer[i] = base_lookup[digit];
		number /= base;
		i++;
	}

	buffer[i] = '\0';
	strrev(buffer);

	return buffer;
}
