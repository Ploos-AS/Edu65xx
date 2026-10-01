# Rev A manufacturer references

Pin-numbered hardware work must be reviewed against primary manufacturer documentation.

| Ref | Part | Primary reference | Review use |
|---|---|---|---|
| U1 | WDC W65C02S6TPG-14 | https://www.wdc65xx.com/wdc/documentation/w65c02s.pdf | PDIP-40 pinout, control pins, reset, RDY, timing |
| U2 | WDC W65C22S6TPG-14 | https://www.wdc65xx.com/wdc/documentation/w65c22.pdf | PDIP-40 pinout, chip select, PHI2, IRQ, ports |
| U3 | Alliance AS6C62256-55PCN | https://www.alliancememory.com/as6c62256/ | current part/package/voltage status; use linked manufacturer datasheet for pin review |
| U4 | Microchip AT28C256-15PU | https://ww1.microchip.com/downloads/aemDocuments/documents/MPD/ProductDocuments/DataSheets/AT28C256-Industrial-Grade-256-Kbit-Paged-Parallel-EEPROM-Data-Sheet-DS20006386.pdf | PDIP-28 pinout and control signals |
| U5 | TI SN74HC138N | https://www.ti.com/lit/gpn/SN74HC138 | PDIP-16 pinout, enables, active-low outputs |
| U6 | TI SN74HC00N | https://www.ti.com/lit/gpn/SN74HC00 | PDIP-14 NAND pinout |
| U7 | TI SN74HC32N | https://www.ti.com/lit/gpn/SN74HC32 | PDIP-14 OR pinout |
| U9 | TI SN74HC688N | https://www.ti.com/lit/gpn/SN74HC688 | PDIP-20 comparator pinout |
| U8 | Analog Devices/Maxim DS1813-5+ | https://www.analog.com/media/en/technical-documentation/data-sheets/DS1813.pdf | TO-92 bottom-view pinout and reset behavior |
| Y1 | ECS ECS-100AX-010 | https://ecsxtal.com/products/oscillators/through-hole-oscillators/ecs-100x/ | 1 MHz, 5 V TTL, full-size through-hole oscillator family |

## Review policy

- Do not substitute a package merely because the logical part number looks similar.
- A substitute must get a new pinout review before it is added as an approved alternate.
- The W65C22S and W65C22N are not electrically interchangeable in every IRQ design; Rev A specifically targets W65C22S.
- Datasheet URLs are references, not vendored copies. Manufacturer copyright documents are not committed to this repository.
- Record a future schematic-review date/commit when the graphical schematic is produced.
