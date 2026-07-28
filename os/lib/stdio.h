#ifndef STDIO_H
#define STDIO_H

#include "stdint.h"
#include "stdbool.h"

#include <stdarg.h>

#define PRINT_BUFFER_SIZE 128

#define FORMAT_SPECIFIER_DELIMITER '%'

#define FORMAT_FLAG_LEFT_ALIGN '-'
#define FORMAT_FLAG_SIGN '+'
#define FORMAT_FLAG_SPACE_PREFIX ' '
#define FORMAT_FLAG_BASE_PREFIX '#'
#define FORMAT_FLAG_ZERO_PADDING '0'
#define FORMAT_FLAG_SKIP '*'

#define FORMAT_WIDTH_NONE -1

#define FORMAT_PRECISION_DELIMITER '.'
#define FORMAT_PRECISION_NONE -1

#define FORMAT_LENGTH_SHORT 'h'
#define FORMAT_LENGTH_LONG 'l'
#define FORMAT_LENGTH_MAX 'z'

#define FORMAT_SPECIFIER_ESCAPE '%'
#define FORMAT_SPECIFIER_CHARACTER 'c'
#define FORMAT_SPECIFIER_STRING 's'
#define FORMAT_SPECIFIER_INTEGER_1 'd'
#define FORMAT_SPECIFIER_INTEGER_2 'i'
#define FORMAT_SPECIFIER_UNSIGNED_INTEGER 'u'
#define FORMAT_SPECIFIER_HEXADECIMAL_1 'x'
#define FORMAT_SPECIFIER_HEXADECIMAL_2 'X'
#define FORMAT_SPECIFIER_POINTER 'p'

#define PRINT_ESCAPE_CHARACTER '\x1B'
#define PRINT_ESCAPE_FORMAT "[%hhu;%hhu]"

typedef enum {
	FORMAT_PARSE_TYPE_PRINT,
	FORMAT_PARSE_TYPE_SCAN
} format_parse_t;

typedef enum {
	FORMAT_LENGTH_NONE,
	FORMAT_LENGTH_HH,
	FORMAT_LENGTH_H,
	FORMAT_LENGTH_L,
	FORMAT_LENGTH_LL,
	FORMAT_LENGTH_Z
} format_length_t;

typedef enum {
	FORMAT_SPECIFIER_ESC,
	FORMAT_SPECIFIER_CHAR,
	FORMAT_SPECIFIER_STR,
	FORMAT_SPECIFIER_INT,
	FORMAT_SPECIFIER_UINT,
	FORMAT_SPECIFIER_HEX,
	FORMAT_SPECIFIER_PTR
} format_specifier_t;

typedef struct {
	bool left_align;
	bool sign;
	bool space_prefix;
	bool base_prefix;
	bool zero_padding;

	bool skip;

	int8_t width;

	int8_t precision;

	format_length_t length;
	format_specifier_t specifier;
} format_wrapper_t;

ssize_t printf(char* format, ...);
ssize_t vsnprintf(char* buffer, size_t count, char* format, va_list args);

ssize_t sscanf(char* string, char* format, ...);

size_t format_parse(char* format, format_wrapper_t* format_wrapper, size_t offset, format_parse_t type);

#endif
