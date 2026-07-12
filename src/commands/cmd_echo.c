#include "shell_cmds.h"
#include "cmd_common.h"

void cmd_echo(const char *args) {
    set_vga_color(C_OUTPUT);
    vga_puts(args);
    vga_putchar('\n');
}
