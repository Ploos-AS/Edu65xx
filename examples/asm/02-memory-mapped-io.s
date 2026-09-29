; Conceptual assembly counterpart to examples/c/02-memory-mapped-io.c
; This is handwritten teaching code, NOT claimed compiler output.

SERIAL_DATA   = $8010
SERIAL_STATUS = $8011
TX_READY      = $02

wait_tx:
        lda SERIAL_STATUS
        ; A complete version tests bit 1 and loops until TX is ready.
        ; Branch/test instructions are introduced as M4 expands.
        lda #'C'
        sta SERIAL_DATA
