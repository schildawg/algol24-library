# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

The **runtime library for Algol-24**, written in Algol-24.

⚠️ **This project is a *user* of the language, not part of it.** The compiler
lives in a separate repository and is deliberately not vendored here. `algc` is a
tool on `PATH` (`brew install algol24`) and is treated as one: do not build it,
do not read its sources to work out how something behaves, and do not run its
harnesses. Pulling a copy of the specification to work from is fine — the spec is
reference material, the implementation is not.

⚠️ **Turbo Pascal is an homage, not a compatibility target.** Its routine names
and its reference's shape are borrowed deliberately, but "Turbo Pascal did it
this way" is never on its own a reason. Where a better answer exists, take it
and document why — and in particular **do not inherit Pascal's limitations**,
which answered to a 16-bit machine with a bounded `LongInt`. `Trunc` answering
an exact Integer for any finite Double, rather than refusing past 2^63, is the
worked example.

⚠️ **When the Pascal justification is removed, re-derive rather than reverse.**
`Round`'s ties-to-even survived losing its "Turbo Pascal does it" reason,
because the real reasons — IEEE 754 consistency and freedom from bias — were
better ones. Changing code to look responsive is the wrong move.

⚠️ **Do not describe Algol-24 as "Pascal-flavored".** That sells it as a
derivative of an old thing, when the old-looking surface is the deliberate part
and the capability behind it is the point. Pascal may be named as the *syntax's*
lineage, never as the language's identity.

## How work proceeds

**The user names the unit, and the methods within it.** Do not choose the next
unit, expand a unit's surface beyond the methods named, or derive scope from any
document — including anything found in the compiler repository. Wait to be told.

Each assignment carries the same standing expectations, which do not need to be
restated:

- **Full unit test coverage** of the methods named — per method, not per unit.
- **A `REFERENCE.md` entry** for each method. See below.
- **Algol-24 wherever the language allows it.** Reach for `external` only where
  the language genuinely cannot express the thing, never because a foreign call
  would be shorter.
- **C may be written here** to back an `external` declaration. That is
  application code this repository owns, and is not the same as building the
  compiler.

### Gaps and defects are not the same thing

| | What it is | What to do |
| --- | --- | --- |
| **Gap** | the language does not offer it | write an `external` and carry on |
| **Defect** | the language behaves *wrongly* | stop, record it in `DEFECTS.md`, tell the user |
| **Hazard** | the language behaves *as specified*, sharply | record it in `DEFECTS.md`; the library defends itself, the compiler has nothing to fix |

⚠️ **Check the specification before calling something a defect.** The first
finding here was written up as one and turned out to be specified behavior —
[FUN-014] says a mismatched foreign signature is undefined behavior and outside
the specification. It is now hazard H-1.

⚠️ **Verify a fix before closing it, and check what it did NOT change.**
0.1.4 closed L-2 and L-3; confirming it meant re-running the reproductions,
checking the narrow fix was narrow (an ambiguous overload must still warn),
and running the whole suite against the new compiler. One half of L-2 was
untouched and continues as L-4.

⚠️ **Never fix a defect, never work around one silently, and never open the
compiler to diagnose one.** The user creates the defect in the compiler project
and fixes it there. `DEFECTS.md` is the list.

⚠️ **If you find yourself needing to build the language, something has gone
wrong with the arrangement.** Say so rather than pushing through.

## Commands

```sh
./test.sh                 # everything: units, reference examples, the example app
./test.sh math.a24        # one unit

./build.sh                # the library's own C -> lib*ffi.dylib
./check-reference.py      # run every REFERENCE.md example, diff against its output
examples/build.sh         # the example app; --static for a self-contained binary
examples/check.sh         # the app must agree interpreted and both ways compiled

algc --test math.a24      # one unit directly, with the full report
algc math.a24             # run a program
```

⚠️ **Run from the repository root.** `uses` resolves beside the importing file
and then the working directory, and `external … in 'libmathffi.dylib'` is found
the same way.

Unit tests are the whole of the testing story. Conformance cases, refusals and
interpreted-versus-compiled comparison belong to the compiler project.

