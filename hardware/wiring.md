# M6 Rev A wiring contract

This is the net-level wiring contract used to draw and review the breadboard schematic. Manufacturer datasheets remain authoritative for physical pin numbers.

## Shared buses

```text
CPU A0..A13 -> SRAM A0..A13, EEPROM A0..A13
CPU A14     -> SRAM A14
CPU A0..A3  -> VIA RS0..RS3

CPU D0..D7 <-> SRAM D0..D7
CPU D0..D7 <-> EEPROM D0..D7
CPU D0..D7 <-> VIA D0..D7

CPU RWB -> VIA RWB
CPU PHI2 -> VIA PHI2
CPU RESB -> VIA RESB
VIA IRQB -> CPU IRQB
```

EEPROM A14 is held low for Rev A so the lower 16 KiB bank appears at CPU `$C000-$FFFF`.

## Memory control

Generated nets:

```text
/RD     = NOT(RWB)
/RAM_CS = A15
/ROM_CS = NAND(A15, A14)
```

Connections:

```text
SRAM /CE <- /RAM_CS
SRAM /OE <- /RD
SRAM /WE <- RWB

EEPROM /CE <- /ROM_CS
EEPROM /OE <- /RD
EEPROM /WE <- +5 V
```

The EEPROM is therefore read-only in the running Rev A computer. Program it externally.

## VIA select

```text
REGION_100_B = 74HC138 active-low decode of A15..A13 = 100
A11_A4_EQ_B  = 74HC688 active-low equality for A11..A4 = 00000000

/VIA_CS = REGION_100_B OR A12 OR A11_A4_EQ_B
```

Connections:

```text
VIA CS1  <- +5 V
VIA CS2B <- /VIA_CS
```

This selects only `$8000-$800F`.

## Clock

```text
ECS-100AX-010 OUT -> CPU PHI2
ECS-100AX-010 OUT -> VIA PHI2
```

Add TP_PHI2.

## Reset

```text
DS1813 RSTB -> CPU RESB
DS1813 RSTB -> VIA RESB
DS1813 RSTB -> TP_RESB
RESET button -> RSTB to GND
```

## CPU control

```text
BE   -> +5 V through 10 kOhm
SOB  -> +5 V through 10 kOhm
RDY  -> +5 V through 4.7 kOhm
NMIB -> +5 V through 10 kOhm
NMI button -> NMIB to GND
```

Before the VIA is installed, CPU IRQB is held high through 10 kOhm. After the VIA is installed, its IRQB drives the net; retain only a pull arrangement that is electrically valid for the W65C22S totem-pole IRQ output.

## First LED

```text
VIA PB0 -> 1 kOhm -> LED -> GND
```

The W65C22S has no internal output current limiting; the external resistor is mandatory.

## Power

All Rev A logic is a common 5 V system.

Every IC gets a local 100 nF capacitor between VDD and VSS. Add 10 uF bulk capacitance at the power entry.

Run ground and power rails first. Do not use long daisy-chained ground jumpers for the analyzer reference.

## Review rule

Before applying power with ICs installed, verify every physical pin against the current manufacturer datasheet. This file intentionally uses signal names instead of copied pin numbers so a package substitution cannot silently inherit the wrong pinout.
