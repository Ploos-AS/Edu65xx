#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BASE 0xC000u
#define SIZE 0x4000u
static uint8_t rom[SIZE];
static unsigned pc;
static unsigned fail_patch[8], npatch;

static void b(uint8_t v){ rom[pc++-BASE]=v; }
static void w(uint16_t v){ b((uint8_t)v); b((uint8_t)(v>>8)); }
static void rel(uint8_t op,unsigned target){
    int d=(int)target-(int)(pc+2u);
    if(d < -128 || d > 127){ fprintf(stderr,"branch range\n"); return; }
    b(op); b((uint8_t)(int8_t)d);
}
static void init_ptr(void){ b(0xA9);b(0x02); b(0x85);b(0x00); b(0xA9);b(0x00); b(0x85);b(0x01); }
static void jfail(void){ b(0xF0);b(0x03); b(0x4C); fail_patch[npatch++]=pc; w(0xFFFF); }
static void advance(unsigned loop){
    b(0xE6);b(0x00); rel(0xD0,loop); b(0xE6);b(0x01);
    b(0xA6);b(0x01); b(0xE0);b(0x80); rel(0xD0,loop);
}
static void put16(unsigned a,unsigned v){ unsigned o=a-BASE; rom[o]=(uint8_t)v; rom[o+1]=(uint8_t)(v>>8); }

static void write_pattern(uint8_t pattern){
    unsigned loop;
    init_ptr(); b(0xA9);b(pattern); loop=pc;
    b(0x92);b(0x00); /* STA ($00) */
    advance(loop);
}
static void read_pattern(uint8_t pattern){
    unsigned loop;
    init_ptr(); loop=pc;
    b(0xB2);b(0x00); /* LDA ($00) */
    b(0xC9);b(pattern); jfail();
    advance(loop);
}

int main(int argc,char **argv){
    unsigned fail,pass,i; FILE *out;
    if(argc!=2) return 2;
    memset(rom,0xFF,sizeof(rom)); pc=BASE; npatch=0;

    b(0x78); b(0xD8);                         /* SEI CLD */
    b(0xA9);b(0x03); b(0x8D);w(0x8002);       /* DDRB */
    b(0xA9);b(0x00); b(0x8D);w(0x8000);       /* indicators off */

    write_pattern(0x55); read_pattern(0x55);
    write_pattern(0xAA); read_pattern(0xAA);

    b(0xA9);b(0x55); b(0x85);b(0x00);
    b(0xA9);b(0xAA); b(0x85);b(0x01);
    b(0xA5);b(0x00); b(0xC9);b(0x55); jfail();
    b(0xA5);b(0x01); b(0xC9);b(0xAA); jfail();

    pass=pc; b(0xA9);b(0x01); b(0x8D);w(0x8000); rel(0x80,pass);
    fail=pc; b(0xA9);b(0x02); b(0x8D);w(0x8000); rel(0x80,fail);

    for(i=0;i<npatch;++i){ unsigned o=fail_patch[i]-BASE; rom[o]=(uint8_t)fail; rom[o+1]=(uint8_t)(fail>>8); }
    put16(0xFFFA,BASE); put16(0xFFFC,BASE); put16(0xFFFE,BASE);

    out=fopen(argv[1],"wb"); if(!out){perror("fopen");return 2;}
    if(fwrite(rom,1,sizeof(rom),out)!=sizeof(rom)){perror("fwrite");fclose(out);return 2;}
    if(fclose(out)!=0){perror("fclose");return 2;}
    printf("wrote full RAM test: pass=$%04X fail=$%04X\n",pass,fail);
    return 0;
}
