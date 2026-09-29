# M3 — Serial terminal

A terminal is the first general-purpose human interface for Edu65xx.

## Teaching interface

The simulator reserves:

```text
$8010  SERIAL_DATA
$8011  SERIAL_STATUS
```

Status bits:

```text
bit 0  RX ready
bit 1  TX ready
```

Writing a byte to `$8010` transmits it. Reading `$8010` receives one byte and clears RX-ready.

## First output

```asm
lda #'H'
sta $8010
lda #'i'
sta $8010
```

This is intentionally simple. A student already understands `LDA`, `STA`, addresses and bus writes, so serial output introduces no hidden software abstraction.

## Simulator versus physical hardware

The two-register interface is an Edu65xx teaching device, not a claim that a particular physical UART has this register layout.

For the physical computer we will select and document a real UART/serial solution later. Its driver can present the same conceptual operations:

- status
- receive byte
- transmit byte

This keeps the course progression stable while still requiring students to study the real hardware registers when the physical UART is introduced.

## Milestone

The next ROM milestone is a tiny terminal monitor:

```text
RESET -> initialize -> print banner -> receive command -> respond
```

That monitor will become useful again in later lessons on C, debugging and the emulator.
