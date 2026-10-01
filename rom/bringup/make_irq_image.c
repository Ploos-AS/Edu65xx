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
        0x8D, 0x00, 0x80,       /* C00A STA $8000 ORB */
        0xA9, 0xC0,             /* C00D LDA #$C0 */
        0x8D, 0x0E, 0x80,       /* C00F STA $800E IER */
        0xA9, 0xFF,             /* C012 LDA #$FF */
        0x8D, 0x04, 0x80,       /* C014 STA $8004 T1CL */
        0x8D, 0x05, 0x80,       /* C017 STA $8005 T1CH/start */
        0x58,                   /* C01A CLI */
        0xCB,                   /* C01B wait: WAI */
        0x80, 0xFD,             /* C01C BRA $C01B */
        0x48,                   /* C01E irq_handler: PHA */
        0xAD, 0x04, 0x80,       /* C01F LDA $8004 acknowledge */
        0xA9, 0x00,             /* C022 LDA #$00 */
        0x8D, 0x00, 0x80,       /* C024 STA $8000 ORB */
        0x68,                   /* C027 PLA */
        0x40                    /* C028 RTI */
    };
    uint8_t rom[ROM_SIZE];
    FILE *out;

    if (argc != 2) {
        fprintf(stderr, "usage: %s output.bin\n", argv[0]);
        return 2;
    }

    memset(rom, 0xFF, sizeof(rom));
    memcpy(rom, program, sizeof(program));
    put16(rom, 0xFFFAu, 0xC000u);
    put16(rom, 0xFFFCu, 0xC000u);
    put16(rom, 0xFFFEu, 0xC01Eu);

    out = fopen(argv[1], "wb");
    if (out == NULL) { perror("fopen"); return 2; }
    if (fwrite(rom, 1u, sizeof(rom), out) != sizeof(rom)) {
        perror("fwrite"); fclose(out); return 2;
    }
    if (fclose(out) != 0) { perror("fclose"); return 2; }

    puts("wrote 16384-byte Rev A VIA IRQ qualification ROM; IRQ=$C01E");
    return 0;
}
