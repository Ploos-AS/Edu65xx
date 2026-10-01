#ifndef EDU65XX_DEBUGGER_H
#define EDU65XX_DEBUGGER_H

#include <stddef.h>
#include <stdint.h>

#include "machine.h"

#define EDU65XX_DEBUGGER_MAX_BREAKPOINTS 16u
#define EDU65XX_DEBUGGER_MAX_WATCHPOINTS 16u

typedef enum {
    EDU65XX_DEBUG_STOP_NONE = 0,
    EDU65XX_DEBUG_STOP_BREAKPOINT,
    EDU65XX_DEBUG_STOP_WATCH_READ,
    EDU65XX_DEBUG_STOP_WATCH_WRITE,
    EDU65XX_DEBUG_STOP_STEP_LIMIT,
    EDU65XX_DEBUG_STOP_MACHINE_ERROR
} edu65xx_debug_stop_t;

typedef struct {
    uint16_t address;
    uint8_t on_read;
    uint8_t on_write;
} edu65xx_watchpoint_t;

typedef struct {
    edu65xx_machine_t *machine;
    uint16_t breakpoints[EDU65XX_DEBUGGER_MAX_BREAKPOINTS];
    size_t breakpoint_count;
    edu65xx_watchpoint_t watchpoints[EDU65XX_DEBUGGER_MAX_WATCHPOINTS];
    size_t watchpoint_count;
    edu65xx_debug_stop_t last_stop;
    uint16_t stop_address;
    uint8_t stop_data;
} edu65xx_debugger_t;

void edu65xx_debugger_init(edu65xx_debugger_t *debugger, edu65xx_machine_t *machine);
int edu65xx_debugger_add_breakpoint(edu65xx_debugger_t *debugger, uint16_t address);
int edu65xx_debugger_add_watchpoint(edu65xx_debugger_t *debugger, uint16_t address,
                                    int on_read, int on_write);
int edu65xx_debugger_step(edu65xx_debugger_t *debugger);
edu65xx_debug_stop_t edu65xx_debugger_run(edu65xx_debugger_t *debugger,
                                          uint64_t max_steps);

#endif
