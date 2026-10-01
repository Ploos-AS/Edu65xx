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
    test_stack();
    puts("edu65xx simulator tests: PASS");
    return 0;
}
