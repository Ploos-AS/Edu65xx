# Logic-analyzer channel plans

The analyzer does not need to observe every signal at once. Use staged captures that answer one question clearly.

## Capture A — reset and first fetch

Suggested channels:

| Channel | Signal |
|---|---|
| 0 | PHI2 |
| 1 | RESB |
| 2 | SYNC |
| 3 | VPB |
| 4 | RWB |
| 5 | A15 |
| 6 | A14 |
| 7 | A13 |

Purpose: establish reset release and transition into ROM space.

If the analyzer has 16+ channels, add A0..A12 or the complete address bus and decode the exact `$FFFC/$FFFD/$C000` accesses.

## Capture B — memory/device select

| Channel | Signal |
|---|---|
| 0 | PHI2 |
| 1 | RWB |
| 2 | A15 |
| 3 | A14 |
| 4 | A13 |
| 5 | /ROM_CS |
| 6 | /VIA_CS |
| 7 | /RD |

Purpose: prove mutually exclusive selection and the VIA/ROM regions.

For RAM, A15 itself is the active-low SRAM select.

## Capture C — VIA GPIO write

| Channel | Signal |
|---|---|
| 0 | PHI2 |
| 1 | RWB |
| 2 | /VIA_CS |
| 3 | A3 |
| 4 | A2 |
| 5 | A1 |
| 6 | A0 |
| 7 | PB0 |

Add D0..D7 on a second bank if available.

Expected register addresses:

- `0010` -> DDRB
- `0000` -> ORB

## Capture D — IRQ architecture

Preferred 16+ channel capture:

- PHI2
- IRQB
- VPB
- SYNC
- RWB
- A0..A7 or full A0..A15 in a wider analyzer
- selected data lines / full D0..D7 where possible

The most important evidence is not absolute timing precision. It is the architectural sequence:

```text
IRQB asserted
 -> stack writes in $01xx
 -> VPB/vector access at $FFFE/$FFFF
 -> ISR opcode fetch
 -> acknowledge
 -> RTI
```

## Grounding

Use a short analyzer ground connection near the measured logic. Do not use one long ground lead at the opposite end of the breadboard for every capture.

## Evidence naming

Suggested convention:

```text
rev-a-01-reset-vector.*
rev-a-02-via-gpio.*
rev-a-03-ram-smoke.*
rev-a-04-ram-full.*
rev-a-05-via-irq.*
```

Preserve the raw analyzer file in addition to screenshots or exported CSV.