⚠️ **There is no per-test filter, and no per-file one either.** `--test X.a24`
runs every test block reachable from `X.a24`, its imports included. The narrowest
thing that can be run is a leaf unit.

## Writing a unit

Units are flat in the repository root, so `uses math;` resolves for a consumer in
a sibling directory or the working directory. Unit names are lowercase.

The house style, which `../algol24.com/gen/Strings.a24` models well:

- A `///` doc block above the `unit Name;` header, which comes **before** any
  `uses` clause, and above every declaration.
- The summary is one sentence ending in a full stop, no longer than a second
  line, saying something the signature does not.
- Sections in order: `# Raises`, `# Safety`, `# Examples`, `# Satisfies`,
  `# Since`. `# Examples` **cites a test by name** rather than carrying a
  fragment.
- ⚠️ **No `⚠️` in a doc comment.** It reads as a hazard needing action, where
  what is being recorded is settled knowledge. State the constraint in the
  paragraph's first sentence instead. The convention belongs in files like this
  one.
- Tests inline at the foot of the unit — `test 'Name'; begin … end`, using
  `AssertEqual`, `AssertTrue` and `Fail`, plus `AssertNear` from `testing`.
- ⚠️ **Assert Doubles exactly where IEEE 754 guarantees it, and to a tolerance
  where it does not.** The arithmetic operations and `Sqrt` are required to be
  correctly rounded, so `AssertEqual (3.0, Sqrt (9.0))` holds on any conforming
  platform. Nothing is required of `Sin`, `Cos`, `Exp`, `Ln` or `ArcTan`, whose
  last bit may differ between one libm and another — those get `AssertNear`.
  Using a tolerance where exactness holds only weakens the test.
- ⚠️ **`graph` shadows `Write` and `WriteLn`, so a graph program reports to
  the console through `System.Write` and `System.WriteLn`.** The built-ins
  stay reachable under their own unit's name, variadic and untouched, and
  every `graph` example in `REFERENCE.md` uses them. Getting this backwards
  is easy: a bulk rename that qualifies a *grid* write sends window text to
  the console, which `check-reference.py` catches only because the example
  then prints the wrong thing.
- ⚠️ **`Fail` and the assertions exist only under `--test`.** A helper that
  calls `Fail` answers `Undefined variable 'Fail'.` in an ordinary program, so a
  `REFERENCE.md` example for one has to be a `test` block.

## Writing C for a unit

⚠️ **A foreign declaration must name a C function whose *real* signature is the
one wanted.** Nothing checks this — [FUN-014] says a mismatch is undefined
behavior and outside the specification — so the failure is a plausible wrong
number, not an error. See `DEFECTS.md` H-1.

⚠️ **When no C function has the wanted signature, write one.** Do not bend the
declaration to fit what libm happens to offer. `Trunc` is the worked case: libm's
`trunc` returns a `double` and there is no `lltrunc`, so `mathffi.c` supplies
`alg_trunc` returning an `int64_t`.

- C sources are named `<unit>ffi.c` and build to `lib<unit>ffi.dylib`
  (`.so` off macOS) — `./build.sh` picks up `./*ffi.c` automatically.
- Reach them with `external 'alg_thing' in 'libmathffi.dylib'`. Without the
  `in` clause the symbol is looked for in the running program, which is `algc`
  itself and will not contain ours.
- ⚠️ **The library name carries a platform extension**, and `in 'libmathffi'`
  does **not** resolve — it must be the full file name. That makes the
  declaration macOS-specific as written.
- ⚠️ **A foreign declaration cannot be written once to serve both processors.**
  With `in 'lib….dylib'` the interpreter works and a compiled binary dlopens the
  library at run time *even when the symbol is linked into it*. Without the `in`
  clause a compiled binary is self-contained and the interpreter cannot resolve
  the symbol at all. `DYLD_INSERT_LIBRARIES` does not bridge it. The units keep
  the `in` clause because the tests run interpreted; `examples/build.sh --static`
  rewrites it out of a throwaway copy. See `examples/README.md`.
