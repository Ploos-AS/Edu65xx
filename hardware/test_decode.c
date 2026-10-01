#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static int ram_selected(uint16_t a)
{
    return (a & 0x8000u) == 0u;
}

static int rom_selected(uint16_t a)
{
    return (a & 0xC000u) == 0xC000u;
}

static int via_selected(uint16_t a)
{
    /* U5 Y4: A15:A14:A13 == 100
       plus A12 == 0 and U9: A11..A4 == 0. */
    return (a & 0xFFF0u) == 0x8000u;
}

int main(void)
{
    unsigned a;
    unsigned ram_count = 0u;
    unsigned rom_count = 0u;
    unsigned via_count = 0u;

    for (a = 0u; a <= 0xFFFFu; ++a) {
        int ram = ram_selected((uint16_t)a);
        int rom = rom_selected((uint16_t)a);
        int via = via_selected((uint16_t)a);

        /* No physical Rev A device may overlap another select. */
        assert((ram + rom + via) <= 1);

        if (ram) ++ram_count;
        if (rom) ++rom_count;
        if (via) ++via_count;

        if (a < 0x8000u) {
            assert(ram && !rom && !via);
        } else if (a >= 0x8000u && a <= 0x800Fu) {
            assert(!ram && !rom && via);
        } else if (a >= 0xC000u) {
            assert(!ram && rom && !via);
        } else {
            assert(!ram && !rom && !via);
        }
    }

    assert(ram_count == 32768u);
    assert(via_count == 16u);
    assert(rom_count == 16384u);

    puts("Rev A decode: PASS (RAM=32768 VIA=16 ROM=16384, no overlap)");
    return 0;
}
