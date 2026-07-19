/* ♥ OWT STATUSBAR ~ Barra de status la embaixo!
 * Dica: texto de ate 256 chars, cor secondary~
 * Fica em baixo da janela, mostra info inútil mas bonita
 * set_text atualiza na hora, igual label ♥ */
#ifndef OWT_STATUSBAR_H
#define OWT_STATUSBAR_H
#include "owt_base.h"

typedef struct {
    owt_widget_t base;
    char text[256];
} owt_statusbar_t;

owt_statusbar_t *owt_statusbar_create(int x, int y, int w);
void owt_statusbar_set_text(owt_statusbar_t *sb, const char *text);

#endif
