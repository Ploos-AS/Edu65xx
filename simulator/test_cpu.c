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

    /* $8012-$BFFF is reserved/unmapped I/O, not hidden RAM. */
    cpu.memory[0x9000] = 0x12u;
    edu65xx_write8(&cpu, 0x9000u, 0x34u);
    assert(cpu.memory[0x9000] == 0x12u);
    assert(edu65xx_read8(&cpu, 0x9000u) == 0xFFu);
}

static void test_nmi_entry_and_return(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0xC000] = 0xEA; /* interrupted instruction */
    cpu.memory[0xC100] = 0x40; /* RTI */
    set_reset_vector(&cpu, 0xC000u);
    cpu.memory[0xFFFA] = 0x00u;
    cpu.memory[0xFFFB] = 0xC1u;

    edu65xx_cpu_reset(&cpu);
    edu65xx_cpu_request_nmi(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 2);
    assert(cpu.pc == 0xC100u);
    assert(cpu.sp == 0xFAu);
    assert(cpu.memory[0x01FD] == 0xC0u);
    assert(cpu.memory[0x01FC] == 0x00u);
    assert((cpu.memory[0x01FB] & EDU65XX_FLAG_B) == 0u);
    assert(cpu.nmi_pending == 0u);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.pc == 0xC000u);
    assert(cpu.sp == 0xFDu);
}

static void test_timer_irq_rti(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0xC000] = 0x58; /* CLI */
    cpu.memory[0xC001] = 0xEA; /* NOP: return target */
    cpu.memory[0xC100] = 0x40; /* RTI */
    set_reset_vector(&cpu, 0xC000u);
    cpu.memory[0xFFFE] = 0x00u;
    cpu.memory[0xFFFF] = 0xC1u;

    edu65xx_cpu_reset(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 0); /* CLI */
    assert(cpu.pc == 0xC001u);

    edu65xx_via_write(&cpu.via, 14u, 0xC0u); /* enable T1 IRQ */
    edu65xx_via_write(&cpu.via, 4u, 0x01u);
    edu65xx_via_write(&cpu.via, 5u, 0x00u);  /* start at 1 */
    edu65xx_via_tick(&cpu.via);

    assert((cpu.via.ifr & EDU65XX_VIA_IFR_T1) != 0u);
    assert(edu65xx_via_irq(&cpu.via));

    assert(edu65xx_cpu_step(&cpu) == 1); /* IRQ entry */
    assert(cpu.pc == 0xC100u);
    assert(cpu.sp == 0xFAu);
    assert(cpu.memory[0x01FD] == 0xC0u); /* return PC high */
    assert(cpu.memory[0x01FC] == 0x01u); /* return PC low */
    assert((cpu.memory[0x01FB] & EDU65XX_FLAG_B) == 0u);
    assert((cpu.p & EDU65XX_FLAG_I) != 0u);

    /* ISR acknowledges Timer 1 before RTI. */
    (void)edu65xx_via_read(&cpu.via, 4u);
    assert(!edu65xx_via_irq(&cpu.via));

    assert(edu65xx_cpu_step(&cpu) == 0); /* RTI */
    assert(cpu.pc == 0xC001u);
    assert(cpu.sp == 0xFDu);
    assert((cpu.p & EDU65XX_FLAG_I) == 0u);
}

static void test_serial_terminal(void)
{
    edu65xx_cpu_t cpu = {0};

    /* LDA #'H'; STA $8010; LDA #'i'; STA $8010 */
    cpu.memory[0xC000] = 0xA9; cpu.memory[0xC001] = 'H';
    cpu.memory[0xC002] = 0x8D; cpu.memory[0xC003] = 0x10; cpu.memory[0xC004] = 0x80;
    cpu.memory[0xC005] = 0xA9; cpu.memory[0xC006] = 'i';
    cpu.memory[0xC007] = 0x8D; cpu.memory[0xC008] = 0x10; cpu.memory[0xC009] = 0x80;
    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.serial.tx_count == 2u);
    assert(cpu.serial.tx[0] == 'H');
    assert(cpu.serial.tx[1] == 'i');

    edu65xx_serial_receive(&cpu.serial, '!');
    assert((edu65xx_read8(&cpu, 0x8011u) & EDU65XX_SERIAL_RX_READY) != 0u);
    assert(edu65xx_read8(&cpu, 0x8010u) == '!');
}


