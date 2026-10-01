#ifndef EDU65XX_FRONT_PANEL_H
#define EDU65XX_FRONT_PANEL_H

#include <stdint.h>

#include "machine.h"

typedef struct {
    edu65xx_machine_t *machine;
} edu65xx_front_panel_t;

void edu65xx_front_panel_init(edu65xx_front_panel_t *panel, edu65xx_machine_t *machine);
void edu65xx_front_panel_set_switches_a(edu65xx_front_panel_t *panel, uint8_t value);
void edu65xx_front_panel_set_switches_b(edu65xx_front_panel_t *panel, uint8_t value);
uint8_t edu65xx_front_panel_leds_a(const edu65xx_front_panel_t *panel);
uint8_t edu65xx_front_panel_leds_b(const edu65xx_front_panel_t *panel);

#endif
