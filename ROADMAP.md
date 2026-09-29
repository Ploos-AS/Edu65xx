# Edu65xx Roadmap

## M0 — Foundation

- [x] Establish project scope
- [x] Select W65C02S as primary hardware CPU
- [x] Define assembly-first, C-later teaching model
- [x] Define C-based simulator and emulator as first-class components
- [x] Add initial course learning path
- [x] Add first paired ASM/C example
- [x] Add simulator CPU state/reset skeleton
- [x] Define emulator machine scope
- [x] Add build/test tooling
- [x] Document breadboard minimum system

**M0 status: COMPLETE**

## M1 — CPU fundamentals

- [x] Document registers, flags and execution model
- [x] Add first 65C02 assembly examples
- [x] Introduce opcodes and machine code
- [x] Implement readable fetch/decode/execute single-step core
- [x] Implement LDA/LDX/LDY immediate
- [x] Implement ADC immediate and carry/zero/negative basics
- [x] Add automated instruction/flag tests
- [x] Add GitHub Actions build/test CI
- [x] Add human-readable register/state trace view
- [x] Add a small runnable simulator CLI

**M1 status: COMPLETE**

## M2 — Memory and buses

- [ ] address/data buses
- [ ] ROM, RAM, reset vectors
- [ ] zero page and stack
- [ ] memory map and address decoding
- [ ] simulator bus-cycle view

**M2 status: NEXT**

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