- ⚠️ **Keep the logic in Algol-24 and the C minimal.** Range checks and raises
  belong in the unit, where they are readable and testable; the C function
  should be only the primitive the language genuinely lacks. Make it total
  anyway, so a wrong caller gets a defined answer rather than UB.
- **Every `external` gets a test asserting a known value**, never merely that
  the call returns.
- ⚠️ **A stateful unit keeps its state out of `math`.** `math` is entirely pure
  functions, which is why `uses math` has no consequences and its tests can
  assert exact values. `random` is separate for that reason, not for tidiness.
  A test of anything stateful seeds it first, so the suite stays deterministic.

## The Library Reference

`REFERENCE.md` is patterned after Turbo Pascal's reference. Entries are
**alphabetical across the whole library**, not grouped by unit, and each has
exactly these sections in this order:

**Name** (with kind and unit) · **Function** · **Declaration** · **Remarks** ·
**See also** · **Example**

⚠️ **A documented signature must be the real one, parameter names included.**
The language has named arguments, so `Line (X1: 10, …)` is a call a reader
may write from the reference — which makes a parameter renamed for the
implementation's convenience a lie the reader trips over. Where a parameter
would shadow a field, name it for what it is anyway and reach the field
through `this`.

⚠️ **Every example must be run before it goes in, and its stated output must be
what it actually printed.** The reference is what a user trusts when the source
is not to hand, so an example that does not compile — or that claims output it
does not produce — is worse than no example. They are cheap to re-verify in
bulk: extract each ```algol24``` block paired with its ```console``` block, run
it, and diff.

⚠️ **A reference entry is part of the assignment, not a later tidy-up.**

## Language points that bite

Learned here, by use:

- ⚠️ **A type-preserving routine must be written untyped, and that costs the
  caller something.** `Abs`, `Sqr`, `Min` and `Max` take their result type from
  their argument, which falls out of the arithmetic rather than being arranged.
  A value of type `Any` is not assignable to a written type, so
  `var N : Integer := Abs (-5);` is **refused**, while `Abs (-5) + 1`,
  `Take (Abs (-5))` and `var N := Abs (-5);` are all fine.
- ⚠️ **A type error inside a `uses`d unit reports the wrong line.** The caret
  lands on that line number of the *root* file. The message is right; the
  location is not.
- ⚠️ **A `test` block takes no `var` section.** `test 'X'; var … begin` is
  refused with `Expect 'begin' before test body.` — declare inside the body
  instead, as `var Raised := False;`.
- ⚠️ **Negative zero cannot be made with `0 - 0.0`**, which is `+0.0` by IEEE.
  Write the literal `-0.0`. And `-0.0 = 0.0` is **true**, so an assertion about
  the sign of zero has to compare `Str (…)` or the sign of `1.0 / …`; comparing
  the values passes whether or not the code under test is right.
- ⚠️ **`else` is preceded by `;`, which is the opposite of Pascal.**
  `if C then A := 1; else A := 2;` compiles; leaving the semicolon out answers
  `Expect ';' after expression.` Standard Pascal forbids exactly that semicolon,
  so the habit is actively wrong here.
- `const` is declared with `:=`, not `=` — `const Pi := 3.14…;`.
- `mod` keeps the sign of its left operand, so `-3 mod 2` is `-1`. An oddness
  test must be `<> 0`, never `= 1`.
- `/` is **real division** and always answers a Double. `div` and `mod` are the
  Integer pair, both refuse a Double rather than truncating, and both are the
  only things that raise on a zero divisor — `7 / 0` is `Infinity`.
- Strings are 0-based, `Pos` answers `-1` when absent, and there are **no
  backslash escapes** — `#10` is a newline, `#27` is ESC.
- A one-character literal is a **Char**, never a String, and the two are never
  equal. `Str(…)` widens, which is why assertions on single characters read
  `AssertEqual (Str ('x'), …)`.
- An apostrophe inside a string literal is doubled — `'Argument''s'`.
- Truthiness: `nil`, `False`, the Integer `0` and an enum member with ordinal 0
  are falsey; `0.0`, `''` and any Char are truthy.
- Accumulate text with a `Buffer` for anything large.

## Editor

