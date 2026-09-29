#include "cpu.h"

void edu65xx_cpu_reset(edu65xx_cpu_t *cpu)
{
    cpu->pc = 0x0000;
    cpu->a = 0x00;
    cpu->x = 0x00;
    cpu->y = 0x00;
    cpu->sp = 0xFD;
    cpu->p = 0x24;
}
