# Edu65xx Emulator

The Edu65xx emulator models the complete course machine in readable C.

## M5 architecture

```text
ROM image
   |
   v
edu65xx_machine_t
   |
   +-- W65C02 CPU core
   +-- $0000-$7FFF RAM
   +-- $8000-$800F W65C22 VIA
   +-- $8010-$8011 teaching serial device
   +-- $8012-$BFFF reserved/unmapped I/O
   +-- $C000-$FFFF ROM
```

The machine layer owns deterministic system execution:

- ROM loading with bounds checking
- reset through the hardware reset vector at `$FFFC-$FFFD`
- one explicit machine step at a time
- instruction/IRQ/NMI event reporting
- deterministic functional device ticking
- tests that boot real ROM bytes from `$C000`

One machine step currently advances the functional VIA timer once. This is an emulator convention for deterministic lessons and tests, **not** a claim of exact W65C02/W65C22 PHI2 cycle timing.

## Design boundary

The emulator is not intended to win performance benchmarks. Code clarity and architectural correspondence with the physical Edu65xx computer take priority.

The CPU implementation is currently shared with the simulator. The simulator emphasizes teaching and visualization; `edu65xx_machine_t` is the system-execution boundary used by the emulator.

M5 will expand the W65C02 instruction core until ROM programs can execute as complete Edu65xx machine software.
