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