The VS Code extension is symlinked from the compiler repository:

```sh
ln -s ~/workspace-copilot/algol24/vscode ~/.vscode/extensions/algol24-0.1.0
```

Since 0.1.4 it finds a compiler rather than assuming one — the
`algol24.compilerPath` setting, then `bootstrap/algc` in the workspace, then
`algc` on **PATH**, which is this repository's case — so both its Interpreted
and Compiled test profiles work here with nothing to arrange. The
`bootstrap/algc` shim this repository used to carry is gone.

⚠️ **Code Runner is told separately, in `.vscode/settings.json`.** It knows
nothing about `.a24` and answers `Code language not supported or defined.`
until mapped, which is what that file does — for the language id *and* the
file extension, since the second is what works when the Algol-24 extension
is not loaded. It runs `cd "$workspaceRoot" && algc …`, and the `cd` is not
decoration: `uses` and `external … in 'lib….dylib'` both resolve against the
working directory, so a demo run from `examples/` finds neither its units nor
the library's own C without it.

Run Code is interpreted only, deliberately; the extension's own **Run File
(Compiled)** and **Run File Through Both Processors** cover the rest.

## Status

| Unit | Members | Tests | Reference |
| --- | --- | --- | --- |
| `testing` | `AssertNear` | 9 | complete |
| `math` | `Abs`, `Sqr`, `Min`, `Max`, `Odd`, `Frac`, `Pi` in Algol-24; `Sqrt`, `Exp`, `Ln`, `Sin`, `Cos`, `ArcTan`, `Int`, `Round` as `external` onto libm; `Trunc` exact over any finite Double via `mathffi.c`; `IsNaN`, `IsInfinite`, `NaN`, `Infinity` | 69 | complete |
| `random` | `Random`, `RandomInteger`, `RandomReal`, `Randomize`, `SetSeed`; `drand48` declared directly, seeding via `randomffi.c` | 17 | complete |
| `graph` | `InitGraph`, `CloseGraph`, `Delay`, `GetMaxX`, `GetMaxY`, `GetAspectRatio`, `ScreenWidth`, `ScreenHeight`, `OutText`, `OutTextXY`, `InstallUserFont`; text mode: `Write`, `WriteLn` (variadic, shadowing the built-ins), `GotoXY`, `WhereX/Y`, `TextColor`, `TextBackground`, `Clear` (on both surface kinds, aliased `ClrScr` and `ClearViewPort`), `ClrEol`, `DelLine`, `InsLine`, `TextCols/Rows`, `TextMode` (logical 80×25 grid, any size, GPU-scaled), `HighVideo/LowVideo/NormVideo`, `Blink` (bit 24, on the language's `clock ()`), `SetBlinkRate`, `KeyPressed`, `ReadKey`, 24 key constants incl `KeyClose`; the `Window` and `ViewPort` surface classes, stacked by `Order` around the root grid, every method also a surface-first alias; `ViewPort.SetTextStyle` turns and magnifies free text, `TextWidth`/`TextHeight` measure it in the turn and size in force, `SetTextJustify` places it about a point, `SetBkColor` says what `Clear` returns a viewport to, `GetFillPattern`, `GetFillSettings` and `GetLineSettings` read the pens back, `GetImage`/`PutImage` lift and lay a rectangle of pixels with six put modes (`TransparentPut` being the library's own) and `ImageSize` prices one, `SetWriteMode` puts the pen in XOR so a figure drawn twice leaves no mark, `SetUserCharSize` scales text by a ratio either way, `GetTextSettings`/`GetViewSettings` and `GraphDefaults` complete the pen state, `SetAspectRatio` overrides the measured stretch, `SetPalette`/`GetPalette` rebind the sixteen names per surface (a color equal to one of them is a name, anything else a literal; `PutPixel` and `PutImage` bypass); the pen — `Line`, `LineTo`, `LineRel`, `Rectangle`, `PenTo`, `PenRel`, `GetX/Y`, `SetColor`, `GetColor`, `SetLineStyle` with five styles and any thickness, `Arc`, `Circle`, `Ellipse`, `DrawPoly`, `FillPoly` (scanline in Algol-24, spans in C), `SetFillStyle`/`SetFillPattern` with the thirteen fill patterns, `Bar`, `Bar3D`, `PieSlice`, `Sector`, `FillEllipse` (filled in C, the run being what C is for), `FloodFill` (a boundary fill, the one figure that reads the surface back) and its companion `GetPixel`, `GetArcCoords` answering an `ArcCoords` recorded by all six curve verbs; `CellWidth`/`CellHeight` and `Window.PixelLeft/PixelTop` place a ViewPort against a Window's cells, 16 CGA colors, `Transparent`; `graphffi.c` carries the hex decoder, cell stamper and scroller | 367 | complete |
| `crt` | `graph`'s text mode on a terminal: `InitCrt`, `CloseCrt`, `IsTerminal`, `TextCols/Rows` (the real terminal, following a resize), `Write`, `WriteLn` (variadic, shadowing the built-ins), `GotoXY`, `WhereX/Y`, `TextColor`, `TextBackground`, `HighVideo/LowVideo/NormVideo`, `Clear` (aliased `ClrScr`), `ClrEol`, `DelLine`, `InsLine`, `SetPalette`/`GetPalette`, `KeyPressed`, `ReadKey` with the same 23 key constants, the `Window` class stacked by `Order`, `MoveTo`, `Show`; the sixteen colors, `Transparent` and `Blink` declared a second time rather than shared; `crtffi.c` carries the tty and termios calls, the compositor, the escape transcoder and the nap that keeps `ReadKey` from spinning; the mouse through SGR reports, with the same `Event`, `SurfaceAt` and verbs `graph` has | 35 | complete |

