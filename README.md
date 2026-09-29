# Edu65xx

Edu65xx is a practical, open course for learning how a real computer works by building and programming a 65xx system from the ground up.

The main hardware target is the **W65C02S**, with assembly first and C introduced later so students can directly connect high-level code to the machine instructions, registers, stack, memory and bus activity underneath.

## M0 goals

M0 establishes the project structure and learning model:

- W65C02S as the primary CPU target
- assembly first, then C
- explicit ASM ↔ C comparisons throughout the course
- a readable C-based simulator for teaching internals
- a readable C-based emulator for running complete Edu65xx systems
- physical breadboard hardware as a first-class target
- future Edu65xx Trainer/Computer PCB
- free/open tooling where practical
- course material that does not require prior EduCPU or EduAVR knowledge

## Learning model

The same concepts are revisited at multiple abstraction levels:

```text
C source
   ↓
compiler
   ↓
65C02 assembly
   ↓
machine code
   ↓
CPU state changes
   ↓
bus cycles
   ↓
RAM / ROM / I/O
```

Students should be able to answer not only *what* a program does, but also *how the CPU executes it*.

## Project components

- `course/` — lessons and exercises
- `examples/asm/` — assembly examples
- `examples/c/` — C examples matching the assembly material
- `simulator/` — pedagogical W65C02 simulator in readable C
- `emulator/` — complete Edu65xx system emulator in readable C
- `hardware/` — breadboard, schematics and future PCB material
- `docs/` — architecture, conventions and project documentation

## Hardware direction

The first physical platform should remain intentionally transparent:

```text
W65C02S
  ├── address bus
  ├── data bus
  ├── R/W
  ├── clock/reset
  ├── ROM
  ├── RAM
  └── memory-mapped I/O
```

No hidden microcontroller should replace the architectural concepts the student is supposed to learn.

## Planned progression

1. CPU registers and execution model
2. first assembly program
3. machine code and opcodes
4. clock and reset
5. address and data bus
6. ROM and reset vectors
7. RAM, zero page and stack
8. address decoding
9. memory-mapped I/O
10. interrupts and timers
11. serial/terminal I/O
12. structured assembly
13. C on 65C02
14. compare compiler output with handwritten assembly
15. simulator internals
16. emulator internals
17. complete Edu65xx computer
18. optional W65C816 continuation

## Status

**M0 — project bootstrap**

See [`ROADMAP.md`](ROADMAP.md).

## License

Software source is intended to use the MIT License. Hardware design files are intended to use CERN-OHL-P-2.0. Course/documentation licensing will be documented separately.
