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

**M4 status: COMPLETE**

- [x] introduce C after assembly fundamentals
- [x] pair first C examples with conceptual assembly
- [x] select and document primary C toolchain
- [x] pin reproducible LLVM-MOS build/toolchain
- [x] capture and inspect compiler-generated assembly
- [x] calling convention and ABI labs
- [x] pointers and memory-mapped I/O labs
- [x] arrays and indexing labs
- [x] functions, locals and stack/storage labs
- [x] optimization comparison labs

## M5 — Emulator

**M5 status: COMPLETE**

- [x] complete W65C02 functional execution core
- [x] enforce canonical memory map
- [x] ROM/RAM device behavior
- [x] VIA/serial device routing
- [x] deterministic machine-step API
- [x] ROM boot/reset-vector tests
- [x] deterministic device-tick tests
- [x] execute a monitor ROM end to end
- [x] complete M5 qualification suite

## M6 — Physical computer

**M6 status: IN PROGRESS**

- [x] breadboard architecture and bring-up guide
- [x] select CPU, VIA, RAM and EEPROM candidates
- [x] canonical discrete address-decode design
- [x] debug/test points and analyzer header
- [x] logic-analyzer exercises
- [x] first VIA GPIO bring-up ROM image + emulator qualification
- [x] RAM smoke-test ROM image + emulator qualification
- [x] destructive full 32 KiB RAM ROM + emulator qualification
- [x] staged physical qualification procedure and course lesson
- [x] VIA Timer1 IRQ ROM image + stack/vector/RTI qualification
- [x] Edu65xx Trainer/Computer PCB concept
- [x] complete clock and reset design
- [x] complete Rev A purchasing baseline
- [x] complete net-level breadboard wiring contract
- [x] complete pin-numbered Rev A schematic contract from manufacturer datasheets
- [x] draw graphical Rev A review schematic and perform datasheet/connectivity review
- [x] publish staged assembly checklist and analyzer channel plans
- [x] make complete Rev A hardware/ROM qualification a mandatory CI gate
- [x] publish reproducible `edu65xx-rev-a` CI build artifact
- [ ] build breadboard system
- [ ] capture physical RESET/ROM/RAM/VIA/IRQ evidence
- [ ] qualify physical breadboard system
- [ ] create Trainer PCB schematic/layout after breadboard qualification

## M7 — Advanced 65xx

**M7 status: IN PROGRESS**

- [ ] larger multi-module assembly program
- [x] expanded serial monitor ROM
- [x] deterministic host debugger core
- [x] PC breakpoints
- [x] read/write bus watchpoints
- [x] bounded debugger run and explicit stop reasons
- [x] interactive debugger CLI
- [x] first monitor/debugger course lab
- [x] linker/memory-layout lab
- [x] inspect relocations/symbols/map output
- [x] mixed C/assembly program
- [ ] emulator extension exercise
- [ ] optional W65C816 continuation design
