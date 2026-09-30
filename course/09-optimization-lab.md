# M4 — Optimization lab

The same C semantics can produce very different assembly.

Compile the same source at `-O0`, `-O1` and `-O2`. Compare rather than guessing.

## Suggested sources

- `examples/c/03-functions.c`
- `examples/c/05-arrays.c`
- `examples/c/06-locals.c`

## Record for every build

- LLVM-MOS pin
- CPU target
- optimization level
- source file
- command line

## Compare

Count or identify:

- loads and stores,
- calls that remain,
- functions that are inlined,
- branches,
- zero-page accesses,
- hardware-stack operations,
- temporary values,
- loops and their control flow.

## Core lesson

C specifies program behavior within the language rules. It does not prescribe one fixed instruction sequence.

Therefore Edu65xx keeps three concepts separate:

```text
C source
handwritten conceptual assembly
actual compiler-generated assembly
```

Optimization makes that distinction impossible to ignore.
