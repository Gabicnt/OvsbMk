#include "shell_cmds.h"
#include "cmd_common.h"

void cmd_about(void) {
    set_vga_color(C_HEADER);
    vga_puts("Ovsb.OS - Sistema 64-bit\n");
    set_vga_color(C_OUTPUT);
    vga_puts("Kernel OvsbMkM (XNU-like)\n");
}
