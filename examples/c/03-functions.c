#include <stdint.h>

uint8_t add8(uint8_t a, uint8_t b)
{
    return (uint8_t)(a + b);
}

uint8_t add_then_increment(uint8_t a, uint8_t b)
{
    uint8_t sum = add8(a, b);
    return (uint8_t)(sum + 1u);
}
