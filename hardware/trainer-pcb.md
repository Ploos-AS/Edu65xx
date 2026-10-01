# Edu65xx Trainer / Computer PCB concept

The Trainer is the PCB form of the breadboard course, not a black-box development board.

## Design priorities

1. observable architecture
2. repairability
3. through-hole and socketed parts where practical
4. canonical Edu65xx memory map
5. labelled buses and test points
6. safe experimentation
7. low cost after the teaching goals are satisfied

## Functional blocks

```text
+--------------------+
| W65C02S CPU        |
+---------+----------+
          |
  A0..A15 / D0..D7 / control
          |
+---------+-----------------------------+
| address decode                       |
+------+-------------+-----------+------+
       |             |           |
   32 KiB RAM    16 KiB ROM   W65C22S
                               VIA
                                |
                     LEDs / switches / headers
```

A future serial block may occupy `$8010-$8011`. It must not be silently substituted with a different memory map.

## Front-panel teaching I/O

Recommended Trainer resources:

- 8 LEDs on one VIA port through appropriate current limiting
- 8 switches or buttons on the other VIA port
- dedicated RESET button
- dedicated NMI experiment button
- IRQ indication LED driven through a suitable buffer/indicator circuit, not by loading the CPU interrupt net carelessly
- clock/run controls appropriate to the final clock circuit
- expansion headers for both VIA ports

## Bus visibility

Silkscreen should group:

- ADDRESS BUS
- DATA BUS
- CONTROL
- MEMORY SELECT
- VIA
- POWER

All important nets from `hardware/test-points.md` must be available without probing IC pins.

## Sockets and replaceability

Prefer sockets for:

- CPU
- VIA
- RAM
- ROM/EEPROM
- decode logic

Do not put a microcontroller between the CPU and memory bus merely to simplify decode or hide timing. If a programmable logic device is ever offered as an alternative decoder, the discrete logical equations and a transparent reference implementation remain part of the course.

## Expansion

Reserve an expansion header containing at least:

- buffered or clearly documented A0..A15
- D0..D7
- RWB
- PHI2
- RESB
- IRQB / NMIB subject to electrical interface rules
- one or more decoded expansion selects
- VDD and multiple GND pins

Expansion loading limits must be specified before calling the PCB electrically complete.

## Revisions

### Rev A — teaching computer

Target only what M6 needs:

- CPU
- RAM
- ROM
- VIA
- clock/reset
- discrete decode
- LEDs/switches
- test points
- analyzer header

### Rev B — optional additions

Only after Rev A qualification:

- physical serial
- improved expansion
- optional clock modes
- quality-of-life features discovered during teaching use

Do not delay Rev A by turning the Trainer into a feature-rich SBC.
