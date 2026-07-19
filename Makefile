# ♥ Makefile OvsbMkM ~ Kernel console minimal!
CC := gcc
NASM := nasm
CFLAGS := -ffreestanding -nostdlib -mno-red-zone -mno-mmx -mno-sse
CFLAGS += -mgeneral-regs-only -m64 -O2 -Wall
LDFLAGS := -nostdlib -no-pie -Wl,-nostdlib -T kernel/linker.ld

VPATH := kernel:drivers:lib/gui:lib/owt:lib/wm

C_SRCS := $(notdir $(wildcard kernel/*.c drivers/*.c lib/gui/*.c lib/owt/*.c lib/wm/*.c))
ASM_SRCS := boot64.o idt_asm.o keyboard_asm.o switch_asm.o
OBJS := $(C_SRCS:.c=.o) $(ASM_SRCS)

all: OvsbMkM.iso

boot64.o: boot64.asm
	nasm -f elf64 -o $@ $<

idt_asm.o: idt.asm
	nasm -f elf64 -o $@ $<

keyboard_asm.o: keyboard_asm.asm
	nasm -f elf64 -o $@ $<

switch_asm.o: switch.asm
	nasm -f elf64 -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -I. -Ikernel -Idrivers -Ilib/gui -Ilib/owt -Ilib/wm -c -o $@ $<

kernel.elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

OvsbMkM.iso: kernel.elf
	mkdir -p iso/boot
	cp kernel.elf iso/boot/
	grub-mkrescue -o $@ iso 2>/dev/null || true
	@echo "OvsbMkM.iso gerada!"

user_prog.rebuild: user_prog.asm
	nasm -f bin -o user_prog.bin $<
	python3 -c "import sys; data=open('user_prog.bin','rb').read(); print('const uint8_t _binary_user_prog_start[] = {'); [print('    '+','.join(f'0x{b:02x}' for b in data[i:i+16])+',') for i in range(0,len(data),16)]; print('};'); print(f'const int _binary_user_prog_size = {len(data)};')" > kernel/user_prog_bin.h
	rm -f user_prog.bin
	@echo "user_prog_bin.h atualizado!"

clean:
	rm -f *.o kernel.elf OvsbMkM.iso user_prog.bin
	rm -rf iso/boot/kernel.elf
	@echo "Limpo!"

run: OvsbMkM.iso
	qemu-system-x86_64 -vga std -boot d -cdrom $< -m 256M -serial stdio

.PHONY: all clean run user_prog.rebuild
