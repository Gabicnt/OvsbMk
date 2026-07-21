/* ♥ RTC - Real Time Clock ~ "Acorda CMOS, me dá as horas!"
 * Dica: registrador 0x00 = segundos, 0x02 = minutos, 0x04 = horas~
 * 0x07 = dia do mes, 0x08 = mes, 0x09 = ano~
 * O CMOS fala BCD, então a gente converte~ BCD pra decimal~
 * Se der merda, é porque o NMI tá desabilitado~ kyun~ */

#include "rtc.h"

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static uint8_t read_cmos(uint8_t reg) {
    outb(0x70, reg);
    return inb(0x71);
}

static int bcd_to_dec(int v) {
    return (v & 0x0F) + ((v >> 4) * 10);
}

/* ♥ Leitura completa do RTC ~ todos os campos de uma vez!
 * Ano = BCD + 2000 ~ porque ainda estamos no seculo 21~
 * Se o CMOS nao responder, é porque o PC é muito velho~ */
void rtc_read(rtc_time_t *t) {
    t->s  = bcd_to_dec(read_cmos(0x00));
    t->m  = bcd_to_dec(read_cmos(0x02));
    t->h  = bcd_to_dec(read_cmos(0x04));
    t->dy = bcd_to_dec(read_cmos(0x07));
    t->mo = bcd_to_dec(read_cmos(0x08));
    t->yr = bcd_to_dec(read_cmos(0x09)) + 2000;
}
