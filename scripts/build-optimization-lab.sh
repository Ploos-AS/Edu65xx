#!/bin/sh
set -eu

CC="${LLVM_MOS_CLANG:-mos-clang}"
TARGET="${LLVM_MOS_TARGET:-mos65c02}"
PIN=$(sed -n '/^[^#]/p' toolchain/llvm-mos.version | head -n 1)

mkdir -p build/optimization

for opt in 0 1 2; do
    for src in examples/c/03-functions.c examples/c/05-arrays.c examples/c/06-locals.c; do
        base=$(basename "$src" .c)
        out="build/optimization/${base}-O${opt}.s"
        "$CC" --target=mos -mcpu="$TARGET" -std=c99 -ffreestanding -S "-O$opt" "$src" -o "$out"
    done
done

cat > build/optimization/MANIFEST.txt <<EOF
Edu65xx optimization lab
LLVM-MOS pin: $PIN
target: mos / $TARGET
optimization levels: O0 O1 O2
EOF
