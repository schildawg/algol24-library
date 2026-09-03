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
finding here was written up as one and turned out to be specified behaviour —
[FUN-014] says a mismatched foreign signature is undefined behaviour and outside
the specification. It is now hazard H-1.

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
- ⚠️ **`Fail` and the assertions exist only under `--test`.** A helper that
  calls `Fail` answers `Undefined variable 'Fail'.` in an ordinary program, so a
  `REFERENCE.md` example for one has to be a `test` block.

## Writing C for a unit

⚠️ **A foreign declaration must name a C function whose *real* signature is the
one wanted.** Nothing checks this — [FUN-014] says a mismatch is undefined
behaviour and outside the specification — so the failure is a plausible wrong
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

⚠️ **`bootstrap/algc` here is a shim, not a build product.** The extension
hardcodes `<workspaceRoot>/bootstrap/algc` with no setting to override it, so
without the symlink its Test Explorer refuses to run. Only the **Interpreted**
profile works; the Compiled one also wants `bootstrap/algol.c` and `algol.h`,
which this repository has no reason to carry.

## Status

| Unit | Members | Tests | Reference |
| --- | --- | --- | --- |
| `testing` | `AssertNear` | 9 | complete |
| `random` | `Random`, `RandomInteger`, `RandomReal`, `Randomize`, `SetSeed`; `drand48` declared directly, seeding via `randomffi.c` | 17 | complete |

`examples/statistics.a24` is the worked application — built by
`examples/build.sh`, verified by `examples/check.sh`, explained in
`examples/README.md`. It is the only check that exercises the whole story rather
than one unit, so it is the one to run when anything about linking changes.

Build artifacts — `lib*ffi.dylib` and `examples/build/` — are generated and
should not be committed if this becomes a git repository.
| `math` | `Abs`, `Sqr`, `Min`, `Max`, `Odd`, `Frac`, `Pi` in Algol-24; `Sqrt`, `Exp`, `Ln`, `Sin`, `Cos`, `ArcTan`, `Int`, `Round` as `external` onto libm; `Trunc` exact over any finite Double via `mathffi.c`; `IsNaN`, `IsInfinite`, `NaN`, `Infinity` | 69 (+9) | complete |
