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
    int saw_high = 0;
    int saw_low_after_high = 0;

    if (argc != 2) {
        fprintf(stderr, "usage: %s via-blink.bin\n", argv[0]);
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

    for (steps = 0u; steps < 300000u && !saw_low_after_high; ++steps) {
        assert(edu65xx_machine_step(&machine) >= 0);
        assert((machine.cpu.via.ddrb & 0x01u) != 0u || steps < 4u);

        if ((machine.cpu.via.orb & 0x01u) != 0u)
            saw_high = 1;
        if (saw_high && (machine.cpu.via.orb & 0x01u) == 0u)
            saw_low_after_high = 1;
    }

    assert((machine.cpu.via.ddrb & 0x01u) != 0u);
    assert(saw_high);
    assert(saw_low_after_high);

    printf("Rev A bring-up ROM: PASS after %u machine steps\n", steps);
    return 0;
}
