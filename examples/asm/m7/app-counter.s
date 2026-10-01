        .section .text,"ax"
        .global counter_reset
        .global counter_inc
        .global counter_dec
        .global counter_get

counter_reset:
        stz 0x0200
        rts

counter_inc:
        inc 0x0200
        lda 0x0200
        rts

counter_dec:
        dec 0x0200
        lda 0x0200
        rts

counter_get:
        lda 0x0200
        rts
