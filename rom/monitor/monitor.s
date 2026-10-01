; Edu65xx M5 monitor ROM
; Functional source for the first bootable course-machine monitor.
; Reset vector: $C000
;
; Teaching serial:
;   $8010 DATA
;   $8011 STATUS bit 0 = RX_READY

        .org $C000

reset:
        sei
        cld
        ldx #$ff
        txs
        ldx #$00

banner_loop:
        lda banner,x
        beq wait_rx
        sta $8010
        inx
        bra banner_loop

wait_rx:
        lda $8011
        and #$01
        beq wait_rx
        lda $8010
        cmp #'?'
        beq help
        sta $8010
        bra wait_rx

help:
        ldx #$00
help_loop:
        lda helptext,x
        beq wait_rx
        sta $8010
        inx
        bra help_loop

banner:
        .byte "Edu65xx ready", $0d, $0a, $00
helptext:
        .byte "? help", $0d, $0a, $00

        .org $FFFA
        .word reset             ; NMI placeholder for M5 monitor
        .word reset             ; RESET
        .word reset             ; IRQ/BRK placeholder for M5 monitor
