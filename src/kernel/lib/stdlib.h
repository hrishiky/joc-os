#ifndef STDLIB_H
#define STDLIB_H

#include "stdint.h"

#define NULL (void*) 0

void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);

ssize_t ll_to_str(long long number, char* buffer, unsigned char base);
ssize_t ull_to_str(unsigned long long number, char* buffer, unsigned char base);

int64_t str_to_ll(char* string, uint8_t base);
uint64_t str_to_ull(char* string, uint8_t base);

#endif
