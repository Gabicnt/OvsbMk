<p align="center">
  <img src="https://img.shields.io/badge/arch-x86__64-blue?style=for-the-badge&logo=intel">
  <img src="https://img.shields.io/badge/version-0.2.0-green?style=for-the-badge">
  <img src="https://img.shields.io/badge/license-MIT-orange?style=for-the-badge">
  <img src="https://img.shields.io/badge/status-active-success?style=for-the-badge">
</p>

<h1 align="center">🖥️ ovsb.os</h1>

<p align="center"><strong>Sistema operacional 64-bit com kernel hibrido e shell interativo</strong></p>

<p align="center">
  <a href="#-guia-rapido">Guia Rapido</a> •
  <a href="#-comandos">Comandos</a> •
  <a href="#-estrutura">Estrutura</a> •
  <a href="#-roadmap">Roadmap</a>
</p>

---

## 📦 Guia Rapido

```bash
# Instalar dependencias
sudo apt install -y git nasm gcc binutils grub-pc-bin xorriso qemu-system-x86 dosfstools

# Clonar
git clone https://github.com/Gabicnt/Ovsb.OS.git
cd Ovsb.OS

# Compilar
make clean && make iso

# Criar disco
dd if=/dev/zero of=disk.img bs=1M count=128
mkfs.vfat -F 32 disk.img

# Executar
qemu-system-x86_64 -boot d -cdrom OvsbMkM.iso -hda disk.img -m 256M

⌨️ Comandos
Comando	Descricao
help	Lista comandos
clear	Limpa a tela
echo	Exibe texto
about	Informacoes do sistema
ls	Lista arquivos
touch	Cria arquivo
rm	Remove arquivo
cat	Exibe conteudo
edit	Editor de texto
mkdir	Cria diretorio
cd	Muda diretorio
pwd	Caminho atual
shutdown	Desliga
📁 Estrutura
text

src/
├── kernel/       # Boot, IDT, memoria, syscalls
├── drivers/      # Teclado, ATA, VGA
├── fs/           # FAT32
└── commands/     # Comandos do shell

🗺️ Roadmap
Fase	Meta	Status
1	Boot, terminal, teclado	✅
2	Memoria, disco, FAT32	✅
3	Comandos de arquivo	✅
4	Diretorios e caminhos	🔄
5	Instalador no disco	⬜
6	Modo usuario e processos	⬜
7	Interface grafica (WindowServer)	⬜
🧠 Kernel

O kernel do ovsb.os e o OvsbMkM — um kernel hibrido 64-bit inspirado no XNU.
<p align="center"> <sub>Feito com ☕ por <a href="https://github.com/Gabicnt">Gabicnt</a></sub> </p> ```
