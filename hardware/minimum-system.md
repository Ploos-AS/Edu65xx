# Edu65xx minimum system

This document defines the canonical physical Edu65xx memory map. The goal is not minimum chip count at any cost, but a machine whose important signals remain visible.

## Canonical memory map

```text
$0000-$7FFF   RAM       32 KiB
$8000-$BFFF   I/O       16 KiB window
$C000-$FFFF   ROM       16 KiB
```

The first peripheral occupies:

```text
$8000-$800F   W65C22 VIA
```

Important fixed CPU locations remain:

```text
$0000-$00FF   zero page
$0100-$01FF   hardware stack
$FFFA-$FFFB   NMI vector
$FFFC-$FFFD   RESET vector
$FFFE-$FFFF   IRQ/BRK vector
```

This map is the shared contract for course material, simulator, emulator, breadboard computer and future PCB.

## Core parts

- W65C02S CPU, DIP preferred
- static RAM
- EEPROM/ROM
- address decoding logic
- slow/manual and normal clock options
- reset circuit
- decoupling at every IC
- exposed address, data and control buses
- W65C22 VIA from M3 onward

## Why this map?

RAM contains zero page and stack. I/O gets a large, obvious region that is easy to decode and inspect. ROM occupies the top of memory, where the CPU vectors live. Program examples intended to represent ROM begin at `$C000`.

## Signals to expose

Expose `A0-A15`, `D0-D7`, `RWB`, `PHI2`, `RESB`, `IRQB`, `NMIB` and `SYNC`. `SYNC` is especially useful because it identifies opcode fetches on the physical machine.

## Learning goal

A student should be able to follow one operation all the way from an assembly instruction to an address, an address-decoder decision, a selected device and a physical bus transaction.
