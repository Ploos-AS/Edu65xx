# Rev A schematic and connectivity review

Date: 2026-10-01

Scope: breadboard Rev A only. This is not the later Trainer PCB review.

## Inputs reviewed

- `hardware/schematic-rev-a.md`
- `hardware/rev-a-connectivity.tsv`
- `hardware/rev-a-schematic.svg`
- `hardware/address-decode.md`
- `hardware/clock-reset.md`
- `hardware/BOM.md`
- primary manufacturer references listed in `hardware/DATASHEETS.md`

## Datasheet cross-checks

### W65C02S

PDIP-40 pin numbering was cross-checked against the WDC W65C02S datasheet.

Checked specifically:

- VDD pin 8 / VSS pin 21
- A0..A15
- D0..D7
- RWB pin 34
- BE pin 36
- PHI2 pin 37
- SOB pin 38
- RESB pin 40
- IRQB pin 4
- NMIB pin 6
- RDY pin 2

RDY external pull-up and reset timing assumptions remain consistent with the manufacturer guidance.

### W65C22S

PDIP-40 pin numbering was cross-checked against the WDC W65C22 datasheet.

Checked specifically:

- VSS pin 1 / VDD pin 20
- PA0..PA7 pins 2..9
- PB0..PB7 pins 10..17
- IRQB pin 21
- RWB pin 22
- CS2B pin 23 / CS1 pin 24
- PHI2 pin 25
- D7..D0 pins 26..33
- RESB pin 34
- RS3..RS0 pins 35..38
- CA2 pin 39 / CA1 pin 40

**Review correction:** W65C22S IRQB is a full/totem-pole output. The earlier design-BOM wording could be read as retaining the CPU-only IRQ pull-up after the VIA was installed. Rev A now explicitly removes that temporary pull-up when U2 is installed. One W65C22S IRQB output directly drives the W65C02S IRQB input.

### SRAM and EEPROM

AS6C62256-55PCN remains a 32K x 8, 28-pin DIP, 2.7–5.5 V SRAM candidate.

AT28C256 PDIP pinout was cross-checked, including:

- A14 pin 1 held low for lower 16 KiB bank
- CE# pin 20
- OE# pin 22
- WE# pin 27 held high during execution
- VCC pin 28

**Review correction:** the machine-readable connectivity source now records SRAM CE# directly on net A15 instead of inventing a disconnected `/RAM_CS` alias. Electrically, A15 itself is the active-low RAM select for the lower half of the address space.

### Decode logic

SN74HC138 PDIP pinout and Y4 selection were cross-checked. With A=A13, B=A14 and C=A15, Y4 is active low for `100`.

SN74HC688 PDIP pinout was cross-checked. P0..P7 compare A4..A11 against grounded Q0..Q7; P=Q# therefore goes low only when A11..A4 are zero.

Unused HC inputs are tied to defined logic levels.

## Automated connectivity review

`hardware/check_connectivity.py` checks:

- unique physical pin records
- expected package pin counts
- CPU address/data pin maps
- CPU/VIA shared control nets
- VIA register-select order
- SRAM select/read/write controls
- EEPROM bank/select/read-only controls
- HC138 region decode
- HC688 comparator mapping
- HC32 VIA-select chain
- HC00 ROM/read-control chain
- oscillator/reset connectivity
- no NC on active HC inputs
- single expected drivers for key control nets

The exhaustive address-decode test independently checks all 65,536 CPU addresses.

Both are CI gates.

## Review result

**REV A DESIGN REVIEW: PASS**

This means the documented design is internally consistent and has passed the current datasheet/connectivity review.

It does **not** mean the physical computer has passed.

Remaining M6 evidence must come from real hardware:

1. breadboard build
2. power/clock/reset measurements
3. reset-vector and ROM bus capture
4. RAM smoke/full test
5. VIA GPIO capture
6. VIA Timer 1 IRQ/stack/vector/RTI capture
7. preserved evidence and final physical qualification
