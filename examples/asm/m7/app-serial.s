        .section .text,"ax"
        .global serial_getc
        .global serial_putc

serial_getc:
        lda 0x8011
        and #0x01
        beq serial_getc
        lda 0x8010
        rts

serial_putc:
        sta 0x8010
        rts
