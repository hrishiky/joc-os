#include "stdio.h"

#include "stdlib.h"
#include "stdint.h"
#include "ctype.h"
#include "string.h"
#include "vga_text.h"

#include <stdarg.h>

extern uint8_t vga_text_cursor_x;
extern uint8_t vga_text_cursor_y;
extern uint8_t vga_text_foreground_color;
extern uint8_t vga_text_background_color;

// scan: %d should force into decimal, %i detects base '0' octal '0x' / '0X' hex
// fix reading outside of string buffer for printf / sscanf

ssize_t printf(char* format, ...) {
	va_list args;
	va_start(args, format);

	size_t format_length = strlen(format);

	char* fstring = malloc(format_length + PRINT_BUFFER_SIZE);
	size_t buffer_size = format_length + PRINT_BUFFER_SIZE;

	ssize_t characters_written = vsnprintf(fstring, buffer_size, format, args);

	while (characters_written >= buffer_size) {
		buffer_size += PRINT_BUFFER_SIZE;
		fstring = realloc(fstring, buffer_size);

		if (fstring == NULL) {
			return -1;
		}

		va_end(args);
		va_start(args, format);

		characters_written = vsnprintf(fstring, buffer_size, format, args);

		if (characters_written == -1) {
			free(fstring);
			return -1;
		}
	}

	size_t fstring_length = strlen(fstring);
	ssize_t characters_printed = 0;

	for (size_t i = 0; i < fstring_length; i++) {
		if (fstring[i] == PRINT_ESCAPE_CHARACTER) {
			if (fstring[i + 1] != '_') {
				break;
			}

			switch (fstring[i + 2]) {
				case 'p':
					uint8_t x_position, y_position;
					size_t p_assigments = sscanf(&fstring[i + 3], PRINT_ESCAPE_FORMAT, &x_position, &y_position);

					if (p_assigments != 2) {
						break;
					}

					if (x_position > VGA_TEXT_COLUMNS_MAXIMUM || y_position > VGA_TEXT_ROWS_MAXIMUM) {
						break;
					}

					vga_text_cursor_x = x_position;
					vga_text_cursor_y = y_position;

					while (fstring[i] != ']') {
						i++;
					}

					break;

				case 'c':
					uint8_t foreground_color, background_color;
					size_t c_assigments = sscanf(&fstring[i + 3], PRINT_ESCAPE_FORMAT, &foreground_color, &background_color);

					if (c_assigments != 2) {
						break;
					}

					if (foreground_color > VGA_TEXT_COLOR_CODE_MAXIMUM || background_color > VGA_TEXT_COLOR_CODE_MAXIMUM) {
						break;
					}

					uint8_t cur_fg_color = vga_text_foreground_color;
					uint8_t cur_bg_color = vga_text_background_color;

					if (foreground_color <= VGA_TEXT_COLOR_MAXIMUM) {
						vga_text_foreground_color = foreground_color;
					} else {
						if (foreground_color == VGA_TEXT_COLOR_CODE_BACKGROUND) {
							vga_text_foreground_color = cur_bg_color;
						}
					}

					if (background_color <= VGA_TEXT_COLOR_MAXIMUM) {
						vga_text_background_color = background_color;
					} else {
						if (background_color == VGA_TEXT_COLOR_CODE_FOREGROUND) {
							vga_text_background_color = cur_fg_color;
						}
					}


					// worse than epstein
					while (fstring[i] != ']') {
						i++;
					}

					break;

				case 'r':
					vga_text_foreground_color = VGA_TEXT_FOREGROUND_COLOR_DEFAULT;
					vga_text_background_color = VGA_TEXT_BACKGROUND_COLOR_DEFAULT;
					i += 2;

					break;

				case 'x':
					vga_text_clear_screen();
					i += 2;

					break;
			}

		} else {
			if (fstring[i] == '\0') {
				break;
			}

			vga_text_print_character(fstring[i]);
		}
	}

	free(fstring);
	va_end(args);

	return characters_printed;
}

