#include "string.h"

#include "stdio.h"
#include "stdint.h"

void* memcpy(void* destination, const void* source, size_t count) {
	uint8_t* dest = (uint8_t*) destination;
	const uint8_t* src = (const uint8_t*) source;

	for (size_t i = 0; i < count; i++) {
		dest[i] = src[i];
	}

	return destination;
}

void* memmove(void* destination, const void* source, size_t count) {
	uint8_t* dest = (uint8_t*) destination;
	const uint8_t* src = (const uint8_t*) source;

	if (dest < src) {
		for (size_t i = 0; i < count; i++) {
			dest[i] = src[i];
		}
	} else if (dest > src) {
		for (size_t i = count; i != 0; i--) {
			dest[i - 1] = src[i - 1];
		}
	}

	return destination;
}

void* memset(void *pointer, int value, size_t count) {
	uint8_t* pointer_byte = (uint8_t*) pointer;

	for (size_t i = 0; i < count; i++) {
		*pointer_byte = (uint8_t) value;
		pointer_byte++;
	}

	return pointer;
}

void strcpy(char* destination, char* source) {
	memcpy((void*) destination, (void*) source, strlen(source) + 1);
}

size_t strlen(char* string) {
	unsigned long length = 0;

	while (string[length] != '\0') {
		length++;
	}

	return length;
}

uint8_t strcmp(char* string_1, char* string_2) {
	size_t length = strlen(string_1);

	if (length != strlen(string_2)) {
		return 0;
	}

	for (size_t i = 0; i < length; i++) {
		if (string_1[i] != string_2[i]) {
			return 0;
		}
	}

	return 1;
}

uint8_t strncmp(char* string_1, char* string_2, size_t count) {
	for (size_t i = 0; i < count; i++) {
		if (string_1[i] != string_2[i]) {
			return 0;
		}
	}

	return 1;
}

void strrev(char* string) {
	unsigned short start = 0;
	unsigned short end = strlen(string) - 1;

	while (start < end) {
		char character = string[start];
		string[start] = string[end];
		string[end] = character;
		start++;
		end--;
	}
}
