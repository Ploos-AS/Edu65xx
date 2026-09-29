#include <assert.h>
#include <stdio.h>

#include "cpu.h"

static void test_reset_state(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0xFFFC] = 0x00;
    cpu.memory[0xFFFD] = 0x80;
    edu65xx_cpu_reset(&cpu);

    assert(cpu.pc == 0x8000);
    assert(cpu.a == 0x00);
    assert(cpu.x == 0x00);
    assert(cpu.y == 0x00);
    assert(cpu.sp == 0xFD);
    assert(cpu.p == 0x24);
    assert(cpu.bus_trace_count == 2u);
    assert(cpu.bus_trace[0].address == 0xFFFC);
    assert(cpu.bus_trace[0].data == 0x00);
    assert(cpu.bus_trace[0].is_write == 0u);
    assert(cpu.bus_trace[1].address == 0xFFFD);
    assert(cpu.bus_trace[1].data == 0x80);
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

static void test_zero_page_and_absolute_memory(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0x8000] = 0xA9; /* LDA #$5A */
    cpu.memory[0x8001] = 0x5A;
    cpu.memory[0x8002] = 0x85; /* STA $10 */
    cpu.memory[0x8003] = 0x10;
    cpu.memory[0x8004] = 0xA9; /* LDA #$00 */
    cpu.memory[0x8005] = 0x00;
    cpu.memory[0x8006] = 0xA5; /* LDA $10 */
    cpu.memory[0x8007] = 0x10;
    cpu.memory[0x8008] = 0x8D; /* STA $2345 */
    cpu.memory[0x8009] = 0x45;
    cpu.memory[0x800A] = 0x23;
    cpu.memory[0xFFFC] = 0x00;
    cpu.memory[0xFFFD] = 0x80;

    edu65xx_cpu_reset(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x0010] == 0x5A);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0x5A);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x2345] == 0x5A);
    assert(cpu.bus_trace_count == 4u);
    assert(cpu.bus_trace[3].address == 0x2345);
    assert(cpu.bus_trace[3].data == 0x5A);
    assert(cpu.bus_trace[3].is_write == 1u);
}

static void test_stack_page(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0x8000] = 0xA9; /* LDA #$33 */
    cpu.memory[0x8001] = 0x33;
    cpu.memory[0x8002] = 0x48; /* PHA */
    cpu.memory[0x8003] = 0xA9; /* LDA #$00 */
    cpu.memory[0x8004] = 0x00;
    cpu.memory[0x8005] = 0x68; /* PLA */
    cpu.memory[0xFFFC] = 0x00;
    cpu.memory[0xFFFD] = 0x80;

    edu65xx_cpu_reset(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x01FD] == 0x33);
    assert(cpu.sp == 0xFC);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0x00);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0x33);
    assert(cpu.sp == 0xFD);
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
    test_zero_page_and_absolute_memory();
    test_stack_page();
    test_unknown_opcode();
    puts("edu65xx simulator tests: PASS");
    return 0;
}