⚠️ **`graph` has a design document, `DESIGN.md`, and it governs.** The unit is
one world — celled text on a grid at Order 0, Canvas objects above and below
it — not Turbo Pascal's Crt/Graph split. Read it before adding any `graph`
method; a method that fights it is wrong until the document says otherwise.

`graph` notes: SDL2 comes from Homebrew (`brew install sdl2`) and every
declaration names `/opt/homebrew/lib/libSDL2.dylib` literally, because `in`
takes a string literal — an SDL elsewhere means substituting the path
throughout. `./test.sh` and `./check-reference.py` set `SDL_VIDEODRIVER=dummy`
so tests and examples open no real windows; running `algc --test graph.a24`
directly uses the real display, which is harmless and occasionally worth
watching. On macOS, fullscreen keeps the menu bar, so a fullscreen window is
the desktop's width but short of its height — `GetMaxX`/`GetMaxY` report the
window that actually opened. Text draws through glyph files —
`graphfont.hex` (antialiased, with emoji) is the default and `romfont.hex` the
1-bit alternative; both resolve against the working directory, and
`InstallUserFont` selects any other. A unit-defined `WriteLn` **replaces** the
built-in rather than overloading it, which is why the text routines are
`OutText`/`OutTextXY` and not a window-bound `WriteLn`. `graphffi.c` carries the hex
decoder, the grid's cell stamper and its scroller — transcoding and blitting,
not logic; the unit says which glyph, what mode, what ink, where. The stamper
exists because a full 80×25 screen is 2,000 cells and the interpreted blend
loop (~18 ms/glyph) would have taken half a minute per screenful; with it a
whole IDE screen paints in ~0.4 s interpreted. Free text (`OutTextXY`) still
blends in Algol-24. The screen is 1-based, cells and pixels
alike — `(1, 1)` is home, `GotoXY (TextCols (), TextRows ())` the far corner,
`GetMaxX ()` the width itself — while the language's strings stay 0-based;
the seam is `Text[Col - 1]`, at the memory boundary. `examples/ide.a24` is
the acceptance piece — the Turbo C++ screen rebuilt from the vocabulary.
| `sound` | `Sound`, `NoSound`, `SetVolume`, `Delay`; BASIC's `Play` with `Score` answering the notes without sounding them; `LoadSound`, `PlaySound`, `SoundLength`, `SoundPlaying`, `StopSounds`, `FreeSound`, `CloseSound`; `soundffi.c` is CoreAudio -- a square wave mixed with up to sixteen decoded samples | 13 | complete |
| `file` | `ReadAll`, `WriteAll`, `AppendAll`, `ReadLines`, `WriteLines`, `Exists`, `IsDirectory`, `FileSize`, `Erase`, `Rename`, `CopyFile`; `fileffi.c` is plain C99 and the library's first foreign code that builds anywhere | 9 | complete |

