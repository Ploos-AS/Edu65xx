#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

#define START_PC 0x0400u
#define SUCCESS_PC 0x24F1u
#define MAX_STEPS 50000000u

int main(int argc, char **argv)
{
    edu65xx_cpu_t cpu = {0};
    FILE *file;
    size_t size;
    unsigned long steps;

    if (argc != 2) {
        fprintf(stderr, "usage: %s 65C02_extended_opcodes_test.bin\n", argv[0]);
        return 2;
    }

    file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("fopen");
        return 2;
    }
    size = fread(cpu.memory, 1u, sizeof(cpu.memory), file);
    fclose(file);
    if (size != sizeof(cpu.memory)) {
        fprintf(stderr, "expected 65536-byte qualification image, got %zu\n", size);
        return 2;
    }

    cpu.qualification_flat_memory = 1u;
    cpu.pc = START_PC;
    cpu.sp = 0xFDu;
    cpu.p = EDU65XX_FLAG_U | EDU65XX_FLAG_I;

    for (steps = 0u; steps < MAX_STEPS; ++steps) {
        uint16_t before = cpu.pc;
        int result;

        if (cpu.pc == SUCCESS_PC) {
            printf("Klaus 65C02 extended test: PASS after %lu steps\n", steps);
            return 0;
        }

        result = edu65xx_cpu_step(&cpu);
        if (result < 0) {
            fprintf(stderr, "unsupported opcode path at PC=$%04X after %lu steps\n",
                    before, steps);
            return 1;
        }

        if (cpu.pc == before && cpu.pc != SUCCESS_PC) {
            fprintf(stderr, "Klaus 65C02 extended test trapped at PC=$%04X after %lu steps\n",
                    cpu.pc, steps);
            return 1;
        }
    }

    fprintf(stderr, "Klaus 65C02 extended test exceeded %u steps at PC=$%04X\n",
            MAX_STEPS, cpu.pc);
    return 1;
}
