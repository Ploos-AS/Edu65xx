#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "machine.h"

int main(int argc, char **argv)
{
    edu65xx_machine_t m;
    uint8_t rom[EDU65XX_ROM_SIZE];
    FILE *f;
    unsigned steps;

    if (argc != 2) return 2;
    f = fopen(argv[1], "rb"); assert(f != NULL);
    assert(fread(rom, 1u, sizeof(rom), f) == sizeof(rom));
    assert(fgetc(f) == EOF); fclose(f);

    edu65xx_machine_init(&m);
    assert(edu65xx_machine_load_rom(&m, rom, sizeof(rom), 0u) == 0);
    edu65xx_machine_reset(&m);

    for (steps = 0u; steps < 100u && m.cpu.stopped == 0u; ++steps)
        assert(edu65xx_machine_step(&m) == 0);

    assert(m.cpu.stopped != 0u);
    assert(m.cpu.memory[0x0201] == 42u);
    printf("M7 mixed C/assembly ROM: PASS after %u steps\n", steps);
    return 0;
}
