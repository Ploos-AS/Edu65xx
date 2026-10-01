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
    cpu.p |= EDU65XX_FLAG_D;
    edu65xx_cpu_request_nmi(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 2);
    assert(cpu.pc == 0xC100u);
    assert(cpu.sp == 0xFAu);
    assert(cpu.memory[0x01FD] == 0xC0u);
    assert(cpu.memory[0x01FC] == 0x00u);
    assert((cpu.memory[0x01FB] & EDU65XX_FLAG_B) == 0u);
    assert((cpu.memory[0x01FB] & EDU65XX_FLAG_D) != 0u);
    assert((cpu.p & EDU65XX_FLAG_D) == 0u);
    assert(cpu.nmi_pending == 0u);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.pc == 0xC000u);
    assert((cpu.p & EDU65XX_FLAG_D) != 0u);
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





static void run_imm(edu65xx_cpu_t *cpu, uint8_t opcode, uint8_t operand)
{
    cpu->memory[0xC000] = opcode;
    cpu->memory[0xC001] = operand;
    cpu->pc = 0xC000u;
    assert(edu65xx_cpu_step(cpu) == 0);
}

static void test_adc_sbc_binary_flags(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.p = EDU65XX_FLAG_U;
    cpu.a = 0x50u;
    run_imm(&cpu, 0x69u, 0x50u); /* ADC #$50 */
    assert(cpu.a == 0xA0u);
    assert((cpu.p & EDU65XX_FLAG_V) != 0u);
    assert((cpu.p & EDU65XX_FLAG_N) != 0u);
    assert((cpu.p & EDU65XX_FLAG_C) == 0u);

    cpu.p = EDU65XX_FLAG_U;
    cpu.a = 0xFFu;
    run_imm(&cpu, 0x69u, 0x01u);
    assert(cpu.a == 0x00u);
    assert((cpu.p & EDU65XX_FLAG_C) != 0u);
    assert((cpu.p & EDU65XX_FLAG_Z) != 0u);
    assert((cpu.p & EDU65XX_FLAG_V) == 0u);

    cpu.p = EDU65XX_FLAG_U | EDU65XX_FLAG_C;
    cpu.a = 0x80u;
    run_imm(&cpu, 0xE9u, 0x01u); /* SBC #$01 */
    assert(cpu.a == 0x7Fu);
    assert((cpu.p & EDU65XX_FLAG_C) != 0u);
    assert((cpu.p & EDU65XX_FLAG_V) != 0u);
    assert((cpu.p & EDU65XX_FLAG_N) == 0u);

    cpu.p = EDU65XX_FLAG_U | EDU65XX_FLAG_C;
    cpu.a = 0x00u;
    run_imm(&cpu, 0xE9u, 0x01u);
    assert(cpu.a == 0xFFu);
    assert((cpu.p & EDU65XX_FLAG_C) == 0u);
    assert((cpu.p & EDU65XX_FLAG_N) != 0u);
}

static void test_adc_sbc_decimal_flags(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.p = EDU65XX_FLAG_U | EDU65XX_FLAG_D;
    cpu.a = 0x45u;
    run_imm(&cpu, 0x69u, 0x55u); /* 45 + 55 = 100 */
    assert(cpu.a == 0x00u);
    assert((cpu.p & EDU65XX_FLAG_C) != 0u);
    assert((cpu.p & EDU65XX_FLAG_Z) != 0u);
    assert((cpu.p & EDU65XX_FLAG_N) == 0u);

    cpu.p = EDU65XX_FLAG_U | EDU65XX_FLAG_D | EDU65XX_FLAG_C;
    cpu.a = 0x50u;
    run_imm(&cpu, 0xE9u, 0x01u); /* 50 - 01 = 49 */
    assert(cpu.a == 0x49u);
    assert((cpu.p & EDU65XX_FLAG_C) != 0u);
    assert((cpu.p & EDU65XX_FLAG_Z) == 0u);

    cpu.p = EDU65XX_FLAG_U | EDU65XX_FLAG_D | EDU65XX_FLAG_C;
    cpu.a = 0x00u;
    run_imm(&cpu, 0xE9u, 0x01u); /* 00 - 01 = 99 with borrow */
    assert(cpu.a == 0x99u);
    assert((cpu.p & EDU65XX_FLAG_C) == 0u);
    assert((cpu.p & EDU65XX_FLAG_N) != 0u);
}

