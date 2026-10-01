#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "front_panel.h"

static void load_rom(edu65xx_machine_t *m)
{
    uint8_t rom[EDU65XX_ROM_SIZE] = {0};

    /* Configure PB0-PB3 as outputs, drive 0101, then read all PB pins. */
    rom[0x0000] = 0xA9; rom[0x0001] = 0x0F;
    rom[0x0002] = 0x8D; rom[0x0003] = 0x02; rom[0x0004] = 0x80;
    rom[0x0005] = 0xA9; rom[0x0006] = 0x05;
    rom[0x0007] = 0x8D; rom[0x0008] = 0x00; rom[0x0009] = 0x80;
    rom[0x000A] = 0xAD; rom[0x000B] = 0x00; rom[0x000C] = 0x80;
    rom[0x000D] = 0x8D; rom[0x000E] = 0x00; rom[0x000F] = 0x02;
    rom[0x0010] = 0xDB;
    rom[0x3FFC] = 0x00; rom[0x3FFD] = 0xC0;

    edu65xx_machine_init(m);
    assert(edu65xx_machine_load_rom(m, rom, sizeof(rom), 0u) == 0);
    edu65xx_machine_reset(m);
}

int main(void)
{
    edu65xx_machine_t machine;
    edu65xx_front_panel_t panel;
    unsigned guard = 32u;

    load_rom(&machine);
    edu65xx_front_panel_init(&panel, &machine);

    /* Upper nibble is configured as VIA input and therefore comes from switches. */
    edu65xx_front_panel_set_switches_b(&panel, 0xA0u);

    while (machine.cpu.stopped == 0u && guard-- != 0u)
        assert(edu65xx_machine_step(&machine) == 0);

    assert(machine.cpu.stopped != 0u);
    assert(machine.cpu.memory[0x0200] == 0xA5u);
    assert(edu65xx_front_panel_leds_b(&panel) == 0x05u);

    edu65xx_front_panel_set_switches_a(&panel, 0xC3u);
    assert(machine.cpu.via.input_a == 0xC3u);
    assert(edu65xx_front_panel_leds_a(&panel) == 0x00u);

    puts("Edu65xx virtual front panel: PASS");
    return 0;
}
