# M6 debug and test points

The physical Edu65xx machine is teaching hardware. Important signals must therefore be easy to probe without clipping directly onto IC legs.

## Mandatory labelled test points

### Power

- TP_VDD
- TP_GND near the CPU
- TP_GND near the VIA
- TP_GND near memory

### Clock and reset

- TP_PHI2
- TP_RESB

### CPU control

- TP_RWB
- TP_SYNC
- TP_VPB
- TP_IRQB
- TP_NMIB
- TP_RDY
- TP_BE

### Decode

- TP_RAM_CS_B
- TP_ROM_CS_B
- TP_VIA_CS

### Address bus

At minimum expose:

- A0
- A1
- A2
- A3
- A4
- A14
- A15

For the Trainer PCB, expose all A0..A15 on a grouped header.

### Data bus

Expose D0..D7 as a grouped header.

### VIA

Expose:

- PA0..PA7
- PB0..PB7
- CA1 / CA2
- CB1 / CB2

## Logic-analyzer header

The Trainer PCB should have a keyed or unmistakably labelled header with adjacent ground pins.

Suggested first 16-channel capture set:

```text
0  PHI2
1  RWB
2  SYNC
3  RESB
4  A0
5  A1
6  A2
7  A3
8  A14
9  A15
10 D0
11 D1
12 D2
13 D3
14 /ROM_CS
15 VIA_CS
```

This set is intentionally educational rather than exhaustive. A wider analyzer can capture the full address and data buses.

## Probe rule

A test point is not decoration. Every mandatory point must correspond to a lab or a bring-up measurement.

The breadboard may use labelled male headers. The Trainer PCB should provide silkscreen names visible while probes are attached.
