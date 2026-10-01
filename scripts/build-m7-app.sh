#!/bin/sh
set -eu

OUT=${1:-build/m7-app}
mkdir -p "$OUT"

for module in main serial counter hex; do
  llvm-mc -triple mos -mcpu=mosw65c02 --filetype=obj     "examples/asm/m7/app-${module}.s" -o "$OUT/${module}.o"
done

ld.lld -flavor gnu   -T toolchain/m7/link.ld   -Map="$OUT/app.map"   "$OUT/main.o" "$OUT/serial.o" "$OUT/counter.o" "$OUT/hex.o"   -o "$OUT/app.elf"

llvm-nm -n "$OUT/app.elf" > "$OUT/symbols.txt"
llvm-readelf -S -s -r "$OUT/app.elf" > "$OUT/readelf.txt"
llvm-objdump -d --print-imm-hex "$OUT/app.elf" > "$OUT/disassembly.txt"
llvm-objcopy -O binary --gap-fill=0xff "$OUT/app.elf" "$OUT/app.bin"

test "$(wc -c < "$OUT/app.bin")" -eq 16384
for symbol in reset serial_getc serial_putc counter_reset counter_inc counter_dec counter_get print_hex; do
  grep -q " ${symbol}$" "$OUT/symbols.txt"
done

printf 'M7 multi-module application generated in %s\n' "$OUT"
