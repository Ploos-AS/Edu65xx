#include "front_panel.h"

void edu65xx_front_panel_init(edu65xx_front_panel_t *panel, edu65xx_machine_t *machine)
{
    panel->machine = machine;
}

void edu65xx_front_panel_set_switches_a(edu65xx_front_panel_t *panel, uint8_t value)
{
    panel->machine->cpu.via.input_a = value;
}

void edu65xx_front_panel_set_switches_b(edu65xx_front_panel_t *panel, uint8_t value)
{
    panel->machine->cpu.via.input_b = value;
}

uint8_t edu65xx_front_panel_leds_a(const edu65xx_front_panel_t *panel)
{
    return edu65xx_via_porta_pins(&panel->machine->cpu.via);
}

uint8_t edu65xx_front_panel_leds_b(const edu65xx_front_panel_t *panel)
{
    return edu65xx_via_portb_pins(&panel->machine->cpu.via);
}
