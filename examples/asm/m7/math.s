        .section .text,"ax"
        .global double_a

; Input:  A
; Output: A = A * 2 modulo 256
; Flags:  N/Z/C changed by ASL
double_a:
        asl
        rts
