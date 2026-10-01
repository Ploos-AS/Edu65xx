# Rev A breadboard assembly checklist

This checklist converts the reviewed design into a physical build without installing everything at once.

## Tools

Required or strongly recommended:

- regulated 5 V bench supply with current limit
- multimeter
- logic analyzer with enough channels for staged captures
- EEPROM programmer that supports AT28C256
- breadboards and short solid-core jumpers
- IC insertion/extraction tools if available

An oscilloscope is strongly useful for power/clock/reset integrity even though the logic analyzer handles the architectural exercises.

## Stage A — passive infrastructure

Install no ICs.

Build:

- +5 V and GND distribution
- local rail links
- 10 uF bulk capacitor at power entry
- IC sockets
- test-point ground references

Before continuing:

- [ ] all intended VCC/VDD socket pins reach +5 V
- [ ] all intended GND/VSS socket pins reach GND
- [ ] no unintended +5 V/GND continuity
- [ ] rail polarity marked at every breadboard section

Use the bench supply current limit conservatively for first power.

## Stage B — clock and reset only

Install:

- Y1 ECS-100AX-010
- U8 DS1813-5+
- RESET button
- local decoupling

Do not install CPU yet.

Measure:

- [ ] +5 V at Y1/U8
- [ ] TP_PHI2 = approximately 1 MHz
- [ ] RESET button pulls TP_RESB low
- [ ] RESB returns high after release
- [ ] power-cycle produces delayed reset release

Preserve a VDD/RESB capture.

## Stage C — decode/control logic

Install:

- U5 SN74HC138N
- U6 SN74HC00N
- U7 SN74HC32N
- U9 SN74HC688N
- local 100 nF capacitors

Wire the address inputs, but leave CPU/memory/VIA sockets empty.

Continuity-check every decoder input and output against `rev-a-connectivity.tsv`.

- [ ] all unused HC inputs tied
- [ ] no HC output shorted to a rail
- [ ] U5 enables correct
- [ ] U9 Q0..Q7 and OE# grounded

## Stage D — CPU + EEPROM

Program `build/via-blink.bin` into U4 AT28C256 lower bank and verify the programmed image.

Install:

- U1 W65C02S
- U4 AT28C256
- temporary 10 kOhm CPU IRQB pull-up because U2 is absent
- CPU control pull-ups
- local decoupling

Do **not** install SRAM or VIA.

Capture RESET.

Expected:

- [ ] vector read $FFFC = $00
- [ ] vector read $FFFD = $C0
- [ ] opcode fetch begins at $C000

If this fails, stop. Do not install more devices.

## Stage E — VIA

Power off.

Remove the temporary CPU IRQB pull-up.

Install:

- U2 W65C22S
- PB0/PB1 LED + 1 kOhm resistor circuits
- local decoupling

Power on with `via-blink.bin`.

Expected:

- [ ] write $01 to $8002
- [ ] write $01 to $8000
- [ ] PB0 high
- [ ] later write $00 to $8000
- [ ] PB0 low

Probe /VIA_CS and verify it asserts only for the intended access.

## Stage F — SRAM

Power off and install U3 AS6C62256-55PCN.

First use `ram-smoke.bin`.

- [ ] PB0 PASS
- [ ] PB1 remains off

Then use `ram-full.bin`.

- [ ] PB0 PASS
- [ ] PB1 remains off

The full test is destructive.

## Stage G — IRQ

Program `via-irq.bin`.

Capture at minimum:

- PHI2
- IRQB
- SYNC
- RWB
- selected address lines or full address bus where channel count permits

Prove:

- [ ] VIA IRQB low
- [ ] CPU stack writes
- [ ] vector reads $FFFE/$FFFF
- [ ] ISR fetch at $C01E
- [ ] Timer 1 acknowledge
- [ ] RTI return

## Stop conditions

Immediately remove power if:

- supply current rises unexpectedly
- an IC becomes warm
- VDD collapses
- two known push-pull outputs appear connected together
- the clock/reset signals exceed valid rail levels
- a part was inserted with uncertain orientation

Do not debug a suspected wiring error by repeatedly power-cycling it.
