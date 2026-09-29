# M3 — Timer 1 and interrupts

After GPIO, the VIA introduces an event that can demand CPU attention without polling.

## Timer path

```text
Timer 1 counts down
      ↓
T1 flag in IFR becomes 1
      ↓
matching bit in IER is enabled
      ↓
VIA asserts IRQ
      ↓
65C02 notices IRQ between instructions
      ↓
PC and status are pushed to page 1
      ↓
I flag is set
      ↓
CPU reads $FFFE/$FFFF
      ↓
interrupt service routine
      ↓
RTI restores status and PC
```

The simulator models this path deliberately rather than hiding it behind a callback.

## VIA registers used

```text
$8004  T1 counter/latch low
$8005  T1 counter/latch high; starts timer
$800D  IFR — interrupt flags
$800E  IER — interrupt enables
```

For IER, bit 7 selects set/clear behavior and bit 6 controls Timer 1 interrupt enable.

## CPU instructions

M3 adds `CLI`, `SEI` and `RTI`. IRQ is accepted only when the CPU I flag is clear.

The IRQ vector is:

```text
$FFFE low byte
$FFFF high byte
```

## Important teaching distinction

The current simulator is functionally educational, not yet cycle-accurate. `edu65xx_via_tick()` advances the timer explicitly. Later emulator work can tie peripheral timing to CPU cycles. Keeping these stages separate makes the interrupt mechanism easier to understand first.

## Exercise

Set an IRQ vector to an ISR in ROM. Load Timer 1, enable its interrupt, clear the CPU I flag, tick the timer to zero and predict:

1. the VIA IFR value,
2. the stack addresses written during IRQ entry,
3. the new PC,
4. what `RTI` restores.

Then inspect the bus trace.
