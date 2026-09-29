# M3 — Memory-mapped I/O and the VIA

M3 turns memory accesses into interaction with the outside world.

## 1. I/O is an address

Edu65xx reserves the first M3 device window at:

```text
$8000..$800F   VIA
```

The CPU still performs ordinary reads and writes. Address decoding decides that these addresses belong to an I/O device rather than RAM.

## 2. First VIA registers

The teaching model begins with four W65C22-style registers:

```text
$8000  ORB   Port B data
$8001  ORA   Port A data
$8002  DDRB  Port B data-direction register
$8003  DDRA  Port A data-direction register
```

A DDR bit of 1 makes the corresponding pin an output. A DDR bit of 0 makes it an input.

## 3. ASM: make an LED output

```asm
lda #$01
sta $8002      ; PB0 is output

lda #$01
sta $8000      ; PB0 high
```

Machine code:

```text
A9 01 8D 02 80 A9 01 8D 00 80
```

The important observation is that `STA $8000` is electrically different from `STA $2000`, but from the CPU's point of view both are writes to an address.

## 4. Input example

To make PB1 an input, leave bit 1 of DDRB clear. An external switch can then determine the value read through ORB.

This gives us the first complete path:

```text
assembly instruction
      ↓
machine code
      ↓
CPU bus write/read
      ↓
address decode
      ↓
VIA register
      ↓
physical pin
      ↓
LED or switch
```

## 5. Study the C model

The simulator implementation is intentionally small. Read `simulator/via.c` and locate:

- register decoding
- DDR handling
- output latches
- external input values

The model is not intended to hide the VIA behind a framework. Students should be able to understand it.

## 6. Exercise

Predict the bus accesses and final PB pin values for:

```asm
lda #$0F
sta $8002
lda #$05
sta $8000
```

Then compare the prediction with the simulator as the machine model is extended through M3.

## Next

Timers and interrupt registers will extend this same device rather than introduce a different abstraction.
