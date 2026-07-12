#include "shell_cmds.h"
#include "../fs/fat32.h"

extern void vga_puts(const char *s);
extern void vga_putchar(char c);

void cmd_pwd(void) {
    char path[256];
    fat32_get_path(path);
    vga_puts(path);
    vga_putchar('\n');
}
