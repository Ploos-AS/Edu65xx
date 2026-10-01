#!/bin/sh
set -eu

CC="${LLVM_MOS_CLANG:-clang}"
TARGET="${LLVM_MOS_TARGET:-mosw65c02}"

if [ "$#" -ne 2 ]; then
    echo "usage: $0 INPUT.c OUTPUT.s" >&2
    exit 2
fi

"$CC" --target=mos -mcpu="$TARGET" -std=c99 -ffreestanding -S -O1 "$1" -o "$2"
