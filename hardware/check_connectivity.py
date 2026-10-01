#!/usr/bin/env python3
import csv
from collections import defaultdict
from pathlib import Path

path = Path("hardware/rev-a-connectivity.tsv")
rows = list(csv.DictReader(path.open(), delimiter="\t"))

seen = set()
by_ref = defaultdict(dict)
by_net = defaultdict(list)

for row in rows:
    key = (row["ref"], int(row["pin"]))
    assert key not in seen, f"duplicate physical pin: {key}"
    seen.add(key)
    by_ref[row["ref"]][int(row["pin"])] = row
    by_net[row["net"]].append(row)

expected_pin_counts = {
    "U1": 40, "U2": 40, "U3": 28, "U4": 28,
    "U5": 16, "U6": 14, "U7": 14, "U8": 3,
    "U9": 20, "Y1": 4,
}
for ref, count in expected_pin_counts.items():
    assert len(by_ref[ref]) == count, (ref, len(by_ref[ref]), count)

def net(ref, pin):
    return by_ref[ref][pin]["net"]

# W65C02S power/control and W65C22S shared control.
assert net("U1", 8) == "+5V"
assert net("U1", 21) == "GND"
assert net("U1", 37) == "PHI2"
assert net("U1", 40) == "RESB"
assert net("U2", 20) == "+5V"
assert net("U2", 1) == "GND"
assert net("U2", 25) == "PHI2"
assert net("U2", 34) == "RESB"
assert net("U1", 4) == net("U2", 21) == "IRQB"
assert net("U1", 34) == net("U2", 22) == "RWB"

# VIA register select must be A0..A3 in increasing significance.
for bit, via_pin in enumerate((38, 37, 36, 35)):
    assert net("U2", via_pin) == f"A{bit}"

# CPU address/data bus pin map.
cpu_addr_pins = list(range(9, 21)) + [22, 23, 24, 25]
for bit, pin in enumerate(cpu_addr_pins):
    assert net("U1", pin) == f"A{bit}"
for bit, pin in enumerate((33, 32, 31, 30, 29, 28, 27, 26)):
    assert net("U1", pin) == f"D{bit}"

# RAM is lower 32 KiB: /CE is driven directly by A15.
assert net("U3", 20) == "A15"
assert net("U3", 22) == "/RD"
assert net("U3", 27) == "RWB"

# EEPROM lower 16 KiB bank and read-only run-time wiring.
assert net("U4", 1) == "GND"
assert net("U4", 20) == "/ROM_CS"
assert net("U4", 22) == "/RD"
assert net("U4", 27) == "+5V"

# 74HC138 high region decode: C:B:A = A15:A14:A13 -> Y4.
assert net("U5", 1) == "A13"
assert net("U5", 2) == "A14"
assert net("U5", 3) == "A15"
assert net("U5", 11) == "REGION_100_B"

# ROM select and read strobe.
assert net("U6", 1) == "A15"
assert net("U6", 2) == "A14"
assert net("U6", 3) == "/ROM_CS"
assert net("U6", 4) == net("U6", 5) == "RWB"
assert net("U6", 6) == "/RD"

# VIA decode: Y4 OR A12 OR comparator-not-equal.
assert net("U7", 1) == "REGION_100_B"
assert net("U7", 2) == "A12"
assert net("U7", 3) == net("U7", 4) == "VIA_PRE_B"
assert net("U7", 5) == "A11_A4_EQ_B"
assert net("U7", 6) == net("U2", 23) == "/VIA_CS"

# Comparator P0..P7 = A4..A11, all Q inputs and OE low.
assert net("U9", 1) == "GND"
for bit in range(8):
    ppin = (2,4,6,8,11,13,15,17)[bit]
    qpin = (3,5,7,9,12,14,16,18)[bit]
    assert net("U9", ppin) == f"A{bit+4}"
    assert net("U9", qpin) == "GND"
assert net("U9", 19) == "A11_A4_EQ_B"

# Reset and oscillator.
assert net("U8", 1) == "RESB"
assert net("U8", 2) == "+5V"
assert net("U8", 3) == "GND"
assert net("Y1", 8) == "PHI2"
assert net("Y1", 14) == "+5V"
assert net("Y1", 7) == "GND"

# Every active HC input is tied to a defined net; no input may be NC.
for ref in ("U5", "U6", "U7", "U9"):
    for row in by_ref[ref].values():
        if row["role"] == "input":
            assert row["net"] != "NC", f"floating input {ref}.{row['pin']}"

# Key single-driver nets. Bidirectional buses are intentionally excluded.
for signal, driver in {
    "PHI2": ("Y1", 8),
    "IRQB": ("U2", 21),
    "/ROM_CS": ("U6", 3),
    "/RD": ("U6", 6),
    "REGION_100_B": ("U5", 11),
    "A11_A4_EQ_B": ("U9", 19),
    "/VIA_CS": ("U7", 6),
    "RESB": ("U8", 1),
}.items():
    outputs = {(r["ref"], int(r["pin"])) for r in by_net[signal] if r["role"] == "output"}
    assert outputs == {driver}, (signal, outputs, driver)

print(f"Rev A connectivity: PASS ({len(rows)} physical pin records)")

