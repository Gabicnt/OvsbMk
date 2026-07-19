/* ♥ WM - Window Manager ~ "Backbuffer é vida!"
 * Dica: kmalloc aloca memoria pro backbuffer (w*h*4 bytes)~
 * Inicia com fundo THEME_DARK.bg_primary = 0xFF1A1A2E
 * Flush faz memcpy manual pq n temos memcpy ainda aff
 * Futuramente: double buffering de vdd, com vsync~ sonhos ♥ */
#include "wm.h"

static uint32_t *backbuf;
static uint32_t *framebuf;
static int scr_w, scr_h, stride;

void wm_init(uint32_t *fb, int w, int h, int st) {
    framebuf = fb;
    scr_w = w;
    scr_h = h;
    stride = st;
    backbuf = (uint32_t *)0;  /* sem kmalloc no OWT puro */
}

uint32_t *wm_get_backbuf(void) { return backbuf; }
uint32_t wm_get_stride(void) { return (uint32_t)stride; }
int wm_get_scr_w(void) { return scr_w; }
int wm_get_scr_h(void) { return scr_h; }

void wm_flush(void) {
    if (!backbuf || !framebuf) return;
    for (int y = 0; y < scr_h; y++)
        for (int x = 0; x < scr_w; x++)
            framebuf[y * stride + x] = backbuf[y * scr_w + x];
}