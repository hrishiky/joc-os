#ifndef VGA_TEXT_H
#define VGA_TEXT_H

#define VGA_TEXT_ROWS 25
#define VGA_TEXT_ROWS_MINIMUM 0
#define VGA_TEXT_ROWS_MAXIMUM 24

#define VGA_TEXT_COLUMNS 80
#define VGA_TEXT_COLUMNS_MINIMUM 0
#define VGA_TEXT_COLUMNS_MAXIMUM 79

#define VGA_TEXT_TAB_LENGTH 4

#define VGA_TEXT_COLOR_MAXIMUM 0xF
#define VGA_TEXT_COLOR_CODE_MAXIMUM 0x11

#define VGA_TEXT_COLOR_BLACK 0x0
#define VGA_TEXT_COLOR_BLUE 0x1
#define VGA_TEXT_COLOR_GREEN 0x2
#define VGA_TEXT_COLOR_CYAN 0x3
#define VGA_TEXT_COLOR_RED 0x4
#define VGA_TEXT_COLOR_MAGENTA 0x5
#define VGA_TEXT_COLOR_BROWN 0x6
#define VGA_TEXT_COLOR_LIGHT_GRAY 0x7
#define VGA_TEXT_COLOR_DARK_GRAY 0x8
#define VGA_TEXT_COLOR_LIGHT_BLUE 0x9
#define VGA_TEXT_COLOR_LIGHT_GREEN 0xA
#define VGA_TEXT_COLOR_LIGHT_CYAN 0xB
#define VGA_TEXT_COLOR_LIGHT_RED 0xC
#define VGA_TEXT_COLOR_LIGHT_MAGENTA 0xD
#define VGA_TEXT_COLOR_LIGHT_YELLOW 0xE
#define VGA_TEXT_COLOR_WHITE 0xF

#define VGA_TEXT_COLOR_CODE_FOREGROUND 0x10
#define VGA_TEXT_COLOR_CODE_BACKGROUND 0x11

#define VGA_TEXT_FOREGROUND_COLOR_DEFAULT VGA_TEXT_COLOR_LIGHT_GRAY
#define VGA_TEXT_BACKGROUND_COLOR_DEFAULT VGA_TEXT_COLOR_BLACK

#define VGA_TEXT_BW 0x0F
#define VGA_TEXT_BUFFER_SIZE 4000

void vga_text_blinking_cursor_enable(unsigned char start, unsigned char end);
void vga_text_blinking_cursor_disable(void);
void vga_text_blinking_cursor_shift(void);

void vga_text_scroll(long count);

void vga_text_cursor_rectify(void);
void vga_text_cursor_shift(long x, long y);

void vga_text_reset_screen(void);
void vga_text_clear_screen(void);
void vga_text_clear_segment(unsigned short start, unsigned short end);

void vga_text_print_character(char character);
void vga_text_print_character_color(char character, unsigned char foreground_color, unsigned char background_color);

void vga_text_print_position(char* string, unsigned char x, unsigned char y);
void vga_text_print(char* string);
void vga_text_print_color(char* string, unsigned char foreground_color, unsigned char background_color);

void vga_text_print_integer(long long number);
void vga_text_print_unsigned_integer(unsigned long long number);
void vga_text_print_hex(unsigned long long number);

void vga_text_change_colors(unsigned char foreground_color, unsigned char background_color);
void vga_text_refresh_colors(void);

#endif