ssize_t vsnprintf(char* buffer, size_t count, char* format, va_list args) {
	size_t format_length = strlen(format);
	size_t format_index = 0;
	size_t buffer_index = 0;

	format_wrapper_t format_wrapper;

	char string_buffer[PRINT_BUFFER_SIZE];
	size_t string_size;

	bool overflow = false;

	while(1) {
		if (format[format_index] == FORMAT_SPECIFIER_ESCAPE) {
			format_index = format_parse(format, &format_wrapper, format_index, FORMAT_PARSE_TYPE_PRINT);

			if (!format_index) {
				return -1;
			}

			switch (format_wrapper.specifier) {
				case FORMAT_SPECIFIER_ESCAPE:
					string_buffer[0] = FORMAT_SPECIFIER_ESCAPE;
					string_size = 1;

					break;

				case FORMAT_SPECIFIER_CHARACTER:
					char character = (char) va_arg(args, int);

					if (format_wrapper.width == FORMAT_WIDTH_NONE) {
						string_buffer[0] = character;
						string_size = 1;
					} else {
						for (size_t i = 0; i < format_wrapper.width; i++) {
							string_buffer[i] = ' ';
						}

						if (format_wrapper.left_align) {
							string_buffer[0] = character;
						} else {
							string_buffer[format_wrapper.width - 1] = character;
						}

						string_size = format_wrapper.width;
					}

					break;

				case FORMAT_SPECIFIER_STRING:
					char* str = va_arg(args, char*);
					size_t str_length = strlen(str);

					if (format_wrapper.precision != FORMAT_PRECISION_NONE) {
						if (str_length > format_wrapper.precision) {
							str_length = format_wrapper.precision;
						}
					}

					if (format_wrapper.width == FORMAT_WIDTH_NONE) {
						memcpy((void*) string_buffer, (void*) str, str_length);
						string_size = str_length;
					} else {
						for (size_t i = 0; i < format_wrapper.width; i++) {
							string_buffer[i] = ' ';
						}

						if (format_wrapper.left_align) {
							memcpy((void*) string_buffer, (void*) str, str_length);
						} else {
							memcpy((void*) &string_buffer[(format_wrapper.width - 1) - (str_length - 1)], (void*) str, str_length);
						}

						string_size = format_wrapper.width;
					}

					break;

				case FORMAT_SPECIFIER_INTEGER_1:
				case FORMAT_SPECIFIER_INTEGER_2:
					int64_t integer = va_arg(args, long long);

					switch (format_wrapper.length) {
						case FORMAT_LENGTH_NONE:
							integer = (int) integer;
							break;

						case FORMAT_LENGTH_HH:
							integer = (char) integer;
							break;

						case FORMAT_LENGTH_H:
							integer = (short) integer;
							break;

						case FORMAT_LENGTH_L:
							integer = (long) integer;
							break;

						case FORMAT_LENGTH_LL:
							integer = (long long) integer;
							break;

						case FORMAT_LENGTH_Z:
							integer = (ssize_t) integer;
							break;
					}

					bool i_is_negative = integer < 0;

					char integer_buffer[PRINT_BUFFER_SIZE];
					size_t integer_index;
					ssize_t i_digits_written = ll_to_str(integer, (void*) integer_buffer, 10);

					if (i_digits_written == -1) {
						return -1;
					}

					char i_sign;

					if (!i_is_negative) {
						i_sign = '-';
						i_digits_written--;
						memmove((void*) integer_buffer, (void*) &integer_buffer[1], i_digits_written);
					} else {
						if (format_wrapper.sign) {
							i_sign = '+';
						} else if (format_wrapper.space_prefix) {
							i_sign = ' ';
						} else {
							i_sign = -1;
						}
					}

					bool i_zero_pad = !format_wrapper.left_align &&
							  format_wrapper.zero_padding &&
							  (format_wrapper.width != FORMAT_WIDTH_NONE);

					size_t i_zero_pad_count = 0;

					if (format_wrapper.width != FORMAT_WIDTH_NONE) {
						ssize_t absolute_width = format_wrapper.width - i_digits_written;

						if (absolute_width > 0) {
							i_zero_pad_count = absolute_width;
						}
					}

					if (format_wrapper.width == FORMAT_WIDTH_NONE || i_zero_pad_count == 0) {
						if (i_sign == -1) {
							memcpy((void*) string_buffer, (void*) integer_buffer, i_digits_written);
							string_size = i_digits_written;
						} else {
							string_buffer[0] = i_sign;
							memcpy((void*) &string_buffer[1], (void*) integer_buffer, i_digits_written);
							string_size = i_digits_written + 1;
						}

						break;
					}

					char i_pad_character = ' ';

					if (i_zero_pad) {
						i_pad_character = '0';
					}

					for (size_t i = 0; i < format_wrapper.width; i++) {
						string_buffer[i] = i_pad_character;
					}

					if (i_zero_pad) {
						if (i_sign != -1) {
							string_buffer[0] = i_sign;
						}

						memcpy((void*) &string_buffer[format_wrapper.width - i_digits_written], (void*) integer_buffer, i_digits_written);
					} else {
						if (format_wrapper.left_align) {
							if (i_sign != -1) {
								string_buffer[0] = i_sign;
								memcpy((void*) &string_buffer[1], (void*) integer_buffer, i_digits_written);
							} else {
								memcpy((void*) string_buffer, (void*) integer_buffer, i_digits_written);
							}
						} else {
							if (i_sign != -1) {
								string_buffer[0] = i_sign;
							}

							memcpy((void*) &string_buffer[format_wrapper.width - i_digits_written], (void*) integer_buffer, i_digits_written);
						}
					}

					string_size = format_wrapper.width;

					break;

				case FORMAT_SPECIFIER_UNSIGNED_INTEGER:
					uint64_t unsigned_integer = va_arg(args, long long);

					switch (format_wrapper.length) {
						case FORMAT_LENGTH_NONE:
							unsigned_integer = (unsigned int) unsigned_integer;
							break;

						case FORMAT_LENGTH_HH:
							unsigned_integer = (unsigned char) unsigned_integer;
							break;

						case FORMAT_LENGTH_H:
							unsigned_integer = (unsigned short) unsigned_integer;
							break;

						case FORMAT_LENGTH_L:
							unsigned_integer = (unsigned long) unsigned_integer;
							break;

						case FORMAT_LENGTH_LL:
							unsigned_integer = (unsigned long long) unsigned_integer;
							break;

						case FORMAT_LENGTH_Z:
							unsigned_integer = (size_t) unsigned_integer;
							break;
					}

					char unsigned_integer_buffer[PRINT_BUFFER_SIZE];
					size_t unsigned_integer_index;
					ssize_t u_digits_written = ll_to_str(unsigned_integer, (void*) unsigned_integer_buffer, 10);

					if (u_digits_written == -1) {
						return -1;
					}

					char u_sign;

					if (format_wrapper.sign) {
						u_sign = '+';
					} else if (format_wrapper.space_prefix) {
						u_sign = ' ';
					} else {
						u_sign = -1;
					}

					bool u_zero_pad = !format_wrapper.left_align &&
							  format_wrapper.zero_padding &&
							  (format_wrapper.width != FORMAT_WIDTH_NONE);

					size_t u_zero_pad_count = 0;

					if (format_wrapper.width != FORMAT_WIDTH_NONE) {
						ssize_t absolute_width = format_wrapper.width - u_digits_written;

						if (absolute_width > 0) {
							u_zero_pad_count = absolute_width;
						}
					}

					if (format_wrapper.width == FORMAT_WIDTH_NONE || u_zero_pad_count == 0) {
						if (u_sign == -1) {
							memcpy((void*) string_buffer, (void*) unsigned_integer_buffer, u_digits_written);
							string_size = u_digits_written;
						} else {
							string_buffer[0] = u_sign;
							memcpy((void*) &string_buffer[1], (void*) unsigned_integer_buffer, u_digits_written);
							string_size = u_digits_written + 1;
						}

						break;
					}

					char u_pad_character = ' ';

					if (u_zero_pad) {
						u_pad_character = '0';
					}

					for (size_t i = 0; i < format_wrapper.width; i++) {
						string_buffer[i] = u_pad_character;
					}

					if (u_zero_pad) {
						if (u_sign != -1) {
							string_buffer[0] = u_sign;
						}

						memcpy((void*) &string_buffer[format_wrapper.width - u_digits_written], (void*) unsigned_integer_buffer, u_digits_written);
					} else {
						if (format_wrapper.left_align) {
							if (u_sign != -1) {
								string_buffer[0] = u_sign;
								memcpy((void*) &string_buffer[1], (void*) unsigned_integer_buffer, u_digits_written);
							} else {
								memcpy((void*) string_buffer, (void*) unsigned_integer_buffer, u_digits_written);
							}
						} else {
							if (u_sign != -1) {
								string_buffer[0] = u_sign;
							}

							memcpy((void*) &string_buffer[format_wrapper.width - u_digits_written], (void*) unsigned_integer_buffer, u_digits_written);
						}
					}

					string_size = format_wrapper.width;

					break;

				case FORMAT_SPECIFIER_HEXADECIMAL_1:
				case FORMAT_SPECIFIER_HEXADECIMAL_2:
					bool h_is_uppercase = false;

					if (format_wrapper.specifier == FORMAT_SPECIFIER_HEXADECIMAL_2) {
						h_is_uppercase = true;
					}

					uint64_t hexadecimal_integer = va_arg(args, unsigned long long);

					char hexadecimal_integer_buffer[PRINT_BUFFER_SIZE];
					size_t hexadecimal_integer_index;
					ssize_t h_digits_written = ull_to_str(hexadecimal_integer, (void*) hexadecimal_integer_buffer, 16);

					if (h_digits_written == -1) {
						return -1;
					}

					bool h_zero_pad = !format_wrapper.left_align &&
							  format_wrapper.zero_padding &&
							  (format_wrapper.width != FORMAT_WIDTH_NONE);

					size_t h_zero_pad_count = 0;

					if (format_wrapper.width != FORMAT_WIDTH_NONE) {
						ssize_t absolute_width = format_wrapper.width - h_digits_written;

						if (absolute_width > 0) {
							h_zero_pad_count = absolute_width;
						}
					}

					if (format_wrapper.width == FORMAT_WIDTH_NONE || h_zero_pad_count == 0) {
						if (format_wrapper.base_prefix) {
							string_buffer[0] = '0';
							string_buffer[1] = 'x';
							memcpy((void*) &string_buffer[2], (void*) hexadecimal_integer_buffer, h_digits_written);
							string_size = h_digits_written + 2;
						} else {
							memcpy((void*) string_buffer, (void*) hexadecimal_integer_buffer, h_digits_written);
							string_size = h_digits_written;
						}

						if (h_is_uppercase) {
							to_uppercase(string_buffer, string_size);
						}

						break;
					}

					char h_pad_character = ' ';

					if (h_zero_pad) {
						h_pad_character = '0';
					}

					for (size_t i = 0; i < format_wrapper.width + 2; i++) {
						string_buffer[i] = h_pad_character;
					}

					string_buffer[0] = '0';
					string_buffer[1] = 'x';

					if (format_wrapper.left_align) {
						memcpy((void*) &string_buffer[2], (void*) hexadecimal_integer_buffer, h_digits_written);
					} else {
						memcpy((void*) &string_buffer[format_wrapper.width - h_digits_written], (void*) hexadecimal_integer_buffer, h_digits_written);
					}

					if (format_wrapper.base_prefix) {
						string_size = format_wrapper.width + 2;
					} else {
						string_size = format_wrapper.width;

						if (format_wrapper.left_align) {
							memmove((void*) string_buffer, (void*) &string_buffer[2], string_size);
						} else {
							memmove((void*) &string_buffer[format_wrapper.width - h_digits_written - 2], (void*) &string_buffer[format_wrapper.width - h_digits_written], string_size);
						}
					}

					if (h_is_uppercase) {
						to_uppercase(string_buffer, string_size);
					}

					break;

				case FORMAT_SPECIFIER_POINTER:
					;
					break;
			}

			if (!overflow) {
				if (buffer_index + string_size >= count) {
					overflow = true;
					buffer_index += string_size;
					continue;
				}

				memcpy((void*) &buffer[buffer_index], (void*) string_buffer, string_size);
			}

			buffer_index += string_size;

		} else {
			if (!overflow) {
				buffer[buffer_index] = format[format_index];
			}

			buffer_index++;
			format_index++;
		}
	}
}