⚠️ **`file` owns its primitive rather than using the core's `TextFile`.**
That class exists for the compiler and is being pared to what the compiler
needs; the library reads exactly one file -- a glyph file, in
`InstallUserFont` -- and now does it through `file`. Nothing in the core is
needed for file I/O: C can take a path as a String and answer a count, which
is all `fileffi.c` uses.

⚠️ **`alg_` is the runtime's prefix, and a common noun after it will
collide.** `fileffi.c` first used `alg_file_exists`, which the compiler's own
runtime already defines. A shared library hides such a clash -- the
interpreter resolves ours first -- so it appeared only in
`examples/build.sh --static`, where everything is linked into one executable.
That build is the last thing `./test.sh` runs, and it is the only check that
would have caught it. Library C is now `alg_libfile_*`; name new C so that it
cannot be anything but ours.

⚠️ **No foreign function here may answer a pointer.** A `String` returned from
C **borrows** the bytes rather than copying them, so a C function that reuses
its buffer corrupts a String still in hand -- silently. `fileffi.c` fills a
caller-owned Buffer and answers a count instead. `DEFECTS.md` H-2 has the
measurement. Going the other way is safe: a String passed *into* C is read
during the call and not kept.

⚠️ **A Buffer must be sized exactly to become Text.** `Buffer.Text` refuses a
buffer holding a zero byte, so a buffer larger than its contents fails on its
own trailing zeros rather than answering a truncated String. That is why
`ReadAll` asks `alg_file_size` first.

⚠️ **Sound is its own unit for a portability reason, and must stay there.**
`soundffi.c` reaches CoreAudio, which is a system framework on macOS and
exists nowhere else. `crt` calls only POSIX -- termios, ioctl, nanosleep --
and would build on Linux tomorrow but for the hardcoded `.dylib`; folding the
noise into it would narrow that unit to one operating system for the sake of a
beep. `Delay` is in `crt` and `graph` and deliberately **not** in `sound`: a third
copy made a bare `Delay` ambiguous in the two combinations worth writing --
`graph` with `sound` is every game, `crt` with `sound` every console program
that beeps -- and taxing those with `graph.Delay` to serve a rarer case was the
wrong way round. `Play` times itself; a program timing raw tones by hand says
`uses crt`, whose `Delay` wants neither a terminal nor `InitCrt`.

⚠️ **Two units exporting one name is ambiguous at the call, not at the
`uses`.** `uses graph; uses sound;` loads fine and then answers `'Delay' is
ambiguous: graph or sound.` when the name is used. Qualifying works --
`graph.Delay (10)` -- but a name worth having in two units is usually a name
that should be in one.

`sound` notes: `ALG_SOUND=dummy` computes everything and plays nothing, which
`test.sh` and `check-reference.py` both set -- the same bargain
`SDL_VIDEODRIVER=dummy` strikes for `graph`. Under it `Play` still takes
exactly as long and `LoadSound` still decodes, but `PlaySound` answers False,
there being no voice to start on a device that was never opened. `Score`
answers an MML string's notes without sounding them, which is what makes the
dialect testable. `build.sh` links CoreAudio frameworks for `soundffi.c` only,
and `examples/build.sh --static` does the same.

⚠️ **`crt` duplicates `graph`'s constants deliberately, and must not be
refactored to share them.** `uses` is **not transitive**: a program saying
`uses crt` cannot see names a unit `crt` itself uses. Factoring the sixteen
colors and the keys into a shared unit would force every consumer to say
`uses` of that unit as well, merely to write `LightCyan` — breaking every
program, example and reference entry at once. One import for the thing you
actually want beats one definition.

