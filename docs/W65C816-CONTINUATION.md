# W65C816 continuation design

Status: M7 design baseline. This document defines an optional continuation after the W65C02 course. It does **not** change the primary Edu65xx machine.

Primary reference: Western Design Center W65C816S datasheet, March 13, 2024.

## Design rule

Do not turn the readable W65C02 core into a conditional maze.

The continuation should use a separate W65C816 CPU state/execution implementation while reusing concepts and host-side infrastructure where that remains honest:

- deterministic machine stepping
- device boundaries
- debugger concepts
- ROM qualification
- linker/symbol inspection
- course style and tests

The W65C02 remains the machine students learn first.

## Why the 816 is a continuation

The W65C816S starts after reset in Emulation mode (E=1). WDC documents Emulation mode as software-compatible with W65C02 code and fixes M=X=1 in that mode.

That gives the course a natural bridge:

```text
W65C02 mental model
 -> W65C816 reset in emulation mode
 -> observe what is familiar
 -> XCE
 -> native mode
 -> widen registers deliberately
 -> introduce banks and 24-bit addresses
```

Do not begin by presenting a 16 MiB flat address space and dozens of new modes at once.

## Phase 816-0 — compatibility bridge

Goal: boot a small, explicitly qualified Edu65xx-compatible Bank 0 ROM in Emulation mode.

Minimum CPU state adds:

```text
E    emulation flag
PBR  program bank
DBR  data bank
D    16-bit direct register
S    16-bit stack pointer
A    16-bit accumulator storage
X/Y  16-bit index storage
```

The visible widths of A, X and Y depend on mode/status.

Reset baseline from the WDC datasheet:

- E = 1
- PBR = 0
- DBR = 0
- stack high byte = $01
- RESET vector fetched from Bank 0 $FFFC/$FFFD
- M and X behave as 1 in Emulation mode

Qualification must test these facts directly.

Compatibility is a test target, not an excuse to alias the existing W65C02 implementation.

## Phase 816-1 — entering native mode

Teach `XCE` as the explicit boundary.

First native-mode lab:

1. establish known Carry state
2. execute `XCE`
3. prove E changed
4. use `REP`/`SEP` to control M and X
5. demonstrate an 8-bit accumulator operation
6. demonstrate a 16-bit accumulator operation
7. inspect instruction bytes and PC movement

The decoder must know operand width from CPU state. An immediate instruction cannot always be decoded correctly from opcode alone.

That is a major conceptual difference from the first Edu65xx core.

## Phase 816-2 — banks and 24-bit addresses

The W65C816S has a 24-bit address model and an 8-bit external data bus.

Teach three addresses separately:

```text
program address = PBR : PC
data address    = DBR : effective-address
Bank 0 address = 00 : effective-address
```

The emulator should use an explicit 24-bit address type represented in a wider host integer, for example:

```c
uint32_t address; /* invariant: address <= 0x00FFFFFF */
```

Do not silently truncate addresses through `uint16_t`.

Initial virtual memory size may be sparse or allocated as 16 MiB, but the architectural address must always remain 24-bit.

## Phase 816-3 — control flow across banks

Add and qualify:

- JSL
- RTL
- JML
- long addressing
- PBR changes
- native interrupt/return state

A useful lab is:

```text
Bank 00 reset code
 -> JSL Bank 01 function
 -> function changes a value
 -> RTL to Bank 00
```

The debugger must display banked PC as six hex digits:

```text
PC=01:C234
```

Breakpoints become 24-bit addresses.

## Phase 816-4 — Direct Register and stack-relative code

Only after banks are understood:

- move Direct Register away from zero
- compare zero-page thinking with direct-page thinking
- native 16-bit stack pointer
- stack-relative addressing
- stack-relative indirect indexed addressing

This is where relocatable/re-entrant programming becomes a concrete topic rather than a feature list.

## Phase 816-5 — block moves

Teach MVN/MVP after X/Y width and banks are already familiar.

The lab should move a visible byte pattern between banks and inspect:

- source bank
- destination bank
- X
- Y
- accumulator/count behavior
- final memory

Do not make block moves the student's first exposure to banked memory.

## Interrupt model

Emulation and Native mode do not share one vector table.

The emulator must model the vector choice explicitly.

Emulation-mode vectors include the familiar RESET at Bank 0 $FFFC/$FFFD and IRQ/BRK at $FFFE/$FFFF.

Native mode has distinct locations for COP, BRK, ABORT, NMI and IRQ in the $FFE4-$FFEF region.

Tests must cover vector selection by mode and RTI restoration.

## Machine and device strategy

Do not immediately redesign VIA or serial devices for 24-bit space.

First continuation machine:

```text
Bank 00
  $0000-$7FFF  RAM compatibility window
  $8000-$BFFF  Edu65xx I/O compatibility window
  $C000-$FFFF  boot ROM compatibility window

other banks
  continuation RAM/ROM regions defined by the specific lab
```

This preserves a recognizable starting point without pretending the old 64 KiB map defines the whole W65C816 machine.

A later 816 machine map should be its own documented contract.

## Physical hardware is a separate milestone

Do not treat the W65C02 Rev A breadboard as a drop-in W65C816 hardware design.

The W65C816S exposes a bank address multiplexed on the data/bank pins during the first half of a bus cycle. A physical 24-bit system therefore needs explicit bank-address handling/latching.

VDA and VPA also matter for qualifying valid memory cycles.

Any physical 816 board gets:

- its own schematic review
- its own address-latch design
- its own memory map
- its own analyzer plan
- its own qualification ROMs

No W65C816 PCB work belongs in the W65C02 Rev A gate.

## Toolchain gate

Do not assume the current `mosw65c02` LLVM-MOS path automatically provides the desired W65C816 ABI or code generation.

Before a C-on-816 milestone:

1. select a compiler/toolchain with explicit W65C816 support
2. pin its version
3. compile tiny ABI probes
4. inspect generated assembly
5. document register widths and calling convention
6. only then add mixed C/assembly labs

Assembly-first remains the default.

## Proposed repository layout

```text
simulator/
  cpu.c                 W65C02 stays focused

w65c816/
  cpu816.h
  cpu816.c
  machine816.h
  machine816.c
  test_reset.c
  test_modes.c
  test_banks.c
  test_interrupts.c

course/
  19-w65c816-bridge.md
  20-w65c816-native-mode.md
  21-w65c816-banks.md
  ...
```

Do not create these implementation files until the continuation becomes an active milestone.

## Qualification ladder

A future implementation is not complete until these gates are independently green:

- reset/emulation state
- W65C02-compatible teaching ROM subset
- XCE and M/X width transitions
- 16-bit arithmetic/data paths
- 24-bit effective addresses
- PBR/DBR behavior
- JSL/RTL/JML
- Direct Register
- native stack behavior
- native/emulation interrupt vectors
- MVN/MVP
- debugger 24-bit addresses
- external functional-test corpus where licensing and provenance permit

## Non-goals for the first continuation

- cycle-accurate PHI2 emulation
- SNES emulation
- Apple IIgs emulation
- replacing the W65C02 course
- sharing one giant opcode switch between 02 and 816
- designing physical 816 hardware before the software model is qualified

The purpose is to teach the architectural step from a clean 8-bit 65C02 machine to the 8/16-bit W65C816 without losing the causal model students built earlier.