ssize_t sscanf(char* string, char* format, ...) {
	va_list args;
	va_start(args, format);

	size_t assignments = 0;

	size_t string_length = strlen(string);
	size_t format_length = strlen(format);

	if (string_length == 0 || format_length < 2) {
		return assignments;
	}

	format_wrapper_t format_wrapper;

	size_t string_index = 0;
	size_t format_index = 0;

	while (1) {
		if (format[format_index] == FORMAT_SPECIFIER_DELIMITER) {
			format_index = format_parse(format, &format_wrapper, format_index, FORMAT_PARSE_TYPE_SCAN);

			if (!format_index) {
				return -1;
			}

			switch (format_wrapper.specifier) {
				case FORMAT_SPECIFIER_ESC:
					format_index++;

					if (format[format_index] != string[string_index]) {
						return assignments;
					}

					string_index++;
					format_index++;

					break;

				case FORMAT_SPECIFIER_CHAR:
					if (format_wrapper.width == FORMAT_WIDTH_NONE) {
						format_wrapper.width = 1;
					}

					if (format_wrapper.skip) {
						for (size_t i = 0; i < format_wrapper.width; i++) {
							string_index++;
						}

						break;
					}

					char* character = va_arg(args, char*);

					for (size_t i = 0; i < format_wrapper.width; i++) {
						character[i] = string[string_index];
						string_index++;
					}

					assignments++;

					break;

				case FORMAT_SPECIFIER_STR:
					if (format_wrapper.skip) {
						if (format_wrapper.width == FORMAT_WIDTH_NONE) {
							while (!is_whitespace(string[string_index])) {
								string_index++;
							}
						} else {
							for (size_t i = 0; i < format_wrapper.width; i++) {
								if (is_whitespace(string[string_index])) {
									break;
								}

								string_index++;
							}
						}

						break;
					}

					char* str = va_arg(args, char*);

					if (format_wrapper.width == FORMAT_WIDTH_NONE) {
						size_t str_length = 0;

						while (!is_whitespace(string[string_index])) {
							str[str_length] = string[string_index];
							string_index++;
							str_length++;
						}

						str[str_length] = '\0';

					} else {
						size_t str_length = 0;

						for (size_t i = 0; i < format_wrapper.width; i++) {
							if (is_whitespace(string[string_index])) {
								break;
							}

							str[i] = string[string_index];
							string_index++;
							str_length++;
						}

						str[str_length] = '\0';
					}

					assignments++;

					break;

				case FORMAT_SPECIFIER_INT:
					if (format_wrapper.skip) {
						while (is_digit(string[string_index])) {
							string_index++;
						}

						break;
					}

					long long signed_integer = 0;
					bool is_negative = false;

					if (string[string_index] == '-') {
						is_negative = true;
						string_index++;
					}

					while (is_digit(string[string_index])) {
						uint8_t digit = string[string_index] - '0';

						if (signed_integer > INT64_MAX / 10 || (signed_integer == INT64_MAX / 10 && digit > INT64_MAX % 10)) {
							return assignments;
						}

						signed_integer = signed_integer * 10 + digit;
						string_index++;
					}

					if (is_negative) {
						signed_integer *= -1;
					}

					switch (format_wrapper.length) {
						case FORMAT_LENGTH_NONE:
							*va_arg(args, int*) = (int) signed_integer;
							break;

						case FORMAT_LENGTH_HH:
							*va_arg(args, char*) = (char) signed_integer;
							break;

						case FORMAT_LENGTH_H:
							*va_arg(args, short*) = (short) signed_integer;
							break;

						case FORMAT_LENGTH_L:
							*va_arg(args, long*) = (long) signed_integer;
							break;

						case FORMAT_LENGTH_LL:
							*va_arg(args, long long*) = (long long) signed_integer;
							break;

						case FORMAT_LENGTH_Z:
							*va_arg(args, ssize_t*) = (ssize_t) signed_integer;
							break;
					}

					assignments++;

					break;

				case FORMAT_SPECIFIER_UINT:
					if (format_wrapper.skip) {
						while (is_digit(string[string_index])) {
							string_index++;
						}

						break;
					}

					unsigned long long unsigned_integer = 0;

					while (is_digit(string[string_index])) {
						uint8_t digit = string[string_index] - '0';

						if (unsigned_integer > UINT64_MAX / 10 || (unsigned_integer == UINT64_MAX / 10 && digit > UINT64_MAX % 10)) {
							return assignments;
						}

						unsigned_integer = unsigned_integer * 10 + digit;
						string_index++;
					}

					switch (format_wrapper.length) {
						case FORMAT_LENGTH_NONE:
							*va_arg(args, unsigned int*) = (unsigned int) unsigned_integer;
							break;

						case FORMAT_LENGTH_HH:
							*va_arg(args, unsigned char*) = (unsigned char) unsigned_integer;
							break;

						case FORMAT_LENGTH_H:
							*va_arg(args, unsigned short*) = (unsigned short) unsigned_integer;
							break;

						case FORMAT_LENGTH_L:
							*va_arg(args, unsigned long*) = (unsigned long) unsigned_integer;
							break;

						case FORMAT_LENGTH_LL:
							*va_arg(args, unsigned long long*) = (unsigned long long) unsigned_integer;
							break;

						case FORMAT_LENGTH_Z:
							*va_arg(args, size_t*) = (size_t) unsigned_integer;
							break;
					}

					assignments++;

					break;

				case FORMAT_SPECIFIER_HEX:
					;
					break;

				case FORMAT_SPECIFIER_PTR:
					;
					break;
			}

		} else {
			if (format[format_index] != string[string_index]) {
				return assignments;
			}

			string_index++;
			format_index++;
		}
	}

	va_end(args);

	return assignments;
}

