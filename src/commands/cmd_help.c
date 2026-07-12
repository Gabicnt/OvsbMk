#include "shell_cmds.h"
#include "cmd_common.h"

void cmd_help(void) {
    set_vga_color(C_HEADER);
    vga_puts("=== Comandos ===\n");
    set_vga_color(C_OUTPUT);
    vga_puts("help clear echo about shutdown\n");
    vga_puts("ls touch rm cat edit\n");
    vga_puts("mkdir cd pwd\n");
}
