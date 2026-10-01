# Edu65xx C toolchain

The primary M4 compiler is LLVM-MOS.

The upstream SDK documents downloadable prebuilt archives, while the compiler documents `--target=mos` and `-mcpu=mosw65c02`. Edu65xx records an upstream identifier in `llvm-mos.version` so course results never intentionally float with upstream `main`.

## Compile an example to assembly

With LLVM-MOS installed:

```sh
LLVM_MOS_CLANG=clang ./scripts/compile-c-to-asm.sh \
  examples/c/02-memory-mapped-io.c build/02-memory-mapped-io.s
```

The script uses:

```text
--target=mos
-mcpu=mosw65c02
-std=c99
-ffreestanding
-S
-O1
```

Generated assembly belongs under `build/` until CI has generated and verified it with the pinned toolchain. We do not hand-write a file and label it compiler output.

## Reproducibility rule

A compiler-output artifact used in a lesson must record:

1. LLVM-MOS pin,
2. exact command/flags,
3. input source,
4. optimization level.

Changing any of these can legitimately change the assembly.