static void test_adc_sbc_addressing(void)
{
    edu65xx_cpu_t cpu = {0};

    /* ADC ($20) using the W65C02 zero-page indirect mode. */
    cpu.memory[0x0020] = 0x00u;
    cpu.memory[0x0021] = 0x20u;
    cpu.memory[0x2000] = 0x05u;
    cpu.memory[0xC000] = 0xA9; cpu.memory[0xC001] = 0x10;
    cpu.memory[0xC002] = 0x72; cpu.memory[0xC003] = 0x20;

    /* SBC $2000,X */
    cpu.memory[0x2001] = 0x03u;
    cpu.memory[0xC004] = 0x38; /* SEC */
    cpu.memory[0xC005] = 0xA2; cpu.memory[0xC006] = 0x01;
    cpu.memory[0xC007] = 0xFD; cpu.memory[0xC008] = 0x00; cpu.memory[0xC009] = 0x20;
    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0x15u);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0x12u);
}






static void test_reserved_nop_lengths(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0xC000] = 0x02; cpu.memory[0xC001] = 0xAA; /* two-byte NOP */
    cpu.memory[0xC002] = 0x5C; cpu.memory[0xC003] = 0x34; cpu.memory[0xC004] = 0x12; /* three-byte NOP */
    cpu.memory[0xC005] = 0x03; /* one-byte NOP */
    cpu.memory[0xC006] = 0xEB; /* one-byte NOP */
    cpu.pc = 0xC000u;

    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.pc == 0xC002u);
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.pc == 0xC005u);
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.pc == 0xC006u);
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.pc == 0xC007u);
}

static void assert_via_irq(edu65xx_cpu_t *cpu)
{
    cpu->via.ier |= EDU65XX_VIA_IFR_T1;
    cpu->via.ifr |= EDU65XX_VIA_IFR_T1;
    assert(edu65xx_via_irq(&cpu->via));
}

static void test_wai_and_stp_states(void)
{
    edu65xx_cpu_t cpu = {0};

    /* With I=1, IRQ wakes WAI but does not vector. */
    cpu.memory[0xC000] = 0xCB; /* WAI */
    cpu.memory[0xC001] = 0xEA; /* NOP */
    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);
    assert((cpu.p & EDU65XX_FLAG_I) != 0u);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.waiting != 0u && cpu.pc == 0xC001u);
    assert(edu65xx_cpu_step(&cpu) == 3); /* still waiting */
    assert_via_irq(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 3); /* wake, IRQ masked */
    assert(cpu.waiting == 0u && cpu.pc == 0xC001u);
    cpu.via.ifr = 0u;
    assert(edu65xx_cpu_step(&cpu) == 0); /* NOP */

    /* With I=0, IRQ wakes WAI and takes the IRQ vector. */
    cpu = (edu65xx_cpu_t){0};
    cpu.memory[0xC000] = 0x58; /* CLI */
    cpu.memory[0xC001] = 0xCB; /* WAI */
    cpu.memory[0xC002] = 0xEA; /* return target */
    cpu.memory[0xC100] = 0x40; /* RTI */
    cpu.memory[0xFFFE] = 0x00; cpu.memory[0xFFFF] = 0xC1;
    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.waiting != 0u && cpu.pc == 0xC002u);
    assert_via_irq(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 1);
    assert(cpu.waiting == 0u && cpu.pc == 0xC100u);
    cpu.via.ifr = 0u;
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.pc == 0xC002u);

    /* STP is released only by reset in the functional model. */
    cpu = (edu65xx_cpu_t){0};
    cpu.memory[0xC000] = 0xDB; /* STP */
    cpu.memory[0xC001] = 0xEA;
    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.stopped != 0u && cpu.pc == 0xC001u);
    assert(edu65xx_cpu_step(&cpu) == 4);
    assert(cpu.pc == 0xC001u);
    edu65xx_cpu_reset(&cpu);
    assert(cpu.stopped == 0u && cpu.pc == 0xC000u);
}

