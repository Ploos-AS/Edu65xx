# 16 — Calling C from assembly

M4 showed C becoming assembly.

M7 now links C and handwritten assembly into the **same ROM**.

## The experiment

The assembly entry point does this:

```asm
lda #41
jsr c_plus_one
sta 0x0201
stp
```

The called function is C:

```c
uint8_t c_plus_one(uint8_t value)
{
    return (uint8_t)(value + 1u);
}
```

The final machine must store `42` at `$0201`.

## What makes this possible?

The two source files do not agree by magic.

They agree because both sides follow an ABI — an application binary interface.

For this simple `uint8_t -> uint8_t` leaf function, the generated LLVM-MOS code uses A for the value crossing the boundary.

That means the handwritten caller can deliberately place the argument in A and read the result from A.

Do not generalize this one observation into a complete calling-convention specification.

Larger values, multiple arguments, locals, recursion and optimization can require different storage and compiler-generated support.

The correct workflow is:

1. write the C declaration
2. compile with the pinned compiler
3. inspect generated code/symbols
4. write the assembly side to the documented/observed ABI
5. test the linked program

## Object files meet at the linker

The build creates:

```text
mixed-c.o
mixed-entry.o
       |
       v
     LLD
       |
       +-- mixed.elf
       +-- mixed.map
       +-- mixed.bin
```

The C compiler does not need to know the final address of `reset`.

The assembler does not need to know the final address of `c_plus_one`.

The linker resolves both.

## Inspect the boundary

Open the final disassembly.

Find:

- `reset`
- `c_plus_one`
- the `JSR`
- the return instruction
- the store to `$0201`

Then inspect `symbols.txt` and `mixed.map`.

Connect the symbolic call in source to the numeric address in the final instruction bytes.

## Why not inline assembly?

Inline assembly is useful for small operations, but it also gives the compiler and programmer a complicated shared contract.

For a whole assembly routine, a separate `.s` module makes the boundary explicit:

```text
C semantics | ABI | assembly semantics
```

That is the boundary this lab is teaching.

## CI qualification

The CI pipeline:

1. compiles the C object
2. assembles the entry object
3. links them with the Edu65xx ROM layout
4. emits a 16 KiB image
5. boots it in the Edu65xx emulator
6. requires `memory[$0201] == 42`
7. requires the CPU to reach `STP`

So the lesson is tested at the machine-code boundary, not only by checking that compilation succeeded.

## Exercise

Reverse the direction.

Write a C function that calls a handwritten assembly function.

Before implementing it, answer:

- where does the argument arrive?
- where must the result be placed?
- which registers/flags may the assembly routine destroy?
- how will you prove the answer from compiler output rather than memory?
