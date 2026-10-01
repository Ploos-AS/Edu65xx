#!/bin/sh
set -eu

OUT=${1:-build/m7-mixed}
mkdir -p "$OUT"

clang --target=mos -mcpu=mosw65c02 -std=c99 -ffreestanding -O2 -c   examples/c/m7/mixed.c -o "$OUT/mixed-c.o"

llvm-mc -triple mos -mcpu=mosw65c02 --filetype=obj   examples/asm/m7/mixed-entry.s -o "$OUT/mixed-entry.o"

ld.lld -flavor gnu   -T toolchain/m7/link.ld   -Map="$OUT/mixed.map"   "$OUT/mixed-entry.o" "$OUT/mixed-c.o"   -o "$OUT/mixed.elf"

llvm-nm -n "$OUT/mixed.elf" > "$OUT/symbols.txt"
llvm-objdump -d --print-imm-hex "$OUT/mixed.elf" > "$OUT/disassembly.txt"
llvm-objcopy -O binary --gap-fill=0xff "$OUT/mixed.elf" "$OUT/mixed.bin"

test "$(wc -c < "$OUT/mixed.bin")" -eq 16384
grep -q ' c_plus_one$' "$OUT/symbols.txt"
printf 'M7 mixed C/assembly ROM generated in %s\n' "$OUT"
