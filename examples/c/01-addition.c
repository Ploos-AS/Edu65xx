#include <stdint.h>

/*
 * Edu65xx example 01: the C counterpart to examples/asm/01-addition.s
 *
 * The course will use examples like this to compare C source with
 * compiler-generated 65C02 assembly and the final machine behavior.
 */

volatile uint8_t result;

int main(void)
{
    result = (uint8_t)(5u + 3u);
    return 0;
}
