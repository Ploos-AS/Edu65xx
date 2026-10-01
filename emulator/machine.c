#include "machine.h"

#include <string.h>

void edu65xx_machine_init(edu65xx_machine_t *machine)
{
    memset(machine, 0, sizeof(*machine));
}

int edu65xx_machine_load_rom(edu65xx_machine_t *machine,
                             const uint8_t *data,
                             size_t size,
                             size_t offset)
{
    if (data == NULL || offset > EDU65XX_ROM_SIZE ||
        size > EDU65XX_ROM_SIZE - offset) {
        return -1;
    }

    memcpy(&machine->cpu.memory[EDU65XX_ROM_BASE + offset], data, size);
    return 0;
}

void edu65xx_machine_reset(edu65xx_machine_t *machine)
{
    machine->steps = 0u;
    machine->last_event = EDU65XX_MACHINE_EVENT_INSTRUCTION;
    edu65xx_cpu_reset(&machine->cpu);
}

int edu65xx_machine_step(edu65xx_machine_t *machine)
{
    int result = edu65xx_cpu_step(&machine->cpu);

    if (result < 0) {
        return -1;
    }

    machine->last_event = (edu65xx_machine_event_t)result;
    ++machine->steps;

    /* Functional device time: one VIA tick per active/waiting machine step.
       STP halts this functional clock until reset. This is deterministic,
       not a claim of exact PHI2 timing. */
    if (machine->cpu.stopped == 0u)
        edu65xx_via_tick(&machine->cpu.via);
    return 0;
}
