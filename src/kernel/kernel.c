#include "idt.h"
#include "memory.h"
#include "../drivers/ata.h"
#include "../commands/shell_cmds.h"
#include "../fs/fat32.h"
#include "colors.h"
#include "../lib/libgui/vesa.h"
#include "../apps/terminal/terminal.h"
#include <stdint.h>

framebuffer_t fb;

void keyboard_init(void);
void keyboard_handler(void);
char keyboard_read(void);
void pic_init(void);

void parse_multiboot2(uint32_t magic, uint32_t addr) {
    if (magic != 0x36D76289) return;
    uint8_t *tags = (uint8_t *)(uintptr_t)addr;
    uint32_t total_size = *(uint32_t *)(uintptr_t)addr;
    for (uint32_t i = 8; i < total_size; ) {
        uint32_t type = *(uint32_t *)(tags + i);
        uint32_t size = *(uint32_t *)(tags + i + 4);
        if (type == 8) {
            fb.addr = *(uint64_t *)(tags + i + 8);
            fb.pitch = *(uint32_t *)(tags + i + 16);
            fb.width = *(uint32_t *)(tags + i + 20);
            fb.height = *(uint32_t *)(tags + i + 24);
            fb.bpp = *(uint8_t *)(tags + i + 28);
        }
        i += size;
        if (i % 8) i += 8 - (i % 8);
    }
}

void kmain(uint32_t magic, uint32_t mb_info) {
    parse_multiboot2(magic, mb_info);

    idt_init();
    pic_init();
    idt_set_syscall();
    idt_set_irq1();
    keyboard_init();
    memory_init();
    __asm__ volatile ("sti");
    ata_init();
    fat32_init();

    if (fb.addr != 0 && fb.bpp == 32) {
        vesa_map_framebuffer(fb.addr);
        vesa_init(&fb);
        terminal_app_init();
        terminal_app_run();
    }

    while (1) __asm__ volatile ("hlt");
}

int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a - *b;
}

int strncmp(const char *a, const char *b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
        if (!a[i]) return 0;
    }
    return 0;
}

void execute_command(const char *cmd) {
    if (strcmp(cmd, "help") == 0) cmd_help();
    else if (strcmp(cmd, "clear") == 0) cmd_clear();
    else if (strncmp(cmd, "echo ", 5) == 0) cmd_echo(cmd + 5);
    else if (strcmp(cmd, "about") == 0) cmd_about();
    else if (strcmp(cmd, "shutdown") == 0) cmd_shutdown();
    else if (strcmp(cmd, "ls") == 0) cmd_ls();
    else if (strncmp(cmd, "touch ", 6) == 0) cmd_touch(cmd + 6);
    else if (strncmp(cmd, "rm ", 3) == 0) cmd_rm(cmd + 3);
    else if (strncmp(cmd, "cat ", 4) == 0) cmd_cat(cmd + 4);
    else if (strncmp(cmd, "edit ", 5) == 0) cmd_edit(cmd + 5);
    else if (strncmp(cmd, "mkdir ", 6) == 0) cmd_mkdir(cmd + 6);
    else if (strncmp(cmd, "cd ", 3) == 0) cmd_cd(cmd + 3);
    else if (strcmp(cmd, "pwd") == 0) cmd_pwd();
}
