# Lesson 01 — CPU fundamentals

This lesson introduces the visible state of the W65C02S and connects assembly source, machine code and the Edu65xx simulator.

## Registers

The first registers to learn are:

- `A` — accumulator
- `X` — index register X
- `Y` — index register Y
- `PC` — program counter
- `SP` — stack pointer
- `P` — processor status register

The status register contains flags. We begin with:

- `N` — negative
- `Z` — zero
- `C` — carry

Other flags are introduced when the course needs them.

## First instruction

Assembly:

```asm
lda #$42
```

Machine code:

```text
A9 42
```

The CPU performs two important actions:

1. fetch opcode `$A9` at `PC`
2. fetch operand `$42` and place it in `A`

Afterwards the program counter has advanced by two bytes.

## Immediate addressing

The `#` means that the value is part of the instruction itself.

```asm
lda #$05
ldx #$10
ldy #$80
```

This differs from loading data from a memory address, which we introduce later.

## Flags

`LDA`, `LDX` and `LDY` update the `N` and `Z` flags.

Examples:

```asm
lda #$00   ; Z = 1, N = 0
lda #$01   ; Z = 0, N = 0
lda #$80   ; Z = 0, N = 1
```

The simulator implements this in a small helper function named `set_nz()` so students can see that CPU flags are simply state updated according to defined rules.

## Addition

```asm
lda #$05
adc #$03
```

With carry initially clear, `A` becomes `$08`.

The carry flag represents a ninth bit when an 8-bit addition overflows:

```asm
lda #$FE
adc #$02
```

The result stored in `A` is `$00`, while `C` becomes `1`.

## Single-step execution

The educational simulator exposes:

```c
int edu65xx_cpu_step(edu65xx_cpu_t *cpu);
```

One call executes exactly one instruction.

The implementation deliberately follows the basic CPU loop:

```text
fetch opcode
    ↓
decode opcode
    ↓
fetch operands
    ↓
execute
    ↓
update registers and flags
```

Read `simulator/cpu.c` while working through this lesson. The source code is part of the teaching material, not merely infrastructure.

## Exercise 1

Encode this program by hand:

```asm
lda #$07
adc #$04
```

Then place the bytes in simulator memory and single-step twice. Verify `A` and `PC` after each step.

## Exercise 2

Try these values with `LDA` and predict the `N` and `Z` flags before running the simulator:

- `$00`
- `$01`
- `$7F`
- `$80`
- `$FF`

## Exercise 3

Open `simulator/cpu.c` and locate the code for `LDA #imm`.

Identify:

1. where the opcode is fetched
2. where the operand is fetched
3. where `A` is modified
4. where `N` and `Z` are updated

The goal is to connect the documented W65C02 behaviour to a small, readable implementation in C.
