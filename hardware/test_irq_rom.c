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
    int saw_irq = 0;
    int saw_isr_effect = 0;
    int saw_rti = 0;

    if (argc != 2) {
        fprintf(stderr, "usage: %s via-irq.bin\n", argv[0]);
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

    for (steps = 0u; steps < 70000u && !saw_rti; ++steps) {
        uint16_t before = machine.cpu.pc;
        assert(edu65xx_machine_step(&machine) == 0);

        if (machine.last_event == EDU65XX_MACHINE_EVENT_IRQ) {
            saw_irq = 1;
            assert(machine.cpu.pc == 0xC01Eu);
            assert(machine.cpu.sp == 0xFCu);
            assert(machine.cpu.memory[0x01FF] == 0xC0u);
            assert(machine.cpu.memory[0x01FE] == 0x1Cu);
            assert((machine.cpu.memory[0x01FD] & EDU65XX_FLAG_B) == 0u);
        }

        if (saw_irq && (machine.cpu.via.orb & 0x01u) == 0u)
            saw_isr_effect = 1;

        if (saw_isr_effect && before == 0xC028u && machine.cpu.pc == 0xC01Cu) {
            saw_rti = 1;
            assert(machine.cpu.sp == 0xFFu);
        }
    }

    assert((machine.cpu.via.ddrb & 0x01u) != 0u);
    assert(saw_irq);
    assert(saw_isr_effect);
    assert(saw_rti);
    assert((machine.cpu.via.ifr & EDU65XX_VIA_IFR_T1) == 0u);

    printf("Rev A VIA IRQ ROM: PASS after %u machine steps\n", steps);
    return 0;
}
