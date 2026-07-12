#include "shell_cmds.h"
#include "cmd_common.h"
#include "../fs/fat32.h"

void cmd_rm(const char *name) {
    if (!name || !*name) { set_vga_color(C_ERROR); vga_puts("Uso: rm <nome>\n"); return; }
    if (fat32_delete_file(name) == 0) {
        set_vga_color(C_SUCCESS);
        vga_puts("Removido\n");
    } else {
        set_vga_color(C_ERROR);
        vga_puts("Erro\n");
    }
}
