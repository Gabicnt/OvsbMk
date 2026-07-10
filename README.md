<p align="center">
  <img src="https://img.shields.io/badge/arch-x86__64-blue?style=for-the-badge&logo=intel">
  <img src="https://img.shields.io/badge/version-0.2.0-green?style=for-the-badge">
  <img src="https://img.shields.io/badge/license-MIT-orange?style=for-the-badge">
  <img src="https://img.shields.io/badge/status-active-success?style=for-the-badge">
</p>

<h1 align="center">⚙️ OvsbMkM</h1>

<p align="center"><strong>Kernel hibrido 64-bit inspirado no XNU (Mach + BSD)</strong></p>

<p align="center">
  <a href="#-componentes">Componentes</a> •
  <a href="#-build">Build</a> •
  <a href="#-estrutura">Estrutura</a> •
  <a href="#-roadmap">Roadmap</a>
</p>

---

## 🧠 Visao Geral

O **OvsbMkM** e o nucleo do sistema operacional [ovsb.os](https://github.com/Gabicnt/Ovsb.OS).  
Combina conceitos de microkernel (IPC Mach, servidores em userspace) com desempenho monolítico (drivers e FS no kernel).

┌─────────────────────────────────────────┐
│ Ovsb.OS (userspace) │
│ ┌─────────┐ ┌──────────┐ ┌─────────┐ │
│ │ Shell │ │ Editor │ │ GUI │ │
│ └────┬────┘ └────┬─────┘ └────┬────┘ │
│ │ │ │ │
├───────┴───────────┴────────────┴────────┤
│ OvsbMkM (kernel) │
│ ┌──────┐ ┌──────┐ ┌──────┐ ┌───────┐ │
│ │ IDT │ │ PIC │ │ ATA │ │ FAT32 │ │
│ └──────┘ └──────┘ └──────┘ └───────┘ │
│ ┌──────┐ ┌──────┐ ┌──────┐ ┌───────┐ │
│ │ MM │ │ PS/2 │ │ VGA │ │ Sysc │ │
│ └──────┘ └──────┘ └──────┘ └───────┘ │
└─────────────────────────────────────────┘
text


---

## ✅ Componentes

| Modulo | Descricao | Status |
|--------|-----------|--------|
| **Boot** | GRUB + Multiboot2, transicao 32→64-bit | ✅ |
| **IDT/PIC** | Tabela de interrupcoes, controlador 8259 | ✅ |
| **Memoria** | Alocador de paginas + heap (`kmalloc`/`kfree`) | ✅ |
| **VGA** | Terminal 80×25 com scroll e cores | ✅ |
| **PS/2** | Teclado com Shift, maiusculas, simbolos, ESC | ✅ |
| **ATA** | Driver IDE/ATA PIO (leitura/escrita de setores) | ✅ |
| **FAT32** | Criar, ler, escrever, deletar arquivos e listar diretorios | ✅ |
| **Syscalls** | Stubs para chamadas de sistema BSD | 🟡 |
| **Mach-O** | Carregador de binarios Mach-O (prototipo) | 🟡 |

---

## 🔨 Build

### Dependencias

```bash
sudo apt install -y git nasm gcc binutils grub-pc-bin xorriso qemu-system-x86 dosfstools

Compilar e testar
bash

git clone https://github.com/Gabicnt/OvsbMkM.git
cd OvsbMkM
make clean && make iso
qemu-system-x86_64 -cdrom OvsbMkM.iso -m 256M

Com disco virtual (FAT32)
bash

dd if=/dev/zero of=disk.img bs=1M count=128
mkfs.vfat -F 32 disk.img
qemu-system-x86_64 -cdrom OvsbMkM.iso -hda disk.img -m 256M

📁 Estrutura
text

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

🗺️ Roadmap
Fase	Meta	Status
1	Boot, terminal, teclado	✅
2	Memoria, ATA, FAT32	✅
3	Diretorios (mkdir, cd), caminhos	🔄
4	Modo usuario, processos, syscalls reais	⬜
5	IPC Mach, servidores externos	⬜
6	Driver grafico, WindowServer	⬜
🤝 Projeto relacionado

Ovsb.OS — O sistema operacional completo, com shell, comandos e futura interface grafica.
<p align="center"> <sub>Feito com ☕ por <a href="https://github.com/Gabicnt">Gabicnt</a></sub> </p> ```