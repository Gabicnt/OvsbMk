#ifndef CMD_COMMON_H
#define CMD_COMMON_H

#include <stdint.h>
#include "../kernel/colors.h"

extern void vga_puts(const char *s);
extern void vga_putchar(char c);
extern void vga_clear(void);
extern void set_vga_color(uint8_t color);
extern char keyboard_read(void);

#endif
