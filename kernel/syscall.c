/* ♥ SYSCALL ~ Chamadas de sistema! "Pede pro kernel fazer~ hihi!"
 * 0 = exit, 1 = write, 2 = getpid, 3 = read, 4 = sbrk, 5 = time
 * Argumentos em RDI, RSI, RDX (igual System V ABI)~
 * Mesugaki mode: syscall desconhecida retorna -1 ~ *~ qui qui!~" */
#include "syscall.h"
#include "process.h"
#include "console.h"
#include "serial.h"
#include "pit.h"
#include "../drivers/keyboard.h"

void syscall_init(void) {
}

void syscall_handler(uint64_t *regs) {
    uint64_t num  = regs[0];
    uint64_t arg1 = regs[4];
    uint64_t arg2 = regs[3];
    uint64_t arg3 = regs[2];

    switch (num) {
    case 0:
        process_exit_current((int)arg1);
        break;

    case 1: {
        int fd = (int)arg1;
        const char *buf = (const char *)arg2;
        int count = (int)arg3;
        if (fd == 1 || fd == 2) {
            for (int i = 0; i < count; i++)
                if (buf[i]) console_putchar(buf[i]);
        }
        regs[14] = count;
        break;
    }

    case 2:
        regs[14] = process_current_pid();
        break;

    case 3: {
        int fd = (int)arg1;
        char *buf = (char *)arg2;
        int count = (int)arg3;
        if (fd == 0) {
            int total = 0;
            while (total < count) {
                while (!keyboard_avail()) {
                    for (volatile int i = 0; i < 100; i++);
                }
                buf[total++] = keyboard_read();
                if (total < count && buf[total-1] != '\n' && buf[total-1] != 3)
                    continue;
                break;
            }
            regs[14] = total;
        } else {
            regs[14] = -1;
        }
        break;
    }

    case 4: {
        /* void *sbrk(int increment) */
        pcb_t *pcb = process_current();
        if (!pcb) { regs[14] = -1; break; }
        int64_t inc = (int64_t)(int)arg1;
        uint64_t old = pcb->program_break;
        pcb->program_break += inc;
        regs[14] = old;
        break;
    }

    case 5:
        /* uint64_t time(void) */
        regs[14] = timer_ticks;
        break;

    default:
        regs[14] = -1;
        break;
    }
}