`crt` notes: nothing is emitted unless standard output is a terminal, so a
piped run produces its own output and nothing else — which is also what makes
the unit testable with no tty, the same discipline `SDL_VIDEODRIVER=dummy`
gives `graph`. The screen composites in memory and paints by difference: a
frame that changes six cells sends six cells, and a frame that changes nothing
sends nothing. `alg_crt_paint` deliberately does **not** write; the unit
decides whether anything reaches the terminal, which is what lets a test paint
a screen and read the escapes back through `EscapeText ()`. Colors go out as
24-bit where `COLORTERM` says truecolor, and as the nearest of the 256-color
cube otherwise. `Blink` is the terminal's own cadence, so there is no
`SetBlinkRate`. `Alpha` is 0 or visible, a cell holding one character. `ReadKey` naps ten milliseconds between polls through `alg_crt_nap`, so waiting for a key is a wait rather than a spin -- `graph` delays through SDL and this unit has no SDL. ⚠️ **The language's `clock ()` answers SECONDS**, not milliseconds; reading it the other way makes every duration wrong by a thousand.
`examples/ide-crt.a24` is the acceptance piece — `examples/ide.a24` with three
lines changed.

⚠️ **Language points `crt` and `sound` learned the hard way**: it is
`Char (65)`, not `Chr`; `#0` is **not a Char**, so NUL cannot be a sentinel;
`List` has no `Remove`, so a queue keeps a head index; `Buffer` has `PutInt`/`GetInt` for words **and
indexes by byte** -- `B[4]` reads and `B[4] := 200` writes, which `graph`'s
key decoder uses and `crt`'s `ByteAt` predates; `end` before `else` takes **no** semicolon,
where a simple statement before `else` takes one; a one-character map key is a
**Char**, so `Str (C) in Map` never matches a `'C' : 0` entry; **Lists and
Maps compare by identity, not by content** -- `AssertEqual` on two
equal-looking Lists always fails, so a comparison has to walk them; and the
**Char versus String seam bites constantly** -- it has now cost this session
three separate bugs (`sound`'s note table, `file`'s tests, `file`'s reference
example), always as `X = 'a'` silently answering False where `X` is a String.
Write `Str ('a')` on the literal side.

`examples/piano.a24` is where `graph` and `sound` meet: one octave of keys,
Ode to Joy, each key lit as it sounds. It is a walk over what `Score` answers
and knows nothing about music itself, which is the point. Lighting a key is
two filled rectangles rather than a redraw -- a white key lights only below
where the black keys end, so it never has to repair a border or a neighbor.

⚠️ **The mouse is off until a program asks.** Turbo Vision always had it on
and owned the screen; a library's default should cost nothing. Off means a
key-reading program is untouched, nothing accumulates in a queue nobody
drains, and `crt` never sends tracking escapes a terminal would print as junk.
`ReadKey` still answers keys alone and discards mouse events it passes over,
so nothing written before the mouse existed changed meaning.

⚠️ **A mouse coordinate must be mapped back into the logical space.**
`graph` scales that space to the window, so SDL's window pixels need the
inverse -- `WindowToLogicalX` -- or every coordinate is wrong at any window
size but one to one, which reads as bad aim rather than as a bug. `crt` needs
no mapping, a terminal reporting cells already.

`examples/jukebox.a24` is where the mouse pays off: each tune title is a
ViewPort of its own, so identifying a click is `E.Surface = Titles[I]` rather
than arithmetic on coordinates, and the keyboard uses `E.LocalX`/`LocalY`
because it is one surface. Turbo Vision computed both by hand in `TView`.

⚠️ **The examples keep catching what the tests cannot.** The interactive IDE
found the escape-sequence race in `crt`'s key reader; the piano found the
`Delay` ambiguity; the jukebox found that `CloseGraph` was not resetting the
mouse. All three were correct against every test and wrong in use.

`examples/statistics.a24` is the worked application — built by
`examples/build.sh`, verified by `examples/check.sh`, explained in
`examples/README.md`. It is the only check that exercises the whole story rather
than one unit, so it is the one to run when anything about linking changes.

Build artifacts — `lib*ffi.dylib` and `examples/build/` — are generated and
should not be committed.
