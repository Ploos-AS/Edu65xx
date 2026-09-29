#ifndef EDU65XX_VIA_H
#define EDU65XX_VIA_H

#include <stdint.h>

#define EDU65XX_VIA_BASE 0x8000u
#define EDU65XX_VIA_END  0x800Fu

typedef struct {
    uint8_t orb;
    uint8_t ora;
    uint8_t ddrb;
    uint8_t ddra;
    uint8_t input_b;
    uint8_t input_a;
} edu65xx_via_t;

void edu65xx_via_reset(edu65xx_via_t *via);
uint8_t edu65xx_via_read(edu65xx_via_t *via, uint8_t reg);
void edu65xx_via_write(edu65xx_via_t *via, uint8_t reg, uint8_t value);
uint8_t edu65xx_via_porta_pins(const edu65xx_via_t *via);
uint8_t edu65xx_via_portb_pins(const edu65xx_via_t *via);

#endif
