# Edu65xx Roadmap

## M0 — Foundation

- [x] Establish project scope
- [x] Select W65C02S as primary hardware CPU
- [x] Define assembly-first, C-later teaching model
- [x] Define C-based simulator and emulator as first-class components
- [ ] Add initial course lessons
- [ ] Add build/test tooling
- [ ] Add simulator CPU skeleton
- [ ] Add emulator machine skeleton
- [ ] Document breadboard minimum system

## M1 — CPU fundamentals

- registers, flags and execution model
- first 65C02 assembly programs
- opcodes and machine code
- simulator register view and single-step execution

## M2 — Memory and buses

- address/data buses
- ROM, RAM, reset vectors
- zero page and stack
- memory map and address decoding
- simulator bus-cycle view

## M3 — I/O and interrupts

- memory-mapped I/O
- W65C22 VIA
- timers
- IRQ/NMI
- serial terminal path

## M4 — C bridge

- introduce C after assembly fundamentals
- pair C examples with equivalent assembly
- inspect compiler-generated assembly
- calling conventions, stack frames, pointers and arrays

## M5 — Emulator

- complete W65C02 execution core
- memory map
- ROM/RAM devices
- VIA/serial device models
- deterministic tests

## M6 — Physical computer

- breadboard system
- debug/test points
- logic-analyzer exercises
- Edu65xx Trainer/Computer PCB concept

## M7 — Advanced 65xx

- larger programs
- monitor/debugger
- toolchain internals
- emulator extension exercises
- optional W65C816 path
