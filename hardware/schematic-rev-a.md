# M6 Rev A pin-numbered schematic contract

**Status:** design review baseline. This is the pin-numbered translation of `hardware/wiring.md`.

All pin numbers below are for the exact through-hole packages named in the Rev A BOM. Manufacturer datasheets remain authoritative. Re-check orientation and package before inserting an IC.

## U1 — WDC W65C02S6TPG-14, PDIP-40

| Pin | Signal | Rev A connection |
|---:|---|---|
| 1 | VPB | TP_VPB only |
| 2 | RDY | 4.7 kOhm to +5 V, TP_RDY |
| 3 | PHI1O | NC |
| 4 | IRQB | U2 pin 21, TP_IRQB |
| 5 | MLB | optional TP_MLB, otherwise NC |
| 6 | NMIB | 10 kOhm to +5 V, NMI button to GND, TP_NMIB |
| 7 | SYNC | TP_SYNC / analyzer |
| 8 | VDD | +5 V, local 100 nF |
| 9..20 | A0..A11 | address bus A0..A11 |
| 21 | VSS | GND |
| 22 | A12 | address bus A12 |
| 23 | A13 | address bus A13 |
| 24 | A14 | address bus A14 |
| 25 | A15 | address bus A15 |
| 26..33 | D7..D0 | data bus D7..D0 |
| 34 | RWB | RWB net |
| 35 | NC | NC |
| 36 | BE | 10 kOhm to +5 V |
| 37 | PHI2 | Y1 oscillator output, TP_PHI2 |
| 38 | SOB | 10 kOhm to +5 V |
| 39 | PHI2O | NC |
| 40 | RESB | U8 pin 1, U2 pin 34, TP_RESB |

## U2 — WDC W65C22S6TPG-14, PDIP-40

| Pin | Signal | Rev A connection |
|---:|---|---|
| 1 | VSS | GND |
| 2..9 | PA0..PA7 | VIA Port A header / switches |
| 10..17 | PB0..PB7 | VIA Port B header; PB0 also LED circuit |
| 18 | CB1 | VIA header / TP_CB1 |
| 19 | CB2 | VIA header / TP_CB2 |
| 20 | VDD | +5 V, local 100 nF |
| 21 | IRQB | U1 pin 4 |
| 22 | RWB | U1 pin 34 |
| 23 | CS2B | /VIA_CS from U7 pin 6 |
| 24 | CS1 | +5 V |
| 25 | PHI2 | Y1 oscillator output |
| 26..33 | D7..D0 | data bus D7..D0 |
| 34 | RESB | reset net |
| 35..38 | RS3..RS0 | A3..A0 respectively |
| 39 | CA2 | VIA header / TP_CA2 |
| 40 | CA1 | VIA header / TP_CA1 |

Thus U2 pin 38 RS0 <- A0, pin 37 RS1 <- A1, pin 36 RS2 <- A2 and pin 35 RS3 <- A3.

## U3 — Alliance AS6C62256-55PCN, PDIP-28 SRAM

| Pin | Signal | Connection |
|---:|---|---|
| 1 | A14 | A14 |
| 2 | A12 | A12 |
| 3 | A7 | A7 |
| 4 | A6 | A6 |
| 5 | A5 | A5 |
| 6 | A4 | A4 |
| 7 | A3 | A3 |
| 8 | A2 | A2 |
| 9 | A1 | A1 |
| 10 | A0 | A0 |
| 11..13 | DQ0..DQ2 | D0..D2 |
| 14 | VSS | GND |
| 15..19 | DQ3..DQ7 | D3..D7 |
| 20 | CE# | A15 = /RAM_CS |
| 21 | A10 | A10 |
| 22 | OE# | /RD from U6 pin 6 |
| 23 | A11 | A11 |
| 24 | A9 | A9 |
| 25 | A8 | A8 |
| 26 | A13 | A13 |
| 27 | WE# | RWB |
| 28 | VCC | +5 V, local 100 nF |

## U4 — Microchip AT28C256-15PU, PDIP-28 EEPROM

| Pin | Signal | Connection |
|---:|---|---|
| 1 | A14 | GND — select lower 16 KiB bank |
| 2 | A12 | A12 |
| 3 | A7 | A7 |
| 4 | A6 | A6 |
| 5 | A5 | A5 |
| 6 | A4 | A4 |
| 7 | A3 | A3 |
| 8 | A2 | A2 |
| 9 | A1 | A1 |
| 10 | A0 | A0 |
| 11..13 | I/O0..I/O2 | D0..D2 |
| 14 | GND | GND |
| 15..19 | I/O3..I/O7 | D3..D7 |
| 20 | CE# | /ROM_CS from U6 pin 3 |
| 21 | A10 | A10 |
| 22 | OE# | /RD from U6 pin 6 |
| 23 | A11 | A11 |
| 24 | A9 | A9 |
| 25 | A8 | A8 |
| 26 | A13 | A13 |
| 27 | WE# | +5 V |
| 28 | VCC | +5 V, local 100 nF |

The EEPROM is programmed externally. Rev A never asserts EEPROM WE#.

## U5 — TI SN74HC138N, PDIP-16

Use it only for the high-address `100` region.

