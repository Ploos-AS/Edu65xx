#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ROM_BASE 0xC000u
#define ROM_SIZE 0x4000u

static uint8_t rom[ROM_SIZE];
static unsigned pc;

static void b(uint8_t value) { rom[pc++ - ROM_BASE] = value; }
static void w(uint16_t value) { b((uint8_t)value); b((uint8_t)(value >> 8)); }
static void lda_imm(uint8_t v) { b(0xA9); b(v); }
static void sta_abs(uint16_t a) { b(0x8D); w(a); }
static void lda_abs(uint16_t a) { b(0xAD); w(a); }

static void put16(unsigned address, unsigned value)
{
    unsigned offset = address - ROM_BASE;
    rom[offset] = (uint8_t)value;
    rom[offset + 1u] = (uint8_t)(value >> 8);
}

int main(int argc, char **argv)
{
    static const uint16_t p55[] = {0x0000,0x00FF,0x0100,0x01FF,0x0200,0x4000,0x7FFF};
    static const uint16_t paa[] = {0x0001,0x0080,0x0180,0x2000,0x6000,0x7FFE};
    unsigned fail_patch[sizeof(p55)/sizeof(p55[0]) + sizeof(paa)/sizeof(paa[0])];
    unsigned patches = 0u, i, pass, fail;
    FILE *out;

    if (argc != 2) { fprintf(stderr, "usage: %s output.bin\n", argv[0]); return 2; }
    memset(rom, 0xFF, sizeof(rom));
    pc = ROM_BASE;

    b(0x78); b(0xD8); b(0xA2); b(0xFF); b(0x9A); /* SEI CLD LDX #FF TXS */
    lda_imm(0x03); sta_abs(0x8002);
    lda_imm(0x00); sta_abs(0x8000);

    lda_imm(0x55);
    for (i=0; i<sizeof(p55)/sizeof(p55[0]); ++i) sta_abs(p55[i]);
    lda_imm(0xAA);
    for (i=0; i<sizeof(paa)/sizeof(paa[0]); ++i) sta_abs(paa[i]);

    for (i=0; i<sizeof(p55)/sizeof(p55[0]); ++i) {
        lda_abs(p55[i]); b(0xC9); b(0x55); /* CMP #55 */
        b(0xF0); b(0x03);                 /* BEQ +3 */
        b(0x4C); fail_patch[patches++] = pc; w(0xFFFF); /* JMP fail */
    }
    for (i=0; i<sizeof(paa)/sizeof(paa[0]); ++i) {
        lda_abs(paa[i]); b(0xC9); b(0xAA);
        b(0xF0); b(0x03);
        b(0x4C); fail_patch[patches++] = pc; w(0xFFFF);
    }

    pass = pc;
    lda_imm(0x01); sta_abs(0x8000); b(0x80); b(0xF9); /* BRA pass */

    fail = pc;
    lda_imm(0x02); sta_abs(0x8000); b(0x80); b(0xF9); /* BRA fail */

    for (i=0; i<patches; ++i) {
        unsigned off = fail_patch[i] - ROM_BASE;
        rom[off] = (uint8_t)fail;
        rom[off+1u] = (uint8_t)(fail >> 8);
    }

    put16(0xFFFAu, 0xC000u);
    put16(0xFFFCu, 0xC000u);
    put16(0xFFFEu, 0xC000u);

    out = fopen(argv[1], "wb");
    if (!out) { perror("fopen"); return 2; }
    if (fwrite(rom,1u,sizeof(rom),out) != sizeof(rom)) { perror("fwrite"); fclose(out); return 2; }
    if (fclose(out) != 0) { perror("fclose"); return 2; }

    printf("wrote RAM smoke ROM: pass=$%04X fail=$%04X size=%u\n", pass, fail, ROM_SIZE);
    return 0;
}
