# Edu65xx licensing

Edu65xx is a mixed-license repository.

## Software — MIT

The following material is licensed under the MIT License unless a file states otherwise:

- `simulator/`
- `emulator/`
- `examples/asm/`
- `examples/c/`
- `rom/` source code
- `scripts/`
- software-oriented files under `toolchain/`
- build/test source and repository automation authored for Edu65xx

The full MIT text is in `LICENSES/MIT.txt`.

## Hardware — CERN-OHL-P-2.0

Hardware design material under `hardware/` is licensed under the CERN Open Hardware Licence Version 2 — Permissive (CERN-OHL-P-2.0), unless a file states otherwise.

The repository notice is in `LICENSES/CERN-OHL-P-2.0-NOTICE.txt`. A public hardware release must also carry the authoritative verbatim CERN-OHL-P-2.0 text.

## Course and documentation — CC-BY-4.0

Course and documentation material is licensed under Creative Commons Attribution 4.0 International (CC-BY-4.0), unless a file states otherwise.

This includes:

- `course/`
- `docs/`
- prose documentation in the repository root
- prose-only hardware documentation where no hardware design file is embedded

The repository notice is in `LICENSES/CC-BY-4.0-NOTICE.txt`. A public course/documentation release must also carry the authoritative verbatim CC-BY-4.0 legal code.

## Mixed files

If a file contains both an implementation/design artifact and explanatory prose, the implementation/design license takes precedence for that file unless the file carries an explicit SPDX identifier or notice.

New files should use an SPDX identifier where practical.

## Third-party material

Third-party files, tools and test corpora retain their upstream licenses and copyright.

Edu65xx does not vendor the Klaus W65C02 functional-test binary. CI downloads the selected binary from a pinned upstream revision for qualification.

A dependency or downloaded test is not relicensed merely because Edu65xx uses it.

## Copyright

Copyright © Ploos AS and individual contributors, as applicable.
