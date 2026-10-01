# 11 — Reading a real W65C02 bus

The emulator bus trace taught you *what* the machine accesses. A logic analyzer lets you observe the physical bus.

Do not begin by decoding everything. Begin with questions.

## Lab 1 — Find instruction fetches

Capture:

- PHI2
- SYNC
- RWB
- enough address lines to recognize the ROM region

W65C02S exposes SYNC specifically so an external observer can identify opcode-fetch cycles.

Goal:

1. reset the machine
2. locate the first ROM instruction fetch
3. compare its address with the RESET vector
4. compare the observed instruction sequence with the ROM listing

## Lab 2 — See the RESET vector

Capture the full address bus if possible.

Find accesses to:

```text
$FFFC
$FFFD
```

Record the two data bytes and reconstruct the 16-bit start address.

Then prove that subsequent instruction fetches begin at that address.

## Lab 3 — Distinguish RAM, ROM and I/O

Capture:

- A15
- A14
- RWB
- /RAM_CS
- /ROM_CS
- VIA_CS
- PHI2

Trigger the ROM program so that it performs one RAM access and one VIA write.

For each transaction predict the select signals *before* looking at the trace.

## Lab 4 — Decode one VIA write

Configure one VIA pin as an output.

Capture:

- PHI2
- RWB
- A0..A3
- VIA_CS
- D0..D7
- the selected VIA output pin

Answer:

- Which VIA register was addressed?
- What byte was on the data bus?
- When did the physical output change relative to the bus transaction?

Do not infer exact timing from the software emulator. Measure it.

## Lab 5 — IRQ to vector

After Timer 1 is working physically, capture:

- IRQB
- PHI2
- SYNC
- RWB
- address bus
- data bus

Identify:

1. IRQB assertion
2. stack writes
3. reads of `$FFFE-$FFFF`
4. first ISR opcode fetch
5. RTI return

Compare the sequence with the functional emulator trace.

## Lab 6 — Find a deliberate fault

Introduce one controlled fault at a time, for example a deliberately incorrect ROM image or a disconnected decode input while power is off.

Use the analyzer to locate the first point where observed behavior diverges from the prediction.

The objective is not merely to repair the machine. It is to learn a repeatable debugging method:

```text
predict -> capture -> compare -> isolate -> correct -> recapture
```

## Evidence

Save captures under a future hardware qualification directory with:

- board/build identifier
- ROM commit
- clock configuration
- analyzer model
- channel map
- trigger condition
- expected behavior
- observed behavior
- PASS/FAIL conclusion

Screenshots alone are not sufficient if the raw capture format can also be saved.
