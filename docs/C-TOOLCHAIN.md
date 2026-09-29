# M4 toolchain — LLVM-MOS

Edu65xx uses **LLVM-MOS** as the primary C toolchain for the C bridge.

## Why

LLVM-MOS gives the course a modern Clang-based compiler while still exposing the constraints that make 65xx interesting:

- explicit 65C02 CPU targeting,
- C99-oriented freestanding programming,
- assembly output that can be inspected,
- a documented MOS ABI/calling convention,
- zero-page imaginary registers used to compensate for the small physical register set.

The course does not treat compiler output as magic or as a fixed translation table. Optimization can radically change the emitted program.

## Target

Course compiler experiments should explicitly target the 65C02 family. Pinning an exact tested toolchain version/container is a later reproducibility task.

## Three views

Every compiler lab should retain three distinct artifacts:

```text
source.c              human C source
conceptual.s          handwritten explanation
compiler-output.s     actual output from the pinned compiler
```

The second and third files must never be presented as interchangeable.

## ABI topics to inspect

LLVM-MOS uses both physical registers and zero-page-backed imaginary registers. Later M4 labs will inspect:

- arguments,
- return values,
- caller/callee responsibilities,
- hardware stack usage,
- soft/static stack behavior,
- zero-page register allocation,
- optimization and inlining.

## Optimization

We will compare at least an easy-to-read low-optimization build with an optimized build. The lesson is not that one assembly listing is the canonical translation of C; the lesson is that C describes semantics and the compiler chooses a legal implementation.

## Alternative toolchains

cc65 remains useful as a comparison/appendix toolchain because it is a mature 6502 C development suite. It is not the primary ABI for the core M4 lessons.
