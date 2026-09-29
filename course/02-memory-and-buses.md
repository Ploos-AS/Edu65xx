# M2 — Memory and buses

M2 connects instructions to physical addresses and signals.

## Canonical Edu65xx map

```text
$0000-$7FFF   RAM
$8000-$BFFF   I/O
$C000-$FFFF   ROM
```

Inside RAM, `$0000-$00FF` is zero page and `$0100-$01FF` is the hardware stack. ROM contains the vectors at the top of the address space.

## Reset vector

The CPU reads `$FFFC` and `$FFFD` after reset. Edu65xx ROM examples start at `$C000`, so a reset vector for `$C000` contains low byte `$00` followed by high byte `$C0`.

The simulator records both reads in its bus trace.

## Address and data buses

The CPU presents an address on `A0..A15`. Address-decoding logic selects RAM, I/O or ROM. Reads return a byte on `D0..D7`; writes drive a byte toward the selected device. `RWB` distinguishes the direction.

## Zero page

```asm
lda $10
lda $2345
```

encode differently:

```text
A5 10
AD 45 23
```

The first uses a one-byte zero-page address; the second carries a full 16-bit address.

## Stack

The hardware stack always occupies `$0100-$01FF`. With `SP=$FD`, `PHA` writes to `$01FD` and then decrements SP. `PLA` increments SP before reading the value back.

## Bus trace exercise

For:

```asm
lda #$42
sta $2345
lda $2345
```

predict every opcode fetch, operand fetch and data access before comparing your answer with the simulator trace.

M3 reuses exactly the same read/write path for the VIA. That is the key idea: to the CPU, a peripheral register is another address.
