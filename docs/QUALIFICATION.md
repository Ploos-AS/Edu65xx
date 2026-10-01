# M5 qualification

Edu65xx uses two complementary qualification layers.

## Native Edu65xx tests

The default `make test` suite verifies the architecture that belongs to this project:

- canonical RAM / I/O / ROM decoding
- reset, IRQ, NMI, BRK and stack behavior
- addressing modes
- arithmetic and flags, including W65C02 decimal mode
- shifts, rotates and read/modify/write operations
- W65C02 bit instructions
- WAI / STP functional states
- reserved NOP instruction lengths
- all 256 opcode bytes have a dispatch path
- deterministic machine stepping
- end-to-end monitor ROM boot and serial interaction

## Independent W65C02 test

CI also runs Klaus Dormann's `65C02_extended_opcodes_test.bin` from:

- upstream repository: `Klaus2m5/6502_65C02_functional_tests`
- pinned upstream commit: `7954e2dbb49c469ea286070bf46cdd71aeb29e4b`
- program start: `$0400`
- success trap: `$24F1`
- upstream license: GPLv3

The GPL test image is **not vendored into Edu65xx**. CI downloads the unmodified binary directly from the pinned upstream commit and executes it.

## Flat-memory qualification mode

The external test assumes a flat 64 KiB address space. The real Edu65xx machine does not: `$8000-$BFFF` is its I/O window and `$C000-$FFFF` is ROM.

For that reason the CPU core has an explicit `qualification_flat_memory` mode. It exists only to isolate CPU instruction semantics from the Edu65xx machine decoder while running third-party CPU tests.

Normal simulator and emulator execution leave this mode disabled.

This separation is deliberate:

```text
external CPU test
      |
      v
flat 64 KiB qualification bus
      |
      v
W65C02 execution core

Edu65xx ROM
      |
      v
canonical Edu65xx bus decoder
      |
      +-- RAM
      +-- VIA
      +-- serial
      +-- ROM
```

Passing the external CPU test is evidence for functional instruction compatibility. It is not evidence for exact PHI2 cycle timing or electrical pin behavior.
