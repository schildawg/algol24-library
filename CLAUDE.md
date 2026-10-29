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

./build.sh                # the library's own C -> lib*ffi.so
./check-reference.py      # run every REFERENCE.md example, diff against its output
examples/build.sh         # the example app; --static for a self-contained binary
examples/check.sh         # the app must agree interpreted and both ways compiled

algc --test math.a24      # one unit directly, with the full report
algc math.a24             # run a program
```

⚠️ **Run from the repository root.** `uses` resolves beside the importing file
and then the working directory, and `external … in 'libmathffi.so'` is found
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

- C sources are named `<unit>ffi.c` and build to `lib<unit>ffi.so` — **`.so`
  on every platform, macOS included** — and `./build.sh` picks up `./*ffi.c`
  automatically.
- Reach them with `external 'alg_thing' in 'libmathffi.so'`. Without the
  `in` clause the symbol is looked for in the running program, which is `algc`
  itself and will not contain ours.
- ⚠️ **`.so` everywhere is deliberate, and it is what makes the units
  portable in place.** `dlopen` reads a file's contents and not its extension
  — measured: a Mach-O built with `-dynamiclib` and named `libprobeffi.so`
  loads on macOS and its symbol resolves. Since an `in` clause takes a string
  literal and cannot ask what host it is on, a `.dylib` spelling would have
  forced every source to be rewritten per platform, the way
  `shroom-slots` rewrites its own. One spelling serves macOS, Linux and the
  BSDs instead. Only `build.sh`'s *flag* differs — `-dynamiclib` against
  `-shared -fPIC` — so `file lib*.so` says Mach-O here and ELF on Linux.
- ⚠️ **The name must still be the full file name.** `in 'libmathffi'` does
  **not** resolve.
- ⚠️ **POSIX declarations need a feature-test macro, and macOS needs a second
  one.** Under `-std=c11` glibc and musl hide `nanosleep`, `strdup`,
  `localtime_r`, `gmtime_r`, `clock_gettime` and the `drand48` family, where
  Apple's headers expose them whatever the standard asked for — so four of the
  shims failed to compile on Linux and nothing here would have shown it.
  `_XOPEN_SOURCE 700` asks for POSIX.1-2008 plus XSI; on macOS that makes the
  headers *stricter* and hides Apple's own additions, `_NSGetEnviron` and
  `TIOCGWINSZ` among them, so `_DARWIN_C_SOURCE` has to go back alongside it.
  Both lines precede every include. `crtffi.c` is the model.
- ⚠️ **A foreign declaration cannot be written once to serve both processors.**
  With `in 'lib….so'` the interpreter works and a compiled binary dlopens the
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

## Portability, and what has actually been measured

Checked on 2026-10-25 by compiling every shim in an Alpine container — gcc,
musl, `-std=c11 -O2 -Wall -Wextra`:

| | |
| --- | --- |
| `crtffi.c` `dosffi.c` `fileffi.c` `mathffi.c` `randomffi.c` `timeffi.c` | compile clean on Linux, **0 warnings** |
| `soundffi.c` `graphffi.c` | compile clean on Linux against `sdl2-dev`, **0 warnings** — `graphffi.c` including its 22 SDL wrappers |
| the six POSIX shims again | cross-compile for **Windows** with mingw-w64, **0 warnings** |
| all ten units | still pass on macOS afterwards |

⚠️ **Four of those six POSIX shims did NOT compile on Linux before this, and
nothing on macOS would have revealed it.** Apple's headers expose POSIX
declarations whatever `-std=` asked for; glibc and musl hide them. That is what
the `_XOPEN_SOURCE` / `_DARWIN_C_SOURCE` preamble is for, and why a portability
claim about this library should be compiled rather than reasoned about.

**What is NOT verified, and should not be claimed:**

- ⚠️ **No unit has been *run* on Linux**, only compiled. `algc` here is a macOS
  binary, and building the compiler is outside this repository's remit — so the
  Algol-24 half of every unit is unexercised off macOS. Compiling the C is the
  most this project can honestly check by itself.
- ⚠️ **Windows is not supported and is not close.** `crtffi.c` wants the Win32
  console API in place of termios and `ioctl`; `dosffi.c` wants
  `GetDiskFreeSpaceEx` and `CreateProcess` in place of `statvfs` and
  `posix_spawn`. Both are real backends behind the same function signatures,
  not a flag.
- ⚠️ **Windows compiles and has never been run.** Every shim cross-compiles
  with `x86_64-w64-mingw32-gcc`, 0 warnings, so the `#ifdef _WIN32` branches
  are at least well-formed against the real Win32 headers. Nothing beyond
  that is known: no unit has executed on Windows, and the parts most likely
  to need work on real hardware are `crt`'s virtual-terminal flags and its
  non-blocking console poll. Treat the Windows branches as a scaffold that
  compiles, not as support.

### The Windows branches, and what they cost

⚠️ **`crtffi.c` keeps one implementation and a platform layer of six
primitives**, not scattered `#ifdef`s: `tty_is_terminal`, `tty_size`,
`tty_raw`, `tty_read_byte`, `tty_sleep_ms` and `tty_write`. Everything above
them is platform-free. Add a seventh only if a real host forces it.

Three things in that port are worth knowing:

- **`ENABLE_VIRTUAL_TERMINAL_INPUT` is load-bearing.** With it the console
  translates arrow and function keys into the same ANSI escapes the unit's
  decoder already reads, so the key reader needs no Windows branch at all.
  Without it they arrive as Windows key records and nothing decodes them. The
  output side needs `ENABLE_VIRTUAL_TERMINAL_PROCESSING` for the same reason,
  and both want Windows 10 1511 or newer.
- **The console poll must peek, not just count.** A console input queue holds
  resize, focus and mouse records as well as keys, so asking whether the queue
  is non-empty and then reading would block on a resize. Non-key records are
  discarded until a key is at the head.
- ⚠️ **`Exec` cannot keep its POSIX contract on Windows.** `CreateProcess`
  takes a command *line* and offers no vector form, which is the exact hazard
  `Exec` exists to avoid — so `dosffi.c` rebuilds the line with
  `CommandLineToArgvW`'s quoting rules, doubling backslash runs before a quote
  so the child reconstructs the arguments that went in. And Windows has no
  signals, so the documented "128 plus the signal number" has no meaning
  there: a faulting process answers its NTSTATUS, a large number rather than a
  small one.

⚠️ **`random` stopped borrowing libc's `drand48`, and that was a Windows
fix.** The unit declared it with no `in` clause, resolving out of the running
program — fine wherever libc has the XSI family, impossible on Windows, which
has none. `randomffi.c` now carries the generator. POSIX specifies the
algorithm exactly rather than leaving it to an implementation, so this changes
no stream: measured bit-identical against the platform's own over 4000 draws
across four seeds, and all five `random` reference examples still print what
they claimed. A seed now answers the same sequence on every platform, which it
did not before.

### How `graph` stopped being macOS-bound

It used to declare SDL's own entry points, naming
`/opt/homebrew/lib/libSDL2.dylib` in twenty-four `external` clauses. Renaming
our own shims could not fix that: the literal was an absolute path to somebody
else's library, and SDL is `/usr/lib/<triple>/libSDL2-2.0.so.0` on Debian,
`/usr/lib64/…` on Fedora, elsewhere again on Alpine and MSYS2.

So `graphffi.c` now carries a thin wrapper per call and the declarations name
`libgraphffi.so`, with `sdl2-config` resolving SDL at build time — the pattern
`sound` already used by linking SDL in C rather than declaring it.

⚠️ **The declarations keep SDL's own names**, so a call site still reads as
the SDL call it is; only where it resolves from changed. Do not rename them to
match the wrappers.

**It also closed two bugs, which is why this was worth more than portability.**

1. `SDL_GetError` and `SDL_GetWindowTitle` answered a `String` from C, which
   this file forbids outright — a String returned from C borrows the bytes
   (H-2). `SDL_GetError` had nine call sites and two held the borrowed pointer
   across further SDL calls, one across the whole of `CloseGraph`, so the
   message raised could be whatever SDL had written since. Both now copy into
   a caller-owned Buffer. **No foreign declaration in this library answers a
   String any more**, and that is worth keeping true.
2. Twenty-four hand-copied prototypes were twenty-four unchecked signatures
   (H-1). A wrapper is compiled against SDL's real headers, so a mismatch is a
   compile error instead of a plausible wrong number.

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

- ⚠️ **Bitwise operators arrived in 0.1.6 — `&`, `|`, `^`, `~`, `<<`, `>>`
  [EXP-023, EXP-024] — and `and`/`or` are still NOT them.** The old hazard
  remains underneath: `and` and `or` test truthiness and yield an *operand*
  [EXP-010], so `6 and 3` is **3** where `6 & 3` is 2. Since 0.1.6 handing
  either an Integer **warns** and names the bitwise operators, which closes
  most of `DEFECTS.md` H-3 — but ⚠️ **that warning goes to stdout**, so a unit
  using `and` on an Integer would add a line to every consumer's output. That
  is L-4's third trigger. No unit here does; checked by compiling all ten.
- ⚠️ **`^` is FASTER than a foreign call, so bit work needs no C.** Measured
  over three runs: the native operator 1.87 µs a call against `alg_xor`'s
  7.6 µs, the FFI overhead dwarfing a one-instruction operation — and against
  547 µs for the hand-rolled `div`/`mod` version this file used to recommend
  against. `graph`'s `XorWords` and its C are gone.
- ⚠️ **`^` does not narrow to 32 bits, where the C it replaced did**, and that
  changed a behavior for the better. `alg_xor` masked and sign-extended, so an
  out-of-range color silently drew a wrong pixel in `XorPut` while `CopyPut`
  already raised on the same value. `Buffer.PutInt` refuses anything outside
  int32, so the operator makes the two write modes agree. `SetColor` is now
  bounded at `White + Blink` as well, which stops such a color reaching either.
- ⚠️ **`div` truncates toward zero, and `>>` floors, so they are not the same
  thing.** `-7 div 2` is `-3` where `-7 >> 1` is `-4` — and now that both
  spellings exist this matters more, not less, since they disagree on exactly
  the negative values nobody checks. A right shift is **arithmetic**: the sign
  carries in. The `+ 4294967296` in `graph` and `crt` brought a negative word
  into range before a `div`; after a shift it is unnecessary, because a
  following `& 255` discards the sign-extended bits — measured on `-1` and
  `-16777216` at three widths. Those call sites are not yet converted.

- ⚠️ **An extension's visibility is per-file and non-transitive, so every
  consumer must say `uses` of the unit itself.** Since 0.1.5 a top-level
  subprogram marked `extension` may also be called on its first parameter —
  `Trim (S)` and `S.Trim ()` both, always [FUN-015]. But [MOD-015] finds one
  only among the extensions visible in the file being compiled, so a unit that
  `uses strings` internally does **not** give *its* importers `S.Trim ()`.
  Measured: a program importing a unit that imports the extension reaches the
  ordinary function fine and answers `Undefined property 'Upper'.` for the
  method spelling. This is the same non-transitivity that made `crt` duplicate
  `graph`'s constants, and it applies to method syntax too.
- ⚠️ **A member of the name always wins, and the warning that says so goes to
  stdout.** `x.f` resolves to a member first [EXP-022] and reaches an extension
  only where the program was an error before, so nothing already written can
  change meaning. Where a member hides a visible extension the compiler warns
  and names the class — onto **stdout**, which is `DEFECTS.md` L-4. Any
  extension this library exports must therefore be named clear of the member
  names on its own classes, or a consumer's output gains a line it did not
  print.
- ⚠️ **Extensions do not remove the surface-first aliases.** The 59 delegates
  in `graph` and `crt` go the other way — they give a *method* a free-function
  spelling, `Line (V, …)` for `V.Line (…)` — where `extension` gives a function
  a method spelling. `ViewPort` already has a `Line` member, so step 1 of
  [EXP-022] answers it; there is nothing for an extension to add. The aliases
  stay hand-written.
- An extension's receiver must have a **written type**, and a declaration with
  no parameters at all is refused. `extension` is also refused on a method, a
  constructor, a nested subprogram and a `property`. It is **not** a keyword
  [LEX-011] — `var extension := 8;` still compiles.

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
decoration: `uses` and `external … in 'lib….so'` both resolve against the
working directory, so a demo run from `examples/` finds neither its units nor
the library's own C without it.

Run Code is interpreted only, deliberately; the extension's own **Run File
(Compiled)** and **Run File Through Both Processors** cover the rest.

## Status

| Unit | Members | Tests | Reference |
| --- | --- | --- | --- |
| `testing` | `AssertNear` | 9 | complete |
| `math` | `Abs`, `Sqr`, `Min`, `Max`, `Odd`, `Frac`, `Pi` in Algol-24; `Sqrt`, `Exp`, `Ln`, `Sin`, `Cos`, `ArcTan`, `Int`, `Round` as `external` onto libm; `Trunc` exact over any finite Double via `mathffi.c`; `IsNaN`, `IsInfinite`, `NaN`, `Infinity` | 69 | complete |
| `random` | `Random`, `RandomInteger`, `RandomReal`, `Randomize`, `SetSeed`; `drand48` carried in `randomffi.c` rather than taken from libc, bit-identical and so portable, seeding via `randomffi.c` | 17 | complete |
| `graph` | `InitGraph`, `CloseGraph`, `Delay`, `GetMaxX`, `GetMaxY`, `GetAspectRatio`, `ScreenWidth`, `ScreenHeight`, `OutText`, `OutTextXY`, `InstallUserFont`; text mode: `Write`, `WriteLn` (variadic, shadowing the built-ins), `GotoXY`, `WhereX/Y`, `TextColor`, `TextBackground`, `Clear` (on both surface kinds, aliased `ClrScr` and `ClearViewPort`), `ClrEol`, `DelLine`, `InsLine`, `TextCols/Rows`, `TextMode` (logical 80×25 grid, any size, GPU-scaled), `HighVideo/LowVideo/NormVideo`, `Blink` (bit 24, on the language's `clock ()`), `SetBlinkRate`, `KeyPressed`, `ReadKey`, 24 key constants incl `KeyClose`; the `Window` and `ViewPort` surface classes, stacked by `Order` around the root grid, every method also a surface-first alias; `ViewPort.SetTextStyle` turns and magnifies free text, `TextWidth`/`TextHeight` measure it in the turn and size in force, `SetTextJustify` places it about a point, `SetBkColor` says what `Clear` returns a viewport to, `GetFillPattern`, `GetFillSettings` and `GetLineSettings` read the pens back, `GetImage`/`PutImage` lift and lay a rectangle of pixels with six put modes (`TransparentPut` being the library's own) and `ImageSize` prices one, `SetWriteMode` puts the pen in XOR so a figure drawn twice leaves no mark, `SetUserCharSize` scales text by a ratio either way, `GetTextSettings`/`GetViewSettings` and `GraphDefaults` complete the pen state, `SetAspectRatio` overrides the measured stretch, `SetPalette`/`GetPalette` rebind the sixteen names per surface (a color equal to one of them is a name, anything else a literal; `PutPixel` and `PutImage` bypass); the pen — `Line`, `LineTo`, `LineRel`, `Rectangle`, `PenTo`, `PenRel`, `GetX/Y`, `SetColor`, `GetColor`, `SetLineStyle` with five styles and any thickness, `Arc`, `Circle`, `Ellipse`, `DrawPoly`, `FillPoly` (scanline in Algol-24, spans in C), `SetFillStyle`/`SetFillPattern` with the thirteen fill patterns, `Bar`, `Bar3D`, `PieSlice`, `Sector`, `FillEllipse` (filled in C, the run being what C is for), `FloodFill` (a boundary fill, the one figure that reads the surface back) and its companion `GetPixel`, `GetArcCoords` answering an `ArcCoords` recorded by all six curve verbs; `CellWidth`/`CellHeight` and `Window.PixelLeft/PixelTop` place a ViewPort against a Window's cells, 16 CGA colors, `Transparent`; `graphffi.c` carries the hex decoder, cell stamper and scroller | 298 | complete |
| `crt` | `graph`'s text mode on a terminal: `InitCrt`, `CloseCrt`, `IsTerminal`, `TextCols/Rows` (the real terminal, following a resize), `Write`, `WriteLn` (variadic, shadowing the built-ins), `GotoXY`, `WhereX/Y`, `TextColor`, `TextBackground`, `HighVideo/LowVideo/NormVideo`, `Clear` (aliased `ClrScr`), `ClrEol`, `DelLine`, `InsLine`, `SetPalette`/`GetPalette`, `KeyPressed`, `ReadKey` with the same 23 key constants, the `Window` class stacked by `Order`, `MoveTo`, `Show`; the sixteen colors, `Transparent` and `Blink` declared a second time rather than shared; `crtffi.c` carries the tty and termios calls, the compositor, the escape transcoder and the nap that keeps `ReadKey` from spinning; the mouse through SGR reports, with the same `Event`, `SurfaceAt` and verbs `graph` has | 35 | complete |
| `sound` | `Sound`, `NoSound`, `SetVolume`, `SoundAvailable`; BASIC's `Play` with `Score` answering the notes without sounding them; `LoadSound`, `PlaySound`, `SoundLength`, `SoundPlaying`, `StopSounds`, `FreeSound`, `CloseSound`; `soundffi.c` is SDL2 audio -- a square wave mixed with up to sixteen decoded samples, WAV only | 14 | complete |
| `file` | whole-file: `ReadAll`, `WriteAll`, `AppendAll`, `ReadLines`, `WriteLines`, `Exists`, `IsDirectory`, `FileSize`, `Erase`, `Rename`, `CopyFile`; streaming: the `InFile` reader (`ReadLine`, `AtEnd`, `ReadBytes`, `Close`) and the `OutFile` writer (`Write`, `WriteLine`, `WriteBytes`, `Flush`, `Close`) with `AppendFile` inheriting it, no free-function aliases on any of them; `fileffi.c` is plain C99 and the library's first foreign code that builds anywhere | 25 | complete |
| `strings` | `Trim`, `Upper`, `Lower`, `Repeat`, `Split`, `Join`, `Delete`, `Insert`, `Replace` — every one an `extension`, so each may be written `Trim (S)` or `S.Trim ()`; `Join`'s receiver is a List rather than a String; ASCII-only case, `Delete` and `Insert` total over any index, no C at all | 47 | complete |
| `time` | `Now`, `UtcNow`, `At`, `UtcAt` answering a `DateTime` with `Text`; `UtcOffset`, `IsDaylightSaving`; `timeffi.c` is C89 `localtime_r`. `GetDate`/`GetTime`'s four `var` parameters become one returned object; `SetDate`/`SetTime` are absent, needing root | 9 | complete |
| `dos` | `DiskFree`, `DiskSize` by path rather than drive number and unbounded rather than `LongInt`; `GetEnv`, `HasEnv`, `Environment` answering a List, `EnvName`/`EnvValue`; `Exec` through `posix_spawn` with an argument List and **no shell**; `dosffi.c` is POSIX, not C99 | 13 | complete |

The Tests column is the unit's own `test` blocks, 537 in all. `./test.sh`
reports what `--test` can reach instead, imports included, so it prints 402
for `graph` (its 298, plus `math`'s 69, `file`'s 26 and `testing`'s 9) and 92
for `sound`. `graph` reaches `file` because `InstallUserFont` reads its glyph
file through it.

⚠️ **`graph` has a design document, `DESIGN.md`, and it governs.** The unit is
one world — celled text on a grid at Order 0, Canvas objects above and below
it — not Turbo Pascal's Crt/Graph split. Read it before adding any `graph`
method; a method that fights it is wrong until the document says otherwise.

`graph` notes: SDL2 comes from Homebrew (`brew install sdl2`) and is reached
through wrappers in `graphffi.c` rather than declared, so nothing names SDL's
own file and an SDL installed elsewhere needs no substituting — `build.sh` asks
`sdl2-config`. `./test.sh` and `./check-reference.py` set `SDL_VIDEODRIVER=dummy`
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

⚠️ **`file` owns its primitive rather than using the core's `TextFile`.**
That class exists for the compiler and is being pared to what the compiler
needs; the library reads exactly one file -- a glyph file, in
`InstallUserFont` -- and now does it through `file`. Nothing in the core is
needed for file I/O: C can take a path as a String and answer a count, which
is all `fileffi.c` uses.

⚠️ **`time` and `dos` split Turbo Pascal's `Dos` unit, and four of its members
are gone deliberately.** `SetDate` and `SetTime` need root on every system this
runs on, so their honest doc comment would read "raises unless you are root".
`GetCBreak` and `SetCBreak` controlled a DOS flag with no equivalent -- the
question they really asked, whether Ctrl-C interrupts the program or arrives as
a key, is termios' `ISIG` and so `crt`'s business if it is anyone's. The
calendar went to `time` because it is its own subject; `dos` keeps what the host
offers beyond files. `PROPOSAL-BITWISE-OPERATORS.md` records the rest of that
analysis.

`time` notes: the gap is the **time zone**, not the arithmetic -- leap years are
a closed rule a program could hand-roll, and the zone lives in the host's
database and changes by legislation. So `UtcOffset` takes a moment rather than
answering a constant, a zone observing daylight saving having two. The seven
calendar fields come back from **one** C call into a caller-owned Buffer, not
seven, or they could straddle a tick and report the hour from before midnight
with the minute from after. `timeffi.c` is C89 throughout, so this unit is as
portable as the C gets.

`dos` notes: ⚠️ **`Exec` does not go through a shell**, which is the whole
point of it taking a `List` rather than a command line -- an argument holding a
space, a quote or a semicolon arrives whole and cannot become syntax. The cost
is that there is no `PATH` search, no globbing and no redirection, so `Path`
must be `'/bin/echo'` and not `'echo'`. The argument vector is built in C one
argument per call rather than packed here, because packing needs a separator
and no byte is safe: an argument may hold any byte but NUL, and NUL cannot
cross the boundary inside a String. A status above 255 is a signal, 128 plus
its number, which is unambiguous because an exit code cannot reach that far.
`dosffi.c` is **POSIX rather than C99** -- `statvfs` and `posix_spawn` -- so
unlike `file` it does not build anywhere; Windows would need
`GetDiskFreeSpaceEx` and `CreateProcess` behind the same three functions.

⚠️ **`strings` needs 0.1.5, and so therefore does the whole library.** It is
the first unit to use `extension` [FUN-015], which 0.1.4 cannot parse. Before
it there was no version floor at all; now `./test.sh` fails on anything older.

`strings` notes: it is the second unit after `testing` with no `*ffi.c`, and
so the only one besides it that is portable as written — nothing for a
per-platform build to rewrite. ⚠️ **`Upper` and `Lower` delegate to the core's
`ToUpper`/`ToLower` [RT-025] and must not be reimplemented** -- they were, at
first, because nobody checked the built-in table before adding them, and two
of the unit's nine methods were a duplicate of the core for a while. A test
now asserts they agree with the core routines so they cannot drift. They exist
only for the extension spelling, `S.Upper ()` composing in a chain where
`ToUpper (S)` would not. The ASCII-only limit is the core's decision, not this
unit's.

⚠️ **`Pos` takes a start index**, which `Split` and `Replace` use to scan in
one pass. They re-sliced the remainder with `Copy` at first -- quadratic in
the length, and a fresh string per separator. Check the specification's
built-in table before writing a loop that walks a string. `Repeat` takes a `String` and a Char is
accepted where one is declared, which is what makes `Repeat ('-', 40)` the
call it exists for. `Split` keeps the empty pieces, which is what makes
`Join (Split (S, By), By)` answer `S` for every input — a test asserts the
round trip. `Delete` and `Insert` exist because the obvious composition is
**not** total: `Copy` clamps an over-long count but *raises* on a start past
the end, so `Copy (S, 0, From) + Copy (S, From + Count, Length (S))` fails on
exactly the common "cut to the end with a generous count" call. `Replace`
changes every occurrence and never rescans what it wrote, so
`Replace ('aa', 'a', 'aa')` terminates rather than looping. `Insert`'s argument
order is **not** Turbo Pascal's — the string inserted into comes first, because
an extension's receiver must. The private whitespace helper uses the language's `private`
[MOD-006], which no other unit here does yet; `crt`'s `ByteAt` says "private
to this unit" in a comment and is in fact exported.

⚠️ **The units do not `uses strings`, and that is decided rather than
pending.** Adopting it inside `graph`, `crt` or `sound` would tidy their
internals and buy their consumers nothing — extension visibility is per-file
and non-transitive [MOD-015], so a unit importing `strings` does not pass
`S.Trim ()` to anyone importing *it* — while adding an inter-unit dependency
that moves every `--test` count. Three things would reopen it: a
consumer-visible benefit, a reason the dependency earns its cost, or a unit
whose own code cannot be stated clearly without it. Tidier internals alone is
not one of them, so do not make that change as cleanup. `PLAN-STRINGS.md`
carries the reasoning.

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

⚠️ **`stat` is not a file's size for anything but a regular file holding
still, and `ReadAll` learned that the hard way.** It answers the bytes
currently **buffered** for a pipe, zero for most character devices, and a
stale figure for a file that grows between the size call and the read. The
first version of `ReadAll` sized a Buffer from it and read once, so a megabyte
piped in came back as 65536 bytes -- one pipe buffer -- **reported as
success**, which is the worst shape a fault can take. Reproduction:

```console
$ yes abcdefghijklmnopqrstuvwxyz | head -c 1048576 | algc big.a24
read 65536 of 1048576 bytes          # before
read 1048576 of 1048576 bytes        # after
```

`ReadAll` now holds a handle **internally** and reads to end of file, growing
its buffer -- `fileffi.c` gained `alg_libfile_open`, `read_into` and `close`
for it. A caller still sees no handle.

⚠️ **The pipe case is not unit-testable and the tests do not pretend to cover
it.** A pipe needs a second process writing to it, so the two regression tests
drive the growth loop with large regular files instead -- and **the buggy
version would have passed both**, `stat` being accurate for exactly that case.
The pipe was verified by hand. Do not read those tests as a reproduction.

⚠️ **Whole files is a scope decision, not a principle**, and `file.a24` now
says so in the unit rather than only in a commit message -- where the
reasoning used to live, which is why it read as an assumption. The original
objection to streaming was that it "needs a way to hand back a line without
handing back a borrowed pointer"; that pattern is now used three times here
(`ReadAll`, `dos.GetEnv`, `graph`'s SDL error reader), so the obstacle is
gone. Every other runtime makes the stream the primitive and the whole-file
read the sugar; this unit has it the other way round.
`SKETCH-STREAMING.md` has the whole surface, of which groups 1 to 3 -- the
handle, reading and writing -- are now built. Groups 4 and 5 are not: `Seek`
and `Position` carry the question of what seeking a pipe should do, and
`Buffer` still has no `PutDouble`, so a record holding a price has no
encoding. Those were left for a separate decision rather than guessed at.

⚠️ **The streaming classes have NO free-function aliases, and that breaks
the house convention deliberately.** Nearly every surface verb in `graph` and
`crt` has one -- `Line (V, …)` for `V.Line (…)` -- but both those units export
a free `Write` and `WriteLn` already, and a third would make a bare `Write`
ambiguous in the combinations worth writing. That is the `Delay` collision
that keeps a third copy out of `sound`, and it is why these are
`F.Write (…)` only. `WriteLine` is spelled out rather than `WriteLn` for the
same reason.

⚠️ **`InFile.ReadLine` and `ReadLines` must agree, and a test holds them
together.** Both take a carriage return before a newline as part of the
newline, treat a final line with no terminator as a line, and refuse to
invent an empty last line after a trailing one. The test asserts they answer
identically across five bodies plus the empty file, so neither can drift.

⚠️ **A class gets ONE reference entry, with its members in the Declaration
block** -- not an entry per method. That is the specification's own convention
([RT-024] documents the whole of `TextFile` in one rule with a member table)
and `REFERENCE.md`'s own precedent in `## ArcCoords`. A first attempt here
used dotted headings, `## OutFile.Write` and the rest, which was wrong on both
counts. The constructor is written under the class's own name --
`constructor InFile (Path : String)` -- again following `ArcCoords`.

⚠️ **`ReadAll` reads through `InFile`**, so the whole-file calls really are
the convenience layer and not the foundation. That needed `ReadBytes` to take
an **offset**: it fills a caller's buffer at a position, which is what lets
`ReadAll` grow one buffer across several reads. Without the offset the two
would have had a grow loop each, and the layering would have been a claim
rather than a fact. The pipe reproduction still answers 1048576 of 1048576
through the new path.

⚠️ **A Buffer must be sized exactly to become Text.** `Buffer.Text` refuses a
buffer holding a zero byte, so a buffer larger than its contents fails on its
own trailing zeros rather than answering a truncated String. That is why
`ReadAll` asks `alg_file_size` first.

⚠️ **Sound is its own unit for a portability reason, and must stay there --
but the reason inverted in 0.1.5.** `soundffi.c` used to reach CoreAudio, a
macOS framework that exists nowhere else, so the unit could only ever have made
a noise on one operating system; it now plays through **SDL2**, which carries
audio on macOS, Linux, Windows and Android alike. The unit still stands apart,
and the argument is now the other way round: `crt` calls only POSIX -- termios,
ioctl, nanosleep -- and needs nothing installed, so folding the noise into it
would make a terminal program acquire SDL for the sake of a beep.
`SDL_INIT_AUDIO` opens no window and needs no display server, so `sound` asks
for a library and not a desktop. What the change cost is **file formats**: WAV
only, where CoreAudio read AIFF, CAF, MP3 and M4A through the system's codecs.
SDL2_mixer would buy those back at the price of a second dependency and is
deliberately not here. `Delay` is in `crt` and `graph` and deliberately **not**
in `sound`: a third
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
dialect testable. `build.sh` links `sdl2-config --cflags --libs` for
`soundffi.c` only, and `examples/build.sh --static` does the same -- a build
script may ask `sdl2-config` where SDL is, where `graph`'s own declarations
have to hardcode `/opt/homebrew/lib/libSDL2.dylib` because an `in` clause takes
a string literal.

⚠️ **`sound`'s test fixture is `beep.wav`, which this repository owns.** It
used to load `/System/Library/Sounds/Ping.aiff` -- both a format SDL2 cannot
read and a path only macOS has, so the test was itself a portability blocker.
`beep.wav` is 300 ms of 880 Hz at 22050 Hz in 16-bit mono, chosen so it is
**not** the mixer's float32 at 44100: loading it therefore exercises SDL's
conversion rather than a pass-through, and the test asserts the duration
survives resampling. ⚠️ **`check-reference.py` copies `.wav` as well as `.a24`
and `.hex`** into its work directory, or the `LoadSound` example cannot find
it.

⚠️ **A `--static` binary is only as self-contained as its units.** With
CoreAudio a `sound`-using binary needed nothing extra, the framework shipping
with the operating system. With SDL2 it links `libSDL2.dylib`, which Homebrew
installs and macOS does not ship -- measured. `examples/statistics.a24` reaches
neither `graph` nor `sound`, so its own binary still needs only `libffi` and
`libSystem`, which is why `examples/check.sh` passing does not prove this.

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
`examples/ide-crt.a24` is the acceptance piece — the same Turbo C++ screen on a
terminal. Not `examples/ide.a24` with a few lines changed, which this file used
to claim: the two share their three helpers (`Frame`, `MenuItem`, `StatusKey`)
and the whole celled half — menu bar, status line, framed windows stacked by
`Order`, and an event loop that moves one — and then necessarily part company,
because `crt` has no pixels. `ide.a24`'s two ViewPorts, the line-style panel
and the plotted graph with axes, have no counterpart here and `ide-crt.a24`
puts its own windows where they were. Ignoring comments the two differ in some
180 lines. What the port actually demonstrates is that the **text** vocabulary
is shared between the units closely enough to transfer almost verbatim, which
is the interesting half of the claim and survives stating it accurately.

⚠️ **`In` is a keyword and cannot be a variable.** `var In := Buffer (4);`
answers `Expect variable name.`, because `in` is the `for … in` and
membership operator -- so a reader's destination buffer is `Dst` and not `In`.
And `Length` is **text only**: a List wants `Seen.Length` [RT-003], where
`Length (Seen)` answers `Length expects text; use .Length for a collection.`
Both cost a compile here.

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

Build artifacts — `lib*ffi.so` and `examples/build/` — are generated and
should not be committed.
