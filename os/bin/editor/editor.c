#include "editor.h"

#include "file.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "vga_text.h"
#include "keyboard.h"

// cpu exception 0xd when edit is run on a non existing file in a folder -> fs_path_to_parent_inode

// add ansi type character color escapes to printf
// handle banner when file path is longer than 80 and needs to fit unsaved warning

extern uint8_t shell_foreground_color;
extern uint8_t shell_background_color;

editor_cursor_t editor_cursor;
editor_buffer_t editor_buffer;

void editor_main(char* path, size_t cwd, char* cwd_path) {
	file_t* file = file_open(path, cwd, cwd_path, "rw");

	if (file == NULL) {
		return;
	}

	file_seek(file, 0, FILE_SEEK_END);
	size_t file_size = file_tell(file);
	file_seek(file, 0, FILE_SEEK_START);

	// editor_buffer.buffer = malloc(file_size + EDITOR_GAP_BUFFER_SIZE);
	// file_read(file, editor_buffer.buffer, file_size);
	// editor_buffer.
	// editor_buffer.index = 0;
	// editor_buffer.bytes_used = 0;

	editor_cursor.line = 0;
	editor_cursor.column = 0;
	editor_cursor.index = 0;
	editor_cursor.top_line = 0;
	editor_cursor.preferred_column = 0;

	editor_print_banner(path);

	for (size_t i = 0; i < VGA_TEXT_ROWS_MAXIMUM; i++) {
		// editor_load_line(&editor_cursor, &editor_buffer, i);
	}

	while (1) {
		editor_handle_input();
	}

	file_close(file);
}

void editor_print_banner(char* path) {
	size_t banner_path_offset = (VGA_TEXT_COLUMNS - strlen(path)) / 2;

	vga_text_clear_screen();

	printf("\x1B_c[17;16]");

	for (size_t i = 0; i < banner_path_offset; i++) {
		printf(" ");
	}

	printf("%s", path);

	for (size_t i = 0; i < VGA_TEXT_COLUMNS - banner_path_offset - strlen(path); i++) {
		printf(" ");
	}

	printf("\x1B_r");
}

void editor_handle_input(void) {
	while (1) {
		uint16_t input = keyboard_get_keycode();

		switch (input) {
			case '\n':
				break;

			case '\b':
				// if at gap index, absolb char into gap buffer
				break;

			case KEYBOARD_UP_ARROW_SET:
				break;

			case KEYBOARD_DOWN_ARROW_SET:
				break;

			case KEYBOARD_LEFT_ARROW_SET:
				break;

			case KEYBOARD_RIGHT_ARROW_SET:
				break;

			default:
				char character = (char) input;
				break;
		}

		;
	}
}

// add buf size to accomodate backspace

void editor_gap_buffer_create(editor_buffer_t* buffer, size_t index) {
	if (buffer->bytes_remaining > 0) {
		editor_gap_buffer_close(buffer);
		memmove((void*) &buffer->buffer[index + buffer->bytes_remaining], (void*) &buffer->buffer[index], buffer->buffer_size - (index + buffer->bytes_remaining));
		memset((void*) &buffer->buffer[index], UINT8_MAX, buffer->bytes_remaining);
		buffer->index = index;
	} else {
		uint8_t* new_buffer = malloc(buffer->buffer_size + EDITOR_GAP_BUFFER_SIZE);

		if (new_buffer == NULL) {
			return; // error
		}

		memcpy((void*) new_buffer, (void*) buffer->buffer, buffer->buffer_size);
		memmove((void*) &new_buffer[index + EDITOR_GAP_BUFFER_SIZE], (void*) &new_buffer[index], buffer->buffer_size - index);
		memset((void*) &new_buffer[index], UINT8_MAX, EDITOR_GAP_BUFFER_SIZE);
		free(buffer->buffer);

		buffer->buffer = new_buffer;
		buffer->buffer_size += EDITOR_GAP_BUFFER_SIZE;
		buffer->index = index;
		buffer->bytes_remaining = 500;
	}
}

void editor_gap_buffer_close(editor_buffer_t* buffer) {
	memmove((void*) &buffer->buffer[buffer->index + (EDITOR_GAP_BUFFER_SIZE - buffer->bytes_remaining)], (void*) &buffer->buffer[buffer->index + EDITOR_GAP_BUFFER_SIZE], buffer->bytes_remaining);
}

void editor_load_line(editor_cursor_t* cursor, editor_buffer_t* buffer, size_t line) {
	if (line < cursor->top_line || line > cursor->top_line + VGA_TEXT_ROWS_MAXIMUM) {
		return;
	}

	size_t buffer_index = 0;
	size_t lines_read = 0;

	for (size_t i = 0; i < buffer->buffer_size; i++) {
		if (buffer->buffer[i] == '\n') {
			lines_read++;

			if (lines_read == line - 1) {
				break;
			}
		}

		buffer_index++;
	}

	buffer_index++;

	size_t line_start = buffer_index;
	size_t line_length = 0;

	while (buffer_index <= buffer->buffer_size) {
		if (buffer->buffer[line_start] == '\n') {
			break;
		}

		line_length++;
	}

	uint8_t vga_line = (line - cursor->top_line) + 1;

	if (line_length > VGA_TEXT_COLUMNS_MAXIMUM) {
		for (uint8_t i = 0; i < VGA_TEXT_COLUMNS_MAXIMUM; i++) {
			// vga_text_print_character_position(buffer->buffer[line_start + i], i, vga_line);
		}

		// vga_text_print_character_position(EDITOR_LINE_OVERFLOW_CHARACTER, VGA_TEXT_COLUMNS_MAXIMUM, vga_line);
	} else {
		for (uint8_t i = 0; i < line_length; i++) {
			// vga_text_print_character_position(buffer->buffer[line_start + i], i, vga_line);
		}
	}
}

void editor_save_file(file_t* file, editor_buffer_t* buffer) {
	;
}
