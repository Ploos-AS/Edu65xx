; Edu65xx M6 physical VIA Timer 1 IRQ qualification ROM
;
; Expected analyzer story:
; RESET -> C000
; enable PB0 output and drive it high
; enable VIA Timer 1 interrupt
; start one-shot Timer 1
; CLI / WAI
; IRQB low -> CPU stack writes -> FFFE/FFFF -> irq_handler
; read T1C-L to acknowledge -> PB0 low -> RTI

VIA_ORB   = $8000
VIA_DDRB  = $8002
VIA_T1CL  = $8004
VIA_T1CH  = $8005
VIA_IER   = $800E

        .org $C000

reset:
        sei
        cld
        ldx #$ff
        txs

        lda #$01
        sta VIA_DDRB
        sta VIA_ORB

        lda #$c0              ; bit7=set, bit6=Timer1
        sta VIA_IER

        lda #$ff
        sta VIA_T1CL
        sta VIA_T1CH          ; load/start $FFFF one-shot

        cli

wait:
        wai
        bra wait

irq_handler:
        pha
        lda VIA_T1CL          ; acknowledge Timer 1
        lda #$00
        sta VIA_ORB           ; analyzer-visible ISR evidence
        pla
        rti

        .org $FFFA
        .word reset
        .word reset
        .word irq_handler
