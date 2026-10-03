# Edu65xx

Edu65xx is a practical, open course for learning how a real computer works by building and programming a 65xx system from the ground up.

The primary CPU and physical target is the **W65C02S**. The course starts with assembly and machine behavior, then introduces C only after students can connect source code to registers, memory, stack, instructions and bus activity.

## What Edu65xx includes

- a readable W65C02 simulator in C
- a complete Edu65xx machine emulator
- deterministic tests and independent W65C02 functional qualification
- assembly-first course material
- LLVM-MOS C/assembly labs
- debugger, linker and ROM-monitor labs
- Rev A breadboard design, ROMs and qualification procedures
- an optional W65C816 continuation design

The central learning path is:

```text
C source
   ↓
compiler / assembler / linker
   ↓
65C02 machine code
   ↓
CPU state and instructions
   ↓
bus accesses
   ↓
RAM / ROM / VIA / serial I/O
   ↓
physical signals
```

Students should be able to explain not only *what* a program does, but *how the machine makes it happen*.

## Repository map

- `course/` — lessons and exercises
- `examples/asm/` — assembly examples
- `examples/c/` — C examples and mixed-language labs
- `simulator/` — pedagogical W65C02 CPU and devices
- `emulator/` — complete machine, debugger and host-side extensions
- `rom/` — monitor and physical qualification ROM sources
- `hardware/` — Rev A breadboard design and future Trainer material
- `toolchain/` — pinned toolchain inputs and linker material
- `docs/` — architecture and project contracts
- `scripts/` — reproducible build/qualification helpers

## Current status

Software milestones **M0-M5 and M7 are complete**.

**M6 — Physical computer** remains intentionally in progress. The Rev A design, BOM, schematic contracts, bring-up ROMs, qualification procedure and CI package are complete, but physical PASS requires a real breadboard build and captured RESET/ROM/RAM/VIA/IRQ evidence.

See [`ROADMAP.md`](ROADMAP.md) for the exact gates.

## Hardware direction

The first physical platform remains deliberately transparent:

```text
W65C02S
  ├── address bus
  ├── data bus
  ├── R/W
  ├── PHI2 / reset / interrupt signals
  ├── ROM
  ├── RAM
  └── W65C22 VIA
```

No hidden microcontroller replaces the architectural concepts the student is meant to observe.

The Trainer PCB is deliberately gated on successful breadboard qualification.

## Reproduce the software qualification

A host with a C compiler and Make can run the core tests:

```sh
make all
```

The Rev A software/design qualification package is built with:

```sh
make rev-a-package
```

LLVM-MOS-dependent C/linker labs use the SDK version pinned in `toolchain/llvm-mos.version`. CI installs that exact release.

## Course progression

The course grows from CPU fundamentals through memory, I/O and interrupts into C, emulator internals, debugging, linking and larger multi-module programs.

Start with:

```text
course/00-learning-path.md
```

The optional W65C816 continuation begins only after the W65C02 path and does not change the W65C02 Rev A baseline.

## Licensing

Edu65xx is a mixed-license repository. The license follows the type of material:

- software, simulator/emulator code, scripts and ROM source: **MIT**
- hardware design files: **CERN-OHL-P-2.0**
- course and documentation: **CC-BY-4.0**

See [`LICENSES.md`](LICENSES.md) for the applicability rules and license texts/references.

Third-party material keeps its own license. The external Klaus W65C02 qualification binary is downloaded by CI from a pinned upstream revision and is not vendored into this repository.
