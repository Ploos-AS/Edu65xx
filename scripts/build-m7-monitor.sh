#!/bin/sh
set -eu

OUT=${1:-build/m7-monitor}
mkdir -p "$OUT"

llvm-mc -triple mos -mcpu=mosw65c02 --filetype=obj   rom/monitor/m7-monitor.s -o "$OUT/monitor.o"

ld.lld -flavor gnu   -T toolchain/m7/link.ld   -Map="$OUT/monitor.map"   "$OUT/monitor.o"   -o "$OUT/monitor.elf"

llvm-nm -n "$OUT/monitor.elf" > "$OUT/symbols.txt"
llvm-objdump -d --print-imm-hex "$OUT/monitor.elf" > "$OUT/disassembly.txt"
llvm-objcopy -O binary --gap-fill=0xff "$OUT/monitor.elf" "$OUT/m7-monitor.bin"

test "$(wc -c < "$OUT/m7-monitor.bin")" -eq 16384
printf 'M7 monitor generated in %s\n' "$OUT"
