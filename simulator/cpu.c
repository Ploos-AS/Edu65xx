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
    uint8_t value;

    if (address >= EDU65XX_VIA_BASE && address <= EDU65XX_VIA_END) {
        value = edu65xx_via_read(&cpu->via, (uint8_t)(address - EDU65XX_VIA_BASE));
    } else {
        value = cpu->memory[address];
    }

    trace_cycle(cpu, address, value, 0u);
    return value;
}

void edu65xx_write8(edu65xx_cpu_t *cpu, uint16_t address, uint8_t value)
{
    if (address >= EDU65XX_VIA_BASE && address <= EDU65XX_VIA_END) {
        edu65xx_via_write(&cpu->via, (uint8_t)(address - EDU65XX_VIA_BASE), value);
    } else if (address < EDU65XX_ROM_BASE) {
        cpu->memory[address] = value;
    }

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
    if (value == 0u) cpu->p |= EDU65XX_FLAG_Z;
    if ((value & 0x80u) != 0u) cpu->p |= EDU65XX_FLAG_N;
}

void edu65xx_cpu_reset(edu65xx_cpu_t *cpu)
{
    uint8_t lo;
    uint8_t hi;

    cpu->a = 0u;
    cpu->x = 0u;
    cpu->y = 0u;
    cpu->sp = 0xFDu;
    cpu->p = 0x24u;
    cpu->nmi_pending = 0u;
    edu65xx_via_reset(&cpu->via);

    edu65xx_bus_trace_clear(cpu);
    lo = edu65xx_read8(cpu, 0xFFFCu);
    hi = edu65xx_read8(cpu, 0xFFFDu);
    cpu->pc = (uint16_t)lo | ((uint16_t)hi << 8);
}

static void push8(edu65xx_cpu_t *cpu, uint8_t value)
{
    edu65xx_write8(cpu, (uint16_t)(0x0100u | cpu->sp), value);
    --cpu->sp;
}

static uint8_t pull8(edu65xx_cpu_t *cpu)
{
    ++cpu->sp;
    return edu65xx_read8(cpu, (uint16_t)(0x0100u | cpu->sp));
}

void edu65xx_cpu_request_nmi(edu65xx_cpu_t *cpu)
{
    cpu->nmi_pending = 1u;
}

static void enter_interrupt(edu65xx_cpu_t *cpu, uint16_t vector)
{
    uint8_t lo;
    uint8_t hi;
    push8(cpu, (uint8_t)(cpu->pc >> 8));
    push8(cpu, (uint8_t)cpu->pc);
    push8(cpu, (uint8_t)((cpu->p & (uint8_t)~EDU65XX_FLAG_B) | EDU65XX_FLAG_U));
    cpu->p |= EDU65XX_FLAG_I;
    lo = edu65xx_read8(cpu, vector);
    hi = edu65xx_read8(cpu, (uint16_t)(vector + 1u));
    cpu->pc = (uint16_t)lo | ((uint16_t)hi << 8);
}

int edu65xx_cpu_step(edu65xx_cpu_t *cpu)
{
    uint8_t opcode;

    edu65xx_bus_trace_clear(cpu);
    if (cpu->nmi_pending != 0u) {
        cpu->nmi_pending = 0u;
        enter_interrupt(cpu, 0xFFFAu);
        return 2;
    }
    if (edu65xx_via_irq(&cpu->via) && (cpu->p & EDU65XX_FLAG_I) == 0u) {
        enter_interrupt(cpu, 0xFFFEu);
        return 1;
    }
    opcode = fetch8(cpu);

    switch (opcode) {
    case 0xA9:
        cpu->a = fetch8(cpu); set_nz(cpu, cpu->a); return 0;
    case 0xA5: {
        uint8_t address = fetch8(cpu);
        cpu->a = edu65xx_read8(cpu, address); set_nz(cpu, cpu->a); return 0;
    }
    case 0xAD: {
        uint16_t address = fetch16(cpu);
        cpu->a = edu65xx_read8(cpu, address); set_nz(cpu, cpu->a); return 0;
    }
    case 0x85: {
        uint8_t address = fetch8(cpu);
        edu65xx_write8(cpu, address, cpu->a); return 0;
    }
    case 0x8D: {
        uint16_t address = fetch16(cpu);
        edu65xx_write8(cpu, address, cpu->a); return 0;
    }
    case 0xA2:
        cpu->x = fetch8(cpu); set_nz(cpu, cpu->x); return 0;
    case 0xA0:
        cpu->y = fetch8(cpu); set_nz(cpu, cpu->y); return 0;
    case 0x69: {
        uint8_t value = fetch8(cpu);
        uint16_t sum = (uint16_t)cpu->a + (uint16_t)value;
        if ((cpu->p & EDU65XX_FLAG_C) != 0u) ++sum;
        cpu->p &= (uint8_t)~EDU65XX_FLAG_C;
        if (sum > 0xFFu) cpu->p |= EDU65XX_FLAG_C;
        cpu->a = (uint8_t)sum; set_nz(cpu, cpu->a); return 0;
    }
    case 0x48:
        push8(cpu, cpu->a); return 0;
    case 0x68:
        cpu->a = pull8(cpu); set_nz(cpu, cpu->a); return 0;
    case 0x40: { /* RTI */
        uint8_t lo;
        uint8_t hi;
        cpu->p = (uint8_t)((pull8(cpu) & (uint8_t)~EDU65XX_FLAG_B) | EDU65XX_FLAG_U);
        lo = pull8(cpu);
        hi = pull8(cpu);
        cpu->pc = (uint16_t)lo | ((uint16_t)hi << 8);
        return 0;
    }
    case 0x58: /* CLI */
        cpu->p &= (uint8_t)~EDU65XX_FLAG_I; return 0;
    case 0x78: /* SEI */
        cpu->p |= EDU65XX_FLAG_I; return 0;
    case 0xEA:
        return 0;
    default:
        return -1;
    }
}
