#include "shell_cmds.h"
#include "../fs/fat32.h"

extern void vga_puts(const char *s);

void cmd_cd(const char *name) {
    if (!name || !*name) {
        vga_puts("Uso: cd <diretorio>\n");
        return;
    }
    if (fat32_change_dir(name) != 0) {
        vga_puts("Diretorio nao encontrado\n");
    }
}
