#include <stdint.h>

uint8_t load_byte(const uint8_t *p)
{
    return *p;
}

void store_byte(uint8_t *p, uint8_t value)
{
    *p = value;
}

void serial_putc(uint8_t value)
{
    volatile uint8_t *const serial_data = (volatile uint8_t *)0x8010u;
    *serial_data = value;
}
