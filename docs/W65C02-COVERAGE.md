# W65C02 execution-core coverage

M5 tracks instruction **families and semantics**, not just opcode count. An opcode is not considered qualified until its relevant addressing modes and flags have automated tests.

## Implemented and exercised

- reset vector and register reset state
- LDA / STA across immediate, zero-page, indexed, absolute and indirect forms
- LDX / STX
- LDY / STY
- register transfers: TAX, TXA, TAY, TYA
- index increments/decrements: INX, DEX, INY, DEY
- CMP / CPX / CPY
- AND / ORA / EOR
- BIT including W65C02 immediate BIT semantics
- JMP absolute
- JSR / RTS
- BEQ / BNE / BRA
- PHA / PLA, PHX / PLX, PHY / PLY
- TSX / TXS
- STZ
- INC / DEC including accumulator forms
- CLC / SEC / CLD / SED / CLV / CLI / SEI
- ADC / SBC in binary and decimal mode, including W65C02-valid N/V/Z/C qualification
- ASL / LSR / ROL / ROR
- all eight conditional branches plus BRA
- PHP / PLP
- BRK two-byte signature behavior and IRQ/BRK vector path
- JMP absolute, indirect and W65C02 indexed-indirect
- TSB / TRB
- RMB0..7 / SMB0..7 and BBR0..7 / BBS0..7
- RTI
- NOP
- IRQ and requested-NMI entry
- functional bus accesses through the canonical Edu65xx memory map

## Partially implemented

- interrupt timing is functional rather than PHI2 cycle-accurate.
- NMI is represented as a deterministic pending request, not a pin-level edge detector.

## Remaining M5 ISA families

- WAI / STP machine-state behavior
- documented treatment of defined W65C02 NOP encodings

## Qualification rule

M5 is not complete merely because the monitor ROM boots. Completion requires the intended W65C02 instruction surface to have deterministic tests for results, flags, addressing and relevant stack/vector behavior.

Exact cycle timing is a separate qualification dimension and must not be inferred from the current functional bus-access trace.
