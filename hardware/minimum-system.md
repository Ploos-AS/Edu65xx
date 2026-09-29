# Edu65xx minimum system — M0

This document defines the first physical Edu65xx computer used by the course.

The goal is not minimum chip count at any cost. The goal is a machine whose important signals and architectural ideas remain visible to the student.

## Core parts

- W65C02S CPU, DIP package preferred
- clock source suitable for slow single-step-friendly operation
- reset circuit with manual reset button
- static RAM
- EEPROM/ROM
- simple address decoding logic
- decoupling capacitors at every IC
- power rail suitable for all selected components
- headers/test points for address, data and control buses

A W65C22 VIA is deliberately postponed until the basic RAM/ROM machine is understood.

## First memory map

The M0/M1 teaching machine uses a deliberately simple map:

```text
$0000-$7FFF   RAM       32 KiB
$8000-$DFFF   reserved  24 KiB
$E000-$FFFF   ROM        8 KiB
```

Important locations inside this map include:

```text
$0000-$00FF   zero page
$0100-$01FF   hardware stack
$FFFA-$FFFB   NMI vector
$FFFC-$FFFD   RESET vector
$FFFE-$FFFF   IRQ/BRK vector
```

The large reserved area leaves room for later I/O without forcing an early redesign of the teaching machine.

## Why this map?

The student can immediately see that:

- zero page is ordinary RAM with special CPU addressing modes
- the stack occupies page 1
- ROM must occupy the top of memory because the CPU fetches vectors there
- address decoding decides which physical chip responds to an address

This connects assembly concepts directly to physical hardware.

## Bus signals to expose

At minimum, provide clearly labelled access to:

### Address bus

`A0-A15`

### Data bus

`D0-D7`

### Control / timing

- `RWB`
- `PHI2`
- `RESB`
- `IRQB`
- `NMIB`
- `SYNC`

`SYNC` is especially useful in the course because it identifies opcode-fetch cycles and makes instruction execution visible on a logic analyser.

## Clock philosophy

The course should support two clock modes:

1. **very slow/manual educational clock** for observing bus activity
2. **normal oscillator clock** for running useful programs

Do not design the first lessons around maximum clock frequency.

## Reset behaviour

The first ROM program should do as little as possible:

1. CPU reset
2. fetch RESET vector from `$FFFC/$FFFD`
3. begin executing code in ROM
4. enter a known infinite loop

This lets the student capture the complete startup sequence before RAM or I/O software complexity is introduced.

## Breadboard principles

- Prefer DIP ICs and sockets where practical.
- Keep buses visually organized.
- Use short, consistent wiring.
- Put decoupling capacitors close to each IC.
- Make important signals accessible to a multimeter, oscilloscope or logic analyser.
- Label connections in course diagrams by signal name, not only by physical pin number.

## What the student should be able to explain

After building the minimum system, the student should be able to answer:

- Why does the CPU need a clock?
- What happens during reset?
- Why must ROM appear at the top of the address space?
- What is the difference between the address bus and data bus?
- How does `RWB` distinguish reads and writes?
- How does address decoding select RAM or ROM?
- Where are zero page and the hardware stack?
- What are the reset, IRQ and NMI vectors?

## Next hardware milestone

After the minimum system is stable, add memory-mapped I/O, initially with a W65C22 VIA. That belongs to a later milestone and should not obscure the fundamentals taught by this machine.
