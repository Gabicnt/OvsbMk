#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include "../../lib/libgui/vesa.h"

void terminal_app_init(void);
void terminal_app_run(void);
void terminal_write(const char *text);

extern int graphics_mode;

#endif
