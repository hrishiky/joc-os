#ifndef STRING_H
#define STRING_H

void* memcpy(void* destination, const void* source, unsigned long count);
void* memset(void* pointer, int value, unsigned long count);

unsigned long strlen(char* string);
unsigned char strcmp(char* string_1, char* string_2);
unsigned char strcmp_partial(char* string_1, char* string_2, unsigned long length);
long long strtol(char* string, unsigned char base);
void strrev(char* string);

// char* string_copy(char* string, char* string_copy, unsigned long start, unsigned long end);
// char* string_append_character(char* string, char character);
// char* string_append(char* string, char* string_append);
// char* string_delete(char* string, unsigned long start, unsigned long end);

#endif
