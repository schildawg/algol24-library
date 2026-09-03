# The Algol-24 Library

The runtime library for [Algol-24](https://github.com/schildawg/algol24), written
in Algol-24.

```pascal
uses math;
uses random;

SetSeed (2024);

WriteLn (Trunc (1.0E30));            // 1000000000000000019884624838656
WriteLn (Sqrt (2.0));                // 1.4142135623730951
WriteLn (Round (2.5));               // 2  -- ties go to the even neighbour
WriteLn (Random (6) + 1);            // a die
```

The compiler is a separate repository and is not vendored here. This project is
an ordinary **user** of the language: `algc` is a tool on the path, the
specification is reference material, and nothing here needs the compiler to be
built.

## Units

| | |
| --- | --- |
| `math` | arithmetic, the transcendentals, the Double-to-Integer conversions, `NaN` and `Infinity` |
| `random` | a seedable generator, kept apart from `math` because it has state |
| `testing` | `AssertNear`, for results whose exact bits are not guaranteed |

`REFERENCE.md` documents every routine and constant, after the pattern of Turbo
Pascal's own reference: what it does, how it is declared, what is worth knowing,
and a worked example. **Every example in it has been run**, and the output shown
is what it printed.

## Building and testing

Needs `algc` on the path (`brew install algol24`) and a C compiler.

```sh
./test.sh                 # every unit, the reference examples, and the example app
./test.sh math.a24        # one unit

./build.sh                # the library's own C -> lib*ffi.dylib
./check-reference.py      # every REFERENCE.md example, against its stated output
```

Tests live inline in the unit they cover, as `test` blocks, and are run by
`algc --test`. There are no conformance cases here — those pin the *language*,
and belong to the compiler project.

## The example application

```sh
examples/build.sh --static     # -> examples/build/statistics
examples/check.sh              # interpreted and both ways compiled must agree
```

`examples/statistics.a24` estimates pi by throwing darts and rolls two dice ten
thousand times. Built with `--static` it depends on nothing but what macOS
already ships, and prints exactly what the interpreter prints.

It is the only check that exercises the whole story rather than one unit at a
time — that the library's units compile into an application, that its C reaches
them, and that a compiled program agrees with the interpreter.
`examples/README.md` explains what is linked in and what is loaded, which turns
out to be the interesting part.

## Turbo Pascal

The routine names are Turbo Pascal's and so is the reference's shape, as an
homage. It is not a compatibility target, and where a better answer exists the
library takes it and says why:

- **`Trunc` answers every finite Double exactly**, however large, because an
  Algol-24 Integer is unbounded. Turbo Pascal's gave up at the width of a
  `LongInt`; there is no such width here.
- **`Round` breaks ties toward the even neighbour**, which is what IEEE 754
  already does for every arithmetic result in the language, and what does not
  accumulate a bias.
- **`Random` and `RandomInteger` are separate names.** Overload selection
  happens at run time and warns at every call site, so one name answering both
  shapes would make the unit's commonest call its noisiest.

## Licence

MIT.
