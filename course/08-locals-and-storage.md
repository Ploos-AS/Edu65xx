# M4 — Locals and storage

A C local variable is a language concept, not a promise that one byte will be pushed onto the W65C02 hardware stack.

Use `examples/c/06-locals.c` and inspect actual compiler output.

## Questions

For `transform()`:

1. Does `doubled` exist as stored memory at all?
2. Does `adjusted` exist independently?
3. Which values live in physical or imaginary registers?
4. Does optimization eliminate either named variable?

For `combine()`:

1. How is a 16-bit C value represented on an 8-bit CPU?
2. Where are the high and low bytes while the function executes?
3. How is the 16-bit return value represented by the ABI?

## Rule

Do not teach:

```text
local variable = stack slot
```

Teach:

```text
C semantics
    ↓
compiler + ABI + optimization
    ↓
chosen storage
```

Storage may include physical registers, zero-page imaginary registers, static/software-stack areas, hardware stack activity, or no persistent storage at all when a value can be optimized away.
