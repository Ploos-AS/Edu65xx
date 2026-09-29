; Edu65xx example 01: simple addition
;
; Purpose: show immediate loads/addition and a memory store.

        lda #$05
        clc
        adc #$03
        sta result

result: .byte $00