| Pin | Signal | Connection |
|---:|---|---|
| 1 | A | CPU A13 |
| 2 | B | CPU A14 |
| 3 | C | CPU A15 |
| 4 | G2A | GND |
| 5 | G2B | GND |
| 6 | G1 | +5 V |
| 7 | Y7 | NC |
| 8 | GND | GND |
| 9..11 | Y6..Y4 | Y4 pin 11 -> U7 pin 1; Y5/Y6 NC |
| 12..15 | Y3..Y0 | NC |
| 16 | VCC | +5 V, local 100 nF |

Because the select order is C:B:A = A15:A14:A13, `$8000-$9FFF` produces active-low Y4.

## U6 — TI SN74HC00N, PDIP-14

Standard quad NAND pinout: 1A=1, 1B=2, 1Y=3; 2A=4, 2B=5, 2Y=6; GND=7; 3Y=8, 3A=9, 3B=10; 4Y=11, 4A=12, 4B=13; VCC=14.

### Gate 1 — ROM select

```text
pin 1 <- A15
pin 2 <- A14
pin 3 -> /ROM_CS
```

### Gate 2 — read strobe inverter

```text
pin 4 <- RWB
pin 5 <- RWB
pin 6 -> /RD
```

Unused gates 3 and 4: tie their inputs to GND; leave outputs unconnected.

```text
pins 9,10,12,13 -> GND
pins 8,11 -> NC
pin 7 -> GND
pin 14 -> +5 V
```

## U7 — TI SN74HC32N, PDIP-14

Standard quad OR pinout is the same gate-position layout as U6.

### Gate 1

```text
pin 1 <- U5 pin 11 (Y4 = REGION_100_B)
pin 2 <- A12
pin 3 -> VIA_PRE_B
```

### Gate 2

```text
pin 4 <- VIA_PRE_B
pin 5 <- U9 pin 19 (P=Q_B)
pin 6 -> /VIA_CS -> U2 pin 23
```

Unused gates 3 and 4: inputs to GND, outputs NC. Pin 7 GND, pin 14 +5 V.

## U9 — TI SN74HC688N, PDIP-20

Compare A11..A4 against zero.

| Bit | P pin <- address | Q pin -> |
|---:|---|---|
| 0 | pin 2 <- A4 | pin 3 -> GND |
| 1 | pin 4 <- A5 | pin 5 -> GND |
| 2 | pin 6 <- A6 | pin 7 -> GND |
| 3 | pin 8 <- A7 | pin 9 -> GND |
| 4 | pin 11 <- A8 | pin 12 -> GND |
| 5 | pin 13 <- A9 | pin 14 -> GND |
| 6 | pin 15 <- A10 | pin 16 -> GND |
| 7 | pin 17 <- A11 | pin 18 -> GND |

Other pins:

```text
pin 1  OE#  -> GND
pin 10 GND  -> GND
pin 19 P=Q# -> U7 pin 5
pin 20 VCC  -> +5 V
```

P=Q# is low only when A11..A4 are all zero.

## U8 — Analog Devices / Maxim DS1813-5+, TO-92

The manufacturer drawing numbers the TO-92 package from the **bottom view**:

```text
pin 1 RST# -> CPU RESB + VIA RESB + reset button + TP_RESB
pin 2 VCC  -> +5 V
pin 3 GND  -> GND
```

Do not translate this into breadboard orientation without checking the package drawing; bottom-view/top-view confusion here can reverse the device.

## Y1 — ECS ECS-100AX-010, full-size through-hole oscillator

Industry-standard full-size DIP oscillator footprint:

```text
pin 1  -> NC
pin 7  -> GND / case ground
pin 8  -> 1 MHz OUT -> CPU U1 pin 37 + VIA U2 pin 25 + TP_PHI2
pin 14 -> +5 V
```

Local 100 nF between +5 V and GND.

## LED1 / LED2 — bring-up evidence

```text
U2 pin 10 PB0 -> 1 kOhm -> LED1 anode -> LED1 cathode -> GND
U2 pin 11 PB1 -> 1 kOhm -> LED2 anode -> LED2 cathode -> GND
```

The simple GPIO ROM uses PB0. The RAM smoke ROM uses PB0=PASS and PB1=FAIL.

## Address-decode proof

For VIA:

```text
U5 Y4 low    <=> A15:A14:A13 = 100
A12 low
U9 P=Q# low  <=> A11..A4 = 00000000

U7 /VIA_CS low only when all three conditions are low.
A3..A0 then select one of the 16 VIA registers.
```

Therefore the only VIA addresses are `$8000-$800F`.

## Power and decoupling

U1, U2, U3, U4, U5, U6, U7 and U9 each get a 100 nF ceramic directly between their supply and ground pins. Y1 gets local 100 nF. U8 is physically close to the reset/power entry and should also receive local supply decoupling.

Add 10 uF bulk capacitance at the 5 V entry.

## Pre-power review

With all socketed ICs removed:

1. continuity-check every VCC/VDD pin to +5 V rail
2. continuity-check every GND/VSS pin to ground
3. verify no low-resistance +5 V/GND short
4. verify U4 pin 27 is high and U4 pin 1 is low
5. verify U5 enables: pins 4/5 low, pin 6 high
6. verify U9 pin 1 and all Q inputs are low
7. verify unused HC inputs are tied, never floating
8. verify DS1813 orientation from its bottom-view drawing
9. power the empty sockets and measure rails
10. install Y1/U8 first and qualify clock/reset before CPU installation
