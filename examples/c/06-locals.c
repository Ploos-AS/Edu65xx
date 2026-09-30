#include <stdint.h>

uint8_t transform(uint8_t input)
{
    uint8_t doubled = (uint8_t)(input + input);
    uint8_t adjusted = (uint8_t)(doubled + 3u);
    return adjusted;
}

uint16_t combine(uint8_t high, uint8_t low)
{
    uint16_t value = (uint16_t)high << 8;
    value |= low;
    return value;
}
