#ifndef EDU65XX_CPU_H
#define EDU65XX_CPU_H

#include <stddef.h>
#include <stdint.h>

#include "via.h"
#include "serial.h"

#define EDU65XX_MEM_SIZE 65536u
#define EDU65XX_BUS_TRACE_MAX 16u
#define EDU65XX_ROM_BASE 0xC000u

#define EDU65XX_FLAG_C 0x01u
#define EDU65XX_FLAG_Z 0x02u
#define EDU65XX_FLAG_I 0x04u
#define EDU65XX_FLAG_D 0x08u
#define EDU65XX_FLAG_B 0x10u
#define EDU65XX_FLAG_U 0x20u
#define EDU65XX_FLAG_V 0x40u
#define EDU65XX_FLAG_N 0x80u

typedef struct {
    uint16_t address;
    uint8_t data;
    uint8_t is_write;
} edu65xx_bus_cycle_t;

typedef struct {
    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    uint8_t p;
    uint8_t nmi_pending;
    uint8_t waiting;
    uint8_t stopped;
    uint8_t memory[EDU65XX_MEM_SIZE];
    edu65xx_via_t via;
    edu65xx_serial_t serial;
    edu65xx_bus_cycle_t bus_trace[EDU65XX_BUS_TRACE_MAX];
    size_t bus_trace_count;
} edu65xx_cpu_t;

void edu65xx_cpu_reset(edu65xx_cpu_t *cpu);
void edu65xx_cpu_request_nmi(edu65xx_cpu_t *cpu);
int edu65xx_cpu_step(edu65xx_cpu_t *cpu);
void edu65xx_bus_trace_clear(edu65xx_cpu_t *cpu);
uint8_t edu65xx_read8(edu65xx_cpu_t *cpu, uint16_t address);
void edu65xx_write8(edu65xx_cpu_t *cpu, uint16_t address, uint8_t value);

#endif
