#ifndef EDU65XX_MACHINE_H
#define EDU65XX_MACHINE_H

#include <stddef.h>
#include <stdint.h>

#include "../simulator/cpu.h"

#define EDU65XX_ROM_SIZE 0x4000u

typedef enum {
    EDU65XX_MACHINE_EVENT_INSTRUCTION = 0,
    EDU65XX_MACHINE_EVENT_IRQ = 1,
    EDU65XX_MACHINE_EVENT_NMI = 2,
    EDU65XX_MACHINE_EVENT_WAIT = 3,
    EDU65XX_MACHINE_EVENT_STOP = 4
} edu65xx_machine_event_t;

typedef struct {
    edu65xx_cpu_t cpu;
    edu65xx_machine_event_t last_event;
    uint64_t steps;
} edu65xx_machine_t;

void edu65xx_machine_init(edu65xx_machine_t *machine);
int edu65xx_machine_load_rom(edu65xx_machine_t *machine,
                             const uint8_t *data,
                             size_t size,
                             size_t offset);
void edu65xx_machine_reset(edu65xx_machine_t *machine);
int edu65xx_machine_step(edu65xx_machine_t *machine);

#endif
