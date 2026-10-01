# 18 — Extending the emulator without changing the CPU

An emulator extension does not have to mean adding another opcode or inventing another memory-mapped device.

The first M7 extension models something outside the computer:

```text
switches -> VIA input pins -> CPU
CPU -> VIA output pins -> LEDs
```

## The boundary

Edu65xx already models a W65C22-like teaching VIA.

The virtual front panel therefore does not modify CPU semantics.

It only connects host-side state to the VIA's external pins.

The API exposes:

```text
set_switches_a
set_switches_b
leds_a
leds_b
```

This is deliberately a small adapter around the machine.

## DDR decides who drives a pin

Suppose Port B has:

```text
DDRB = $0F
ORB  = $05
switches = $A0
```

Bits 0–3 are outputs.

Bits 4–7 are inputs.

Reading Port B therefore gives:

```text
output bits:  $05
input bits:   $A0
combined:     $A5
```

The LEDs connected to output pins see only the driven output bits:

```text
$05
```

This is why an emulator should not treat a GPIO register as merely a byte of RAM.

## The qualification program

The test boots actual W65C02 code that:

1. writes `$0F` to DDRB
2. writes `$05` to ORB
3. reads Port B
4. stores the read value at `$0200`
5. executes `STP`

Before execution, the host front panel sets the input switches to `$A0`.

The required result is:

```text
RAM[$0200] = $A5
LEDs B     = $05
```

## Why this belongs outside the CPU

The W65C02 does not know what an LED is.

It does not know what a toggle switch is.

It only performs bus transactions.

The VIA knows about direction registers and port pins.

The front-panel adapter knows what the host means by “switch” and “LED”.

Keeping these layers separate makes each one easier to test.

## Exercise — build another environment

Use the same principle to add one host-side environment without changing `simulator/cpu.c`.

Ideas:

- eight push buttons on Port A
- an LED bar on Port B
- a scripted sequence of switch changes
- a text UI that displays VIA outputs
- a test fixture that changes an input after N machine steps

Rules:

- use existing machine/device boundaries
- do not add a fake CPU opcode
- do not bypass DDR semantics
- make the behavior deterministic enough for an automated test

## Harder extension

Model an external event over time.

For example:

```text
step 0-99:  PA0 = 0
step 100+:  PA0 = 1
```

Then write a ROM that waits for the input transition.

This introduces a central emulator concept:

> the emulated machine and its environment advance together, but they are not the same component.
