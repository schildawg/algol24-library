# Building an application with the library

`statistics.a24` estimates pi by throwing darts and rolls two dice ten thousand
times, which between them reach most of `math` and all of `random`. It exists to
show what shipping an application built on this library involves.

Run it, from the repository root:

```sh
algc examples/statistics.a24
```

Build it into a native executable:

```sh
examples/build.sh              # -> examples/build/statistics, beside its shims
examples/build.sh --static     # -> examples/build/statistics, depending on nothing else
examples/check.sh              # all three must print the same thing
```

## What is linked in

`algc --compile` emits one `.c` per unit the program reaches, so `math.a24`,
`random.a24` and `testing.a24` become `math.c`, `random.c` and `testing.c` and
are compiled into the binary alongside the program's own. **The Algol-24 half of
the library is genuinely linked in**, and there is nothing to ship beside it.

The C half — `mathffi.c` and `randomffi.c` — is where it gets interesting.

## The one real constraint

⚠️ **A foreign declaration cannot be written once to serve both processors.**
Measured, not assumed:

| Declared as | Interpreted | Compiled |
| --- | --- | --- |
| `external 'alg_trunc' in 'libmathffi.dylib'` | works | dlopens the library at run time, **even when the symbol is already linked into the executable** |
| `external 'alg_trunc'` | `No foreign symbol 'alg_trunc'.` | resolves from the executable, so the binary is self-contained |

The units keep the `in` clause, because the library's own tests run interpreted
and that has to keep working. `--static` rewrites it out of a throwaway copy of
the sources at build time; nothing under version control is touched.

Preloading with `DYLD_INSERT_LIBRARIES` does not rescue the second spelling for
the interpreter, which was the obvious thing to try.

## What each mode gives you

**Default.** The binary dlopens `libmathffi.dylib` and `librandomffi.dylib` by
bare name, which resolves against the working directory — so the shims are
copied beside it and it must be run from there. Fine for development, awkward to
ship.

**`--static`.** One file, and `otool -L` shows it needs only what macOS already
ships:

```
/usr/lib/libffi.dylib
/usr/lib/libSystem.B.dylib
```

It runs from anywhere and prints exactly what the interpreter prints. This is the
mode to ship.

## Why the example seeds instead of randomizing

`SetSeed (20240902)` rather than `Randomize ()`, so every run prints the same
numbers. That is what lets `check.sh` compare the three runs at all — a program
choosing its own seed could not be checked against anything.

It also happens to be the default behavior: a program that never calls
`Randomize` is reproducible, and only becomes unpredictable where it asks to be.

## What the output shows

The dice histogram is worth a glance beyond the mechanics: it peaks at seven and
falls away symmetrically, which is the distribution two dice should give and a
cheap sanity check on the generator.

The pi estimate is the opposite lesson. The error falls with the square root of
the dart count, so a hundredfold more darts buys about one more digit — a
hundred thousand darts still only gets two.
