#include "cpu.h"

static uint8_t fetch8(edu65xx_cpu_t *cpu)
{
    return cpu->memory[cpu->pc++];
}

static void set_nz(edu65xx_cpu_t *cpu, uint8_t value)
{
    cpu->p &= (uint8_t)~(EDU65XX_FLAG_N | EDU65XX_FLAG_Z);

    if (value == 0u) {
        cpu->p |= EDU65XX_FLAG_Z;
    }
    if ((value & 0x80u) != 0u) {
        cpu->p |= EDU65XX_FLAG_N;
    }
}

void edu65xx_cpu_reset(edu65xx_cpu_t *cpu)
{
    cpu->pc = 0x0000;
    cpu->a = 0x00;
    cpu->x = 0x00;
    cpu->y = 0x00;
    cpu->sp = 0xFD;
    cpu->p = 0x24;
}

int edu65xx_cpu_step(edu65xx_cpu_t *cpu)
{
    uint8_t opcode = fetch8(cpu);

    switch (opcode) {
    case 0xA9: /* LDA #imm */
        cpu->a = fetch8(cpu);
        set_nz(cpu, cpu->a);
        return 0;

    case 0xA2: /* LDX #imm */
        cpu->x = fetch8(cpu);
        set_nz(cpu, cpu->x);
        return 0;

    case 0xA0: /* LDY #imm */
        cpu->y = fetch8(cpu);
        set_nz(cpu, cpu->y);
        return 0;

    case 0x69: { /* ADC #imm */
        uint8_t value = fetch8(cpu);
        uint16_t sum = (uint16_t)cpu->a + (uint16_t)value;

        if ((cpu->p & EDU65XX_FLAG_C) != 0u) {
            ++sum;
        }

        cpu->p &= (uint8_t)~EDU65XX_FLAG_C;
        if (sum > 0xFFu) {
            cpu->p |= EDU65XX_FLAG_C;
        }

        cpu->a = (uint8_t)sum;
        set_nz(cpu, cpu->a);
        return 0;
    }

    case 0xEA: /* NOP */
        return 0;

    default:
        return -1;
    }
}
