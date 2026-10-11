#ifndef EDITOR_H
#define EDITOR_H

#include "stdint.h"

#define EDITOR_GAP_BUFFER_SIZE 512
#define EDITOR_LINE_OVERFLOW_CHARACTER '>'

typedef struct {
	size_t line;
	size_t column;
	size_t index;
	size_t top_line;
	size_t preferred_column;
} editor_cursor_t;

typedef struct {
	uint8_t* buffer;
	size_t buffer_size;
	size_t index;
	size_t bytes_remaining;
} editor_buffer_t;

void editor_main(char* path, size_t cwd, char* cwd_path);

void editor_print_banner(char* path);
void editor_handle_input(void);

void editor_gap_buffer_create(editor_buffer_t* buffer, size_t index);
void editor_gap_buffer_close(editor_buffer_t* buffer);

void editor_load_line(editor_cursor_t* cursor, editor_buffer_t* buffer, size_t line);

#endif
