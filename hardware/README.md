# Edu65xx physical computer — Rev A

Rev A is the breadboard teaching computer used by M6.

## Start here

1. `BOM.md` — selected parts and quantities
2. `DATASHEETS.md` — primary manufacturer references
3. `rev-a-schematic.svg` — graphical design overview
4. `schematic-rev-a.md` — pin-numbered schematic contract
5. `rev-a-connectivity.tsv` — machine-readable physical-pin source
6. `rev-a-review.md` — completed design review
7. `assembly-checklist.md` — staged physical assembly
8. `bringup-sequence.md` — qualification order
9. `analyzer-channels.md` — capture channel plans
10. `evidence-template.md` — physical PASS record

## Design support

- `address-decode.md` — canonical discrete decode
- `clock-reset.md` — 1 MHz clock and reset supervisor
- `test-points.md` — debug/test points
- `breadboard.md` — architecture and breadboard guidance
- `wiring.md` — net-level wiring contract
- `trainer-pcb.md` — later PCB direction

## Automated qualification

```sh
make hardware-test
```

This validates the documented connectivity, all 65,536 decode addresses, and the physical bring-up ROM images in the emulator.

It is not a substitute for physical M6 evidence.

## Physical ROM order

1. `via-blink.bin`
2. `ram-smoke.bin`
3. `ram-full.bin`
4. `via-irq.bin`

See `../rom/bringup/README.md` for programming details and image hashes.

## Current gate

Rev A design review: **PASS**

M6 physical qualification: **NOT YET RUN**

The next milestone action requires a real breadboard build.
