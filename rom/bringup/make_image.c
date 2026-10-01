#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ROM_BASE 0xC000u
#define ROM_SIZE 0x4000u

static void put16(uint8_t *rom, unsigned address, unsigned value)
{
    unsigned offset = address - ROM_BASE;
    rom[offset] = (uint8_t)value;
    rom[offset + 1u] = (uint8_t)(value >> 8);
}

int main(int argc, char **argv)
{
    static const uint8_t program[] = {
        0x78,                   /* C000 SEI */
        0xD8,                   /* C001 CLD */
        0xA2, 0xFF,             /* C002 LDX #$FF */
        0x9A,                   /* C004 TXS */
        0xA9, 0x01,             /* C005 LDA #$01 */
        0x8D, 0x02, 0x80,       /* C007 STA $8002 DDRB */
        0xA9, 0x01,             /* C00A loop: LDA #$01 */
        0x8D, 0x00, 0x80,       /* C00C STA $8000 ORB */
        0x20, 0x1C, 0xC0,       /* C00F JSR $C01C delay */
        0xA9, 0x00,             /* C012 LDA #$00 */
        0x8D, 0x00, 0x80,       /* C014 STA $8000 ORB */
        0x20, 0x1C, 0xC0,       /* C017 JSR $C01C delay */
        0x80, 0xEE,             /* C01A BRA $C00A */
        0xA2, 0x00,             /* C01C delay: LDX #$00 */
        0xA0, 0x00,             /* C01E outer: LDY #$00 */
        0x88,                   /* C020 inner: DEY */
        0xD0, 0xFD,             /* C021 BNE $C020 */
        0xCA,                   /* C023 DEX */
        0xD0, 0xF8,             /* C024 BNE $C01E */
        0x60                    /* C026 RTS */
    };
    uint8_t rom[ROM_SIZE];
    FILE *out;

    if (argc != 2) {
        fprintf(stderr, "usage: %s output.bin\n", argv[0]);
        return 2;
    }

    memset(rom, 0xFF, sizeof(rom));
    memcpy(rom, program, sizeof(program));

    put16(rom, 0xFFFAu, 0xC000u); /* NMI */
    put16(rom, 0xFFFCu, 0xC000u); /* RESET */
    put16(rom, 0xFFFEu, 0xC000u); /* IRQ/BRK */

    out = fopen(argv[1], "wb");
    if (out == NULL) {
        perror("fopen");
        return 2;
    }
    if (fwrite(rom, 1u, sizeof(rom), out) != sizeof(rom)) {
        perror("fwrite");
        fclose(out);
        return 2;
    }
    if (fclose(out) != 0) {
        perror("fclose");
        return 2;
    }

    printf("wrote %u-byte Rev A bring-up ROM; RESET=$C000\n", ROM_SIZE);
    return 0;
}
