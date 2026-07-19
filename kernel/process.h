#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>

typedef struct pcb {
    int pid;
    int state;
    char name[32];
    int is_user;
    int padding;
    uint64_t kernel_rsp;
    uint64_t rsp0;
    uint64_t pml4;
    uint64_t user_rsp;
    uint64_t user_rip;
    uint64_t user_rflags;
    uint64_t s_rbx, s_rbp, s_r12, s_r13, s_r14, s_r15;
    int parent_pid;
    int exit_code;
    uint64_t program_break;
    uint64_t heap_start;
} pcb_t;

void process_init(void);
int  process_create_user(const char *name, void *entry, void *user_stack, uint64_t user_stack_size);
void process_switch_to(int pid);
void process_exit_current(int code);
pcb_t *process_current(void);
int    process_current_pid(void);

#endif
