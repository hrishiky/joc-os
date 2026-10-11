#include "keyboard.h"

#include "stdint.h"

keyboard_letter_entry keyboard_letter_list[] = {
	{ KEYBOARD_KEY_A, 'a' },
	{ KEYBOARD_KEY_B, 'b' },
	{ KEYBOARD_KEY_C, 'c' },
	{ KEYBOARD_KEY_D, 'd' },
	{ KEYBOARD_KEY_E, 'e' },
	{ KEYBOARD_KEY_F, 'f' },
	{ KEYBOARD_KEY_G, 'g' },
	{ KEYBOARD_KEY_H, 'h' },
	{ KEYBOARD_KEY_I, 'i' },
	{ KEYBOARD_KEY_J, 'j' },
	{ KEYBOARD_KEY_K, 'k' },
	{ KEYBOARD_KEY_L, 'l' },
	{ KEYBOARD_KEY_M, 'm' },
	{ KEYBOARD_KEY_N, 'n' },
	{ KEYBOARD_KEY_O, 'o' },
	{ KEYBOARD_KEY_P, 'p' },
	{ KEYBOARD_KEY_Q, 'q' },
	{ KEYBOARD_KEY_R, 'r' },
	{ KEYBOARD_KEY_S, 's' },
	{ KEYBOARD_KEY_T, 't' },
	{ KEYBOARD_KEY_U, 'u' },
	{ KEYBOARD_KEY_V, 'v' },
	{ KEYBOARD_KEY_W, 'w' },
	{ KEYBOARD_KEY_X, 'x' },
	{ KEYBOARD_KEY_Y, 'y' },
	{ KEYBOARD_KEY_Z, 'z' }
};

keyboard_digit_symbol_entry keyboard_digit_symbol_list[] = {
	{ KEYBOARD_KEY_1_EXCLAMATION, '1', '!' },
	{ KEYBOARD_KEY_2_AT, '2', '@' },
	{ KEYBOARD_KEY_3_HASH, '3', '#' },
	{ KEYBOARD_KEY_4_DOLLAR, '4', '$' },
	{ KEYBOARD_KEY_5_PERCENT, '5', '%' },
	{ KEYBOARD_KEY_6_CARET, '6', '^' },
	{ KEYBOARD_KEY_7_AMPERSAND, '7', '&' },
	{ KEYBOARD_KEY_8_ASTERISK, '8', '*' },
	{ KEYBOARD_KEY_9_LEFT_PARENTHESIS, '9', '(' },
	{ KEYBOARD_KEY_0_RIGHT_PARENTHESIS, '0', ')' },
	{ KEYBOARD_KEY_MINUS_UNDERSCORE, '-', '_' },
	{ KEYBOARD_KEY_EQUAL_PLUS, '=', '+' },
	{ KEYBOARD_KEY_LEFT_BRACKET_BRACE, '[', '{' },
	{ KEYBOARD_KEY_RIGHT_BRACKET_BRACE, ']', '}' },
	{ KEYBOARD_KEY_SEMICOLON_COLON, ';', ':' },
	{ KEYBOARD_KEY_APOSTROPHE_QUOTE, '\'', '"' },
	{ KEYBOARD_KEY_BACKTICK_TILDE, '`', '~' },
	{ KEYBOARD_KEY_BACKSLASH_PIPE, '\\', '|' },
	{ KEYBOARD_KEY_COMMA_LESS_THAN, ',', '<' },
	{ KEYBOARD_KEY_PERIOD_GREATER_THAN, '.', '>' },
	{ KEYBOARD_KEY_SLASH_QUESTION, '/', '?' }
};

keyboard_control_entry keyboard_control_list[] = {
 	{ KEYBOARD_KEY_ENTER, '\n' },
	{ KEYBOARD_KEY_BACKSPACE, '\b' },
	{ KEYBOARD_KEY_TAB, '\t' },
	{ KEYBOARD_KEY_SPACE, ' ' }
};

uint8_t KEYBOARD_LETTER_COUNT = sizeof(keyboard_letter_list) / sizeof(keyboard_letter_list[0]);
uint8_t KEYBOARD_DIGIT_SYMBOL_COUNT = sizeof(keyboard_digit_symbol_list) / sizeof(keyboard_digit_symbol_list[0]);
uint8_t KEYBOARD_CONTROL_COUNT = sizeof(keyboard_control_list) / sizeof(keyboard_control_list[0]);

uint8_t keyboard_shift_down;
uint8_t keyboard_ctrl_down;
uint8_t keyboard_alt_down;
uint8_t keyboard_caps_lock;

uint8_t keyboard_extended;
uint8_t keyboard_up_arrow_down;
uint8_t keyboard_down_arrow_down;
uint8_t keyboard_left_arrow_down;
uint8_t keyboard_right_arrow_down;

uint16_t keyboard_buffer[KEYBOARD_BUFFER_SIZE];
uint8_t keyboard_buffer_head;
uint8_t keyboard_buffer_tail;

