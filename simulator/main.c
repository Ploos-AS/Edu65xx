#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

static void print_state(const edu65xx_cpu_t *cpu)
{
    printf("PC=%04X A=%02X X=%02X Y=%02X SP=%02X P=%02X "
           "[N=%u V=%u D=%u I=%u Z=%u C=%u]\n",
           cpu->pc,
           cpu->a,
           cpu->x,
           cpu->y,
           cpu->sp,
           cpu->p,
           (cpu->p & EDU65XX_FLAG_N) != 0u,
           (cpu->p & EDU65XX_FLAG_V) != 0u,
           (cpu->p & EDU65XX_FLAG_D) != 0u,
           (cpu->p & EDU65XX_FLAG_I) != 0u,
           (cpu->p & EDU65XX_FLAG_Z) != 0u,
           (cpu->p & EDU65XX_FLAG_C) != 0u);
}

static void print_bus_trace(const edu65xx_cpu_t *cpu)
{
    size_t i;

    puts("bus:");
    for (i = 0; i < cpu->bus_trace_count; ++i) {
        const edu65xx_bus_cycle_t *cycle = &cpu->bus_trace[i];
        printf("  %c $%04X  $%02X\n",
               cycle->is_write != 0u ? 'W' : 'R',
               cycle->address,
               cycle->data);
    }
}

static int parse_byte(const char *text, uint8_t *value)
{
    char *end = NULL;
    unsigned long parsed;

    errno = 0;
    parsed = strtoul(text, &end, 16);

    if (errno != 0 || end == text || *end != '\0' || parsed > 0xFFul) {
        return -1;
    }

    *value = (uint8_t)parsed;
    return 0;
}

int main(int argc, char **argv)
{
    edu65xx_cpu_t cpu = {0};
    size_t program_size;
    size_t i;

    if (argc < 2) {
        fprintf(stderr,
                "usage: %s BYTE [BYTE ...]\n"
                "example: %s A9 05 85 10 A5 10 EA\n",
                argv[0], argv[0]);
        return 2;
    }

    program_size = (size_t)(argc - 1);
    if (program_size > (0xFFFAu - EDU65XX_ROM_BASE)) {
        fputs("program is too large for ROM before the vectors\n", stderr);
        return 2;
    }

    for (i = 0; i < program_size; ++i) {
        if (parse_byte(argv[i + 1], &cpu.memory[EDU65XX_ROM_BASE + i]) != 0) {
            fprintf(stderr, "invalid hex byte: %s\n", argv[i + 1]);
            return 2;
        }
    }

    cpu.memory[0xFFFC] = (uint8_t)(EDU65XX_ROM_BASE & 0xFFu);
    cpu.memory[0xFFFD] = (uint8_t)(EDU65XX_ROM_BASE >> 8);
    edu65xx_cpu_reset(&cpu);

    puts("reset:");
    print_state(&cpu);
    print_bus_trace(&cpu);

    while (cpu.pc >= EDU65XX_ROM_BASE &&
           (size_t)(cpu.pc - EDU65XX_ROM_BASE) < program_size) {
        uint16_t instruction_pc = cpu.pc;
        uint8_t opcode = cpu.memory[instruction_pc];

        printf("\nstep @ %04X opcode=%02X\n", instruction_pc, opcode);

        if (edu65xx_cpu_step(&cpu) != 0) {
            fprintf(stderr,
                    "unsupported opcode %02X at %04X\n",
                    opcode, instruction_pc);
            print_bus_trace(&cpu);
            return 1;
        }

        print_state(&cpu);
        print_bus_trace(&cpu);
    }

    return 0;
}
