# M6 breadboard computer

This is the first physical Edu65xx target. It is deliberately conservative: socketed through-hole parts, visible buses and simple decode logic.

The first hardware milestone is **not** the serial monitor. It is:

> RESET -> ROM -> W65C02S -> W65C22S -> observable GPIO

The teaching serial device at `$8010-$8011` remains an emulator contract until a physical UART is selected and documented.

## Canonical address map

| Range | Physical M6 use |
|---|---|
| `$0000-$7FFF` | 32 KiB SRAM |
| `$8000-$800F` | W65C22S VIA |
| `$8010-$8011` | reserved for future physical serial implementation |
| `$8012-$BFFF` | unmapped/reserved |
| `$C000-$FFFF` | 16 KiB ROM/EEPROM |

Do not make the VIA mirror through the whole I/O window. The physical decoder should preserve the same address contract used by the simulator and emulator.

## Minimum parts

Core:

- W65C02S, 40-pin DIP
- W65C22S, 40-pin DIP
- 32 KiB x 8 SRAM, e.g. 62256-compatible device
- 16 KiB x 8 EEPROM/ROM, or a larger device wired so that the visible CPU window is exactly 16 KiB
- CMOS 74HC/HCT-family decode/buffer logic as required by the final schematic
- clock source suitable for slow bring-up
- reset circuit plus manual RESET button
- 5 V regulated supply for the initial all-5-V build
- 100 nF local decoupling capacitor at every IC, plus bulk decoupling at power entry
- LED + resistor for a VIA output test
- sockets, headers and labelled test points

The exact memory and logic part numbers belong in the BOM only after their pinouts and electrical compatibility have been checked against their manufacturer datasheets.

## CPU bus

Expose these groups as named nets:

```text
A0..A15
D0..D7
RWB
PHI2
RESB
IRQB
NMIB
RDY
SYNC
VPB
BE
VDD
VSS
```

W65C02S has a 16-bit address bus and 8-bit data bus. Its RESET, IRQ and NMI vectors are at `$FFFC`, `$FFFE` and `$FFFA` respectively.

## Memory connections

### SRAM

CPU `A0..A14` -> SRAM address inputs.

CPU `D0..D7` <-> SRAM data inputs/outputs.

The RAM occupies exactly `$0000-$7FFF`, so its active-low select is logically:

```text
/RAM_CS = A15
```

When `A15=0`, RAM is selected.

Use `RWB` and the SRAM's output/write controls according to the selected SRAM datasheet.

### ROM

CPU `A0..A13` -> 16 KiB ROM address inputs.

ROM is selected only when `A15=1` and `A14=1`:

```text
/ROM_CS = NOT(A15 AND A14)
```

This gives `$C000-$FFFF`.

The ROM must never drive the data bus outside that window.

## VIA

The W65C22S is selected only for `$8000-$800F`.

```text
VIA selected when:
A15 = 1
A14..A4 = 0

A3..A0 -> VIA RS3..RS0
D7..D0 <-> VIA D7..D0
RWB      -> VIA RWB
PHI2     -> VIA PHI2
RESB     -> VIA RESB
IRQB     -> CPU IRQB
```

The final schematic may implement this equation with discrete decode logic, but the equation is the architectural source of truth.

WDC documents that PHI2 controls data transfers for the W65C22S and that RS0-RS3 select its 16 internal registers.

## Inputs that must not float

Before power-up, every CPU control input must have an intentional level or an intentional driving circuit.

For W65C02S, unused input-only control pins are held high. Rev A therefore uses:

- `BE`: high
- `SOB`: high
- `NMIB`: high, with the later NMI experiment able to create a falling edge
- `IRQB`: high until the VIA IRQ connection is installed
- `RDY`: external pull-up; WDC specifically notes that current W65C02S devices no longer provide an active pull-up and WAI uses this bidirectional pin
- `RESB`: reset circuit; it must remain low for at least two clock cycles after VDD reaches its operating level

The exact resistor values remain a schematic calculation, not a copied folklore value.

## First ROM

The first physical ROM should be smaller in ambition than the emulator monitor:

1. disable/initialize CPU state
2. configure one VIA port bit as output
3. toggle that bit in a visible loop

Only after this passes do we add timer IRQ, input switches and eventually physical serial.

## Bring-up order

### 1. Power only

With ICs removed from sockets:

- verify supply polarity
- verify VDD at every socket
- verify ground continuity
- verify no VDD-to-VSS short

### 2. Clock and reset

Install CPU only.

Verify:

- PHI2 is present
- RESET is asserted and released cleanly
- control inputs have defined levels

### 3. Address activity

Install ROM and decode logic.

After RESET, verify that the CPU fetches the RESET vector at `$FFFC-$FFFD` and then accesses the ROM address named by that vector.

### 4. RAM

Install SRAM and run a ROM memory test before relying on a stack-heavy program.

### 5. VIA

Install W65C22S.

Run the GPIO ROM and verify both:

- bus write to the expected VIA register address
- physical PA/PB pin transition

### 6. Interrupt

Only after polling/GPIO works, connect and qualify VIA IRQ behavior.

## Hardware PASS evidence

M6 breadboard is not PASS until the repository contains measured evidence from a real build:

- supply voltage
- clock observation
- RESET observation
- RESET-vector bus trace
- ROM fetch trace
- RAM read/write result
- VIA register write trace
- GPIO pin observation
- IRQ/vector trace

Photographs are useful evidence, but measurements and captured traces are the qualification artifacts.