static void test_w65c02_bit_manipulation(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.a = 0x0Fu;
    cpu.memory[0x0020] = 0x30u;
    cpu.memory[0xC000] = 0x04; cpu.memory[0xC001] = 0x20; /* TSB $20 */
    cpu.memory[0xC002] = 0x14; cpu.memory[0xC003] = 0x20; /* TRB $20 */
    cpu.pc = 0xC000u;

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x0020] == 0x3Fu);
    assert((cpu.p & EDU65XX_FLAG_Z) != 0u); /* original $30 & A was zero */
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x0020] == 0x30u);
    assert((cpu.p & EDU65XX_FLAG_Z) == 0u);

    /* RMB3 then SMB6. */
    cpu.memory[0x0021] = 0xFFu;
    cpu.memory[0xC010] = 0x37; cpu.memory[0xC011] = 0x21;
    cpu.memory[0xC012] = 0xE7; cpu.memory[0xC013] = 0x21;
    cpu.pc = 0xC010u;
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x0021] == 0xF7u);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x0021] == 0xF7u); /* bit 6 was already set */

    /* BBR3 sees cleared bit 3 and branches; BBS6 sees set bit 6. */
    cpu.memory[0xC020] = 0x3F; cpu.memory[0xC021] = 0x21; cpu.memory[0xC022] = 0x02;
    cpu.pc = 0xC020u;
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.pc == 0xC025u);

    cpu.memory[0xC030] = 0xEF; cpu.memory[0xC031] = 0x21; cpu.memory[0xC032] = 0xFE;
    cpu.pc = 0xC030u;
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.pc == 0xC031u); /* PC after operand+offset is C033, -2 => C031 */
}

static void test_brk_php_plp_and_indirect_jumps(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0xC000] = 0x08; /* PHP */
    cpu.memory[0xC001] = 0x18; /* CLC */
    cpu.memory[0xC002] = 0x28; /* PLP */
    cpu.memory[0xC003] = 0x00; /* BRK */
    cpu.memory[0xC004] = 0xAA; /* signature byte, skipped */
    cpu.memory[0xC100] = 0x40; /* RTI */
    cpu.memory[0xFFFE] = 0x00; cpu.memory[0xFFFF] = 0xC1;
    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);
    cpu.p |= EDU65XX_FLAG_C | EDU65XX_FLAG_D;

    assert(edu65xx_cpu_step(&cpu) == 0); /* PHP */
    assert((cpu.memory[0x01FD] & (EDU65XX_FLAG_B | EDU65XX_FLAG_C)) ==
           (EDU65XX_FLAG_B | EDU65XX_FLAG_C));
    assert(edu65xx_cpu_step(&cpu) == 0); /* CLC */
    assert((cpu.p & EDU65XX_FLAG_C) == 0u);
    assert(edu65xx_cpu_step(&cpu) == 0); /* PLP */
    assert((cpu.p & EDU65XX_FLAG_C) != 0u);
    assert((cpu.p & EDU65XX_FLAG_B) == 0u);

    assert(edu65xx_cpu_step(&cpu) == 0); /* BRK */
    assert(cpu.pc == 0xC100u);
    assert(cpu.memory[0x01FD] == 0xC0u);
    assert(cpu.memory[0x01FC] == 0x05u); /* BRK return address C005 */
    assert((cpu.memory[0x01FB] & EDU65XX_FLAG_B) != 0u);
    assert((cpu.memory[0x01FB] & EDU65XX_FLAG_D) != 0u);
    assert((cpu.p & EDU65XX_FLAG_D) == 0u);
    assert(edu65xx_cpu_step(&cpu) == 0); /* RTI */
    assert(cpu.pc == 0xC005u);
    assert((cpu.p & EDU65XX_FLAG_D) != 0u);

    /* W65C02 fixed indirect JMP page crossing: pointer $20FF reads high at $2100. */
    cpu.memory[0xC200] = 0x6C; cpu.memory[0xC201] = 0xFF; cpu.memory[0xC202] = 0x20;
    cpu.memory[0x20FF] = 0x34; cpu.memory[0x2100] = 0x12;
    cpu.pc = 0xC200u;
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.pc == 0x1234u);

    /* JMP ($2200,X), X=2 -> pointer at $2202. */
    cpu.memory[0xC210] = 0x7C; cpu.memory[0xC211] = 0x00; cpu.memory[0xC212] = 0x22;
    cpu.memory[0x2202] = 0x78; cpu.memory[0x2203] = 0x56;
    cpu.x = 2u;
    cpu.pc = 0xC210u;
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.pc == 0x5678u);
}

static void test_remaining_conditional_branches(void)
{
    edu65xx_cpu_t cpu = {0};
    struct branch_case {
        uint8_t opcode;
        uint8_t flags;
        int taken;
    } cases[] = {
        {0x90u, 0u, 1}, {0x90u, EDU65XX_FLAG_C, 0},
        {0xB0u, EDU65XX_FLAG_C, 1}, {0xB0u, 0u, 0},
        {0x30u, EDU65XX_FLAG_N, 1}, {0x30u, 0u, 0},
        {0x10u, 0u, 1}, {0x10u, EDU65XX_FLAG_N, 0},
        {0x50u, 0u, 1}, {0x50u, EDU65XX_FLAG_V, 0},
        {0x70u, EDU65XX_FLAG_V, 1}, {0x70u, 0u, 0}
    };
    size_t i;

    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        cpu.memory[0xC000] = cases[i].opcode;
        cpu.memory[0xC001] = 0x02u;
        cpu.pc = 0xC000u;
        cpu.p = (uint8_t)(EDU65XX_FLAG_U | cases[i].flags);
        assert(edu65xx_cpu_step(&cpu) == 0);
        assert(cpu.pc == (cases[i].taken ? 0xC004u : 0xC002u));
    }
}

