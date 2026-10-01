        .section .text,"ax"
        .global reset

reset:
        sei
        cld
        ldx #0xff
        txs
        ldx #0x00

banner_loop:
        lda banner,x
        beq wait_rx
        sta 0x8010
        inx
        bra banner_loop

wait_rx:
        lda 0x8011
        and #0x01
        beq wait_rx
        lda 0x8010
        cmp #0x3f
        beq help
        cmp #0x50
        beq ping
        cmp #0x4d
        beq memory
        sta 0x8010
        bra wait_rx

help:
        ldx #0x00
help_loop:
        lda helptext,x
        beq wait_rx
        sta 0x8010
        inx
        bra help_loop

ping:
        ldx #0x00
ping_loop:
        lda pong,x
        beq wait_rx
        sta 0x8010
        inx
        bra ping_loop

memory:
        ldx #0x00
memory_prefix_loop:
        lda memory_prefix,x
        beq memory_value
        sta 0x8010
        inx
        bra memory_prefix_loop

memory_value:
        lda 0x0200
        jsr print_hex
        lda #0x0d
        sta 0x8010
        lda #0x0a
        sta 0x8010
        bra wait_rx

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
        sta 0x8010
        rts

        .section .rodata,"a"
banner:
        .asciz "Edu65xx M7 ready\r\n"
helptext:
        .asciz "? help  P ping  M mem0200\r\n"
pong:
        .asciz "PONG\r\n"
memory_prefix:
        .asciz "M 0200="

        .section .vectors,"a"
        .word reset
        .word reset
        .word reset
