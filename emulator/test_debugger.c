#include <assert.h>
#include <stdio.h>

#include "debugger.h"

static void load_program(edu65xx_machine_t *machine)
{
    uint8_t rom[EDU65XX_ROM_SIZE] = {0};

    /* C000: LDA #$42
       C002: STA $0010
       C005: LDA $0010
       C008: NOP */
    rom[0x0000] = 0xA9; rom[0x0001] = 0x42;
    rom[0x0002] = 0x8D; rom[0x0003] = 0x10; rom[0x0004] = 0x00;
    rom[0x0005] = 0xAD; rom[0x0006] = 0x10; rom[0x0007] = 0x00;
    rom[0x0008] = 0xEA;
    rom[0x3FFC] = 0x00; rom[0x3FFD] = 0xC0;

    edu65xx_machine_init(machine);
    assert(edu65xx_machine_load_rom(machine, rom, sizeof(rom), 0u) == 0);
    edu65xx_machine_reset(machine);
}

static void test_breakpoint_before_instruction(void)
{
    edu65xx_machine_t machine;
    edu65xx_debugger_t debugger;

    load_program(&machine);
    edu65xx_debugger_init(&debugger, &machine);
    assert(edu65xx_debugger_add_breakpoint(&debugger, 0xC002u) == 0);
    assert(edu65xx_debugger_run(&debugger, 10u) == EDU65XX_DEBUG_STOP_BREAKPOINT);
    assert(machine.cpu.pc == 0xC002u);
    assert(machine.cpu.a == 0x42u);
    assert(machine.cpu.memory[0x0010] == 0x00u);
}

static void test_write_watchpoint(void)
{
    edu65xx_machine_t machine;
    edu65xx_debugger_t debugger;

    load_program(&machine);
    edu65xx_debugger_init(&debugger, &machine);
    assert(edu65xx_debugger_add_watchpoint(&debugger, 0x0010u, 0, 1) == 0);
    assert(edu65xx_debugger_run(&debugger, 10u) == EDU65XX_DEBUG_STOP_WATCH_WRITE);
    assert(debugger.stop_address == 0x0010u);
    assert(debugger.stop_data == 0x42u);
    assert(machine.cpu.memory[0x0010] == 0x42u);
    assert(machine.cpu.pc == 0xC005u);
}

static void test_read_watchpoint(void)
{
    edu65xx_machine_t machine;
    edu65xx_debugger_t debugger;

    load_program(&machine);
    edu65xx_debugger_init(&debugger, &machine);
    assert(edu65xx_debugger_add_watchpoint(&debugger, 0x0010u, 1, 0) == 0);
    assert(edu65xx_debugger_run(&debugger, 10u) == EDU65XX_DEBUG_STOP_WATCH_READ);
    assert(debugger.stop_data == 0x42u);
    assert(machine.cpu.pc == 0xC008u);
}

static void test_step_limit(void)
{
    edu65xx_machine_t machine;
    edu65xx_debugger_t debugger;

    load_program(&machine);
    edu65xx_debugger_init(&debugger, &machine);
    assert(edu65xx_debugger_run(&debugger, 2u) == EDU65XX_DEBUG_STOP_STEP_LIMIT);
    assert(machine.cpu.pc == 0xC005u);
}

int main(void)
{
    test_breakpoint_before_instruction();
    test_write_watchpoint();
    test_read_watchpoint();
    test_step_limit();
    puts("edu65xx debugger tests: PASS");
    return 0;
}
