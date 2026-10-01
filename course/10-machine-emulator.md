# 10 — From CPU simulator to machine emulator

Up to this point, Edu65xx has used the CPU simulator to make individual instructions, registers, flags and bus accesses visible.

M5 adds a different idea:

> A CPU is not a computer until it is connected to memory and devices under one machine contract.

## The Edu65xx machine

The canonical machine is:

```text
$0000-$7FFF  RAM
$8000-$800F  W65C22 VIA
$8010        SERIAL_DATA
$8011        SERIAL_STATUS
$8012-$BFFF  reserved / unmapped I/O
$C000-$FFFF  ROM
```

The CPU starts by reading the RESET vector at `$FFFC-$FFFD`. The vector points into ROM at `$C000`.

That means the first instructions are not called by the host program. The emulated CPU discovers them in the same architectural way that the physical W65C02S does.

## One machine step

`edu65xx_machine_step()` connects several concepts you have already learned:

1. inspect interrupt state
2. fetch an opcode through the bus
3. decode the instruction
4. form its effective address
5. read or write RAM, ROM or a device
6. update registers and flags
7. advance functional device time

The current emulator is deterministic and instruction-oriented. Its bus trace shows functional accesses. It does **not** claim exact PHI2 cycle timing.

This distinction matters. Functional correctness answers:

- Did the correct instruction execute?
- Was the effective address correct?
- Did the correct register or memory byte change?
- Were flags correct?
- Did an interrupt use the correct vector and stack state?

Cycle accuracy additionally asks exactly *when* each bus action happens.

## The first monitor ROM

`rom/monitor/monitor.s` is the first complete Edu65xx system program.

After RESET it:

- enters a known CPU state
- initializes the stack
- writes `Edu65xx ready` to the teaching serial device
- polls `SERIAL_STATUS`
- reads input from `SERIAL_DATA`
- responds to `?` with a help message

Follow the banner one layer at a time:

```text
monitor source
    |
    v
LDA / STA / branches
    |
    v
W65C02 execution core
    |
    v
bus decode
    |
    v
$8010 SERIAL_DATA
    |
    v
terminal output
```

No host-side print function produces the banner. The bytes appear because the ROM program executes stores to the memory-mapped serial device.

## Why all 256 opcode bytes matter

The W65C02S has official instructions plus reserved opcode encodings that behave as NOPs.

A robust emulator must therefore remain synchronized for every possible opcode byte. A reserved two-byte or three-byte NOP cannot simply be treated as a one-byte NOP, because the next operand byte would incorrectly become an opcode.

Edu65xx combines:

- family-specific semantic tests
- addressing-mode tests
- flag tests
- stack/vector tests
- decimal arithmetic tests
- W65C02-specific instruction tests
- a 256-opcode dispatch sweep
- end-to-end monitor ROM execution

The sweep proves that every byte has a decode path. It does **not** replace the semantic tests.

## Lab

Start the monitor in the emulator and trace only the accesses that create the first character of the banner.

Write down:

1. the PC of the `LDA`
2. the effective address used to read the character
3. the value loaded into A
4. the PC of the `STA`
5. the address written by `STA`
6. the byte observed by the serial device

Then repeat for one iteration of the input polling loop.

## Think

Why is it useful to keep the teaching simulator and the machine emulator conceptually separate even when they currently share the same CPU implementation?

What would have to change before Edu65xx could claim exact W65C02S/W65C22S cycle timing?
