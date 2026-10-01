#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "machine.h"

int main(int argc, char **argv)
{
    edu65xx_machine_t machine;
    uint8_t rom[EDU65XX_ROM_SIZE];
    FILE *file;
    unsigned steps;

    if (argc != 2) {
        fprintf(stderr, "usage: %s program.bin\n", argv[0]);
        return 2;
    }

    file = fopen(argv[1], "rb");
    assert(file != NULL);
    assert(fread(rom, 1u, sizeof(rom), file) == sizeof(rom));
    assert(fgetc(file) == EOF);
    fclose(file);

    edu65xx_machine_init(&machine);
    assert(edu65xx_machine_load_rom(&machine, rom, sizeof(rom), 0u) == 0);
    edu65xx_machine_reset(&machine);
    assert(machine.cpu.pc == 0xC000u);

    for (steps = 0u; steps < 100u && machine.cpu.stopped == 0u; ++steps)
        assert(edu65xx_machine_step(&machine) == 0);

    assert(machine.cpu.stopped != 0u);
    assert(machine.cpu.memory[0x0200] == 42u);
    printf("M7 linked multi-module ROM: PASS after %u steps\n", steps);
    return 0;
}
