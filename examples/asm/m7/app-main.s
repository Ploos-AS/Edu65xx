        .section .text,"ax"
        .global reset
        .extern serial_getc
        .extern serial_putc
        .extern counter_reset
        .extern counter_inc
        .extern counter_dec
        .extern counter_get
        .extern print_hex

reset:
        sei
        cld
        ldx #0xff
        txs
        jsr counter_reset

        ldx #0x00
banner_loop:
        lda banner,x
        beq command_loop
        jsr serial_putc
        inx
        bra banner_loop

command_loop:
        jsr serial_getc
        cmp #0x2b
        beq do_inc
        cmp #0x2d
        beq do_dec
        cmp #0x52
        beq do_reset
        cmp #0x56
        beq do_view
        cmp #0x3f
        beq do_help
        jsr serial_putc
        bra command_loop

do_inc:
        jsr counter_inc
        bra show_value

do_dec:
        jsr counter_dec
        bra show_value

do_reset:
        jsr counter_reset
        bra show_value

do_view:
show_value:
        lda #0x3d
        jsr serial_putc
        jsr counter_get
        jsr print_hex
        lda #0x0d
        jsr serial_putc
        lda #0x0a
        jsr serial_putc
        bra command_loop

do_help:
        ldx #0x00
help_loop:
        lda helptext,x
        beq command_loop
        jsr serial_putc
        inx
        bra help_loop

        .section .rodata,"a"
banner:
        .asciz "Edu65xx M7 app\r\n"
helptext:
        .asciz "+ inc  - dec  R reset  V view  ? help\r\n"

        .section .vectors,"a"
        .word reset
        .word reset
        .word reset
