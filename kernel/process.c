/* ♥ PROCESS ~ Gerenciamento de processos fofinho!
 * Dica: PCB = Process Control Block~ salva tudo do processo!
 * O context_witch troca de processo que nem trocar de roupa~
 * Hihi, olha o PID! kyun~ <3
 * 
 * Processo idle (PID 0) = o shell do kernel~
 * Processo usuario (PID 1) = programinha ring 3 mesugaki! */
#include "process.h"
#include "tss.h"
#include "memory.h"
#include "serial.h"
#include "console.h"

#define MAX_PROC 16
#define STACK_SIZE 8192

/* Prototipo do assembly */
extern void context_switch(pcb_t *current, pcb_t *next);

static pcb_t pcb_table[MAX_PROC];
static int pcb_count = 0;
static int current_pid = 0;
static int next_pid = 1;

/* ♥ Programinha ring 3 ~ gerado de user_prog.asm via user_prog.rebuild */
#include "user_prog_bin.h"

static pcb_t *alloc_pcb(void) {
    if (pcb_count >= MAX_PROC) return 0;
    return &pcb_table[pcb_count++];
}

static void setup_user_stack(pcb_t *pcb, void *entry, void *ustack, uint64_t ustack_size) {
    uint64_t *sp = (uint64_t *)pcb->kernel_rsp;
    /* ♥ Ordem do push: o primeiro push fica no TOPO (maior endereco).
     * O context_switch no switch.asm da POP na ordem: R15...RAX, IRETQ.
     * Entao o layout da pilha (baixo pra cima) precisa ser:
     * R15, R14, ..., RCX, RAX, RIP, CS, RFLAGS, RSP, SS. */
    *--sp = 0x23;                      /* SS (primeiro push = maior endereco) */
    *--sp = (uint64_t)ustack + ustack_size; /* user_RSP */
    *--sp = 0x202;                     /* RFLAGS */
    *--sp = 0x1B;                      /* CS */
    *--sp = (uint64_t)entry;           /* RIP */
    *--sp = 0; /* RAX */
    *--sp = 0; /* RCX */
    *--sp = 0; /* RDX */
    *--sp = 0; /* RSI */
    *--sp = 0; /* RDI */
    *--sp = 0; /* R8 */
    *--sp = 0; /* R9 */
    *--sp = 0; /* R10 */
    *--sp = 0; /* R11 */
    *--sp = 0; /* RBX */
    *--sp = 0; /* RBP */
    *--sp = 0; /* R12 */
    *--sp = 0; /* R13 */
    *--sp = 0; /* R14 */
    *--sp = 0; /* R15 (ultimo push = menor endereco = RSP apos restore) */
    pcb->kernel_rsp = (uint64_t)sp;
}

static void idle_entry(void) {
    serial_puts("[PROCESS] Idle process running~ kyun!\r\n");
    console_write("[Processo idle rodando]\n");
    for (;;) {
        __asm__ volatile("hlt");
    }
}

void process_init(void) {
    serial_puts("[PROC] init~\r\n");
    for (int i = 0; i < MAX_PROC; i++) {
        for (int j = 0; j < (int)sizeof(pcb_t); j++)
            ((uint8_t *)&pcb_table[i])[j] = 0;
        pcb_table[i].pid = -1;
    }
    pcb_count = 0;
    current_pid = 0;
    next_pid = 1;

    /* Cria processo idle (PID 0) — o shell do kernel */
    pcb_t *idle = alloc_pcb();
    if (!idle) return;

    /* Aloca pilha de kernel pro idle */
    void *kstack = kmalloc(STACK_SIZE);
    if (!kstack) return;

    idle->pid = 0;
    idle->state = 0;  /* running */
    idle->is_user = 0;
    for (int i = 0; "idle"[i]; i++) idle->name[i] = "idle"[i];

    /* Pilha do idle: topo = kstack + STACK_SIZE - 8 */
    uint64_t *sp = (uint64_t *)((uint8_t *)kstack + STACK_SIZE);
    /* Endereco de retorno pro idle_entry */
    *--sp = (uint64_t)idle_entry;
    idle->kernel_rsp = (uint64_t)sp;
    idle->rsp0 = (uint64_t)sp;
    idle->pml4 = pml4_get_current();
}

int process_create_user(const char *name, void *entry, void *user_stack, uint64_t user_stack_size) {
    pcb_t *pcb = alloc_pcb();
    if (!pcb) return -1;

    void *kstack = kmalloc(STACK_SIZE);
    if (!kstack) return -1;

    pcb->pid = next_pid++;
    pcb->state = 0;
    pcb->is_user = 1;
    for (int i = 0; name[i] && i < 31; i++) pcb->name[i] = name[i];

    uint64_t *ksp = (uint64_t *)((uint8_t *)kstack + STACK_SIZE);
    pcb->kernel_rsp = (uint64_t)ksp;
    pcb->rsp0 = (uint64_t)ksp;
    pcb->pml4 = pml4_get_current();
    pcb->program_break = 0x201000;
    pcb->heap_start = 0x201000;

    setup_user_stack(pcb, entry, user_stack, user_stack_size);

    return pcb->pid;
}

void process_switch_to(int pid) {
    pcb_t *current = process_current();
    if (!current) return;

    pcb_t *next = 0;
    for (int i = 0; i < pcb_count; i++) {
        if (pcb_table[i].pid == pid) { next = &pcb_table[i]; break; }
    }
    if (!next) return;

    current_pid = pid;
    tss_set_rsp0(next->rsp0);
    context_switch(current, next);
}

void process_exit_current(int code) {
    pcb_t *current = process_current();
    if (!current) return;
    current->state = 3;  /* zombie */
    current->exit_code = code;

    /* Volta pro processo idle (PID 0) */
    current_pid = 0;
    pcb_t *idle = &pcb_table[0];
    tss_set_rsp0(idle->rsp0);
    context_switch(current, idle);
}

pcb_t *process_current(void) {
    for (int i = 0; i < pcb_count; i++)
        if (pcb_table[i].pid == current_pid) return &pcb_table[i];
    return 0;
}

int process_current_pid(void) {
    return current_pid;
}