static void test_shift_rotate_flags_and_memory(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.p = EDU65XX_FLAG_U;
    cpu.a = 0x80u;
    run_imm(&cpu, 0x0Au, 0x00u); /* ASL A; operand byte ignored by one-byte opcode */
    assert(cpu.a == 0x00u);
    assert((cpu.p & EDU65XX_FLAG_C) != 0u);
    assert((cpu.p & EDU65XX_FLAG_Z) != 0u);

    cpu.p = EDU65XX_FLAG_U | EDU65XX_FLAG_C;
    cpu.a = 0x00u;
    cpu.memory[0xC000] = 0x2Au; /* ROL A */
    cpu.pc = 0xC000u;
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0x01u);
    assert((cpu.p & EDU65XX_FLAG_C) == 0u);

    cpu.p = EDU65XX_FLAG_U | EDU65XX_FLAG_C;
    cpu.a = 0x01u;
    cpu.memory[0xC000] = 0x6Au; /* ROR A */
    cpu.pc = 0xC000u;
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0x80u);
    assert((cpu.p & EDU65XX_FLAG_C) != 0u);
    assert((cpu.p & EDU65XX_FLAG_N) != 0u);

    cpu.p = EDU65XX_FLAG_U;
    cpu.memory[0x0020] = 0x03u;
    cpu.memory[0xC000] = 0x46; cpu.memory[0xC001] = 0x20; /* LSR $20 */
    cpu.pc = 0xC000u;
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x0020] == 0x01u);
    assert((cpu.p & EDU65XX_FLAG_C) != 0u);

    cpu.x = 1u;
    cpu.memory[0x2001] = 0x40u;
    cpu.memory[0xC000] = 0x1E; cpu.memory[0xC001] = 0x00; cpu.memory[0xC002] = 0x20; /* ASL $2000,X */
    cpu.pc = 0xC000u;
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x2001] == 0x80u);
    assert((cpu.p & EDU65XX_FLAG_N) != 0u);
}

static void test_logic_status_stack_and_rmw(void)
{
    edu65xx_cpu_t cpu = {0};

    /*
       Exercise monitor-oriented W65C02 operations:
       AND/ORA/EOR, BIT #, flag controls, PHX/PLX, PHY/PLY, STZ, INC/DEC.
    */
    cpu.memory[0xC000] = 0xA9; cpu.memory[0xC001] = 0xF0; /* LDA #$F0 */
    cpu.memory[0xC002] = 0x29; cpu.memory[0xC003] = 0x0F; /* AND #$0F -> 0 */
    cpu.memory[0xC004] = 0x09; cpu.memory[0xC005] = 0x55; /* ORA #$55 */
    cpu.memory[0xC006] = 0x49; cpu.memory[0xC007] = 0x0F; /* EOR #$0F -> $5A */
    cpu.memory[0xC008] = 0x89; cpu.memory[0xC009] = 0x0A; /* BIT #$0A */
    cpu.memory[0xC00A] = 0x38;                         /* SEC */
    cpu.memory[0xC00B] = 0x18;                         /* CLC */
    cpu.memory[0xC00C] = 0xA2; cpu.memory[0xC00D] = 0x42;
    cpu.memory[0xC00E] = 0xDA;                         /* PHX */
    cpu.memory[0xC00F] = 0xA2; cpu.memory[0xC010] = 0x00;
    cpu.memory[0xC011] = 0xFA;                         /* PLX */
    cpu.memory[0xC012] = 0x9C; cpu.memory[0xC013] = 0x00; cpu.memory[0xC014] = 0x20; /* STZ $2000 */
    cpu.memory[0xC015] = 0xEE; cpu.memory[0xC016] = 0x00; cpu.memory[0xC017] = 0x20; /* INC */
    cpu.memory[0xC018] = 0xCE; cpu.memory[0xC019] = 0x00; cpu.memory[0xC01A] = 0x20; /* DEC */
    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0u && (cpu.p & EDU65XX_FLAG_Z) != 0u);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.a == 0x5Au);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert((cpu.p & EDU65XX_FLAG_Z) == 0u);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert((cpu.p & EDU65XX_FLAG_C) != 0u);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert((cpu.p & EDU65XX_FLAG_C) == 0u);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.x == 0x42u);

    cpu.memory[0x2000] = 0xAAu;
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.memory[0x2000] == 0u);
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.memory[0x2000] == 1u);
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.memory[0x2000] == 0u);
    assert((cpu.p & EDU65XX_FLAG_Z) != 0u);
}

