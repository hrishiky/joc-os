#ifndef STDLIB_H
#define STDLIB_H

#include "stdint.h"

#define NULL (void*) 0

void* malloc(size_t size);
void free(void* ptr);

char* itoa_ll(long long number, char* buffer, unsigned char base);
char* itoa_ull(unsigned long long number, char* buffer, unsigned char base);

#endif