void keyboard_buffer_write(uint16_t keycode) {
	keyboard_buffer[keyboard_buffer_head] = keycode;
	keyboard_buffer_head = (keyboard_buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

	if (keyboard_buffer_head == keyboard_buffer_tail) {
		keyboard_buffer_tail = (keyboard_buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
	}
}

char keyboard_buffer_read(void) {
	if (keyboard_buffer_head == keyboard_buffer_tail) {
		return -1;
	}

	uint16_t character = keyboard_buffer[keyboard_buffer_tail];
	keyboard_buffer_tail = (keyboard_buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;

	if (character > KEYBOARD_KEYCODE_BASE) {
		return -1;
	}

	return (char) character;
}

uint16_t keyboard_get_keycode(void) {
	if (keyboard_buffer_head == keyboard_buffer_tail) {
		return -1;
	}

	uint16_t character = keyboard_buffer[keyboard_buffer_tail];
	keyboard_buffer_tail = (keyboard_buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;

	return character;
}

void keyboard_central_handler(uint8_t scancode) {
	if (scancode == KEYBOARD_EXTENDED) {
		keyboard_extended = 1;
		return;
	}

	uint8_t base = scancode & KEYBOARD_MASK_BASE;

	if (scancode & KEYBOARD_MASK_BREAK) {
		switch (base) {
			case KEYBOARD_UP_ARROW:
				keyboard_up_arrow_down = 0;
				keyboard_buffer_write(KEYBOARD_UP_ARROW_UNSET);
				break;

			case KEYBOARD_DOWN_ARROW:
				keyboard_down_arrow_down = 0;
				keyboard_buffer_write(KEYBOARD_DOWN_ARROW_UNSET);
				break;

			case KEYBOARD_LEFT_ARROW:
				keyboard_left_arrow_down = 0;
				keyboard_buffer_write(KEYBOARD_LEFT_ARROW_UNSET);
				break;

			case KEYBOARD_RIGHT_ARROW:
				keyboard_right_arrow_down = 0;
				keyboard_buffer_write(KEYBOARD_RIGHT_ARROW_UNSET);
				break;

			case KEYBOARD_LEFT_SHIFT:
				keyboard_shift_down = 0;
				break;

			case KEYBOARD_RIGHT_SHIFT:
				keyboard_shift_down = 0;
				break;

			case KEYBOARD_CTRL:
				keyboard_ctrl_down = 0;
				keyboard_buffer_write(KEYBOARD_CTRL_UNSET);
				break;

			case KEYBOARD_ALT:
				keyboard_alt_down = 0;
				keyboard_buffer_write(KEYBOARD_ALT_UNSET);
				break;
		}

		keyboard_extended = 0;
		return;

	} else {
		if (keyboard_extended) {
			switch (base) {
				case KEYBOARD_UP_ARROW:
					keyboard_up_arrow_down = 1;
					keyboard_buffer_write(KEYBOARD_UP_ARROW_SET);
					break;

				case KEYBOARD_DOWN_ARROW:
					keyboard_down_arrow_down = 1;
					keyboard_buffer_write(KEYBOARD_DOWN_ARROW_SET);
					break;

				case KEYBOARD_LEFT_ARROW:
					keyboard_left_arrow_down = 1;
					keyboard_buffer_write(KEYBOARD_LEFT_ARROW_SET);
					break;

				case KEYBOARD_RIGHT_ARROW:
					keyboard_right_arrow_down = 1;
					keyboard_buffer_write(KEYBOARD_RIGHT_ARROW_SET);
					break;

				case KEYBOARD_CTRL:
					keyboard_ctrl_down = 1;
					keyboard_buffer_write(KEYBOARD_CTRL_SET);
					break;

				case KEYBOARD_ALT:
					keyboard_alt_down = 1;
					keyboard_buffer_write(KEYBOARD_ALT_SET);
					break;
			}

			keyboard_extended = 0;
			return;
		}

		switch (base) {
			case KEYBOARD_LEFT_SHIFT:
				keyboard_shift_down = 1;
				return;

			case KEYBOARD_RIGHT_SHIFT:
				keyboard_shift_down = 1;
				return;

			case KEYBOARD_KEY_CAPSLOCK:
				keyboard_caps_lock ^= 1;
				return;
		}

		for (uint8_t i = 0; i < KEYBOARD_LETTER_COUNT; i++) {
			if (keyboard_letter_list[i].base == base) {
				if (keyboard_shift_down ^ keyboard_caps_lock) {
					keyboard_buffer_write(keyboard_letter_list[i].ascii - KEYBOARD_UPPERCASE_OFFSET);
				} else {
					keyboard_buffer_write(keyboard_letter_list[i].ascii);
				}

				return;
			}
		}

		for (uint8_t i = 0; i < KEYBOARD_DIGIT_SYMBOL_COUNT; i++) {
			if (keyboard_digit_symbol_list[i].base == base) {
				if (keyboard_shift_down ^ keyboard_caps_lock) {
					keyboard_buffer_write(keyboard_digit_symbol_list[i].shifted);
				} else {
					keyboard_buffer_write(keyboard_digit_symbol_list[i].normal);
				}

				return;
			}
		}

		for (uint8_t i = 0; i < KEYBOARD_CONTROL_COUNT; i++) {
			if (keyboard_control_list[i].base == base) {
				keyboard_buffer_write(keyboard_control_list[i].ascii);

				return;
			}
		}

		return;
	}
}

