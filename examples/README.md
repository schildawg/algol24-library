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

**`--static`.** One file, with the library's Algol-24 *and* its C linked in,
so nothing has to sit beside it. It runs from anywhere and prints exactly what
the interpreter prints.

⚠️ **It is NOT self-contained, and this file claimed it was.** The claim here
used to be that `otool -L` showed "only what macOS already ships", namely
`libffi` and `libSystem`. Measured, it shows four things:

```
/usr/lib/libffi.dylib
/usr/lib/libSystem.B.dylib
/opt/homebrew/opt/sdl2-compat/lib/libSDL2-2.0.0.dylib
/System/Library/Frameworks/Cocoa.framework/Versions/A/Cocoa
```

**Homebrew installs SDL2 and macOS does not ship it**, so this binary runs on
this machine because SDL is here, and would fail to launch on one without it.
That is the opposite of what "the mode to ship" implied.

⚠️ **The old reasoning was the real error.** It said self-containment "depends
on which units the program reaches", and `statistics.a24` reaches neither
`graph` nor `sound` — true, and beside the point. `--static` links
`./*ffi.c`, **every shim in this repository**, and passes
`sdl2-config --cflags --libs` unconditionally. So `soundffi.c` and
`graphffi.c` are linked in whatever the program imports, and SDL comes with
them. What the program reaches decides which *units* are in the binary; it
does not decide which *libraries* the binary needs.

`examples/check.sh` passing does not catch this. It proves the three runs
agree, which they do — not that the artifact travels.

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
| `stars.a24` | 3143 real stars and 173 constellation segments, turning a degree a frame — the first example to read a data file, and the only one worth compiling |

`stars.a24` is a port of a Python program that drew with `turtle` and read
coordinates with astropy's `SkyCoord`. Its data is `stars.csv` (the Yale Bright
Star catalog, trimmed) and `lines.csv`. Two things about it are worth knowing:

- **It wants compiling.** Measured on 0.3.0: 0.18 s to load and **112 frames a
  second** compiled, against 3.3 s to load and **0.50** interpreted. Every
  other program here is fine interpreted; this one lays down 1813 stars a frame
  and is the first here to be a slideshow without `algc --compile`. Compiled it
  is now bounded by `FrameDelay` rather than by drawing.
- **`SkyCoord` turned out to be the easy part.** The catalog writes coordinates
  sexagesimally — `6:45:8.9`, `-16:42:58` — and all the program wanted was
  degrees, which is two short functions. The trap is the *sign*: 39 of these
  stars have declinations like `-0:17:57`, Mintaka in Orion's belt among them,
  where the degrees field parses to a zero that has lost which hemisphere it
  was in. Reading the sign from the text rather than the number is the whole
  difficulty, and it is four lines.

Four of them have caught what the test suite could not, each correct against
every test and wrong in use: `ide.a24` found the escape-sequence race in `crt`'s
key reader, `piano.a24` found the `Delay` ambiguity between `graph` and `sound`,
`jukebox.a24` found that `CloseGraph` was not resetting the mouse, and
`stars.a24` found that every drawing verb presented the whole window on every
call — a figure costing an upload of the window's **area** rather than its own.

⚠️ **That is four for four, and none of them was looking.** Each of these
programs was written to exercise a vocabulary, not to test it, and each turned
up something no assertion would have reached: the suite checks what a verb
*computes*, and these found what one *costs*, when it *fires* and what it does
to a terminal or a mouse. `stars.a24` is the sharpest case — the 431 `graph`
tests read pixels through `GetPixel`, which works in logical space, so the
chart was geometrically correct in its own coordinates the whole time and the
drawing verbs returned the right answer however slowly they did it. A program
somebody wants to watch is a different instrument from a test, and the cheapest
way to find this class of fault is to write one.

Which is also the gap worth naming: `check.sh` compiles and compares
`statistics.a24` alone, so nothing reaching SDL or termios is ever built the
two ways and diffed. The `--static` link is the only check that found the
`alg_file_exists` collision with the compiler's own runtime — and it is the
check that cannot see whether a `graph` or `sound` program links at all, which
had to be verified by hand when `sound` moved to SDL2.
