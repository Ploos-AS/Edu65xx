        .section .text,"ax"
        .global reset
        .extern double_a

reset:
        sei
        cld
        ldx #0xff
        txs

        lda #21
        jsr double_a
        sta 0x0200
        stp

        .section .vectors,"a"
        .word reset
        .word reset
        .word reset
