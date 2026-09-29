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

int main(void)
{
    test_reset_state();
    puts("edu65xx simulator tests: PASS");
    return 0;
}
