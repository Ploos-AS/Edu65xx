#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "machine.h"

static void run_until_tx(edu65xx_machine_t *m, size_t count)
{
    unsigned guard = 20000u;
    while (m->cpu.serial.tx_count < count && guard-- != 0u)
        assert(edu65xx_machine_step(m) == 0);
    assert(m->cpu.serial.tx_count >= count);
}

static void expect_tail(const edu65xx_machine_t *m, size_t start, const char *text)
{
    size_t i, n = strlen(text);
    assert(m->cpu.serial.tx_count >= start + n);
    for (i = 0; i < n; ++i)
        assert(m->cpu.serial.tx[start + i] == (uint8_t)text[i]);
}

static void command(edu65xx_machine_t *m, char ch, const char *expected)
{
    size_t start = m->cpu.serial.tx_count;
    edu65xx_serial_receive(&m->cpu.serial, (uint8_t)ch);
    run_until_tx(m, start + strlen(expected));
    expect_tail(m, start, expected);
}

int main(int argc, char **argv)
{
    static const char banner[] = "Edu65xx M7 app\r\n";
    edu65xx_machine_t m;
    uint8_t rom[EDU65XX_ROM_SIZE];
    FILE *f;

    if (argc != 2) return 2;
    f = fopen(argv[1], "rb"); assert(f != NULL);
    assert(fread(rom, 1u, sizeof(rom), f) == sizeof(rom));
    assert(fgetc(f) == EOF); fclose(f);

    edu65xx_machine_init(&m);
    assert(edu65xx_machine_load_rom(&m, rom, sizeof(rom), 0u) == 0);
    edu65xx_machine_reset(&m);

    run_until_tx(&m, sizeof(banner) - 1u);
    expect_tail(&m, 0u, banner);
    assert(m.cpu.memory[0x0200] == 0u);

    command(&m, 'V', "=00\r\n");
    command(&m, '+', "=01\r\n");
    command(&m, '+', "=02\r\n");
    assert(m.cpu.memory[0x0200] == 2u);
    command(&m, '-', "=01\r\n");
    assert(m.cpu.memory[0x0200] == 1u);
    command(&m, 'R', "=00\r\n");
    assert(m.cpu.memory[0x0200] == 0u);
    command(&m, '?', "+ inc  - dec  R reset  V view  ? help\r\n");

    m.cpu.memory[0x0200] = 0xFFu;
    command(&m, '+', "=00\r\n");
    assert(m.cpu.memory[0x0200] == 0u);
    command(&m, '-', "=FF\r\n");
    assert(m.cpu.memory[0x0200] == 0xFFu);

    puts("M7 multi-module assembly application: PASS");
    return 0;
}
