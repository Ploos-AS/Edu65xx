#include "cpu.h"

static void trace_cycle(edu65xx_cpu_t *cpu, uint16_t address, uint8_t data, uint8_t is_write)
{
    if (cpu->bus_trace_count < EDU65XX_BUS_TRACE_MAX) {
        edu65xx_bus_cycle_t *cycle = &cpu->bus_trace[cpu->bus_trace_count++];
        cycle->address = address;
        cycle->data = data;
        cycle->is_write = is_write;
    }
}

void edu65xx_bus_trace_clear(edu65xx_cpu_t *cpu)
{
    cpu->bus_trace_count = 0u;
}

uint8_t edu65xx_read8(edu65xx_cpu_t *cpu, uint16_t address)
{
    uint8_t value = cpu->memory[address];
    trace_cycle(cpu, address, value, 0u);
    return value;
}

void edu65xx_write8(edu65xx_cpu_t *cpu, uint16_t address, uint8_t value)
{
    cpu->memory[address] = value;
    trace_cycle(cpu, address, value, 1u);
}

static uint8_t fetch8(edu65xx_cpu_t *cpu)
{
    return edu65xx_read8(cpu, cpu->pc++);
}

static uint16_t fetch16(edu65xx_cpu_t *cpu)
{
    uint8_t lo = fetch8(cpu);
    uint8_t hi = fetch8(cpu);
    return (uint16_t)lo | ((uint16_t)hi << 8);
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
    uint8_t lo;
    uint8_t hi;

    cpu->a = 0x00;
    cpu->x = 0x00;
    cpu->y = 0x00;
    cpu->sp = 0xFD;
    cpu->p = 0x24;

    edu65xx_bus_trace_clear(cpu);
    lo = edu65xx_read8(cpu, 0xFFFCu);
    hi = edu65xx_read8(cpu, 0xFFFDu);
    cpu->pc = (uint16_t)lo | ((uint16_t)hi << 8);
}

int edu65xx_cpu_step(edu65xx_cpu_t *cpu)
{
    uint8_t opcode;

    edu65xx_bus_trace_clear(cpu);
    opcode = fetch8(cpu);

    switch (opcode) {
    case 0xA9: /* LDA #imm */
        cpu->a = fetch8(cpu);
        set_nz(cpu, cpu->a);
        return 0;

    case 0xA5: { /* LDA zp */
        uint8_t address = fetch8(cpu);
        cpu->a = edu65xx_read8(cpu, address);
        set_nz(cpu, cpu->a);
        return 0;
    }

    case 0xAD: { /* LDA abs */
        uint16_t address = fetch16(cpu);
        cpu->a = edu65xx_read8(cpu, address);
        set_nz(cpu, cpu->a);
        return 0;
    }

    case 0x85: { /* STA zp */
        uint8_t address = fetch8(cpu);
        edu65xx_write8(cpu, address, cpu->a);
        return 0;
    }

    case 0x8D: { /* STA abs */
        uint16_t address = fetch16(cpu);
        edu65xx_write8(cpu, address, cpu->a);
        return 0;
    }

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

    case 0x48: /* PHA */
        edu65xx_write8(cpu, (uint16_t)(0x0100u | cpu->sp), cpu->a);
        --cpu->sp;
        return 0;

    case 0x68: /* PLA */
        ++cpu->sp;
        cpu->a = edu65xx_read8(cpu, (uint16_t)(0x0100u | cpu->sp));
        set_nz(cpu, cpu->a);
        return 0;

    case 0xEA: /* NOP */
        return 0;

    default:
        return -1;
    }
}
