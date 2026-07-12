#ifndef GUI_H
#define GUI_H
#include <stdint.h>

typedef struct {
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
} framebuffer_t;

void gui_init(framebuffer_t *fb);
void gui_fill_screen(uint32_t color);
void gui_draw_pixel(int x, int y, uint32_t color);
void gui_draw_rect(int x, int y, int w, int h, uint32_t color);
void gui_draw_char(int x, int y, char c, uint32_t color);
void gui_draw_text(int x, int y, const char *text, uint32_t color);

#endif