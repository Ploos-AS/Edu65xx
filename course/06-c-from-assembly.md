# M4 — C from assembly

C begins only after the machine underneath it is familiar.

The central question throughout M4 is:

> What did the compiler ask the CPU to do?

## 1. A variable is storage

C:

```c
volatile uint8_t value;
value = 42;
```

Conceptual 65C02:

```asm
lda #$2A
sta value
```

The C name is convenient for humans. At run time the CPU ultimately works with an address and bytes.

## 2. Arithmetic is instructions

C:

```c
result = a + b;
```

At machine level this requires values to be loaded, added, and stored. The exact compiler output depends on compiler, optimization level and ABI, so the course distinguishes **conceptual equivalent assembly** from **actual compiler-generated assembly**.

Never assume the compiler emitted the handwritten example: inspect its output.

## 3. Control flow becomes branches

C:

```c
if (value == 0) {
    result = 1;
}
```

A compiler must test a value and alter the program counter with conditional control flow. Loops use the same principle repeatedly.

## 4. Pointers expose the machine model

C:

```c
volatile uint8_t *serial = (volatile uint8_t *)0x8010;
*serial = 'A';
```

The important machine operation is a write of `$41` to address `$8010`. This is why memory-mapped I/O learned in M3 makes pointers much less mysterious.

## 5. Functions need a convention

A CPU knows instructions, registers, memory and a hardware stack. It does not inherently know C function arguments or local variables.

A C toolchain therefore defines an ABI/calling convention describing such things as:

- where arguments go,
- where return values go,
- which registers a function may change,
- how temporary/local storage is handled,
- how a function returns.

M4 will inspect the selected toolchain's real rules rather than inventing a generic 6502 ABI.

## Lab rule

For every important example:

1. read the C,
2. predict likely machine operations,
3. compile to assembly,
4. inspect the actual output,
5. identify loads/stores/branches/calls,
6. connect those operations to CPU state and bus activity.
