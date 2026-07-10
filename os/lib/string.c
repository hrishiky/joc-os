#include "string.h"

void* memcpy(void* destination, const void* source, unsigned long count) {
	unsigned char* destination_bytes = (unsigned char*) destination;
	const unsigned char* source_bytes = (const unsigned char*) source;

	for (unsigned long i = 0; i < count; i++) {
		destination_bytes[i] = source_bytes[i];
	}

	return destination;
}

void* memset(void *pointer, int value, unsigned long count) {
	unsigned char* pointer_byte = (unsigned char*) pointer;

	for (unsigned long i = 0; i < count; i++) {
		*pointer_byte = (unsigned char) value;
		pointer_byte++;
	}

	return pointer;
}

unsigned long strlen(char* string) {
	unsigned long length = 0;

	while (string[length] != '\0') {
		length++;
	}

	return length;
}

unsigned char strcmp(char* string_1, char* string_2) {
	unsigned long length = strlen(string_1);

	if (length != strlen(string_2)) {
		return 0;
	}

	for (unsigned long i = 0; i < length; i++) {
		if (string_1[i] != string_2[i]) {
			return 0;
		}
	}

	return 1;
}

unsigned char strcmp_partial(char* string_1, char* string_2, unsigned long length) {
	for (unsigned long i = 0; i < length; i++) {
		if (string_1[i] != string_2[i]) {
			return 0;
		}
	}

	return 1;
}

long long strtol(char* string, unsigned char base) {
	unsigned short i = 0;
	long long number = 0;
	unsigned char is_negative = 0;

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
		unsigned char character = string[i];
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

/*
char* string_copy(char* string, char* string_copy, unsigned long start, unsigned long end) {
	for (unsigned long i = 0; i < (end - start); i++) {
		string_copy[i] = string[start + i];
	}

	return string_copy;
}

char* string_append_character(char* string, char character) {
	unsigned long length = string_length(string);

	string[length] = character;
	string[length + 1] = '\0';

	return string;
}

char* string_append(char* string, char* string_append) {
	unsigned long length = string_length(string);
	unsigned long length_append = string_length(string_append);

	for (unsigned long i = 0; i < length_append; i++) {
		string[length + i] = string_append[i];
	}

	string[length + length_append] = '\0';

	return string;
}

char* string_delete(char* string, unsigned long start, unsigned long end) {
	unsigned long tail_length = string_length(string) - end;

	for (unsigned long i = 0; i <= tail_length; i++) {
		string[start + i] = string[end + i];
	}

	return string;
}
*/
