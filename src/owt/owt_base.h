/* ♥ OWT BASE ~ Widget base, a mãe de todos!
 * Dica: todo widget é um owt_widget disfarçado~
 * Se não tiver pai, é órfão ;-; mas fica lindo msm assim
 * draw, on_click, on_key: os 3 pilares da UI moderna >_<
 * Hihi, add_child encadeia q nem matryoshka ♥ */
#ifndef OWT_BASE_H
#define OWT_BASE_H
#include <stdint.h>

typedef struct owt_widget {
    int x, y, w, h;
    int visible;
    int enabled;
    struct owt_widget *parent;
    struct owt_widget **children;
    int child_count;
    void (*draw)(struct owt_widget *self);
    void (*on_click)(struct owt_widget *self, int x, int y);
    void (*on_key)(struct owt_widget *self, char key);
} owt_widget_t;

owt_widget_t *owt_widget_create(int x, int y, int w, int h);
void owt_widget_draw(owt_widget_t *w);
void owt_widget_add_child(owt_widget_t *parent, owt_widget_t *child);

#endif