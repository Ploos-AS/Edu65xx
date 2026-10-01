        .section .text,"ax"
        .global reset
        .extern c_plus_one

reset:
        sei
        cld
        ldx #0xff
        txs

        ; LLVM-MOS uint8_t argument/result ABI uses A for this simple leaf call.
        lda #41
        jsr c_plus_one
        sta 0x0201
        stp

        .section .vectors,"a"
        .word reset
        .word reset
        .word reset
