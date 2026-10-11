#ifndef CTYPE_H
#define CTYPE_H

#include "stdint.h"
#include "stdbool.h"

#define CHARACTER_CASE_OFFSET 32

bool is_digit(char character);
bool is_whitespace(char character);

void to_uppercase(char* string, size_t count);
void to_lowercase(char* string, size_t count);

#endif
