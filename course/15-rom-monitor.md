# 15 — Building a ROM monitor

A monitor is a small program that lets a human inspect or control a machine without a full operating system.

The M7 Edu65xx monitor is intentionally small enough to understand completely.

## Command loop

After RESET:

```text
initialize CPU
 -> print banner
 -> wait for RX_READY
 -> read one byte
 -> dispatch command
 -> print response
 -> repeat
```

Supported commands:

```text
?   help
P   ping
M   read $0200
```

Unknown bytes are echoed.

## P — the smallest protocol

`P` replies:

```text
PONG
```

This command looks trivial, but it tests the complete path:

```text
host input
 -> serial RX device
 -> STATUS
 -> DATA
 -> CPU compare/branch
 -> ROM command handler
 -> serial TX
 -> host output
```

That makes it a useful first command when bringing up a serial monitor.

## M — memory becomes text

The `M` command reads one byte from `$0200`.

If memory contains:

```text
$0200 = $2A
```

the terminal receives:

```text
M 0200=2A
```

The interesting part is not the fixed address. It is the conversion.

A byte contains two hexadecimal nibbles:

```text
0010 1010
  2    A
```

The monitor:

1. saves the byte
2. shifts the high nibble down
3. converts it to ASCII
4. restores the byte
5. masks the low nibble
6. converts it to ASCII

For values 0–9, add ASCII `'0'`.

For values 10–15, add the offset that produces `A`–`F`.

## Why use PHA/PLA?

The accumulator contains the byte we want to print, but the first nibble conversion destroys A.

The monitor therefore uses the hardware stack:

```asm
pha
; print high nibble
pla
; print low nibble
```

This connects an early CPU lesson directly to a useful larger program.

## ROM source versus ROM image

The source is:

```text
rom/monitor/m7-monitor.s
```

The build pipeline produces:

```text
monitor.o
monitor.elf
monitor.map
symbols.txt
disassembly.txt
m7-monitor.bin
```

The EEPROM wants the binary.

The debugger wants the symbols and disassembly.

The student should understand why both representations exist.

## End-to-end qualification

CI does not merely check that the source assembles.

The test boots the linked ROM in the Edu65xx machine model, waits for the banner, injects commands at the serial device boundary and verifies the bytes transmitted by monitor code.

For `M`, the test first writes `$2A` to RAM at `$0200` and then requires the monitor to transmit `2A`.

## Extension exercise

Turn the fixed `M` command into an address-taking command:

```text
M0200
M7FFF
```

Do it in stages:

1. accept one hexadecimal digit
2. convert ASCII to a nibble
3. combine four nibbles into a 16-bit address
4. read memory
5. print the result
6. reject invalid input

Do not begin by writing a general command parser. Grow the monitor one observable behavior at a time.
