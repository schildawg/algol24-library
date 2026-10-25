# The Algol-24 Library

The runtime library for [Algol-24](https://github.com/schildawg/algol24), written
in Algol-24.

```pascal
uses math;
uses random;

SetSeed (2024);

WriteLn (Trunc (1.0E30));            // 1000000000000000019884624838656
WriteLn (Sqrt (2.0));                // 1.4142135623730951
WriteLn (Round (2.5));               // 2  -- ties go to the even neighbor
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
| `graph` | a windowed screen: celled text on a grid, Windows and ViewPorts stacked around it, the drawing vocabulary, the keyboard and the mouse |
| `crt` | the same text mode on a terminal, composited in memory and painted by difference |
| `sound` | tones, BASIC's `Play`, and sampled sounds mixed sixteen at a time |
| `file` | whole-file reads and writes, and the questions to ask about a path |
| `strings` | `Trim`, `Upper`, `Lower`, `Repeat`, `Split`, `Join`, `Delete`, `Insert` and `Replace`, each callable either as a function or on its first argument |
| `testing` | `AssertNear`, for results whose exact bits are not guaranteed |

`REFERENCE.md` documents every routine and constant, after the pattern of Turbo
Pascal's own reference: what it does, how it is declared, what is worth knowing,
and a worked example. **Every example in it has been run**, and the output shown
is what it printed.

## Building and testing

Needs `algc` **0.1.5 or newer** on the path (`brew install algol24`), a C
compiler, and SDL2 for `graph` (`brew install sdl2`). The version floor is
`strings`, which uses extension functions that an earlier compiler cannot
parse. The test run opens no windows and makes no noise — it sets `SDL_VIDEODRIVER=dummy` and `ALG_SOUND=dummy`, so `graph`
draws nowhere and `sound` computes every note and plays none of them.

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

Eleven demos sit beside it, run interpreted and checked by eye — the window and
its glyph files, the keys, the figures and their fills, the arc convention, the
stacked surfaces, a piano that plays Ode to Joy and a jukebox driven by the
mouse. Two are acceptance pieces: `examples/ide.a24` rebuilds the Turbo C++
screen out of `graph`'s vocabulary, and `examples/ide-crt.a24` is the same
screen on a terminal, sharing the celled half almost verbatim and dropping the
two drawn panels, `crt` having no pixels. `examples/README.md` catalogues them.

## Turbo Pascal

The routine names are Turbo Pascal's and so is the reference's shape, as an
homage. It is not a compatibility target, and where a better answer exists the
library takes it and says why:

- **`Trunc` answers every finite Double exactly**, however large, because an
  Algol-24 Integer is unbounded. Turbo Pascal's gave up at the width of a
  `LongInt`; there is no such width here.
- **`Round` breaks ties toward the even neighbor**, which is what IEEE 754
  already does for every arithmetic result in the language, and what does not
  accumulate a bias.
- **`Random`, `RandomInteger` and `RandomReal` are separate names.** Turbo
  Pascal spells all of it `Random` and takes the return type from the argument
  count. Here the name says which you get. The original objection was louder —
  0.1.3 warned at every call site of an overload — and 0.1.4 closed that half;
  the readability half is why the names stay apart.

## Licence

MIT.
