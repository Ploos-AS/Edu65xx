#include <assert.h>
#include <stdio.h>

#include "cpu.h"

static void test_reset_state(void)
{
    edu65xx_cpu_t cpu = {0};

    edu65xx_cpu_reset(&cpu);

    assert(cpu.pc == 0x0000);
    assert(cpu.a == 0x00);
    assert(cpu.x == 0x00);
    assert(cpu.y == 0x00);
    assert(cpu.sp == 0xFD);
    assert(cpu.p == 0x24);
}

static void test_load_immediate(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0x0000] = 0xA9; /* LDA #$42 */
    cpu.memory[0x0001] = 0x42;
    cpu.memory[0x0002] = 0xA2; /* LDX #$00 */
    cpu.memory[0x0003] = 0x00;
    cpu.memory[0x0004] = 0xA0; /* LDY #$80 */
    cpu.memory[0x0005] = 0x80;

    edu65xx_cpu_reset(&cpu);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0x42);
    assert(cpu.pc == 0x0002);
    assert((cpu.p & (EDU65XX_FLAG_N | EDU65XX_FLAG_Z)) == 0);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.x == 0x00);
    assert((cpu.p & EDU65XX_FLAG_Z) != 0);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.y == 0x80);
    assert((cpu.p & EDU65XX_FLAG_N) != 0);
    assert((cpu.p & EDU65XX_FLAG_Z) == 0);
}

static void test_adc_immediate(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0x0000] = 0xA9; /* LDA #$FE */
    cpu.memory[0x0001] = 0xFE;
    cpu.memory[0x0002] = 0x69; /* ADC #$02 */
    cpu.memory[0x0003] = 0x02;

    edu65xx_cpu_reset(&cpu);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);

    assert(cpu.a == 0x00);
    assert((cpu.p & EDU65XX_FLAG_C) != 0);
    assert((cpu.p & EDU65XX_FLAG_Z) != 0);
}

static void test_unknown_opcode(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0x0000] = 0x02;
    edu65xx_cpu_reset(&cpu);

    assert(edu65xx_cpu_step(&cpu) == -1);
    assert(cpu.pc == 0x0001);
}

int main(void)
{
    test_reset_state();
    test_load_immediate();
    test_adc_immediate();
    test_unknown_opcode();
    puts("edu65xx simulator tests: PASS");
    return 0;
}
