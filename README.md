# OvsbMkM ♥

Kernel x86-64 com ring 3, syscalls, VESA text console e OWT!

## Oq tem?

**Kernel core**
- boot64 UEFI/BIOS via GRUB
- IDT com syscall gate (int 0x80, DPL=3)
- TSS pra ring 0 stack qdo ring 3 int
- Process: PCB, context_switch (switch.asm), idle + user proc
- 6 syscalls: exit, write, getpid, read, sbrk, time
- Shell: help, clear, echo, info, hexdump, run, owt, reboot

**Drivers**
- VESA framebuffer 1280x720 32bpp
- Console VESA 80x45 c/ scrollback
- Teclado PS/2 (scancode set 1 → ASCII)
- Serial debug, PIT 100Hz

**OWT** (Ovsb Widget Toolkit)
- Window, Label, Button, TextBox, StatusBar
- ListView, ComboBox, Menu, Dialog
- Tema escuro/claro, primitivas de desenho

**WM** (Window Manager)
- Backbuffer pra desenhar sem flicker
- flush pro framebuffer do video

## Build

```bash
make run    # sobe no QEMU
make clean  # limpa tudo
```

## Comandos do shell

```
help       Mostra ajuda
clear      Limpa tela
echo <t>   Imprime texto
info       Info do sistema (VESA, heap)
hexdump <a> Exibe 64 bytes do endereco
run        Executa programa ring 3
owt        Demo OWT (widget toolkit)
reboot     Reinicia
```

Ring 3 test: digita `run`, programa echo loop até ESC.

---

Feito c/ carinho (e preguiça) por Haruna Himekawa ~ kyun! ♥
