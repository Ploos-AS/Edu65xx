#ifndef EDU65XX_CPU_H
#define EDU65XX_CPU_H

#include <stdint.h>

typedef struct {
    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    uint8_t p;
} edu65xx_cpu_t;

void edu65xx_cpu_reset(edu65xx_cpu_t *cpu);

#endif
