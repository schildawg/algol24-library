# Defects and hazards found while building the library

Problems found by using Algol-24. Nothing here is fixed in this repository.

Three kinds of thing, and the distinction matters:

| | What it is | What to do |
| --- | --- | --- |
| **Defect** | the language behaves *wrongly* | record here, tell the user, who fixes it in the compiler project |
| **Gap** | the language does not offer it | answer with an `external`, no ceremony, not recorded here |
| **Hazard** | the language behaves *as specified*, and the specified behaviour is sharp | record here so the library defends against it; not the compiler's to fix |

---

## Defects

### L-2 — overload selection warns even when arity makes it unambiguous

**Status:** open, reported 2026-09-03. Blocks the Pascal-style aliases below.

A call to an overloaded name warns at *every* call site, whatever the
arities involved, and the warning goes to **stdout** where it mixes with the
program's own output:

```algol24
procedure Bump (By : Integer);              begin … end
procedure Bump (T : Thing, By : Integer);   begin … end
```
```console
[WARN] p.a24:5: 'Bump' selects among 2 overloads at run time.
[WARN] p.a24:6: 'Bump' selects among 2 overloads at run time.
global bump 5
```

[ERR-010] is right that selection is dynamic when argument *types* decide it
— the type system is gradual, so a declared `Any` may hold anything at run
time. But **arity is not a type**: a call written with one argument cannot
reach a two-argument overload, and the compiler knows both counts before the
program runs. Where the arities differ and only one candidate can match, the
selection is static and the warning has nothing to warn about.

Two consequences for a library:

- **The warning is on stdout.** Even one unavoidable overload would corrupt
  every program's output, and would break this repository's
  `check-reference.py` and `examples/check.sh`, which compare a program's
  output byte for byte. Warnings on stderr would leave that harness intact.
- **Named arguments suppress it** — `Bump (T, By: 7)` is silent — which is a
  workaround only where the call site can afford the extra word.

What it costs here: `graph`'s methods cannot have the Pascal-style free
function aliases `Print (W, 'text')` beside the screen-wide `Print ('text')`,
which is the spelling a Turbo Pascal program would use. The aliases work
perfectly; they are just too loud to ship.

---

## Hazards

### H-1 — a foreign declaration's signature is never checked, and a mismatch is silent

**Status:** specified behaviour, not a defect. Recorded 2026-09-02, reclassified
the same day on reading [FUN-014].

This was first written up here as a defect. It is not one, and the specification
says so in terms:

> **THE LANGUAGE DEFINES THE CALL AND NOT THE CALLEE** […] What a foreign
> function does, whether the symbol exists, and whether the declared types match
> the C ones are all outside this specification and cannot be checked by it. A
> declaration that misdescribes a C signature is undefined behavior in the
> ordinary C sense — the conformance corpus tests that a call is *made*, never
> what it reaches.

So there is nothing for the compiler project to fix. It is kept here because the
library reaches for `external` whenever the language runs out, which makes this
the sharpest edge the library routinely stands next to.

**The symbol's existence *is* checked**, and is the only thing that is:

```algol24
function Nope (X : Double) : Double; external 'no_such_function_anywhere';
```
```console
Uncaught: No foreign symbol 'no_such_function_anywhere'.
```

**Every other mismatch is silent.** Measured on macOS/arm64 with `algc 0.1.3`:

| Declared | Reality | Result |
| --- | --- | --- |
| `trunc` as `: Integer` | returns `double` | `0` for every input |
| `strlen` as `: Double` | returns an integer | `7.951317102442115E-275` |
| `sqrt` as `(X : Integer)` | takes `double` | `0.0` where `3.0` was right |
| `atan2` with one argument | takes two | `atan2(x, whatever was in the register)` |

The `strlen` value is not the answer in the wrong register — its bit pattern is
`0x070605ffffffffff`, which is uninitialised memory.

**The wrong value is deterministic**, which is what makes it dangerous rather
than merely wrong. Five runs gave the identical result, so a test asserting it
would pass and keep passing. It is deterministic without being stable — it
tracks the argument in ways unrelated to the function:

```
StrLenD ('a')                         ->  7.94992889512769E-275
StrLenD ('hello')                     ->  7.951317102442115E-275
StrLenD ('hello world, a longer one') ->  7.94992889512769E-275
```

**The two processors disagree.** The same program answers
`7.951317102442115E-275` interpreted and `NaN` compiled. Both are garbage and
neither back end is wrong to produce its own, so this sits below where the
conformance corpus can see — consistent with [FUN-014] rather than a violation
of it.

**Reading cannot catch it.** `function Trunc (X : Double) : Integer; external
'trunc';` looks entirely reasonable — it is what a Pascal programmer would
write, and `Trunc` is *supposed* to answer an Integer. Only libm's actual
prototype says otherwise.

#### How the library defends against it

1. **Every `external` gets a test asserting a known value**, never merely that
   the call returns. `Sqrt (9.0) = 3.0`, not "`Sqrt` returned something".
2. **Where no C function has the wanted signature, write one.** `Trunc` is the
   worked case: libm's `trunc` returns a `double` and there is no `lltrunc`, so
   `mathffi.c` supplies `alg_trunc` returning an `int64_t`. Bending the
   declaration to fit an existing symbol is what this hazard punishes; writing
   the symbol is what avoids it.
3. **Keep the logic in Algol-24 and the C minimal.** `Trunc`'s domain checks are
   in `math.a24`, where they are readable and tested, and `alg_trunc` is four
   lines.
