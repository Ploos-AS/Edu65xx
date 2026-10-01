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
    } else if (address >= EDU65XX_SERIAL_BASE && address <= EDU65XX_SERIAL_END) {
        value = edu65xx_serial_read(&cpu->serial, (uint8_t)(address - EDU65XX_SERIAL_BASE));
    } else if (address >= 0x8000u && address < EDU65XX_ROM_BASE) {
        /* Unmapped I/O reads use a deterministic simulator convention. */
        value = 0xFFu;
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
    } else if (address >= EDU65XX_SERIAL_BASE && address <= EDU65XX_SERIAL_END) {
        edu65xx_serial_write(&cpu->serial, (uint8_t)(address - EDU65XX_SERIAL_BASE), value);
    } else if (address < 0x8000u) {
        cpu->memory[address] = value;
    } else {
        /* Unmapped I/O and ROM writes are ignored. */
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

static uint16_t read_zp16(edu65xx_cpu_t *cpu, uint8_t address)
{
    uint8_t lo = edu65xx_read8(cpu, address);
    uint8_t hi = edu65xx_read8(cpu, (uint8_t)(address + 1u));
    return (uint16_t)lo | ((uint16_t)hi << 8);
}

static uint16_t addr_zp(edu65xx_cpu_t *cpu)
{
    return fetch8(cpu);
}

static uint16_t addr_zpx(edu65xx_cpu_t *cpu)
{
    return (uint8_t)(fetch8(cpu) + cpu->x);
}

static uint16_t addr_zpy(edu65xx_cpu_t *cpu)
{
    return (uint8_t)(fetch8(cpu) + cpu->y);
}

static uint16_t addr_abs(edu65xx_cpu_t *cpu)
{
    return fetch16(cpu);
}

static uint16_t addr_absx(edu65xx_cpu_t *cpu)
{
    return (uint16_t)(fetch16(cpu) + cpu->x);
}

static uint16_t addr_absy(edu65xx_cpu_t *cpu)
{
    return (uint16_t)(fetch16(cpu) + cpu->y);
}

static uint16_t addr_indx(edu65xx_cpu_t *cpu)
{
    uint8_t zp = (uint8_t)(fetch8(cpu) + cpu->x);
    return read_zp16(cpu, zp);
}

static uint16_t addr_indy(edu65xx_cpu_t *cpu)
{
    uint8_t zp = fetch8(cpu);
    return (uint16_t)(read_zp16(cpu, zp) + cpu->y);
}

static uint16_t addr_zp_ind(edu65xx_cpu_t *cpu)
{
    return read_zp16(cpu, fetch8(cpu));
}

static void set_nz(edu65xx_cpu_t *cpu, uint8_t value)
{
    cpu->p &= (uint8_t)~(EDU65XX_FLAG_N | EDU65XX_FLAG_Z);
    if (value == 0u) cpu->p |= EDU65XX_FLAG_Z;
    if ((value & 0x80u) != 0u) cpu->p |= EDU65XX_FLAG_N;
}

static void bit8(edu65xx_cpu_t *cpu, uint8_t value, int immediate)
{
    cpu->p &= (uint8_t)~EDU65XX_FLAG_Z;
    if ((cpu->a & value) == 0u) cpu->p |= EDU65XX_FLAG_Z;
    if (!immediate) {
        cpu->p = (uint8_t)((cpu->p & (uint8_t)~(EDU65XX_FLAG_N | EDU65XX_FLAG_V)) |
                           (value & (EDU65XX_FLAG_N | EDU65XX_FLAG_V)));
    }
}

static void inc_memory(edu65xx_cpu_t *cpu, uint16_t address, int delta)
{
    uint8_t value = edu65xx_read8(cpu, address);
    value = (uint8_t)(value + delta);
    edu65xx_write8(cpu, address, value);
    set_nz(cpu, value);
}

static void compare8(edu65xx_cpu_t *cpu, uint8_t lhs, uint8_t rhs)
{
    uint8_t result = (uint8_t)(lhs - rhs);
    cpu->p &= (uint8_t)~EDU65XX_FLAG_C;
    if (lhs >= rhs) cpu->p |= EDU65XX_FLAG_C;
    set_nz(cpu, result);
}

static void branch_relative(edu65xx_cpu_t *cpu, int condition)
{
    int8_t offset = (int8_t)fetch8(cpu);
    if (condition) {
        cpu->pc = (uint16_t)(cpu->pc + offset);
    }
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
    edu65xx_serial_reset(&cpu->serial);

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
    case 0xA9: cpu->a = fetch8(cpu); set_nz(cpu, cpu->a); return 0; /* LDA # */
    case 0xA5: cpu->a = edu65xx_read8(cpu, addr_zp(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0xB5: cpu->a = edu65xx_read8(cpu, addr_zpx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0xAD: cpu->a = edu65xx_read8(cpu, addr_abs(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0xBD: cpu->a = edu65xx_read8(cpu, addr_absx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0xB9: cpu->a = edu65xx_read8(cpu, addr_absy(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0xA1: cpu->a = edu65xx_read8(cpu, addr_indx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0xB1: cpu->a = edu65xx_read8(cpu, addr_indy(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0xB2: cpu->a = edu65xx_read8(cpu, addr_zp_ind(cpu)); set_nz(cpu, cpu->a); return 0;

    case 0x85: edu65xx_write8(cpu, addr_zp(cpu), cpu->a); return 0; /* STA */
    case 0x95: edu65xx_write8(cpu, addr_zpx(cpu), cpu->a); return 0;
    case 0x8D: edu65xx_write8(cpu, addr_abs(cpu), cpu->a); return 0;
    case 0x9D: edu65xx_write8(cpu, addr_absx(cpu), cpu->a); return 0;
    case 0x99: edu65xx_write8(cpu, addr_absy(cpu), cpu->a); return 0;
    case 0x81: edu65xx_write8(cpu, addr_indx(cpu), cpu->a); return 0;
    case 0x91: edu65xx_write8(cpu, addr_indy(cpu), cpu->a); return 0;
    case 0x92: edu65xx_write8(cpu, addr_zp_ind(cpu), cpu->a); return 0;

    case 0xA2: cpu->x = fetch8(cpu); set_nz(cpu, cpu->x); return 0; /* LDX */
    case 0xA6: cpu->x = edu65xx_read8(cpu, addr_zp(cpu)); set_nz(cpu, cpu->x); return 0;
    case 0xB6: cpu->x = edu65xx_read8(cpu, addr_zpy(cpu)); set_nz(cpu, cpu->x); return 0;
    case 0xAE: cpu->x = edu65xx_read8(cpu, addr_abs(cpu)); set_nz(cpu, cpu->x); return 0;
    case 0xBE: cpu->x = edu65xx_read8(cpu, addr_absy(cpu)); set_nz(cpu, cpu->x); return 0;
    case 0x86: edu65xx_write8(cpu, addr_zp(cpu), cpu->x); return 0; /* STX */
    case 0x96: edu65xx_write8(cpu, addr_zpy(cpu), cpu->x); return 0;
    case 0x8E: edu65xx_write8(cpu, addr_abs(cpu), cpu->x); return 0;

    case 0xA0: cpu->y = fetch8(cpu); set_nz(cpu, cpu->y); return 0; /* LDY */
    case 0xA4: cpu->y = edu65xx_read8(cpu, addr_zp(cpu)); set_nz(cpu, cpu->y); return 0;
    case 0xB4: cpu->y = edu65xx_read8(cpu, addr_zpx(cpu)); set_nz(cpu, cpu->y); return 0;
    case 0xAC: cpu->y = edu65xx_read8(cpu, addr_abs(cpu)); set_nz(cpu, cpu->y); return 0;
    case 0xBC: cpu->y = edu65xx_read8(cpu, addr_absx(cpu)); set_nz(cpu, cpu->y); return 0;
    case 0x84: edu65xx_write8(cpu, addr_zp(cpu), cpu->y); return 0; /* STY */
    case 0x94: edu65xx_write8(cpu, addr_zpx(cpu), cpu->y); return 0;
    case 0x8C: edu65xx_write8(cpu, addr_abs(cpu), cpu->y); return 0;
    case 0x29: cpu->a &= fetch8(cpu); set_nz(cpu, cpu->a); return 0; /* AND */
    case 0x25: cpu->a &= edu65xx_read8(cpu, addr_zp(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x35: cpu->a &= edu65xx_read8(cpu, addr_zpx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x2D: cpu->a &= edu65xx_read8(cpu, addr_abs(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x3D: cpu->a &= edu65xx_read8(cpu, addr_absx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x39: cpu->a &= edu65xx_read8(cpu, addr_absy(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x21: cpu->a &= edu65xx_read8(cpu, addr_indx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x31: cpu->a &= edu65xx_read8(cpu, addr_indy(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x32: cpu->a &= edu65xx_read8(cpu, addr_zp_ind(cpu)); set_nz(cpu, cpu->a); return 0;

    case 0x09: cpu->a |= fetch8(cpu); set_nz(cpu, cpu->a); return 0; /* ORA */
    case 0x05: cpu->a |= edu65xx_read8(cpu, addr_zp(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x15: cpu->a |= edu65xx_read8(cpu, addr_zpx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x0D: cpu->a |= edu65xx_read8(cpu, addr_abs(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x1D: cpu->a |= edu65xx_read8(cpu, addr_absx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x19: cpu->a |= edu65xx_read8(cpu, addr_absy(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x01: cpu->a |= edu65xx_read8(cpu, addr_indx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x11: cpu->a |= edu65xx_read8(cpu, addr_indy(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x12: cpu->a |= edu65xx_read8(cpu, addr_zp_ind(cpu)); set_nz(cpu, cpu->a); return 0;

    case 0x49: cpu->a ^= fetch8(cpu); set_nz(cpu, cpu->a); return 0; /* EOR */
    case 0x45: cpu->a ^= edu65xx_read8(cpu, addr_zp(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x55: cpu->a ^= edu65xx_read8(cpu, addr_zpx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x4D: cpu->a ^= edu65xx_read8(cpu, addr_abs(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x5D: cpu->a ^= edu65xx_read8(cpu, addr_absx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x59: cpu->a ^= edu65xx_read8(cpu, addr_absy(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x41: cpu->a ^= edu65xx_read8(cpu, addr_indx(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x51: cpu->a ^= edu65xx_read8(cpu, addr_indy(cpu)); set_nz(cpu, cpu->a); return 0;
    case 0x52: cpu->a ^= edu65xx_read8(cpu, addr_zp_ind(cpu)); set_nz(cpu, cpu->a); return 0;

    case 0x89: bit8(cpu, fetch8(cpu), 1); return 0; /* BIT # */
    case 0x24: bit8(cpu, edu65xx_read8(cpu, addr_zp(cpu)), 0); return 0;
    case 0x34: bit8(cpu, edu65xx_read8(cpu, addr_zpx(cpu)), 0); return 0;
    case 0x2C: bit8(cpu, edu65xx_read8(cpu, addr_abs(cpu)), 0); return 0;
    case 0x3C: bit8(cpu, edu65xx_read8(cpu, addr_absx(cpu)), 0); return 0;

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
    case 0x4C: /* JMP abs */
        cpu->pc = fetch16(cpu); return 0;
    case 0x20: { /* JSR abs */
        uint16_t target = fetch16(cpu);
        uint16_t return_address = (uint16_t)(cpu->pc - 1u);
        push8(cpu, (uint8_t)(return_address >> 8));
        push8(cpu, (uint8_t)return_address);
        cpu->pc = target;
        return 0;
    }
    case 0x60: { /* RTS */
        uint8_t lo = pull8(cpu);
        uint8_t hi = pull8(cpu);
        cpu->pc = (uint16_t)(((uint16_t)hi << 8) | lo);
        ++cpu->pc;
        return 0;
    }
    case 0xC9: compare8(cpu, cpu->a, fetch8(cpu)); return 0; /* CMP */
    case 0xC5: compare8(cpu, cpu->a, edu65xx_read8(cpu, addr_zp(cpu))); return 0;
    case 0xD5: compare8(cpu, cpu->a, edu65xx_read8(cpu, addr_zpx(cpu))); return 0;
    case 0xCD: compare8(cpu, cpu->a, edu65xx_read8(cpu, addr_abs(cpu))); return 0;
    case 0xDD: compare8(cpu, cpu->a, edu65xx_read8(cpu, addr_absx(cpu))); return 0;
    case 0xD9: compare8(cpu, cpu->a, edu65xx_read8(cpu, addr_absy(cpu))); return 0;
    case 0xC1: compare8(cpu, cpu->a, edu65xx_read8(cpu, addr_indx(cpu))); return 0;
    case 0xD1: compare8(cpu, cpu->a, edu65xx_read8(cpu, addr_indy(cpu))); return 0;
    case 0xD2: compare8(cpu, cpu->a, edu65xx_read8(cpu, addr_zp_ind(cpu))); return 0;
    case 0xE0: compare8(cpu, cpu->x, fetch8(cpu)); return 0; /* CPX */
    case 0xE4: compare8(cpu, cpu->x, edu65xx_read8(cpu, addr_zp(cpu))); return 0;
    case 0xEC: compare8(cpu, cpu->x, edu65xx_read8(cpu, addr_abs(cpu))); return 0;
    case 0xC0: compare8(cpu, cpu->y, fetch8(cpu)); return 0; /* CPY */
    case 0xC4: compare8(cpu, cpu->y, edu65xx_read8(cpu, addr_zp(cpu))); return 0;
    case 0xCC: compare8(cpu, cpu->y, edu65xx_read8(cpu, addr_abs(cpu))); return 0;
    case 0xF0: /* BEQ */
        branch_relative(cpu, (cpu->p & EDU65XX_FLAG_Z) != 0u); return 0;
    case 0xD0: /* BNE */
        branch_relative(cpu, (cpu->p & EDU65XX_FLAG_Z) == 0u); return 0;
    case 0x80: /* BRA (65C02) */
        branch_relative(cpu, 1); return 0;
    case 0xE8: /* INX */
        ++cpu->x; set_nz(cpu, cpu->x); return 0;
    case 0xCA: /* DEX */
        --cpu->x; set_nz(cpu, cpu->x); return 0;
    case 0xC8: /* INY */
        ++cpu->y; set_nz(cpu, cpu->y); return 0;
    case 0x88: /* DEY */
        --cpu->y; set_nz(cpu, cpu->y); return 0;
    case 0xAA: /* TAX */
        cpu->x = cpu->a; set_nz(cpu, cpu->x); return 0;
    case 0x8A: /* TXA */
        cpu->a = cpu->x; set_nz(cpu, cpu->a); return 0;
    case 0xA8: /* TAY */
        cpu->y = cpu->a; set_nz(cpu, cpu->y); return 0;
    case 0x98: /* TYA */
        cpu->a = cpu->y; set_nz(cpu, cpu->a); return 0;
    case 0x18: cpu->p &= (uint8_t)~EDU65XX_FLAG_C; return 0; /* CLC */
    case 0x38: cpu->p |= EDU65XX_FLAG_C; return 0; /* SEC */
    case 0xD8: cpu->p &= (uint8_t)~EDU65XX_FLAG_D; return 0; /* CLD */
    case 0xF8: cpu->p |= EDU65XX_FLAG_D; return 0; /* SED */
    case 0xB8: cpu->p &= (uint8_t)~EDU65XX_FLAG_V; return 0; /* CLV */

    case 0xBA: cpu->x = cpu->sp; set_nz(cpu, cpu->x); return 0; /* TSX */
    case 0x9A: cpu->sp = cpu->x; return 0; /* TXS */
    case 0xDA: push8(cpu, cpu->x); return 0; /* PHX */
    case 0xFA: cpu->x = pull8(cpu); set_nz(cpu, cpu->x); return 0; /* PLX */
    case 0x5A: push8(cpu, cpu->y); return 0; /* PHY */
    case 0x7A: cpu->y = pull8(cpu); set_nz(cpu, cpu->y); return 0; /* PLY */

    case 0x64: edu65xx_write8(cpu, addr_zp(cpu), 0u); return 0; /* STZ */
    case 0x74: edu65xx_write8(cpu, addr_zpx(cpu), 0u); return 0;
    case 0x9C: edu65xx_write8(cpu, addr_abs(cpu), 0u); return 0;
    case 0x9E: edu65xx_write8(cpu, addr_absx(cpu), 0u); return 0;

    case 0x1A: ++cpu->a; set_nz(cpu, cpu->a); return 0; /* INC A */
    case 0x3A: --cpu->a; set_nz(cpu, cpu->a); return 0; /* DEC A */
    case 0xE6: inc_memory(cpu, addr_zp(cpu), 1); return 0; /* INC */
    case 0xF6: inc_memory(cpu, addr_zpx(cpu), 1); return 0;
    case 0xEE: inc_memory(cpu, addr_abs(cpu), 1); return 0;
    case 0xFE: inc_memory(cpu, addr_absx(cpu), 1); return 0;
    case 0xC6: inc_memory(cpu, addr_zp(cpu), -1); return 0; /* DEC */
    case 0xD6: inc_memory(cpu, addr_zpx(cpu), -1); return 0;
    case 0xCE: inc_memory(cpu, addr_abs(cpu), -1); return 0;
    case 0xDE: inc_memory(cpu, addr_absx(cpu), -1); return 0;

    case 0xEA:
        return 0;
    default:
        return -1;
    }
}