static void test_control_flow_and_subroutines(void)
{
    edu65xx_cpu_t cpu = {0};

    /*
       C000: LDA #$03
       C002: JSR $C010
       C005: CMP #$04
       C007: BNE $C00B
       C009: BRA $C00D
       C00B: LDA #$EE
       C00D: NOP
       C010: TAX
       C011: INX
       C012: TXA
       C013: RTS
    */
    cpu.memory[0xC000] = 0xA9; cpu.memory[0xC001] = 0x03;
    cpu.memory[0xC002] = 0x20; cpu.memory[0xC003] = 0x10; cpu.memory[0xC004] = 0xC0;
    cpu.memory[0xC005] = 0xC9; cpu.memory[0xC006] = 0x04;
    cpu.memory[0xC007] = 0xD0; cpu.memory[0xC008] = 0x02;
    cpu.memory[0xC009] = 0x80; cpu.memory[0xC00A] = 0x02;
    cpu.memory[0xC00B] = 0xA9; cpu.memory[0xC00C] = 0xEE;
    cpu.memory[0xC00D] = 0xEA;
    cpu.memory[0xC010] = 0xAA;
    cpu.memory[0xC011] = 0xE8;
    cpu.memory[0xC012] = 0x8A;
    cpu.memory[0xC013] = 0x60;
    set_reset_vector(&cpu, 0xC000u);

    edu65xx_cpu_reset(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 0); /* LDA */
    assert(edu65xx_cpu_step(&cpu) == 0); /* JSR */
    assert(cpu.pc == 0xC010u);
    assert(cpu.sp == 0xFBu);
    assert(cpu.memory[0x01FD] == 0xC0u);
    assert(cpu.memory[0x01FC] == 0x04u);

    assert(edu65xx_cpu_step(&cpu) == 0); /* TAX */
    assert(edu65xx_cpu_step(&cpu) == 0); /* INX */
    assert(edu65xx_cpu_step(&cpu) == 0); /* TXA */
    assert(cpu.a == 0x04u);
    assert(edu65xx_cpu_step(&cpu) == 0); /* RTS */
    assert(cpu.pc == 0xC005u);
    assert(cpu.sp == 0xFDu);

    assert(edu65xx_cpu_step(&cpu) == 0); /* CMP */
    assert((cpu.p & EDU65XX_FLAG_Z) != 0u);
    assert((cpu.p & EDU65XX_FLAG_C) != 0u);
    assert(edu65xx_cpu_step(&cpu) == 0); /* BNE not taken */
    assert(cpu.pc == 0xC009u);
    assert(edu65xx_cpu_step(&cpu) == 0); /* BRA */
    assert(cpu.pc == 0xC00Du);
    assert(edu65xx_cpu_step(&cpu) == 0); /* NOP */
    assert(cpu.a == 0x04u);
}

static void test_jmp_and_negative_branch(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0xC000] = 0x4C; cpu.memory[0xC001] = 0x05; cpu.memory[0xC002] = 0xC0;
    cpu.memory[0xC005] = 0xA2; cpu.memory[0xC006] = 0x02;
    cpu.memory[0xC007] = 0xCA;
    cpu.memory[0xC008] = 0xD0; cpu.memory[0xC009] = 0xFD; /* back to DEX */
    cpu.memory[0xC00A] = 0xEA;
    set_reset_vector(&cpu, 0xC000u);

    edu65xx_cpu_reset(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 0); /* JMP */
    assert(cpu.pc == 0xC005u);
    assert(edu65xx_cpu_step(&cpu) == 0); /* LDX #2 */
    assert(edu65xx_cpu_step(&cpu) == 0); /* DEX => 1 */
    assert(edu65xx_cpu_step(&cpu) == 0); /* BNE back */
    assert(cpu.pc == 0xC007u);
    assert(edu65xx_cpu_step(&cpu) == 0); /* DEX => 0 */
    assert(edu65xx_cpu_step(&cpu) == 0); /* BNE not taken */
    assert(cpu.pc == 0xC00Au);
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
    test_nmi_entry_and_return();
    test_timer_irq_rti();
    test_serial_terminal();
    test_control_flow_and_subroutines();
    test_jmp_and_negative_branch();
    test_stack();
    puts("edu65xx simulator tests: PASS");
    return 0;
}
