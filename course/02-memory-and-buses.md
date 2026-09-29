# M2 — Memory and buses

This lesson connects 65C02 instructions to the physical signals and memory locations they use.

## 1. One address space

The W65C02S has a 16-bit address bus, so it can address 65,536 byte locations:

```text
$0000 -------------------
       RAM
$7FFF -------------------
$8000
       reserved / I/O
$DFFF -------------------
$E000
       ROM
$FFFF -------------------
```

The exact decoding can evolve later, but the important idea is that RAM, ROM and I/O all appear as addresses to the CPU.

## 2. The buses

The CPU exposes:

- `A0..A15` — the address bus
- `D0..D7` — the data bus
- `R/W` — read or write direction
- clock and control signals

A memory read can be thought of as:

```text
CPU places address on A0..A15
        ↓
address decoder selects a device
        ↓
device places a byte on D0..D7
        ↓
CPU reads the byte
```

A write reverses the data direction.

## 3. Reset vector

After reset, the CPU obtains its starting address from:

- `$FFFC` — low byte
- `$FFFD` — high byte

If those bytes contain `$00` and `$80`, execution begins at `$8000`.

The Edu65xx simulator models these reads explicitly so they appear in the bus trace.

## 4. Zero page

Addresses `$0000..$00FF` form the zero page.

Compare:

```asm
lda $10
```

with:

```asm
lda $2345
```

The first uses a one-byte address operand. The second uses a two-byte address operand. Zero-page addressing therefore makes many instructions smaller and often faster.

In the simulator:

```text
A5 10       LDA $10
AD 45 23    LDA $2345
```

This is a direct way to see how an addressing mode changes the machine code.

## 5. The stack

The 6502-family hardware stack always lives in page 1:

```text
$0100..$01FF
```

The stack pointer is only eight bits wide. Its effective address is therefore:

```text
$0100 | SP
```

For example, after reset Edu65xx initializes `SP=$FD`. A `PHA` writes the accumulator to `$01FD`, then decrements SP.

A later `PLA` increments SP and reads the value back.

Try this sequence:

```text
A9 33 48 A9 00 68
```

which means:

```asm
lda #$33
pha
lda #$00
pla
```

After `PLA`, the accumulator contains `$33` again.

## 6. Bus trace

The simulator CLI prints each memory access for every instruction.

For:

```asm
lda #$5A
sta $10
```

the interesting accesses conceptually look like:

```text
R $0000  $A9    opcode fetch
R $0001  $5A    immediate operand

R $0002  $85    opcode fetch
R $0003  $10    zero-page address
W $0010  $5A    data write
```

This is the bridge between software and the physical computer: the instruction is not abstract. It becomes observable bus activity.

## 7. Exercise — predict before running

Before using the simulator, predict the bus accesses for:

```asm
lda #$42
sta $2345
lda $2345
```

Machine code:

```text
A9 42 8D 45 23 AD 45 23
```

Then run the simulator and compare your prediction with the trace.

## 8. Exercise — stack tracing

Run:

```text
A9 7F 48 A9 00 68
```

Record:

- `A` before and after each instruction
- `SP` before and after each instruction
- the address written by `PHA`
- the address read by `PLA`

Explain why both accesses are in page `$01xx`.

## 9. What this prepares us for

The same read/write mechanism will later be used for memory-mapped peripherals. To the CPU, a VIA register can look like another address in the same 64 KiB address space.

That is the next important step toward a complete Edu65xx computer.
