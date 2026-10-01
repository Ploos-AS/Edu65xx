# M6 clock and reset

Rev A uses deliberately boring clock and reset circuits. They should be easy to probe and easy to replace.

## Clock

Selected bring-up oscillator:

- ECS Inc. `ECS-100AX-010`
- 1 MHz
- 5 V
- TTL output
- through-hole

Connect the oscillator output to W65C02S `PHI2` and W65C22S `PHI2`.

WDC recommends an external oscillator as the W65C02S PHI2 system time base. Rev A therefore does not build a crystal oscillator around PHI1O/PHI2O.

Add:

- 100 nF decoupling at the oscillator
- TP_PHI2 at the clock source and near the CPU
- a ground test point adjacent to the analyzer clock probe

Do not gate PHI2 for single stepping. The W65C02S is static, but the course should teach the processor's dedicated control interface rather than create a glitch-prone clock. A later single-step experiment uses `RDY`/SYNC.

## RDY

WDC states that current W65C02S devices no longer have an active RDY pull-up and recommends an external pull-up.

Rev A uses:

```text
RDY --- 4.7 kOhm --- +5 V
```

At 5 V this is approximately 1.06 mA if the processor pulls RDY low during WAI, below the datasheet's 1.6 mA low-output test current at the 5 V condition.

Expose RDY as TP_RDY. Any later external RDY driver must respect that WAI can also pull this bidirectional net low.

## Reset

Selected reset supervisor:

- Analog Devices / Maxim `DS1813-5+`
- 5 V supervisor
- TO-92
- active-low reset
- pushbutton support
- approximately 150 ms reset hold after supply returns in tolerance
- open-drain output with internal pull-up

Connect the DS1813 active-low reset output to:

- W65C02S `RESB`
- W65C22S `RESB`
- TP_RESB
- normally-open RESET pushbutton to ground as specified for the DS1813 pushbutton function

The W65C02S requires RESB low for at least two clock cycles after VDD reaches operating voltage. At the 1 MHz Rev A clock, the DS1813 hold interval is vastly longer than that minimum.

## Other CPU control inputs

Rev A:

```text
BE    -- 10 kOhm --> +5 V
SOB   -- 10 kOhm --> +5 V
NMIB  -- 10 kOhm --> +5 V, momentary experiment switch to GND
IRQB  <-- W65C22S IRQB once VIA is installed
```

Before the VIA is installed, IRQB is held high through a 10 kOhm pull-up.

WDC identifies RDY, IRQB, NMIB, BE and SOB as inputs that must be pulled high when unused. W65C22S IRQB is a totem-pole output, so Rev A connects only this single VIA interrupt source directly to CPU IRQB. Do not wire-OR another push-pull IRQ output onto it.

## Qualification

Before inserting the CPU:

1. power the board
2. verify +5 V
3. verify 1 MHz at TP_PHI2
4. press RESET and verify TP_RESB low
5. release RESET and verify delayed high transition
6. power-cycle and capture VDD + RESB together

After inserting the CPU, verify that RESET release is followed by the expected vector fetch.
