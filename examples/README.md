# Building an application with the library

Twelve programs live here. `statistics.a24` is the one that gets built: it
estimates pi by throwing darts and rolls two dice ten thousand times, which
between them reach most of `math` and all of `random`, and it exists to show
what shipping an application built on this library involves. Most of this file
is about it. The other eleven are demos, catalogued at the foot.

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
| `external 'alg_trunc' in 'libmathffi.so'` | works | dlopens the library at run time, **even when the symbol is already linked into the executable** |
| `external 'alg_trunc'` | `No foreign symbol 'alg_trunc'.` | resolves from the executable, so the binary is self-contained |

The units keep the `in` clause, because the library's own tests run interpreted
and that has to keep working. `--static` rewrites it out of a throwaway copy of
the sources at build time; nothing under version control is touched.

Preloading with `DYLD_INSERT_LIBRARIES` does not rescue the second spelling for
the interpreter, which was the obvious thing to try.

## What each mode gives you

**Default.** The binary dlopens `libmathffi.so` and `librandomffi.so` by
bare name, which resolves against the working directory — so the shims are
copied beside it and it must be run from there. Fine for development, awkward to
ship.

**`--static`.** One file, and for this program `otool -L` shows it needs only
what macOS already ships:

```
/usr/lib/libffi.dylib
/usr/lib/libSystem.B.dylib
```

It runs from anywhere and prints exactly what the interpreter prints. This is the
mode to ship.

⚠️ **How self-contained depends on which units the program reaches.**
`statistics.a24` uses `math` and `random`, whose C needs nothing but libc, so
the list above is the whole of it. A program reaching `graph` or `sound` links
SDL2, which Homebrew installs and macOS does not ship — measured on a
`sound`-using binary built the same way, where `otool -L` names
`libSDL2.dylib`. This got slightly worse when `sound` moved off CoreAudio in
0.1.5: a framework that shipped with the operating system became a library that
has to travel with the binary. That is the cost of the unit making a noise on
more than one operating system, and it is paid at ship time rather than at
build time.

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

## The other programs here

None of these is built or compared — they are run interpreted, by eye, and most
of them want a window and a person:

| | |
| --- | --- |
| `window.a24` | the window itself, and free text through both glyph files — five scripts and emoji, then the same machine one bit per pixel |
| `keys.a24` | `ReadKey` answering one Char per key, every key named as it arrives |
| `video.a24` | `HighVideo`, `LowVideo` and `NormVideo` over the sixteen colors, the Brown/Yellow oddity included |
| `textstyle.a24` | `SetTextStyle` on a ViewPort: four quarter turns and five magnifications, which celled text can have neither of |
| `arcs.a24` | the `Arc` convention drawn so it can be checked by eye — zero at three o'clock, counterclockwise, so 90 degrees is up |
| `figures.a24` | the figures, the thirteen fills woven, and what a boundary `FloodFill` does including when it leaks |
| `surfaces.a24` | the interleave one text plane cannot express — a chart drawn over a window's own text, and a second window dragged over both |
| `ide.a24` | the acceptance piece: the Turbo C++ screen rebuilt from the vocabulary |
| `ide-crt.a24` | the same screen on a terminal — the celled half transfers almost verbatim, the two drawn panels cannot, `crt` having no pixels |
| `piano.a24` | where `graph` meets `sound` — an octave of keys, Ode to Joy, each key lit as it sounds |
| `jukebox.a24` | where the mouse pays off: each title is a ViewPort, so a click is `E.Surface = Titles[I]` rather than arithmetic |

Three of them have caught what the test suite could not, each correct against
every test and wrong in use: `ide.a24` found the escape-sequence race in `crt`'s
key reader, `piano.a24` found the `Delay` ambiguity between `graph` and `sound`,
and `jukebox.a24` found that `CloseGraph` was not resetting the mouse.

Which is also the gap worth naming: `check.sh` compiles and compares
`statistics.a24` alone, so nothing reaching SDL or termios is ever built the
two ways and diffed. The `--static` link is the only check that found the
`alg_file_exists` collision with the compiler's own runtime — and it is the
check that cannot see whether a `graph` or `sound` program links at all, which
had to be verified by hand when `sound` moved to SDL2.
