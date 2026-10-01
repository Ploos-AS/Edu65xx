#include "debugger.h"

#include <string.h>

void edu65xx_debugger_init(edu65xx_debugger_t *debugger, edu65xx_machine_t *machine)
{
    memset(debugger, 0, sizeof(*debugger));
    debugger->machine = machine;
}

int edu65xx_debugger_add_breakpoint(edu65xx_debugger_t *debugger, uint16_t address)
{
    if (debugger->breakpoint_count >= EDU65XX_DEBUGGER_MAX_BREAKPOINTS)
        return -1;
    debugger->breakpoints[debugger->breakpoint_count++] = address;
    return 0;
}

int edu65xx_debugger_add_watchpoint(edu65xx_debugger_t *debugger, uint16_t address,
                                    int on_read, int on_write)
{
    edu65xx_watchpoint_t *watch;
    if ((!on_read && !on_write) ||
        debugger->watchpoint_count >= EDU65XX_DEBUGGER_MAX_WATCHPOINTS)
        return -1;
    watch = &debugger->watchpoints[debugger->watchpoint_count++];
    watch->address = address;
    watch->on_read = on_read != 0;
    watch->on_write = on_write != 0;
    return 0;
}

static int at_breakpoint(const edu65xx_debugger_t *debugger)
{
    size_t i;
    for (i = 0; i < debugger->breakpoint_count; ++i)
        if (debugger->breakpoints[i] == debugger->machine->cpu.pc)
            return 1;
    return 0;
}

static edu65xx_debug_stop_t check_watchpoints(edu65xx_debugger_t *debugger)
{
    size_t i, j;
    const edu65xx_cpu_t *cpu = &debugger->machine->cpu;

    for (i = 0; i < cpu->bus_trace_count; ++i) {
        const edu65xx_bus_cycle_t *cycle = &cpu->bus_trace[i];
        for (j = 0; j < debugger->watchpoint_count; ++j) {
            const edu65xx_watchpoint_t *watch = &debugger->watchpoints[j];
            if (watch->address != cycle->address)
                continue;
            if ((!cycle->is_write && watch->on_read) ||
                (cycle->is_write && watch->on_write)) {
                debugger->stop_address = cycle->address;
                debugger->stop_data = cycle->data;
                return cycle->is_write ? EDU65XX_DEBUG_STOP_WATCH_WRITE
                                       : EDU65XX_DEBUG_STOP_WATCH_READ;
            }
        }
    }
    return EDU65XX_DEBUG_STOP_NONE;
}

int edu65xx_debugger_step(edu65xx_debugger_t *debugger)
{
    edu65xx_debug_stop_t stop;
    debugger->last_stop = EDU65XX_DEBUG_STOP_NONE;
    if (edu65xx_machine_step(debugger->machine) != 0) {
        debugger->last_stop = EDU65XX_DEBUG_STOP_MACHINE_ERROR;
        return -1;
    }
    stop = check_watchpoints(debugger);
    if (stop != EDU65XX_DEBUG_STOP_NONE)
        debugger->last_stop = stop;
    return 0;
}

edu65xx_debug_stop_t edu65xx_debugger_run(edu65xx_debugger_t *debugger,
                                          uint64_t max_steps)
{
    uint64_t steps = 0u;
    debugger->last_stop = EDU65XX_DEBUG_STOP_NONE;

    while (steps < max_steps) {
        if (at_breakpoint(debugger)) {
            debugger->last_stop = EDU65XX_DEBUG_STOP_BREAKPOINT;
            debugger->stop_address = debugger->machine->cpu.pc;
            return debugger->last_stop;
        }
        if (edu65xx_debugger_step(debugger) != 0)
            return debugger->last_stop;
        if (debugger->last_stop != EDU65XX_DEBUG_STOP_NONE)
            return debugger->last_stop;
        ++steps;
    }

    debugger->last_stop = EDU65XX_DEBUG_STOP_STEP_LIMIT;
    debugger->stop_address = debugger->machine->cpu.pc;
    return debugger->last_stop;
}
