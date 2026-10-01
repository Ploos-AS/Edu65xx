# M6 physical qualification record

Copy this file into a dated build directory when the first breadboard exists, for example:

```text
hardware/evidence/2026-xx-xx-rev-a-01/
```

Do not mark a measurement PASS until it was observed on physical hardware.

## Build identity

- Build ID:
- Date:
- Builder:
- Edu65xx commit:
- Bring-up ROM SHA-256:
- CPU marking:
- VIA marking:
- SRAM marking:
- EEPROM marking:
- Oscillator marking:
- Reset supervisor marking:
- Supply:

## Pre-power

- [ ] ICs removed
- [ ] VDD/VCC continuity checked
- [ ] GND/VSS continuity checked
- [ ] no VDD-to-GND short
- [ ] EEPROM WE# high
- [ ] EEPROM A14 low
- [ ] unused HC inputs tied
- [ ] DS1813 orientation checked from bottom-view drawing

Evidence/notes:

## Power, clock and reset

Measured 5 V rail:

Measured PHI2 frequency:

RESET low duration after power-on:

- [ ] TP_PHI2 PASS
- [ ] TP_RESB manual button PASS
- [ ] power-on RESET PASS

Capture filenames:

## RESET vector and ROM fetch

Expected:

```text
read $FFFC -> $00
read $FFFD -> $C0
opcode fetch begins at $C000
```

Observed:

- [ ] RESET vector PASS
- [ ] first ROM fetch PASS

Capture filenames:

## RAM

Test ROM/sequence:

Observed addresses/data:

- [ ] RAM write PASS
- [ ] RAM read-back PASS
- [ ] no ROM/VIA select overlap PASS

Capture filenames:

## VIA GPIO

Expected first program behavior:

```text
write $01 -> $8002 DDRB
write $01 -> $8000 ORB
...
write $00 -> $8000 ORB
```

Observed:

- [ ] VIA select only at $8000-$800F
- [ ] DDRB write PASS
- [ ] ORB write PASS
- [ ] PB0 physical high PASS
- [ ] PB0 physical low PASS

Capture filenames:

## IRQ

Complete only after the IRQ ROM/lab is installed.

- [ ] VIA IRQB assertion observed
- [ ] CPU stack writes observed
- [ ] $FFFE/$FFFF vector reads observed
- [ ] ISR fetch observed
- [ ] RTI return observed

Capture filenames:

## Final result

- [ ] M6 breadboard hardware PASS

Open issues:

Conclusion:
