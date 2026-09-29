#!/bin/sh
set -eu

mkdir -p build/compiler-output

for src in examples/c/03-functions.c examples/c/04-pointers.c examples/c/05-arrays.c; do
    base=$(basename "$src" .c)
    ./scripts/compile-c-to-asm.sh "$src" "build/compiler-output/$base.s"
done

cat > build/compiler-output/MANIFEST.txt <<EOF
Edu65xx compiler-output manifest
LLVM-MOS pin: $(sed -n '/^[^#]/p' toolchain/llvm-mos.version | head -n 1)
target: mos / mos65c02
optimization: O1
sources:
  examples/c/03-functions.c
  examples/c/04-pointers.c
  examples/c/05-arrays.c
EOF
