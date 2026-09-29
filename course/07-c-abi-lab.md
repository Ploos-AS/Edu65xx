# M4 — ABI lab: functions are a contract

The CPU does not know what a C function is. The compiler and runtime agree on an ABI.

## Build the listings

Generate assembly from the lab sources with the pinned Edu65xx LLVM-MOS setup. Keep the generated files separate from the handwritten teaching examples.

## Lab A — arguments and return values

Inspect `examples/c/03-functions.c`.

For `add8()`, identify:

- where each argument arrives,
- where the return value is placed,
- whether the function touches the hardware stack,
- which zero-page imaginary registers appear.

Then inspect `add_then_increment()`.

Find the call, the return path and any values that must survive the call.

## Lab B — pointers

Inspect `examples/c/04-pointers.c`.

For `load_byte()` and `store_byte()`, trace:

```text
C pointer
   ↓
address representation
   ↓
indirect/indexed machine operations
   ↓
bus address
```

For `serial_putc()`, find the operation that ultimately reaches `$8010`.

The pointer is not magic. It represents an address.

## Lab C — arrays

Inspect `examples/c/05-arrays.c`.

Answer:

1. How is `values[i]` turned into an address?
2. Where does `i` live?
3. Where does `sum` live?
4. How is the loop condition implemented?
5. Which branches form the loop?
6. What changes when optimization changes?

## Hardware stack versus compiler storage

Do not assume every C local variable becomes a byte on the W65C02 hardware stack. LLVM-MOS has its own ABI and zero-page-backed imaginary-register model, and optimization may keep, move or eliminate values.

The exercise is therefore always evidence-driven: inspect the generated listing before explaining where a value lives.

## Deliverable

For each function, annotate the generated assembly with:

- argument locations,
- result location,
- loads/stores,
- calls/returns,
- branches,
- zero-page usage,
- hardware-stack activity,
- observable bus accesses.

By the end of the lab, a C function should look like a machine-level agreement rather than a language feature implemented by magic.
