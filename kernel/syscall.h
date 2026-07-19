#ifndef SYSCALL_H
#define SYSCALL_H
#include <stdint.h>

void syscall_init(void);
void syscall_handler(uint64_t *regs);

#endif
