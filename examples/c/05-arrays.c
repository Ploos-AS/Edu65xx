#include <stdint.h>
#include <stddef.h>

uint8_t sum_bytes(const uint8_t *values, size_t count)
{
    uint8_t sum = 0u;
    size_t i;

    for (i = 0u; i < count; ++i) {
        sum = (uint8_t)(sum + values[i]);
    }

    return sum;
}
