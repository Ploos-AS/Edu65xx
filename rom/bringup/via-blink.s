; Edu65xx M6 physical bring-up ROM
; First target: prove RESET -> ROM -> W65C02S -> W65C22S -> PB0.
;
; Canonical addresses:
VIA_ORB  = $8000
VIA_DDRB = $8002

        .org $C000

reset:
        sei
        cld
        ldx #$ff
        txs

        lda #$01
        sta VIA_DDRB            ; PB0 output, PB1..PB7 input

loop:
        lda #$01
        sta VIA_ORB
        jsr delay

        lda #$00
        sta VIA_ORB
        jsr delay
        bra loop

; Deliberately simple software delay.
; Its wall-clock duration depends on the physical PHI2 frequency.
delay:
        ldx #$00
delay_outer:
        ldy #$00
delay_inner:
        dey
        bne delay_inner
        dex
        bne delay_outer
        rts

        .org $FFFA
        .word reset             ; NMI: restart during first bring-up
        .word reset             ; RESET
        .word reset             ; IRQ/BRK: restart until IRQ lab
