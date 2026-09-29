#ifndef EDU65XX_CPU_H
#define EDU65XX_CPU_H

#include <stdint.h>

#define EDU65XX_MEM_SIZE 65536u

#define EDU65XX_FLAG_C 0x01u
#define EDU65XX_FLAG_Z 0x02u
#define EDU65XX_FLAG_I 0x04u
#define EDU65XX_FLAG_D 0x08u
#define EDU65XX_FLAG_B 0x10u
#define EDU65XX_FLAG_U 0x20u
#define EDU65XX_FLAG_V 0x40u
#define EDU65XX_FLAG_N 0x80u

typedef struct {
    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    uint8_t p;
    uint8_t memory[EDU65XX_MEM_SIZE];
} edu65xx_cpu_t;

void edu65xx_cpu_reset(edu65xx_cpu_t *cpu);
int edu65xx_cpu_step(edu65xx_cpu_t *cpu);

#endif
