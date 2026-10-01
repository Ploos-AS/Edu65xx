# 17 — A multi-module assembly application

A larger assembly program should not become one larger file.

The M7 command application is split by responsibility:

```text
app-main.s
  command loop and dispatch
        |
        +--> app-serial.s
        |      serial_getc / serial_putc
        |
        +--> app-counter.s
        |      persistent RAM state at $0200
        |
        +--> app-hex.s
               byte-to-hex output
```

All four modules are assembled independently and linked into one Edu65xx ROM.

## Commands

The application accepts:

```text
+   increment
-   decrement
R   reset to zero
V   view value
?   help
```

A value response is formatted as:

```text
=2A
```

## State belongs in RAM

The counter lives at `$0200`.

The counter module owns the operations:

```text
counter_reset
counter_inc
counter_dec
counter_get
```

Other modules do not need to know how the counter is implemented.

This is already an interface, even though the language is assembly.

## I/O belongs behind an interface

The main module does not poll `$8011` itself.

It calls:

```text
serial_getc
serial_putc
```

Only the serial module knows the teaching serial register addresses.

That separation matters. A later physical UART can require different register-level code without forcing the command dispatcher to be rewritten.

## Cross-module calls are linker work

When `app-main.s` contains:

```asm
.extern counter_inc
jsr counter_inc
```

the assembler can create `main.o` without knowing the final address of `counter_inc`.

The linker resolves that symbol after all modules are available.

Inspect:

```text
build/m7-app/readelf.txt
build/m7-app/symbols.txt
build/m7-app/app.map
build/m7-app/disassembly.txt
```

Follow at least one `JSR` from source symbol to final numeric address.

## Register contracts

Assembly modules need contracts just as C functions do.

For example:

```text
counter_get
  output: A = current counter
  state:  reads $0200
```

and:

```text
serial_putc
  input: A = byte to transmit
```

If a caller depends on X or Y surviving a call, that must also be part of the contract or the caller must save it.

## Boundary behavior is part of the program

The CI test does not stop at:

```text
00 -> 01 -> 02
```

It also checks 8-bit wraparound:

```text
FF + 1 -> 00
00 - 1 -> FF
```

That is not an error in this application. It is the defined behavior of an 8-bit counter.

## End-to-end qualification

CI:

1. assembles four independent objects
2. links them with the canonical ROM layout
3. checks required exported symbols
4. emits a 16 KiB ROM
5. boots the ROM in the emulator
6. sends commands through the serial device
7. verifies text output
8. verifies RAM state

The test therefore crosses every important boundary in the application.

## Exercise — change one module

Change the counter to saturate instead of wrap:

```text
FF + 1 -> FF
00 - 1 -> 00
```

Rules:

- do not change `app-main.s`
- do not change `app-serial.s`
- do not change `app-hex.s`
- preserve the counter module's exported symbol names

If the module boundaries are useful, this behavior change should be local to `app-counter.s`.