size_t format_parse(char* format, format_wrapper_t* format_wrapper, size_t offset, format_parse_t type) {
	if (format[offset] != FORMAT_SPECIFIER_DELIMITER) {
		return 0;
	}

	if (offset >= strlen(format)) {
		return 0;
	}

	*format_wrapper = (format_wrapper_t) {0};
	size_t index = offset + 1;

	if (type == FORMAT_PARSE_TYPE_PRINT) {
		bool exit = false;

		while (1) {
			switch (format[index]) {
				case FORMAT_FLAG_LEFT_ALIGN:
					format_wrapper->left_align = true;
					break;

				case FORMAT_FLAG_SIGN:
					format_wrapper->sign = true;
					break;

				case FORMAT_FLAG_SPACE_PREFIX:
					format_wrapper->space_prefix = true;
					break;

				case FORMAT_FLAG_BASE_PREFIX:
					format_wrapper->base_prefix = true;
					break;

				case FORMAT_FLAG_ZERO_PADDING:
					format_wrapper->zero_padding = true;
					break;

				default:
					exit = true;
			}

			if (exit) {
				break;
			}

			index++;
		}
	}

	if (type == FORMAT_PARSE_TYPE_SCAN) {
		if (format[index] == FORMAT_FLAG_SKIP) {
			format_wrapper->skip = true;
			index++;
		} else {
			format_wrapper->skip = false;
		}
	}

	format_wrapper->width = 0;

	if (format[index] == '-') {
		return 0;
	}

	while (is_digit(format[index])) {
		uint8_t digit = format[index] - '0';

		if (format_wrapper->width > SSIZE_MAX / 10 || (format_wrapper->width == SSIZE_MAX / 10 && digit > SSIZE_MAX % 10)) {
			return 0;
		}

		format_wrapper->width = format_wrapper->width * 10 + digit;
		index++;
	}

	if (format_wrapper->width == 0) {
		if (type == FORMAT_PARSE_TYPE_SCAN) {
			return 0;
		}

		format_wrapper->width = FORMAT_WIDTH_NONE;
	}

	if (type == FORMAT_PARSE_TYPE_PRINT) {
		if (format[index] == FORMAT_PRECISION_DELIMITER) {
			if (format[index] == '-') {
				return 0;
			}

			format_wrapper->precision = 0;
			index++;

			while (is_digit(format[index])) {
				uint8_t digit = format[index] - '0';

				if (format_wrapper->precision > SSIZE_MAX / 10 || (format_wrapper->precision == SSIZE_MAX / 10 && digit > SSIZE_MAX % 10)) {
					return 0;
				}

				format_wrapper->precision = format_wrapper->precision * 10 + digit;
				index++;
			}
		} else {
			format_wrapper->precision = FORMAT_PRECISION_NONE;
		}
	}

	switch (format[index]) {
		case FORMAT_LENGTH_SHORT:
			if (format[index + 1] == FORMAT_LENGTH_SHORT) {
				format_wrapper->length = FORMAT_LENGTH_HH;
				index++;
			} else {
				format_wrapper->length = FORMAT_LENGTH_H;
			}

			index++;

			break;

		case FORMAT_LENGTH_LONG:
			if (format[index + 1] == FORMAT_LENGTH_LONG) {
				format_wrapper->length = FORMAT_LENGTH_LL;
				index++;
			} else {
				format_wrapper->length = FORMAT_LENGTH_L;
			}

			index++;

			break;

		case FORMAT_LENGTH_MAX:
			format_wrapper->length = FORMAT_LENGTH_Z;
			index++;

			break;

		default:
			format_wrapper->length = FORMAT_LENGTH_NONE;
			break;
	}

	switch (format[index]) {
		case FORMAT_SPECIFIER_CHARACTER:
			format_wrapper->specifier = FORMAT_SPECIFIER_CHAR;
			break;

		case FORMAT_SPECIFIER_STRING:
			format_wrapper->specifier = FORMAT_SPECIFIER_STR;
			break;

		case FORMAT_SPECIFIER_INTEGER_1:
			format_wrapper->specifier = FORMAT_SPECIFIER_INT;
			break;

		case FORMAT_SPECIFIER_INTEGER_2:
			format_wrapper->specifier = FORMAT_SPECIFIER_INT;
			break;

		case FORMAT_SPECIFIER_UNSIGNED_INTEGER:
			format_wrapper->specifier = FORMAT_SPECIFIER_UINT;
			break;

		case FORMAT_SPECIFIER_HEXADECIMAL_1:
			format_wrapper->specifier = FORMAT_SPECIFIER_HEX;
			break;

		case FORMAT_SPECIFIER_HEXADECIMAL_2:
			format_wrapper->specifier = FORMAT_SPECIFIER_HEX;
			break;

		case FORMAT_SPECIFIER_POINTER:
			format_wrapper->specifier = FORMAT_SPECIFIER_PTR;
			break;

		case FORMAT_SPECIFIER_ESCAPE:
			format_wrapper->specifier = FORMAT_SPECIFIER_ESC;

		default:
			return 0;
	}

	index++;

	return index;
}
