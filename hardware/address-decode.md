# M6 address decoder

This document turns the canonical Edu65xx map into explicit breadboard logic.

## Source-of-truth equations

```text
RAM      $0000-$7FFF
VIA      $8000-$800F
SERIAL   $8010-$8011   reserved in physical M6
ROM      $C000-$FFFF
```

### RAM

For an active-low SRAM chip select:

```text
/RAM_CS = A15
```

No gate is required.

### ROM

For an active-low ROM chip select:

```text
/ROM_CS = NAND(A15, A14)
```

The selected AT28C256 is 32 KiB, while Edu65xx exposes 16 KiB ROM. Connect CPU A0..A13 to EEPROM A0..A13 and hold EEPROM A14 at a defined bank-select level. Rev A uses the lower bank.

EEPROM write enable remains inactive during normal execution. Output enable must be controlled so the EEPROM does not drive D0..D7 during CPU write cycles.

### VIA

The W65C22S must be selected only at `$8000-$800F`:

```text
VIA_SELECT =
    A15
  & !A14
  & !A13
  & !A12
  & !A11
  & !A10
  & !A9
  & !A8
  & !A7
  & !A6
  & !A5
  & !A4
```

A3..A0 then select the VIA's 16 internal registers.

## Breadboard implementation candidate

A transparent discrete implementation is preferred for Rev A:

- 74HC138: decode A15..A13 and produce an active-low output for the `100` region
- 74HC688: compare A11..A4 with `00000000`
- A12 participates explicitly in the final select
- 74HC32 OR gates: combine the active-low region result, A12 and the comparator's active-low equality result into one active-low `/VIA_CS`
- W65C22S CS1 tied high; CS2B driven by `/VIA_CS`

For the active-low signals:

```text
/VIA_CS = REGION_100_B OR A12 OR A11_A4_NOT_EQUAL_B
```

where each term is low only for the desired condition. The result is low only for addresses `$8000-$800F`.

The exact 74HC138 input ordering and pin numbers must be copied from the chosen manufacturer's datasheet when the schematic is drawn; the Boolean address equation above is authoritative.

## Decode truth examples

| Address | RAM | VIA | ROM |
|---|---:|---:|---:|
| `$0000` | yes | no | no |
| `$7FFF` | yes | no | no |
| `$8000` | no | yes | no |
| `$800F` | no | yes | no |
| `$8010` | no | no | no |
| `$BFFF` | no | no | no |
| `$C000` | no | no | yes |
| `$FFFF` | no | no | yes |

## Qualification

Before installing memory or VIA, exercise address inputs or run the CPU slowly and verify that no two device-select outputs are simultaneously active.

A logic-analyzer capture of `/RAM_CS`, `/ROM_CS` and `/VIA_CS` becomes part of M6 hardware evidence.
