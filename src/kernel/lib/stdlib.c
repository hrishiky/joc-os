#include "stdlib.h"

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "heap.h"

void* malloc(size_t size) {
	return heap_alloc(size);
}

void free(void* ptr) {
	heap_free(ptr);
}

void* realloc(void* ptr, size_t size) {
	heap_free(ptr);
	return heap_alloc(size);
}

ssize_t ll_to_str(long long number, char* buffer, unsigned char base) {
	char* base_lookup = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (base < 2 || base > 36) {
		return -1;
	}

	if (number == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
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

	return i;
}

ssize_t ull_to_str(unsigned long long number, char* buffer, unsigned char base) {
	char* base_lookup = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (base < 2 || base > 36) {
		return -1;
	}

	if (number == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
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

	return i;
}

int64_t str_to_ll(char* string, uint8_t base) {
	size_t i = 0;
	int64_t number = 0;
	bool is_negative = 0;

	while (string[i] == ' ') {
		i++;
	}

	if (string[i] == '-') {
		is_negative = 1;
		i++;
	}

	if (base == 0) {
		if (string[i] == '0') {
			if (string[i + 1] == 'x' ||
				string[i + 1] == 'X') {
				base = 16;
				i += 2;
			} else {
				base = 8;
				i++;
			}
		} else {
			base = 10;
		}
	}

	for (; i < strlen(string); i++) {
		uint8_t character = string[i];
		int digit;

		if (character >= '0' &&
		    character <= '9') {
			digit = character - '0';
		} else if (character >= 'A' &&
			   character <= 'F') {
			digit = character - 'A' + 10;
		} else if (character >= 'a' &&
			character <= 'f') {
			digit = character - 'a' + 10;
		} else {
			break;
		}

		if (digit >= base) {
			break;
		}

		number = number * base + digit;
	}

	if (is_negative) {
		number *= -1;
	}

	return number;
}

uint64_t str_to_ull(char* string, uint8_t base) {
	unsigned short i = 0;
	uint64_t number = 0;

	while (string[i] == ' ') {
		i++;
	}

	if (base == 0) {
		if (string[i] == '0') {
			if (string[i + 1] == 'x' ||
				string[i + 1] == 'X') {
				base = 16;
				i += 2;
			} else {
				base = 8;
				i++;
			}
		} else {
			base = 10;
		}
	}

	for (; i < strlen(string); i++) {
		uint8_t character = string[i];
		int digit;

		if (character >= '0' &&
		    character <= '9') {
			digit = character - '0';
		} else if (character >= 'A' &&
			   character <= 'F') {
			digit = character - 'A' + 10;
		} else if (character >= 'a' &&
			character <= 'f') {
			digit = character - 'a' + 10;
		} else {
			break;
		}

		if (digit >= base) {
			break;
		}

		number = number * base + digit;
	}

	return number;
}
