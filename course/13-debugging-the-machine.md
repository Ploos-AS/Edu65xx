# 13 — Debugging the machine

A debugger is most useful when it exposes the same machine model you have already learned.

Edu65xx therefore does not begin M7 with a source-level IDE. It begins with four facts:

- the program counter
- registers and flags
- memory
- bus accesses

## Step

A single debugger step executes one machine instruction.

After the step, inspect:

```text
PC
A X Y
SP
P
bus trace
```

The debugger is not replacing the CPU. It calls the same deterministic machine-step API used by the emulator tests.

## Breakpoint

A PC breakpoint asks:

> Stop before executing the instruction at this address.

Example:

```text
break $C020
run
```

If the debugger stops with PC=`$C020`, the instruction at `$C020` has not executed yet.

That detail matters when you are debugging a store, stack operation or subroutine call.

## Watchpoint

A watchpoint asks a different question:

> Stop when an instruction actually accesses this address.

Edu65xx distinguishes:

- read watchpoint
- write watchpoint

The debugger checks the CPU's bus-access trace after the instruction.

For a write watchpoint at `$0010`:

```asm
LDA #$42
STA $0010
```

the stop happens after `STA` has completed.

You should therefore expect:

```text
memory[$0010] = $42
PC = instruction after STA
stop reason = WATCH_WRITE
address = $0010
data = $42
```

## Why bus watchpoints?

A variable name exists in source code.

The processor sees an address.

A bus watchpoint deliberately connects those two views.

When C later stores a variable, you can watch the generated machine code access the variable's address without giving the emulator any knowledge of the C language.

## Bounded run

`run` must not mean “execute forever and hope”.

The debugger core accepts a maximum number of machine steps. If no breakpoint or watchpoint fires, it returns `STEP_LIMIT`.

This makes debugger behavior deterministic and testable in CI.

## Lab — predict the stop

Program:

```asm
        LDA #$42
        STA $0010
        LDA $0010
        NOP
```

Before running it, predict the result of:

1. breakpoint at `$C002`
2. write watchpoint at `$0010`
3. read watchpoint at `$0010`
4. run with a two-step limit

For each case write:

```text
stop reason:
PC:
A:
memory[$0010]:
last bus access:
```

Then compare your prediction with the debugger.

## Extension

Add one debugger feature without changing the CPU core.

Possible exercises:

- breakpoint removal
- breakpoint listing
- memory dump
- register formatting
- disassembly of the next instruction
- range watchpoint

The design rule is important:

> debugger policy belongs around the machine; CPU semantics stay in the CPU.
