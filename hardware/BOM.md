# M6 breadboard BOM

This is a **design BOM**, not yet an approved purchasing BOM. Exact manufacturer order codes for memory, logic, clock and reset parts remain open until the schematic and electrical checks are complete.

| Qty | Function | Requirement | Status |
|---:|---|---|---|
| 1 | CPU | WDC W65C02S6TPG-14, PDIP-40 | selected |
| 1 | VIA | WDC W65C22S6TPG-14, PDIP-40 | selected |
| 1 | RAM | Alliance Memory AS6C62256-55PCN, 32 KiB x 8, PDIP-28, 2.7-5.5 V | selected |
| 1 | ROM | Microchip AT28C256-15PU, 32 KiB x 8 EEPROM, PDIP-28, 5 V | selected; use one 16 KiB bank |
| TBD | decode | CMOS logic implementing canonical select equations | schematic pending |
| 1 | clock | slow, observable bring-up clock plus later normal clock | circuit pending |
| 1 | reset | power-on + manual reset compatible with W65C02S RESB | circuit pending |
| 1 each | sockets | CPU, VIA, RAM, ROM, decode ICs | required |
| 1 / IC | decoupling | 100 nF ceramic located at each IC | required |
| 1+ | bulk decoupling | at power entry | size after supply design |
| 1 | LED | first VIA output | required |
| 1 | LED resistor | chosen for LED/supply/current target | calculate |
| 1+ | switches/buttons | GPIO experiments | required |
| many | headers | buses, VIA, test points | required |
| many | jumpers | breadboard wiring | required |
| 1 | regulated supply | initial 5 V system | required |

## Do not order from this file yet

The exact order list is intentionally blocked on:

- final RAM/ROM control wiring
- decode schematic
- reset topology
- clock topology
- input pull resistor values
- LED current calculation
- electrical loading review

When those are resolved, this file becomes the purchasing BOM and records manufacturer, MPN, package, quantity and acceptable substitutes.
