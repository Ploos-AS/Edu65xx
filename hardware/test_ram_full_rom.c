#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "machine.h"

int main(int argc,char **argv)
{
    edu65xx_machine_t m;
    uint8_t rom[EDU65XX_ROM_SIZE];
    FILE *f;
    unsigned long steps;
    int pass=0;

    if(argc!=2) return 2;
    f=fopen(argv[1],"rb"); assert(f);
    assert(fread(rom,1,sizeof(rom),f)==sizeof(rom));
    assert(fgetc(f)==EOF); fclose(f);

    edu65xx_machine_init(&m);
    assert(edu65xx_machine_load_rom(&m,rom,sizeof(rom),0)==0);
    edu65xx_machine_reset(&m);

    for(steps=0;steps<1000000ul && !pass;++steps){
        assert(edu65xx_machine_step(&m)==0);
        assert((m.cpu.via.orb & 0x03u)!=0x02u);
        if((m.cpu.via.ddrb & 0x03u)==0x03u && (m.cpu.via.orb & 0x03u)==0x01u)
            pass=1;
    }
    assert(pass);
    assert(m.cpu.memory[0x0000]==0x55u);
    assert(m.cpu.memory[0x0001]==0xAAu);
    assert(m.cpu.memory[0x0002]==0xAAu);
    assert(m.cpu.memory[0x0100]==0xAAu);
    assert(m.cpu.memory[0x7FFF]==0xAAu);
    printf("Rev A full 32 KiB RAM ROM: PASS after %lu steps\n",steps);
    return 0;
}
