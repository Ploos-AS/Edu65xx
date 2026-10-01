ENTRY(reset)

MEMORY
{
    ram (rw) : ORIGIN = 0x0200, LENGTH = 0x7E00
    rom (rx) : ORIGIN = 0xC000, LENGTH = 0x4000
}

SECTIONS
{
    .text :
    {
        *(.text*)
        *(.rodata*)
    } > rom

    .vectors 0xFFFA :
    {
        KEEP(*(.vectors))
    } > rom

    .bss (NOLOAD) :
    {
        *(.bss*)
    } > ram
}
