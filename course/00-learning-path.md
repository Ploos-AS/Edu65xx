# Edu65xx Learning Path

Edu65xx teaches the same machine from several levels instead of treating assembly, C, simulation and hardware as unrelated subjects.

## Phase A — Assembly first

Students begin with the W65C02S execution model:

- registers
- flags
- instructions
- addressing modes
- machine code
- stack
- memory
- bus activity

## Phase B — Build the machine

Students connect the CPU to clock/reset, ROM, RAM and memory-mapped I/O and learn to observe real bus behavior.

## Phase C — Introduce C

C is introduced only after the student can read basic 65C02 assembly. C examples are paired with assembly and compiler output wherever practical.

The recurring question is:

> What did the compiler ask the CPU to do?

## Phase D — Study the simulator

The simulator is written in readable C. Students use it first as a learning tool, then later study and modify its implementation.

## Phase E — Study the emulator

The emulator expands the CPU model into a complete Edu65xx machine. Students learn how RAM, ROM and I/O devices are represented in software.

## Phase F — Connect all layers

By the end of the course, students should be able to follow a concept through the complete stack:

```text
C
↓
assembly
↓
machine code
↓
CPU state
↓
bus transactions
↓
physical memory and I/O
```


## Phase G — Build larger programs

M7 stops treating each concept as an isolated example.

Students combine the earlier layers through:

- deterministic debugging and bus watchpoints
- object files, symbols, relocations and linker maps
- a linked ROM monitor
- mixed C/assembly calls
- a four-module assembly application
- host-side emulator extensions around real device boundaries

The advanced sequence is:

```text
13  Debugging the machine
14  Linker and symbols
15  ROM monitor
16  Mixed C and assembly
17  Multi-module assembly
18  Emulator extension
```

The recurring question becomes:

> Which component owns this behavior, and what contract connects it to the next component?

## Phase H — Optional W65C816 continuation

Only after the W65C02 path is complete, Chapter 19 introduces the architectural bridge to W65C816.

The continuation begins from reset in Emulation mode and grows the model deliberately toward:

```text
native mode
 -> 16-bit register widths
 -> bank registers
 -> 24-bit addresses
 -> cross-bank control flow
 -> Direct Register and native stack
 -> block moves
```

This is an optional continuation. It does not replace the W65C02 course or alter the W65C02 Rev A hardware baseline.

The design contract is documented in:

```text
docs/W65C816-CONTINUATION.md
```
