# Edu65xx Architecture

Edu65xx intentionally keeps the machine understandable.

## Primary CPU

The primary physical target is the W65C02S.

## Teaching stack

```text
Course material
    │
    ├── assembly examples
    ├── paired C examples
    │
    ├── simulator ── teaches CPU state and bus activity
    │
    └── emulator ─── runs complete Edu65xx systems
                         │
                         └── mirrors physical hardware
```

## Simulator vs emulator

The simulator is pedagogical first. It should expose registers, flags, instruction decoding, effective addresses and bus cycles clearly, even when that costs performance.

The emulator is system-oriented. It should run complete Edu65xx software with RAM, ROM and memory-mapped devices while remaining readable enough for students to study.

Both are written in straightforward C. Clever optimizations should not obscure architecture.

## Hardware principles

- use DIP/through-hole parts where practical
- socket major ICs
- expose buses and important control signals
- provide labeled test points
- keep address decoding understandable
- avoid hidden helper MCUs for core architectural functions
- make the breadboard implementation a real course target, not merely a prototype

## Software learning bridge

C is introduced only after students understand enough assembly to inspect generated code meaningfully.

Every major C concept should link back to machine behavior, for example:

- variables → memory/registers
- expressions → ALU instructions
- `if` → compare/test + branch
- loops → branches and counters
- functions → calling convention + stack
- pointers → addresses and indirect access
- arrays → indexed addressing
- interrupts → saved CPU state + ISR + return

## Future compatibility

W65C816 support may be added later, but should not complicate the beginner W65C02 path.
