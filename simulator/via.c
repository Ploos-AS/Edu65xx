#include "via.h"

void edu65xx_via_reset(edu65xx_via_t *via)
{
    via->orb = 0u;
    via->ora = 0u;
    via->ddrb = 0u;
    via->ddra = 0u;
    via->input_b = 0u;
    via->input_a = 0u;
}

static uint8_t port_value(uint8_t output, uint8_t direction, uint8_t input)
{
    return (uint8_t)((output & direction) | (input & (uint8_t)~direction));
}

uint8_t edu65xx_via_read(edu65xx_via_t *via, uint8_t reg)
{
    switch (reg & 0x0Fu) {
    case 0x0: return port_value(via->orb, via->ddrb, via->input_b);
    case 0x1: return port_value(via->ora, via->ddra, via->input_a);
    case 0x2: return via->ddrb;
    case 0x3: return via->ddra;
    default:  return 0u;
    }
}

void edu65xx_via_write(edu65xx_via_t *via, uint8_t reg, uint8_t value)
{
    switch (reg & 0x0Fu) {
    case 0x0: via->orb = value; break;
    case 0x1: via->ora = value; break;
    case 0x2: via->ddrb = value; break;
    case 0x3: via->ddra = value; break;
    default: break;
    }
}

uint8_t edu65xx_via_porta_pins(const edu65xx_via_t *via)
{
    return (uint8_t)(via->ora & via->ddra);
}

uint8_t edu65xx_via_portb_pins(const edu65xx_via_t *via)
{
    return (uint8_t)(via->orb & via->ddrb);
}
