#ifndef STRING_H
#define STRING_H

#include "stdint.h"

void* memcpy(void* destination, const void* source, size_t count);
void* memmove(void* destination, const void* source, size_t count);
void* memset(void* pointer, int value, size_t count);

void strcpy(char* destination, char* source);
size_t strlen(char* string);
uint8_t strcmp(char* string_1, char* string_2);
uint8_t strncmp(char* string_1, char* string_2, size_t length);
void strrev(char* string);

#endif
