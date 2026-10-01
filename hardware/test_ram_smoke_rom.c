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
    int pass = 0;

    if (argc != 2) return 2;
    file = fopen(argv[1], "rb"); assert(file != NULL);
    assert(fread(rom,1u,sizeof(rom),file) == sizeof(rom));
    assert(fgetc(file) == EOF); fclose(file);

    edu65xx_machine_init(&machine);
    assert(edu65xx_machine_load_rom(&machine,rom,sizeof(rom),0u) == 0);
    edu65xx_machine_reset(&machine);

    for (steps=0; steps<1000u && !pass; ++steps) {
        assert(edu65xx_machine_step(&machine) == 0);
        if ((machine.cpu.via.ddrb & 0x03u) == 0x03u &&
            (machine.cpu.via.orb & 0x03u) == 0x01u)
            pass = 1;
        assert((machine.cpu.via.orb & 0x03u) != 0x02u);
    }

    assert(pass);
    assert(machine.cpu.memory[0x0000] == 0x55u);
    assert(machine.cpu.memory[0x7FFF] == 0x55u);
    assert(machine.cpu.memory[0x0001] == 0xAAu);
    assert(machine.cpu.memory[0x7FFE] == 0xAAu);
    puts("Rev A RAM smoke ROM: PASS");
    return 0;
}
