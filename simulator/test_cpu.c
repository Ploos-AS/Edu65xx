#include <assert.h>
#include <stdio.h>

#include "cpu.h"

static void set_reset_vector(edu65xx_cpu_t *cpu, uint16_t address)
{
    cpu->memory[0xFFFC] = (uint8_t)(address & 0xFFu);
    cpu->memory[0xFFFD] = (uint8_t)(address >> 8);
}

static void test_reset_state(void)
{
    edu65xx_cpu_t cpu = {0};
    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);
    assert(cpu.pc == 0xC000u);
    assert(cpu.sp == 0xFDu);
    assert(cpu.p == 0x24u);
    assert(cpu.bus_trace_count == 2u);
    assert(cpu.bus_trace[0].address == 0xFFFCu);
    assert(cpu.bus_trace[1].address == 0xFFFDu);
}

static void test_via_gpio(void)
{
    edu65xx_cpu_t cpu = {0};

    /* LDA #$0F; STA $8002; LDA #$05; STA $8000 */
    cpu.memory[0xC000] = 0xA9; cpu.memory[0xC001] = 0x0F;
    cpu.memory[0xC002] = 0x8D; cpu.memory[0xC003] = 0x02; cpu.memory[0xC004] = 0x80;
    cpu.memory[0xC005] = 0xA9; cpu.memory[0xC006] = 0x05;
    cpu.memory[0xC007] = 0x8D; cpu.memory[0xC008] = 0x00; cpu.memory[0xC009] = 0x80;
    set_reset_vector(&cpu, 0xC000u);

    edu65xx_cpu_reset(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.via.ddrb == 0x0Fu);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.via.orb == 0x05u);
    assert(edu65xx_via_portb_pins(&cpu.via) == 0x05u);
    assert(cpu.bus_trace[3].address == 0x8000u);
    assert(cpu.bus_trace[3].is_write == 1u);
}

static void test_via_input(void)
{
    edu65xx_cpu_t cpu = {0};
    cpu.via.ddrb = 0x01u;
    cpu.via.orb = 0x01u;
    cpu.via.input_b = 0x02u;
    assert(edu65xx_via_read(&cpu.via, 0u) == 0x03u);
}

static void test_ram_and_rom_write_rules(void)
{
    edu65xx_cpu_t cpu = {0};
    edu65xx_write8(&cpu, 0x1234u, 0x55u);
    assert(cpu.memory[0x1234] == 0x55u);

    cpu.memory[0xC123] = 0xAAu;
    edu65xx_write8(&cpu, 0xC123u, 0x11u);
    assert(cpu.memory[0xC123] == 0xAAu);
}

static void test_stack(void)
{
    edu65xx_cpu_t cpu = {0};
    cpu.memory[0xC000] = 0xA9; cpu.memory[0xC001] = 0x33;
    cpu.memory[0xC002] = 0x48;
    cpu.memory[0xC003] = 0xA9; cpu.memory[0xC004] = 0x00;
    cpu.memory[0xC005] = 0x68;
    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x01FD] == 0x33u);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0x33u);
}

int main(void)
{
    test_reset_state();
    test_via_gpio();
    test_via_input();
    test_ram_and_rom_write_rules();
    test_stack();
    puts("edu65xx simulator tests: PASS");
    return 0;
}
