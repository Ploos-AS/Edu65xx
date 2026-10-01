#include <assert.h>
#include <stdio.h>

#include "machine.h"
#include "monitor_rom.h"


static void run_until_tx(edu65xx_machine_t *machine, size_t count)
{
    size_t guard = 2000u;
    while (machine->cpu.serial.tx_count < count && guard-- != 0u) {
        assert(edu65xx_machine_step(machine) == 0);
    }
    assert(machine->cpu.serial.tx_count >= count);
}

static void test_monitor_rom_end_to_end(void)
{
    static const char banner[] = "Edu65xx ready\r\n";
    static const char help[] = "? help\r\n";
    edu65xx_machine_t machine;
    size_t i;

    edu65xx_machine_init(&machine);
    assert(edu65xx_machine_load_rom(&machine, edu65xx_monitor_rom,
                                    sizeof(edu65xx_monitor_rom), 0u) == 0);
    edu65xx_machine_reset(&machine);
    assert(machine.cpu.pc == 0xC000u);

    run_until_tx(&machine, sizeof(banner) - 1u);
    for (i = 0; i < sizeof(banner) - 1u; ++i) {
        assert(machine.cpu.serial.tx[i] == (uint8_t)banner[i]);
    }

    /* Input is injected at the device boundary; output must still be
       produced by monitor instructions executing through the machine. */
    edu65xx_serial_receive(&machine.cpu.serial, '?');
    run_until_tx(&machine, (sizeof(banner) - 1u) + (sizeof(help) - 1u));

    for (i = 0; i < sizeof(help) - 1u; ++i) {
        assert(machine.cpu.serial.tx[(sizeof(banner) - 1u) + i] ==
               (uint8_t)help[i]);
    }
}

static void test_rom_boot_and_serial(void)
{
    edu65xx_machine_t machine;
    uint8_t rom[EDU65XX_ROM_SIZE] = {0};

    /* LDA #'H'; STA $8010; LDA #'i'; STA $8010 */
    rom[0x0000] = 0xA9; rom[0x0001] = 'H';
    rom[0x0002] = 0x8D; rom[0x0003] = 0x10; rom[0x0004] = 0x80;
    rom[0x0005] = 0xA9; rom[0x0006] = 'i';
    rom[0x0007] = 0x8D; rom[0x0008] = 0x10; rom[0x0009] = 0x80;
    rom[0x3FFC] = 0x00;
    rom[0x3FFD] = 0xC0;

    edu65xx_machine_init(&machine);
    assert(edu65xx_machine_load_rom(&machine, rom, sizeof(rom), 0u) == 0);
    edu65xx_machine_reset(&machine);
    assert(machine.cpu.pc == 0xC000u);

    assert(edu65xx_machine_step(&machine) == 0);
    assert(edu65xx_machine_step(&machine) == 0);
    assert(edu65xx_machine_step(&machine) == 0);
    assert(edu65xx_machine_step(&machine) == 0);

    assert(machine.steps == 4u);
    assert(machine.cpu.serial.tx_count == 2u);
    assert(machine.cpu.serial.tx[0] == 'H');
    assert(machine.cpu.serial.tx[1] == 'i');
}

static void test_rom_bounds(void)
{
    edu65xx_machine_t machine;
    uint8_t byte = 0xEAu;

    edu65xx_machine_init(&machine);
    assert(edu65xx_machine_load_rom(&machine, &byte, 1u, EDU65XX_ROM_SIZE - 1u) == 0);
    assert(edu65xx_machine_load_rom(&machine, &byte, 1u, EDU65XX_ROM_SIZE) == -1);
}

static void test_deterministic_device_tick(void)
{
    edu65xx_machine_t machine;
    uint8_t rom[EDU65XX_ROM_SIZE] = {0};

    rom[0x0000] = 0xEA; /* NOP */
    rom[0x0001] = 0xEA; /* NOP */
    rom[0x3FFC] = 0x00;
    rom[0x3FFD] = 0xC0;

    edu65xx_machine_init(&machine);
    assert(edu65xx_machine_load_rom(&machine, rom, sizeof(rom), 0u) == 0);
    edu65xx_machine_reset(&machine);

    edu65xx_via_write(&machine.cpu.via, 4u, 0x02u);
    edu65xx_via_write(&machine.cpu.via, 5u, 0x00u);
    assert(machine.cpu.via.t1_counter == 2u);

    assert(edu65xx_machine_step(&machine) == 0);
    assert(machine.cpu.via.t1_counter == 1u);
    assert(edu65xx_machine_step(&machine) == 0);
    assert(machine.cpu.via.t1_counter == 0u);
    assert((machine.cpu.via.ifr & EDU65XX_VIA_IFR_T1) != 0u);
}

int main(void)
{
    test_monitor_rom_end_to_end();
    test_rom_boot_and_serial();
    test_rom_bounds();
    test_deterministic_device_tick();
    puts("edu65xx emulator machine tests: PASS");
    return 0;
}
