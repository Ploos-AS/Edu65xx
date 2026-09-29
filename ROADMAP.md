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

- [x] Document address/data buses
- [x] Model ROM/RAM address space and reset vectors
- [x] Document and exercise zero page and stack
- [x] Document memory map and address-decoding concept
- [x] Route simulator memory access through explicit bus reads/writes
- [x] Add readable simulator bus-cycle trace
- [x] Add zero-page and absolute LDA/STA instructions
- [x] Add PHA/PLA stack operations
- [x] Add automated reset-vector, memory, stack and bus tests

**M2 status: COMPLETE**

## M3 — I/O and interrupts

- [x] define memory-mapped I/O region
- [x] model W65C22 VIA registers
- [x] GPIO input/output teaching example
- [x] timer model and lesson
- [x] IRQ/NMI fundamentals
- [x] CPU interrupt entry/return path
- [x] serial terminal direction

**M3 status: COMPLETE**

## M4 — C bridge

**M4 status: IN PROGRESS**

- [x] introduce C after assembly fundamentals
- [x] pair first C examples with conceptual assembly
- [x] select and document primary C toolchain
- [x] pin reproducible LLVM-MOS build/toolchain
- [ ] capture and inspect compiler-generated assembly
- [ ] calling convention and ABI labs
- [ ] pointers and memory-mapped I/O labs
- [ ] arrays and indexing labs
- [ ] functions, locals and stack/storage labs
- [ ] optimization comparison labs

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
