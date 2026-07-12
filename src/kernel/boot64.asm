; ============================================================
; boot64.asm - Bootloader Multiboot2 com VESA framebuffer
; Transição 32-bit -> 64-bit com paginação e identity mapping
; O framebuffer NÃO é mapeado aqui - isso é feito no kernel
; via vesa_map_framebuffer(), após parse do Multiboot2.
; ============================================================

BITS 32

; ------------------------------------------------------------
; Multiboot2 header
; ------------------------------------------------------------
section .multiboot2
align 8
mb2_header_start:
    dd 0xE85250D6                ; magic
    dd 0                         ; architecture (0 = i386/x86)
    dd mb2_header_end - mb2_header_start   ; header length
    dd -(0xE85250D6 + 0 + (mb2_header_end - mb2_header_start)) & 0xFFFFFFFF ; checksum

    ; --- Tag: framebuffer ---
    align 8
    dw 5            ; type = 5 (framebuffer)
    dw 0            ; flags
    dd fb_tag_end - fb_tag_start ; size
fb_tag_start:
    dd 1024         ; width
    dd 768          ; height
    dd 32           ; depth
fb_tag_end:

    ; --- Tag: end ---
    align 8
    dw 0
    dw 0
    dd 8
mb2_header_end:

; ------------------------------------------------------------
; BSS - tabelas de paginação e pilha
; ------------------------------------------------------------
section .bss
align 4096
pml4_table:
    resb 4096
global pd_table
pd_table:
    resb 4096
pdpt_table:
    resb 4096

stack_bottom:
    resb 16384
stack_top:

; ------------------------------------------------------------
; Área de dados guardados (magic / mb_info)
; ------------------------------------------------------------
section .data
align 8
saved_magic:      dd 0
saved_mbinfo:     dd 0

gdt64:
    dq 0                                   ; null descriptor
.code_seg:
    dq 0x00AF9A000000FFFF                  ; code64: exec/read, long mode
.data_seg:
    dq 0x00CF92000000FFFF                  ; data64: read/write
gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64 - 1
    dq gdt64

CODE64_SEL equ gdt64.code_seg - gdt64
DATA64_SEL equ gdt64.data_seg - gdt64

; ------------------------------------------------------------
; Código de boot em 32-bit
; ------------------------------------------------------------
section .text
BITS 32
global _start
extern kmain

_start:
    cli
    mov esp, stack_top

    ; salva magic (eax) e ponteiro multiboot info (ebx)
    mov [saved_magic], eax
    mov [saved_mbinfo], ebx

    ; --------------------------------------------------------
    ; Monta tabelas de paginação:
    ; PML4[0]      -> PDPT
    ; PDPT[0]      -> PD
    ; PD[0..1]     -> identity map 4MB (páginas de 2MB, flags 0x83 = present+rw+huge)
    ;
    ; O framebuffer NÃO é mapeado aqui. Isso é feito depois,
    ; em runtime, por vesa_map_framebuffer() no kernel, que
    ; escreve diretamente nas entradas restantes deste mesmo
    ; pd_table (exportado via 'global pd_table').
    ; --------------------------------------------------------

    ; limpa tabelas
    mov edi, pml4_table
    xor eax, eax
    mov ecx, 4096/4
    rep stosd

    mov edi, pdpt_table
    xor eax, eax
    mov ecx, 4096/4
    rep stosd

    mov edi, pd_table
    xor eax, eax
    mov ecx, 4096/4
    rep stosd

    ; PML4[0] = PDPT | present+rw
    mov eax, pdpt_table
    or eax, 0x3
    mov [pml4_table], eax

    ; PDPT[0] = PD | present+rw
    mov eax, pd_table
    or eax, 0x3
    mov [pdpt_table], eax

    ; Identity mapping 4MB usando 2 páginas de 2MB (huge pages)
    ; PD[0] = 0x000000 | present+rw+huge(0x83)
    mov dword [pd_table + 0*8], 0x00000083
    mov dword [pd_table + 0*8 + 4], 0x0

    ; PD[1] = 0x200000 | present+rw+huge
    mov dword [pd_table + 1*8], 0x00200083
    mov dword [pd_table + 1*8 + 4], 0x0

    ; --------------------------------------------------------
    ; Habilita PAE (CR4.PAE)
    ; --------------------------------------------------------
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; --------------------------------------------------------
    ; Carrega PML4 em CR3
    ; --------------------------------------------------------
    mov eax, pml4_table
    mov cr3, eax

    ; --------------------------------------------------------
    ; Habilita Long Mode (EFER.LME)
    ; --------------------------------------------------------
    mov ecx, 0xC0000080     ; MSR EFER
    rdmsr
    or eax, 1 << 8          ; LME
    wrmsr

    ; --------------------------------------------------------
    ; Habilita paginação (CR0.PG) e proteção (CR0.PE já ativo)
    ; --------------------------------------------------------
    mov eax, cr0
    or eax, 1 << 31         ; PG
    or eax, 1               ; PE
    mov cr0, eax

    ; --------------------------------------------------------
    ; Carrega GDT 64-bit e faz far jump para código 64-bit
    ; --------------------------------------------------------
    lgdt [gdt64_descriptor]

    jmp CODE64_SEL:long_mode_start

; ------------------------------------------------------------
; Código 64-bit
; ------------------------------------------------------------
BITS 64
long_mode_start:
    mov ax, DATA64_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, stack_top

    ; recupera magic/mbinfo salvos em 32-bit e chama kmain(magic, mbinfo)
    mov eax, [saved_magic]
    mov edi, eax                ; edi = magic (1º argumento, System V AMD64)

    mov eax, [saved_mbinfo]
    mov esi, eax                ; esi = mb_info (2º argumento)

    call kmain

.hang:
    cli
    hlt
    jmp .hang