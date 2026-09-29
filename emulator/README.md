# Edu65xx Emulator

The Edu65xx emulator will model a complete course machine in readable C.

M0 defines the intended boundary:

- W65C02-compatible CPU core
- RAM
- ROM
- memory map
- memory-mapped I/O
- later VIA, timers and serial devices
- deterministic behavior suitable for tests and lessons

The emulator is not intended to win performance benchmarks. Code clarity and architectural correspondence with the physical Edu65xx computer take priority.

The CPU implementation may share concepts with the simulator, but the simulator remains teaching/visualization oriented while the emulator remains system-execution oriented.
