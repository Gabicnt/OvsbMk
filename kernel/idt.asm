; ============================================================
; Ovsb.OS - Handlers de Interrupção (IDT) ~ kyun kyun~!
; "ISRs, IRQs, venham todos pro meu iretq!"
; Dica: ISR_ERR tem error code na stack, ISR_NOERR não~
; Cuidado pra não push a mais e quebrar o iretq! >_<
; ============================================================

bits 64

extern idt_handler

; Macro para criar handler sem código de erro
%macro ISR_NOERR 1
global isr%1
isr%1:
    push 0              ; Código de erro falso
    push %1             ; Número da interrupção
    jmp isr_common
%endmacro

; Macro para criar handler com código de erro
%macro ISR_ERR 1
global isr%1
isr%1:
    push %1             ; Número da interrupção
    jmp isr_common
%endmacro

; Handlers de exceção (0-31)
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

; Syscall handler (int 0x80) — ring 3 friendly! "Vem de int 0x80, seu baka~"
global syscall_isr
extern syscall_handler
syscall_isr:
    ; A CPU ja empilhou SS, RSP, RFLAGS, CS, RIP (vindo do ring 3)
    push r15
    push r14
    push r13
    push r12
    push rbp
    push rbx
    push r11
    push r10
    push r9
    push r8
    push rdi
    push rsi
    push rdx
    push rcx
    push rax
    mov rdi, rsp
    call syscall_handler
    pop rax
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop r8
    pop r9
    pop r10
    pop r11
    pop rbx
    pop rbp
    pop r12
    pop r13
    pop r14
    pop r15
    iretq

; Timer IRQ handler (goes to C function directly)
global irq0
extern timer_tick_handler
irq0:
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15
    cld
    call timer_tick_handler
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax
    iretq

global irq1

irq1:
    push 0
    push 33
    jmp isr_common

isr_common:
    ; Salvar registradores
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Chamar handler C
    mov rdi, rsp        ; Ponteiro para stack frame
    call idt_handler
    
    ; Restaurar registradores
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Limpar código de erro e número
    add rsp, 16
    
    iretq
