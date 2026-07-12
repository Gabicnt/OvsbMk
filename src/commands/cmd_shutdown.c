#include "shell_cmds.h"
#include "cmd_common.h"

void cmd_shutdown(void) {
    set_vga_color(C_ERROR);
    vga_puts("Desligando...\n");
    __asm__ volatile ("cli; hlt");
}
