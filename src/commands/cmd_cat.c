#include "shell_cmds.h"
#include "../fs/fat32.h"

extern void vga_puts(const char *s);
extern void vga_putchar(char c);

void cmd_cat(const char *name) {
    if (!name || !*name) {
        vga_puts("Uso: cat <nome>\n");
        return;
    }
    uint8_t buffer[512];
    int bytes = fat32_read_file(name, buffer, 512);
    if (bytes > 0) {
        for (int i = 0; i < bytes; i++) vga_putchar(buffer[i]);
        vga_putchar('\n');
    } else {
        vga_puts("Erro ao ler arquivo\n");
    }
}
