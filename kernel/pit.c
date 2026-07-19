/* ♥ PIT - Programmable Interval Timer ~ "Tick, tick, tick!"
 * Dica: divisor = 1193182 / freq_desejada ~
 * Se dividir por 0, o timer vai explodir (e o PC também)~ lol
 * 100Hz = 1193182/100 = 11931 = 0x2E9B ♥
 * Agora chama schedule() a cada tick! Round-robin ativado~ kyun! */

#include "pit.h"
#include "process.h"

static inline void outb(uint16_t port, uint8_t val) { __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port)); }

volatile uint64_t timer_ticks = 0;

void timer_tick_handler(void) {
    timer_ticks++;
    outb(0x20, 0x20);
    schedule();
}

void pit_init(void) {
    uint32_t div = 1193182 / 100;
    outb(0x43, 0x36); outb(0x40, div & 0xFF); outb(0x40, (div >> 8) & 0xFF);
}

void sleep_ms(uint64_t ms) {
    uint64_t target = timer_ticks + ms / 10 + 1;
    while (timer_ticks < target) { __asm__ volatile ("pause"); }
}
