<p align="center">
  <img src="https://img.shields.io/badge/arch-x86__64-blue?style=for-the-badge&logo=intel">
  <img src="https://img.shields.io/badge/version-0.2.0-green?style=for-the-badge">
  <img src="https://img.shields.io/badge/license-MIT-orange?style=for-the-badge">
  <img src="https://img.shields.io/badge/status-active-success?style=for-the-badge">
</p>

<h1 align="center">⚙️ OvsbMkM</h1>

<p align="center"><strong>Kernel hibrido 64-bit inspirado no XNU (Mach + BSD)</strong></p>

<p align="center">
  <a href="#componentes">Componentes</a> •
  <a href="#build">Build</a> •
  <a href="#estrutura">Estrutura</a> •
  <a href="#roadmap">Roadmap</a>
</p>

---

## Visao Geral

O **OvsbMkM** e o nucleo do sistema operacional [ovsb.os](https://github.com/Gabicnt/Ovsb.OS).  
Combina conceitos de microkernel (IPC Mach, servidores em userspace) com desempenho monolítico.

---

## Componentes

| Modulo | Descricao | Status |
|--------|-----------|--------|
| **Boot** | GRUB + Multiboot2, transicao 32 para 64-bit | OK |
| **IDT/PIC** | Tabela de interrupcoes, controlador 8259 | OK |
| **Memoria** | Alocador de paginas + heap (kmalloc/kfree) | OK |
| **VGA** | Terminal 80x25 com scroll e cores | OK |
| **PS/2** | Teclado com Shift, maiusculas, simbolos, ESC | OK |
| **ATA** | Driver IDE/ATA PIO (leitura/escrita de setores) | OK |
| **FAT32** | Criar, ler, escrever, deletar arquivos e listar diretorios | OK |
| **Syscalls** | Stubs para chamadas de sistema BSD | Em breve |
| **Mach-O** | Carregador de binarios Mach-O (prototipo) | Em breve |

---

## Build

### Dependencias

sudo apt install -y git nasm gcc binutils grub-pc-bin xorriso qemu-system-x86 dosfstools

### Compilar e testar

git clone https://github.com/Gabicnt/OvsbMkM.git
cd OvsbMkM
make clean && make iso
qemu-system-x86_64 -cdrom OvsbMkM.iso -m 256M

### Com disco virtual (FAT32)

dd if=/dev/zero of=disk.img bs=1M count=128
mkfs.vfat -F 32 disk.img
qemu-system-x86_64 -cdrom OvsbMkM.iso -hda disk.img -m 256M

---

## Estrutura

src/
├── kernel/          # Nucleo do kernel
│   ├── boot64.asm   # Bootloader Multiboot2
│   ├── kernel.c     # kmain + shell parser
│   ├── idt.c/asm    # Interrupt Descriptor Table
│   ├── memory.c     # Gerenciador de memoria
│   ├── pic.c        # Controlador de interrupcoes
│   └── linker.ld    # Script de linkagem
├── drivers/         # Drivers de hardware
│   ├── keyboard.c   # Teclado PS/2
│   └── ata.c        # Disco IDE/ATA
└── fs/              # Sistemas de arquivos
    └── fat32.c      # FAT32 driver

---

## Roadmap

| Fase | Meta | Status |
|------|------|--------|
| **1** | Boot, terminal, teclado | OK |
| **2** | Memoria, ATA, FAT32 | OK |
| **3** | Diretorios (mkdir, cd), caminhos | Pendente |
| **4** | Modo usuario, processos, syscalls reais | Pendente |
| **5** | IPC Mach, servidores externos | Pendente |
| **6** | Driver grafico, WindowServer | Pendente |

---

## Projeto relacionado

**[Ovsb.OS](https://github.com/Gabicnt/Ovsb.OS)** — O sistema operacional completo, com shell, comandos e futura interface grafica.

---

<p align="center">
  <sub>Feito com ☕ por <a href="https://github.com/Gabicnt">Gabicnt</a></sub>
</p>