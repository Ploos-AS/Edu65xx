# Rev A VIA bring-up ROM

`via-blink.s` is the readable course source.

`make_image.c` emits the exact deterministic 16 KiB image used for Rev A qualification and EEPROM programming.

Build and qualify it with:

```sh
make hardware-test
```

The generated file is:

```text
build/via-blink.bin
size:   16384 bytes
SHA256: 5e6b433ce928dae818e7f1e2f72f68f18794152ce82302a0e5c82e011cb9424b
```

CI boots that image through the normal Edu65xx machine emulator and requires:

1. RESET vector -> `$C000`
2. VIA DDRB bit 0 becomes output
3. VIA ORB/PB0 becomes high
4. after the software delay, ORB/PB0 becomes low

## AT28C256 placement

Rev A uses a 32 KiB AT28C256 but exposes only one 16 KiB bank.

```text
EEPROM A14 = 0
CPU $C000-$FFFF -> EEPROM $0000-$3FFF
```

Program `via-blink.bin` at EEPROM offset `$0000`. The upper half `$4000-$7FFF` is not visible while A14 is tied low.

Do not duplicate the 16 KiB image into the upper bank merely to hide a wiring error.

## Programming verification

The EEPROM programmer must verify the written contents after programming.

For qualification evidence record:

- programmer model/software/version
- EEPROM exact part marking
- image Git commit
- image byte count
- image SHA-256
- programmer verify result
- read-back SHA-256 if the programmer can save a read-back image

Expected image SHA-256:

```text
5e6b433ce928dae818e7f1e2f72f68f18794152ce82302a0e5c82e011cb9424b
```

A programmer verify PASS does not replace the later bus-level RESET/ROM qualification.


## VIA Timer 1 IRQ image

`via-irq.s` / `make_irq_image.c` provide the second physical qualification image.

Generated file:

```text
build/via-irq.bin
size:   16384 bytes
SHA256: bd6a7367f353346060cc4852957968d211339d1689dbac6c9691ddc4eb2f314d
RESET:  $C000
IRQ:    $C01E
```

The image drives PB0 high, enables the VIA Timer 1 interrupt, starts a one-shot timer, executes `CLI/WAI`, acknowledges Timer 1 in the ISR, drives PB0 low and returns with `RTI`.

The emulator qualification additionally checks the interrupt stack frame and IRQ vector before accepting the image.

This is the image to use for the physical IRQ/vector logic-analyzer lab after the simpler GPIO ROM has passed.
