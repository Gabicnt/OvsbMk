#include "shell_cmds.h"
#include "../fs/fat32.h"

extern void vga_puts(const char *s);

void cmd_mkdir(const char *name) {
    if (!name || !*name) {
        vga_puts("Uso: mkdir <nome>\n");
        return;
    }
    if (fat32_create_dir(name) == 0) {
        vga_puts("Diretorio criado\n");
    } else {
        vga_puts("Erro ao criar diretorio\n");
    }
}
