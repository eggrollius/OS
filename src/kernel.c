#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_LIGHT_GREY = 7,
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
#define VGA_HEIGHT  25
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

void terminal_putchar(char c)
{
    terminal_putentryat(c, terminal_color, terminal_col, terminal_row);
    // check for overflowing the current row
    if(++terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        // move down a row and check for overflowing the screen
        if(++terminal_row >= VGA_HEIGHT) {
            terminal_row = 0; // TODO implement more sophisticated logic
        }
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

    terminal_writestring("Hello World!\n");
}