static void test_indexed_and_indirect_addressing(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.x = 2u;
    cpu.y = 3u;

    /* Zero-page indexed wraps at $FF -> $00. */
    cpu.memory[0x0001] = 0x11u;
    cpu.memory[0xC000] = 0xB5; cpu.memory[0xC001] = 0xFF; /* LDA $FF,X */

    /* Absolute indexed. */
    cpu.memory[0x2002] = 0x22u;
    cpu.memory[0xC002] = 0xBD; cpu.memory[0xC003] = 0x00; cpu.memory[0xC004] = 0x20;

    cpu.memory[0x2103] = 0x33u;
    cpu.memory[0xC005] = 0xB9; cpu.memory[0xC006] = 0x00; cpu.memory[0xC007] = 0x21;

    /* ($20,X) -> pointer at $22/$23 -> $3000. */
    cpu.memory[0x0022] = 0x00u; cpu.memory[0x0023] = 0x30u;
    cpu.memory[0x3000] = 0x44u;
    cpu.memory[0xC008] = 0xA1; cpu.memory[0xC009] = 0x20;

    /* ($30),Y -> $4000 + 3. */
    cpu.memory[0x0030] = 0x00u; cpu.memory[0x0031] = 0x40u;
    cpu.memory[0x4003] = 0x55u;
    cpu.memory[0xC00A] = 0xB1; cpu.memory[0xC00B] = 0x30;

    /* W65C02 ($40) -> $5000. */
    cpu.memory[0x0040] = 0x00u; cpu.memory[0x0041] = 0x50u;
    cpu.memory[0x5000] = 0x66u;
    cpu.memory[0xC00C] = 0xB2; cpu.memory[0xC00D] = 0x40;

    /* STA ($30),Y stores through the same effective-address machinery. */
    cpu.memory[0xC00E] = 0x91; cpu.memory[0xC00F] = 0x30;

    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);
    cpu.x = 2u;
    cpu.y = 3u;

    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.a == 0x11u);
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.a == 0x22u);
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.a == 0x33u);
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.a == 0x44u);
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.a == 0x55u);
    assert(edu65xx_cpu_step(&cpu) == 0); assert(cpu.a == 0x66u);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(cpu.memory[0x4003] == 0x66u);
}

static void test_compare_family(void)
{
    edu65xx_cpu_t cpu = {0};

    cpu.memory[0xC000] = 0xA2; cpu.memory[0xC001] = 0x10; /* LDX #$10 */
    cpu.memory[0xC002] = 0xE0; cpu.memory[0xC003] = 0x10; /* CPX #$10 */
    cpu.memory[0xC004] = 0xA0; cpu.memory[0xC005] = 0x20; /* LDY #$20 */
    cpu.memory[0xC006] = 0xC0; cpu.memory[0xC007] = 0x21; /* CPY #$21 */
    set_reset_vector(&cpu, 0xC000u);
    edu65xx_cpu_reset(&cpu);

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert((cpu.p & (EDU65XX_FLAG_Z | EDU65XX_FLAG_C)) ==
           (EDU65XX_FLAG_Z | EDU65XX_FLAG_C));

    assert(edu65xx_cpu_step(&cpu) == 0);
    assert(edu65xx_cpu_step(&cpu) == 0);
    assert((cpu.p & EDU65XX_FLAG_C) == 0u);
    assert((cpu.p & EDU65XX_FLAG_N) != 0u);
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
    test_adc_sbc_binary_flags();
    test_adc_sbc_decimal_flags();
    test_adc_sbc_addressing();
    test_reserved_nop_lengths();
    test_wai_and_stp_states();
    test_w65c02_bit_manipulation();
    test_brk_php_plp_and_indirect_jumps();
    test_remaining_conditional_branches();
    test_shift_rotate_flags_and_memory();
    test_logic_status_stack_and_rmw();
    test_indexed_and_indirect_addressing();
    test_compare_family();
    test_control_flow_and_subroutines();
    test_jmp_and_negative_branch();
    test_stack();
    puts("edu65xx simulator tests: PASS");
    return 0;
}
