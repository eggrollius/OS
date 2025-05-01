#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    // first four bits = background color
    // last four bits = foreground color
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    // first 8 bits = character
    // last 8 bits = color data
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while(str[len]) {
        len += 1;
    }

    return len;
}

#define VGA_WIDTH   80
#define VGA_HEIGHT  25 // my virt box height
#define VGA_ADDR    0xB8000

size_t terminal_row;
size_t terminal_col;
uint8_t terminal_color;   
uint16_t* terminal_buffer = (uint16_t*) VGA_ADDR;

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_col = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    // set each entry to ' ' with default color
    for(size_t y = 0; y < VGA_HEIGHT; ++y) {
        for(size_t x = 0; x < VGA_WIDTH; ++x) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll() 
{
    // iterate each row
    for(size_t row = 0; row < VGA_HEIGHT-1; ++row) {
        // iterate each column
        for(size_t col = 0; col < VGA_WIDTH; ++col) {
            terminal_buffer[row * VGA_WIDTH + col] = terminal_buffer[(row+1)*VGA_WIDTH + col];
        }
    }

    // clear the last row
    for(size_t col = 0; col < VGA_WIDTH; ++col) {
        terminal_putentryat(' ', VGA_COLOR_BLACK, col, VGA_HEIGHT-1);
    }

    // set row to the last row and col to the last col
    terminal_row = VGA_HEIGHT-1;
    terminal_col = 0;
}

void terminal_handle_new_line()
{
    terminal_col = 0;

    if(++terminal_row >= VGA_HEIGHT) {
        terminal_scroll();
    }
}

void terminal_putchar(char c)
{
    // check for new line character
    if(c == '\n') {
        terminal_handle_new_line();
        return;
    }

    terminal_putentryat(c, terminal_color, terminal_col, terminal_row);

    if(++terminal_col >= VGA_WIDTH) {
        terminal_handle_new_line();
    }
}


void terminal_write(const char* data, size_t size) 
{
    for(size_t i = 0; i < size; ++i) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}

void kernel_main(void)
{
    terminal_initialize();

    terminal_writestring(    
        "Line 1\nLine 2\nLine 3\nLine 4\nLine 5\n"
        "Line 6\nLine 7\nLine 8\nLine 9\nLine 10\n"
        "Line 11\nLine 12\nLine 13\nLine 14\nLine 15\n"
        "Line 16\nLine 17\nLine 18\nLine 19\nLine 20\n"
        "Line 21\nLine 22\nLine 23\nLine 24\nLine 25\n"
        "Line 26\nLine 27\nLine 28\nLine 29\nLine 30\n"
    );
}