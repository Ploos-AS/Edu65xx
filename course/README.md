# Edu65xx course index

Edu65xx is designed to be followed in order. Later chapters deliberately assume the machine model established by earlier chapters.

## Core path

| Chapter | Topic | Requires |
| --- | --- | --- |
| 00 | Learning path | none |
| 01 | CPU fundamentals | none |
| 02 | Memory and buses | 01 |
| 03 | I/O and W65C22 VIA | 01-02 |
| 04 | Timers and interrupts | 01-03 |
| 05 | Serial terminal | 01-04 |
| 06 | C from assembly | 01-05 |
| 07 | C ABI lab | 06 |
| 08 | Locals and storage | 06-07 |
| 09 | Optimization lab | 06-08 |
| 10 | Machine emulator | 01-09 |

## Physical path

The physical chapters depend on the core machine model but can be postponed until hardware is available.

| Chapter | Topic | Requires |
| --- | --- | --- |
| 11 | Logic analyzer | 01-05, physical Rev A |
| 12 | Physical bring-up | 01-05, Rev A design/package |

A student without physical hardware can continue to Chapter 13 while Chapters 11-12 remain pending.

Physical PASS is never inferred from emulator success.

## Advanced software path

| Chapter | Topic | Requires |
| --- | --- | --- |
| 13 | Debugging the machine | 01-10 |
| 14 | Linker and symbols | 06-10 |
| 15 | ROM monitor | 05, 13-14 |
| 16 | Mixed C and assembly | 07, 14 |
| 17 | Multi-module assembly | 05, 14-15 |
| 18 | Emulator extension | 03, 10, 13 |

These chapters form M7 and can be completed without the physical M6 breadboard.

## Optional continuation

| Chapter | Topic | Requires |
| --- | --- | --- |
| 19 | W65C02 to W65C816 bridge | 01-18 recommended |

Chapter 19 is a design/architecture bridge. It does not mean a W65C816 emulator or physical W65C816 computer has been implemented.

## Tool requirements by phase

Early assembly/CPU concepts can be read without a toolchain.

Repository exercises progressively use:

- host C compiler and `make` for simulator/emulator tests
- pinned LLVM-MOS SDK for C, ELF/linker and mixed-language labs
- logic analyzer and Rev A components only for physical chapters

CI is the reproducibility reference for exact automated commands.

## Suggested checkpoints

After Chapter 05, a student should be able to explain CPU, memory, VIA, interrupts and teaching serial I/O.

After Chapter 10, a student should be able to trace a complete program from source-level intent to machine execution.

After Chapter 18, a student should be able to divide a larger 65C02 system into modules, debug it, link it and extend its emulated environment without hiding the underlying machine.

Chapter 19 then asks how that mental model must change for the W65C816.
