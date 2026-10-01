; Edu65xx M6 physical RAM smoke-test ROM
; Run only after the basic VIA GPIO ROM has passed.
;
; PB0 = PASS
; PB1 = FAIL
;
; This first RAM image checks boundary/representative locations. A later
; destructive full-memory test belongs after the basic wiring is proven.

VIA_ORB  = $8000
VIA_DDRB = $8002

        .org $C000

reset:
        sei
        cld
        ldx #$ff
        txs

        lda #$03
        sta VIA_DDRB
        lda #$00
        sta VIA_ORB

        lda #$55
        sta $0000
        sta $00ff
        sta $0100
        sta $01ff
        sta $0200
        sta $4000
        sta $7fff

        lda #$aa
        sta $0001
        sta $0080
        sta $0180
        sta $2000
        sta $6000
        sta $7ffe

        lda $0000
        cmp #$55
        bne fail
        lda $00ff
        cmp #$55
        bne fail
        lda $0100
        cmp #$55
        bne fail
        lda $01ff
        cmp #$55
        bne fail
        lda $0200
        cmp #$55
        bne fail
        lda $4000
        cmp #$55
        bne fail
        lda $7fff
        cmp #$55
        bne fail

        lda $0001
        cmp #$aa
        bne fail
        lda $0080
        cmp #$aa
        bne fail
        lda $0180
        cmp #$aa
        bne fail
        lda $2000
        cmp #$aa
        bne fail
        lda $6000
        cmp #$aa
        bne fail
        lda $7ffe
        cmp #$aa
        bne fail

pass:
        lda #$01
        sta VIA_ORB
        bra pass

fail:
        lda #$02
        sta VIA_ORB
        bra fail

        .org $FFFA
        .word reset
        .word reset
        .word reset
