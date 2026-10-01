# M6 breadboard BOM

This is the Rev A breadboard purchasing baseline. Manufacturer order codes are locked for the architectural ICs; ordinary passives, sockets, headers and breadboard hardware may use equivalent parts that meet the stated values and package needs.

| Qty | Function | Requirement | Status |
|---:|---|---|---|
| 1 | CPU | WDC W65C02S6TPG-14, PDIP-40 | selected |
| 1 | VIA | WDC W65C22S6TPG-14, PDIP-40 | selected |
| 1 | RAM | Alliance Memory AS6C62256-55PCN, 32 KiB x 8, PDIP-28, 2.7-5.5 V | selected |
| 1 | ROM | Microchip AT28C256-15PU, 32 KiB x 8 EEPROM, PDIP-28, 5 V | selected; use one 16 KiB bank |
| 1 | decode | TI SN74HC138N, PDIP-16 | selected |
| 1 | decode | TI SN74HC688N, PDIP-20 | selected |
| 1 | decode | TI SN74HC32N, PDIP-14 | selected |
| 1 | decode/read control | TI SN74HC00N, PDIP-14 | selected |
| 1 | clock | ECS ECS-100AX-010, 1 MHz, 5 V through-hole oscillator | selected |
| 1 | reset | Analog Devices/Maxim DS1813-5+, TO-92 reset supervisor with pushbutton support | selected |
| 1 each | sockets | CPU, VIA, RAM, ROM, decode ICs | required |
| 10 | decoupling | 100 nF ceramic, one at each active IC/oscillator plus spare | selected |
| 1 | bulk decoupling | 10 uF electrolytic at 5 V power entry | selected |
| 1 | LED | first VIA output | required |
| 1 | LED resistor | 1 kOhm for first PB0 indicator | selected |
| 1 | RDY pull-up | 4.7 kOhm | selected |
| 4 | control pull-ups | 10 kOhm for BE, SOB, NMIB and pre-VIA IRQB | selected |
| 1 | RESET button | normally-open momentary switch to GND on DS1813 reset net | selected |
| 1 | NMI button | normally-open momentary switch to GND | selected |
| 8 | input switches/buttons | VIA GPIO experiments | selected |
| many | headers | buses, VIA, test points | required |
| many | jumpers | breadboard wiring | required |
| 1 | regulated supply | initial 5 V system | required |

## Remaining before wiring

The exact order list is intentionally blocked on:

- final RAM/ROM control wiring
- final pin-by-pin wiring schematic
- LED current calculation
- electrical loading review

When those are resolved, this file becomes the purchasing BOM and records manufacturer, MPN, package, quantity and acceptable substitutes.
