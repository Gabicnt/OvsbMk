#include "colors.h"
#include <stdint.h>

#define VGA_ADDR  0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

volatile unsigned short *vga = (unsigned short *)VGA_ADDR;
int cx = 0, cy = 0;
static uint8_t current_color = 0x07;

void set_vga_color(uint8_t color) { current_color = color; }

void vga_putchar(char c) {
    uint8_t col = current_color;
    if (c == '\n') { cx = 0; cy++; }
    else if (c == '\b') { if (cx > 0) { cx--; vga[cy * VGA_WIDTH + cx] = (col << 8) | ' '; } }
    else if (c == '\r') { cx = 0; }
    else { vga[cy * VGA_WIDTH + cx] = (col << 8) | c; cx++; }
    if (cx >= VGA_WIDTH) { cx = 0; cy++; }
    if (cy >= VGA_HEIGHT) {
        for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) vga[i] = vga[i + VGA_WIDTH];
        for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++) vga[i] = (col << 8) | ' ';
        cy = VGA_HEIGHT - 1;
    }
    unsigned short pos = cy * VGA_WIDTH + cx;
    outb(0x3D4, 0x0F); outb(0x3D5, pos & 0xFF);
    outb(0x3D4, 0x0E); outb(0x3D5, (pos >> 8) & 0xFF);
}

void vga_puts(const char *s) { while (*s) vga_putchar(*s++); }
void vga_clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) vga[i] = (current_color << 8) | ' ';
    cx = cy = 0;
}
