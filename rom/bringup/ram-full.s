; Edu65xx M6 destructive full 32 KiB RAM test
; PB0 = PASS, PB1 = FAIL
; $0000/$0001 are the temporary pointer and are tested last.

VIA_ORB  = $8000
VIA_DDRB = $8002
PTR       = $00

        .org $C000

reset:
        sei
        cld
        lda #$03
        sta VIA_DDRB
        lda #$00
        sta VIA_ORB

        ; write/read $55 over $0002-$7fff
        lda #$02
        sta PTR
        lda #$00
        sta PTR+1
        lda #$55
write55:
        sta (PTR)
        inc PTR
        bne write55
        inc PTR+1
        ldx PTR+1
        cpx #$80
        bne write55

        lda #$02
        sta PTR
        lda #$00
        sta PTR+1
read55:
        lda (PTR)
        cmp #$55
        bne fail
        inc PTR
        bne read55
        inc PTR+1
        ldx PTR+1
        cpx #$80
        bne read55

        ; repeat with complementary pattern $AA
        lda #$02
        sta PTR
        lda #$00
        sta PTR+1
        lda #$aa
writeaa:
        sta (PTR)
        inc PTR
        bne writeaa
        inc PTR+1
        ldx PTR+1
        cpx #$80
        bne writeaa

        lda #$02
        sta PTR
        lda #$00
        sta PTR+1
readaa:
        lda (PTR)
        cmp #$aa
        bne fail
        inc PTR
        bne readaa
        inc PTR+1
        ldx PTR+1
        cpx #$80
        bne readaa

        ; pointer bytes are no longer needed
        lda #$55
        sta $00
        lda #$aa
        sta $01
        lda $00
        cmp #$55
        bne fail
        lda $01
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
