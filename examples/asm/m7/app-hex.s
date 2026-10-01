        .section .text,"ax"
        .global print_hex
        .extern serial_putc

print_hex:
        pha
        lsr
        lsr
        lsr
        lsr
        jsr print_nibble
        pla
        and #0x0f

print_nibble:
        cmp #0x0a
        bcc digit
        clc
        adc #0x37
        bra emit

digit:
        clc
        adc #0x30

emit:
        jsr serial_putc
        rts
