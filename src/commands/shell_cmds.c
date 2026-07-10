#include "shell_cmds.h"
#include "../kernel/idt.h"
#include "../fs/fat32.h"

extern void vga_puts(const char *s);
extern void vga_putchar(char c);
extern void vga_clear(void);
extern char keyboard_read(void);

void cmd_help(void) {
    vga_puts("help, clear, echo, about, shutdown\n");
    vga_puts("ls, touch, rm, cat, edit, mkdir, cd, pwd\n");
}

void cmd_clear(void) {
    vga_clear();
}

void cmd_echo(const char *args) {
    vga_puts(args);
    vga_putchar('\n');
}

void cmd_about(void) {
    vga_puts("OvsbMkM - Microkernel macOS High Sierra\n");
}

void cmd_shutdown(void) {
    vga_puts("Desligando...\n");
    __asm__ volatile ("cli; hlt");
}

void cmd_ls(void) {
    int count = fat32_list_dir();
    if (count == 0) vga_puts("(vazio)\n");
}

void cmd_touch(const char *name) {
    if (fat32_create_file(name) == 0) {
        vga_puts("Criado: ");
        vga_puts(name);
        vga_putchar('\n');
    } else {
        vga_puts("Erro\n");
    }
}

void cmd_rm(const char *name) {
    if (fat32_delete_file(name) == 0) {
        vga_puts("Removido: ");
        vga_puts(name);
        vga_putchar('\n');
    } else {
        vga_puts("Erro\n");
    }
}

void cmd_cat(const char *name) {
    uint8_t buffer[512];
    int bytes = fat32_read_file(name, buffer, 512);
    if (bytes > 0) {
        for (int i = 0; i < bytes; i++) vga_putchar(buffer[i]);
        vga_putchar('\n');
    } else {
        vga_puts("Erro\n");
    }
}

void cmd_edit(const char *name) {
    vga_puts("Editor - ESC para salvar:\n");
    static char buf[512];
    int pos = 0;
    while (1) {
        char c = keyboard_read();
        if (c == 27) { buf[pos] = '\0'; break; }
        else if (c == '\b' && pos > 0) { pos--; vga_putchar('\b'); }
        else if (c == '\n') { buf[pos++] = '\n'; vga_putchar('\n'); }
        else if (c >= ' ' && c <= '~' && pos < 511) { buf[pos++] = c; vga_putchar(c); }
    }
    vga_puts("\nSalvando...\n");
    if (fat32_write_file(name, (uint8_t*)buf, pos) > 0) vga_puts("OK\n");
    else vga_puts("Erro\n");
}

void cmd_mkdir(const char *name) { (void)name; vga_puts("TODO\n"); }
void cmd_cd(const char *name) { (void)name; vga_puts("TODO\n"); }
void cmd_pwd(void) { vga_puts("/\n"); }