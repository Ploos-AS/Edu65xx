# Rev A physical qualification sequence

Use the smallest test that can answer the current question.

## Stage 0 — no CPU

No EEPROM image.

Prove:

- +5 V
- ground
- 1 MHz PHI2
- power-on RESET
- manual RESET
- no simultaneous device-select condition

Do not insert all ICs just because sockets are available.

## Stage 1 — RESET and ROM

Image: `via-blink.bin`

Install CPU, EEPROM and required decode/control logic.

Before installing the VIA, the first useful analyzer evidence is:

```text
$FFFC -> $00
$FFFD -> $C0
SYNC/opcode fetch at $C000
```

If this fails, do not install RAM and continue debugging at a higher level.

## Stage 2 — VIA GPIO

Image: `via-blink.bin`

Install VIA.

Expected architectural writes:

```text
$8002 <- $01    DDRB: PB0 output
$8000 <- $01    PB0 high
...
$8000 <- $00    PB0 low
```

Prove both the bus write and the physical PB0 voltage/LED behavior.

## Stage 3 — RAM smoke test

Image: `ram-smoke.bin`

Install SRAM.

The smoke image checks representative/boundary addresses first.

Indicators:

```text
PB0 = PASS
PB1 = FAIL
```

If FAIL, capture address/data/RWB/RAM_CS/OE/WE before changing wiring.

## Stage 4 — full destructive RAM test

Image: `ram-full.bin`

This image writes and verifies complementary `$55` and `$AA` patterns over `$0002-$7FFF`, then tests `$0000/$0001`.

It is destructive by design.

PASS/FAIL uses PB0/PB1 as above.

A PASS is stronger evidence than the smoke test, but the smoke test remains useful because it reaches a result faster during bring-up.

## Stage 5 — VIA Timer 1 IRQ

Image: `via-irq.bin`

Capture:

- IRQB
- PHI2
- SYNC
- RWB
- address bus
- data bus

Expected sequence:

1. Timer 1 reaches zero.
2. VIA IRQB asserts low.
3. CPU writes return state to stack page.
4. CPU reads `$FFFE/$FFFF`.
5. CPU fetches ISR at `$C01E`.
6. ISR reads `$8004` to acknowledge Timer 1.
7. ISR writes PB0 low.
8. RTI restores execution.

Do not call IRQ qualification PASS from the LED alone; the stack/vector trace is part of the evidence.

## Stage 6 — preserve evidence

For each stage record the commit, image SHA-256, instrument channel map and raw capture using `hardware/evidence-template.md`.

Only after Stages 0–5 pass on the real breadboard can the M6 physical qualification box be checked.
