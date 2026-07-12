#include "shell_cmds.h"
#include "../fs/fat32.h"

extern void vga_puts(const char *s);
extern void vga_putchar(char c);
extern char keyboard_read(void);

void cmd_edit(const char *name) {
    char buf[512];
    int pos = 0;
    int cursor = 0;

    if (!name || !*name) {
        vga_puts("Uso: edit <nome>\n");
        return;
    }

    vga_puts("Editor - ESC para salvar:\n");
    while (1) {
        char c = keyboard_read();
        if (c == 27) break;
        else if (c == 128 && cursor > 0) cursor--;
        else if (c == 129 && cursor < pos) cursor++;
        else if (c == 130) cursor = 0;
        else if (c == 131) cursor = pos;
        else if (c == 132) cursor = 0;
        else if (c == 133) cursor = pos;
        else if (c == '\b' && cursor > 0) {
            for (int i = cursor - 1; i < pos - 1; i++) buf[i] = buf[i + 1];
            pos--;
            cursor--;
            vga_putchar('\b');
        }
        else if (c == '\n' && pos < 511) {
            if (cursor < pos) {
                for (int i = pos; i > cursor; i--) buf[i] = buf[i - 1];
            }
            buf[cursor++] = '\n';
            pos++;
            vga_putchar('\n');
        }
        else if (c >= ' ' && c <= '~' && pos < 511) {
            if (cursor < pos) {
                for (int i = pos; i > cursor; i--) buf[i] = buf[i - 1];
            }
            buf[cursor++] = c;
            pos++;
            vga_putchar(c);
        }
    }

    buf[pos] = '\0';
    vga_puts("\nSalvando...\n");
    if (fat32_write_file(name, (uint8_t *)buf, pos) > 0) vga_puts("OK\n");
    else vga_puts("Erro\n");
}
