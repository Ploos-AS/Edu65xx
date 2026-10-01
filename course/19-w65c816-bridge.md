# 19 — From W65C02 to W65C816

This chapter is a bridge, not a new starting point.

The W65C02 machine remains the reference machine for the main Edu65xx course.

The W65C816 continuation asks:

> Which parts of the model still hold, and which parts must grow?

## Start where RESET starts

The W65C816S resets into Emulation mode.

That is useful pedagogically because the first state is deliberately close to the 65C02 world:

```text
E = 1
PBR = 0
DBR = 0
RESET vector = 00:FFFC
M = 1
X = 1
```

Before learning native mode, explain each of these names.

## The first new idea: mode is CPU state

On W65C02, an immediate `LDA` has an 8-bit operand.

On W65C816, operand width can depend on CPU state.

That means decoding is no longer only:

```text
opcode -> addressing mode -> operand bytes
```

It can become:

```text
opcode + E/M/X state -> operand width and behavior
```

This is why the continuation should have its own CPU model.

## XCE is the bridge

Do not hide the transition to Native mode in startup code.

Make it a lab.

Single-step the sequence and inspect E, Carry, M and X before and after `XCE`, `REP` and `SEP`.

The student should be able to answer:

- Am I in Emulation or Native mode?
- Is A operating as 8 or 16 bits?
- Are X/Y operating as 8 or 16 bits?
- How many immediate operand bytes will the next instruction consume?

## The second new idea: an address has a bank

The old machine can be drawn as:

```text
16-bit address -> one of 65536 byte locations
```

The continuation adds bank context.

Program fetches use PBR with PC.

Many data accesses use DBR with a 16-bit effective address.

Some operations are explicitly Bank 0.

Do not call all of these simply “the bank register”. They have different jobs.

## Keep Bank 0 familiar first

The first continuation ROM should boot in Bank 0 with a compatibility window resembling the existing Edu65xx map.

Only after that works should a lab deliberately cross into another bank.

A good first cross-bank experiment is:

```text
00:C000
  JSL 01:C000

01:C000
  ...
  RTL
```

Now a six-digit address has a visible reason to exist.

## Hardware comes later

The external data bus remains 8 bits, while the architectural address space grows to 24 bits.

A physical W65C816 system therefore introduces bus details that the W65C02 breadboard did not need, including the multiplexed bank address.

That is a separate hardware project phase.

Do not modify the W65C02 Rev A wiring to make the course diagram look “future proof”.

## Before implementation

Read:

```text
docs/W65C816-CONTINUATION.md
```

Then write down the state you believe a W65C816 emulator must hold.

Compare your list with the proposed state in the design document.

For every extra field, answer:

> Which instruction, addressing mode, reset rule or interrupt rule makes this state necessary?

If you cannot answer that, do not add the field yet.
