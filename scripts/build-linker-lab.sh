#!/bin/sh
set -eu

OUT=${1:-build/m7-linker}
mkdir -p "$OUT"

llvm-mc -triple mos -mcpu=mosw65c02 --filetype=obj   examples/asm/m7/main.s -o "$OUT/main.o"
llvm-mc -triple mos -mcpu=mosw65c02 --filetype=obj   examples/asm/m7/math.s -o "$OUT/math.o"

ld.lld -flavor gnu   -T toolchain/m7/link.ld   -Map="$OUT/program.map"   "$OUT/main.o" "$OUT/math.o"   -o "$OUT/program.elf"

llvm-nm -n "$OUT/program.elf" > "$OUT/symbols.txt"
llvm-readelf -S -s -r "$OUT/program.elf" > "$OUT/readelf.txt"
llvm-objdump -d --print-imm-hex "$OUT/program.elf" > "$OUT/disassembly.txt"
llvm-objcopy -O binary --gap-fill=0xff "$OUT/program.elf" "$OUT/program.bin"

test "$(wc -c < "$OUT/program.bin")" -eq 16384
grep -q ' reset$' "$OUT/symbols.txt"
grep -q ' double_a$' "$OUT/symbols.txt"

printf 'M7 linker lab generated in %s\n' "$OUT"
