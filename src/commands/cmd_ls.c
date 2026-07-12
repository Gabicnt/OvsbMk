#include "shell_cmds.h"
#include "cmd_common.h"
#include "../fs/fat32.h"

void cmd_ls(void) {
    int count = fat32_list_dir();
    if (count == 0) {
        set_vga_color(C_OUTPUT);
        vga_puts("(vazio)\n");
    }
}
