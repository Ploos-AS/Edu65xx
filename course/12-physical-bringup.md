# 12 — Bringing up a computer that does not work yet

A new computer should be assumed **not to work**.

That is not pessimism. It is a debugging strategy.

If CPU, RAM, ROM, VIA, interrupts and application software are all introduced at once, a single wrong wire can produce almost any symptom. M6 therefore brings Edu65xx up one layer at a time.

## The rule

At every stage ask one question that the hardware can answer.

```text
power?
  |
clock/reset?
  |
RESET vector?
  |
ROM instructions?
  |
VIA write?
  |
RAM?
  |
interrupt?
```

Do not move upward while the lower layer is uncertain.

## Stage 0 — electricity before software

Remove the socketed ICs.

Measure:

- +5 V at every supply socket
- ground continuity
- PHI2
- RESET

This stage has no CPU program because a CPU program cannot diagnose missing power.

## Stage 1 — can the CPU fetch ROM?

Program `via-blink.bin`.

The first software question is not “does the LED blink?”

It is:

> Does RESET cause the processor to read `$FFFC/$FFFD` and then fetch at `$C000`?

If the answer is no, an LED tells you nothing useful.

## Stage 2 — one output pin

Once ROM fetch is proven, install the VIA.

The first program writes:

```text
$8002 <- $01
$8000 <- $01
...
$8000 <- $00
```

Predict the address bus, data bus, RWB and VIA select for each write.

Then measure them.

Only after the bus transaction is correct should you debug the LED circuit itself.

## Stage 3 — RAM smoke test

The smoke ROM touches deliberately chosen addresses around useful boundaries:

- zero page
- hardware stack page
- low RAM
- middle RAM
- top of RAM

PB0 means PASS and PB1 means FAIL.

This test is quick. It is intended to answer “is the RAM path basically alive?”

## Stage 4 — destructive full RAM test

The full test uses complementary patterns.

Why `$55` and `$AA`?

In binary:

```text
$55 = 01010101
$AA = 10101010
```

Every data bit must therefore store both zero and one across the two passes.

The program tests `$0002-$7FFF` first while `$0000/$0001` hold its indirect pointer. When the pointer is no longer needed, those final two bytes are tested too.

The program intentionally avoids subroutines during the memory test. A broken stack-page should not be able to hide behind a stack-dependent test routine.

A passing memory pattern test is useful evidence, but it does not prove every possible analog/timing failure. That is why the logic analyzer remains part of the course.

## Stage 5 — interrupt as a bus story

The IRQ ROM turns Timer 1 into an observable chain:

```text
Timer 1
  -> IFR/IER
  -> IRQB low
  -> CPU interrupt entry
  -> stack writes
  -> $FFFE/$FFFF
  -> ISR
  -> acknowledge
  -> RTI
```

The LED changing inside the ISR is convenient, but it is not the main evidence.

Capture the stack writes and vector reads.

## Emulator before breadboard

Each physical ROM image is executed in the Edu65xx emulator in CI before it reaches an EEPROM.

This does **not** prove that the breadboard works.

It removes one class of uncertainty:

> The exact ROM bytes have already demonstrated the intended architectural behavior in the functional machine model.

If the physical machine disagrees, compare the measured bus with the emulator's expected architectural accesses.

## Debugging worksheet

For any failure write four lines:

```text
Prediction:
Observation:
First divergence:
Next smallest experiment:
```

Avoid changing several wires at once. A repair that changes five things may make the machine work without teaching you which assumption was wrong.

## Milestone evidence

M6 is complete only from real hardware evidence.

CI can qualify:

- ROM bytes
- CPU semantics
- memory-map equations
- expected device transactions

CI cannot qualify:

- supply integrity
- breadboard contact
- oscillator waveform
- RESET waveform
- propagation on the real wiring
- actual SRAM/EEPROM devices
- physical VIA output
- IRQ electrical behavior

That boundary is part of the lesson.
