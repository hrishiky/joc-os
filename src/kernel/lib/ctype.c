#include "ctype.h"

#include "stdint.h"
#include "stdbool.h"

bool is_digit(char character) {
	if (character >= '0' && character <= '9') {
		return true;
	}

	return false;
}

bool is_whitespace(char character) {
	switch (character) {
		case ' ':
			return true;

		case '\t':
			return true;

		case '\n':
			return true;

		case '\v':
			return true;

		case '\f':
			return true;

		case '\r':
			return true;

		default:
			return false;
	}
}

void to_uppercase(char* string, size_t count) {
	for (size_t i = 0; i < count; i++) {
		if (string[i] < 'z' && string[i] > 'a') {
			string[i] += CHARACTER_CASE_OFFSET;
		}
	}
}

void to_lowercase(char* string, size_t count) {
	for (size_t i = 0; i < count; i++) {
		if (string[i] < 'Z' && string[i] > 'A') {
			string[i] -= CHARACTER_CASE_OFFSET;
		}
	}
}
