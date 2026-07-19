/* ♥ WM - Window Manager ~ "O porteiro do framebuffer!"
 * Dica: init chama antes de qlqr coisa do OWT~
 * backbuf é um espelho do framebuffer pra desenhar sem flicker
 * flush copia tudo pro video de uma vez, lindo demais ♥
 * stride em pixels, n bytes (pitch/4)~ n confunde sua anta >_< */
#ifndef WM_H
#define WM_H
#include <stdint.h>

void wm_init(uint32_t *fb, int w, int h, int stride);
uint32_t *wm_get_backbuf(void);
uint32_t wm_get_stride(void);
int wm_get_scr_w(void);
int wm_get_scr_h(void);
void wm_flush(void);

#endif