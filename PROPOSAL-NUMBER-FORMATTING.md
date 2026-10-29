# Proposal: Number Formatting

**From:** the `algol24-library` project, as a user of the language
**Status:** proposal, for the language project to accept, refuse or reshape
**Concerns:** two overloads of an existing built-in — no new syntax, no change
to types or to the numeric model

---

## Summary

`Str` renders a number as text and offers no control over how. Add two
overloads that do:

```algol24
Str (X, Decimals)               // exactly that many places, never exponent form
Str (X, Width, Decimals)        // the same, right-aligned in Width
```

```algol24
WriteLn (Str (2.675, 2));              // 2.67
WriteLn (Str (1234.5, 10, 2));         // '   1234.50'
```

**Not** Turbo Pascal's `Write (X:8:2)`. The reasons are below, and one of them
is specific to this language.

---

## Why a library cannot do this

This is the whole of the argument, and it is measured rather than reasoned.

A library has two routes and both give wrong answers.

### Route one: surgery on `Str`'s output

`Str` answers the shortest form that round-trips, which is not the form a
fixed-decimal rendering needs:

| Written | `Str` answers | the value actually is |
| --- | --- | --- |
| `2.675` | `2.675` | 2.6749999999999998 |
| `1.005` | `1.005` | 1.0049999999999999 |
| `8.835` | `8.835` | 8.8350000000000009 |
| `0.00001234` | `1.234E-5` | — |
| `1.0E16` | `1.0E16` | — |

So `2.675` *looks* like a tie and is not: the true value sits below the
midpoint, and correct rounding to two places is `2.67`, not `2.68`. Text
surgery cannot know that, because the digit that would tell it has already
been discarded. And the last two rows have no decimal point to work from at
all.

**This is not hypothetical. It is shipping.** `shroom-slots`, the language's
other large user, carries exactly this helper, with a doc comment conceding
"this is for reading, never for comparing":

```algol24
function Fixed (D : Double, Places : Integer) : String;
begin
    var S := Str (D);
    var Point := Pos (S, '.');
    …
    Exit Copy (S, 0, Point + Places + 1);
end
```

Run against the same values:

| `X` | correct to 2 places | `Fixed (X, 2)` answers |
| --- | --- | --- |
| `2.675` | `2.67` | `2.67` |
| `1.005` | `1.00` | `1.00` |
| `0.125` | `0.12` | `0.12` |
| `8.835` | `8.84` | **`8.83`** — a wrong digit |
| `0.00001234` | `0.00` | **`1.23`** — wrong by 10⁵ |
| `1.0E16` | `10000000000000000.00` | **`1.0E`** — not a number |

Three of six wrong, one of them not even numeric. The three that agree do so
because truncation happened to match, not because the routine is right.

### Route two: arithmetic before rendering

`Round (X * 100.0) / 100.0` fails differently — the multiplication introduces
error of its own:

| `X` | correct | `Round (X * 100) / 100` |
| --- | --- | --- |
| `2.675` | `2.67` | **`2.68`** — the product is 267.50000000000006, so it rounds up |
| `1.005` | `1.00` | **`1.0`** — the right value, the wrong text |

And it cannot produce a width or a trailing zero at all, because the result is
a Double and `1.00` is not a number distinct from `1.0`. Trailing zeros are a
property of text.

### Which leaves C

`snprintf ("%.2f", …)` is correct on every row above, because it works from the
binary value rather than from a rendering of it. So a library *can* do this —
by reaching for C.

That is the wrong split. Rendering a number is not a platform primitive the
language happens to lack, like `statvfs`; it is something the language already
does, in `Str` [RT-006]. Putting a second, more capable renderer behind a
`uses` and a shared library would mean two implementations that must agree
forever about every value, and would make `Str (X, 2)` unavailable to a program
that had not imported anything.

---

## Why not Turbo Pascal's syntax

Pascal wrote the width and precision as a suffix on an argument of `Write`:

```pascal
Write (X:8:2);
```

Three reasons not to copy it, and the second is this language's own.

**1. It serves one statement.** `:w:d` is syntax in an argument list, so it is
unavailable when building a string, a log line, or a column of a report —
which is most of the time. Turbo Pascal had to add a separate `Str (X:8:2, S)`
procedure precisely because the syntax did not generalize. Copying the syntax
means copying the need for the workaround.

**2. `Write` is shadowable here, and Pascal's was not.** `Write` and `WriteLn`
are variadic built-ins [RT-015] that a unit may **replace** — `graph` and `crt`
both do, and `DESIGN.md` treats that as the intended arrangement. Formatting
carried in argument position cannot work when the callee may be an ordinary
user-defined function: `graph.Write (X:8:2)` would require that unit's variadic
function to somehow receive formatting instructions it has no parameter for.
This alone rules the Pascal form out.

**3. It would be a second rendering path.** `Str` is already the universal
"render as text". Formatting that does not flow through it gives the language
two ways to turn a number into characters, which then have to agree about
every value forever.

An overload of `Str` has none of these problems: it composes anywhere a String
can go, it is indifferent to which `Write` is in scope, and there is only ever
one renderer.

---

## The proposal

### The two overloads

```algol24
function Str (X : Double, Decimals : Integer) : String;
function Str (X : Double, Width : Integer, Decimals : Integer) : String;
```

**`Decimals` places, exactly.** `Str (1.5, 3)` is `1.500`. A trailing zero is
text and so survives, which is the half arithmetic cannot do.

**Never exponent form.** This is the point of the routine: a fixed-decimal
rendering of `1.0E16` is `10000000000000000.00`, long and correct, and a
rendering that answers `1.0E` is useless. A caller who wants the short form
already has one-argument `Str`.

**Correctly rounded from the binary value**, not from a rendering of it — so
`Str (2.675, 2)` is `2.67` because the value is below the midpoint, and
`Str (0.125, 2)` is `0.12` because a true tie goes to the even neighbor.

⚠️ **Ties to even, which is the language's existing answer.** `math.Round`
already breaks ties that way, because it is what IEEE 754 does for every
arithmetic result and what does not accumulate a bias. A formatter rounding
half away from zero would make `Str (X, 0)` and `Round (X)` disagree, which is
the one outcome worth designing against here.

**`Width` right-aligns, padding with spaces.** A result wider than `Width` is
**not** truncated and overflows the field instead: breaking a column is a
cosmetic fault and truncating a number is a lie.

### Raises

A negative `Decimals` has no reading and should be refused. A negative `Width`
likewise. `NaN` and `Infinity` want a decision — see the open questions.

---

## What does not change

- **One-argument `Str` is untouched**, for every type it takes. Shortest
  round-tripping output stays the default, which is the right default.
- **No new syntax.** The declaration is two overloads of a name that exists.
- **Arity settles them statically.** One, two and three arguments are distinct,
  so 0.1.4's overload fix means no call site warns and nothing selects at run
  time. `DEFECTS.md` L-4 is not touched.
- **`Write` and `WriteLn` keep their meaning**, variadic and shadowable, and a
  unit that replaces them needs no change: `Write (Str (X, 2))` is an ordinary
  call with an ordinary String.
- **No change to the numeric model**, to Double's representation, or to how
  `Val` parses.

---

## Risks

**`Str (X, 2)` reads two ways.** For a Double, two arguments plainly means
decimals. For an Integer it could only mean width — and that would be one arity
meaning different things by argument type, which is precisely what [ERR-010]
warns about and what L-4 makes noisy. **This proposal therefore declines the
Integer form**: the two-argument overload is Double only, and width for an
Integer goes through the three-argument form or through text padding, which a
library can do correctly. Worth stating in the specification rather than
leaving to discovery.

**A long fixed rendering is very long.** `Str (1.0E300, 2)` is three hundred
digits. That is correct and probably surprising; it wants saying.

**It invites a format-string next.** Someone will ask for
`Format ('%08.3f', X)`. That is a different proposal with a mini-language in it
and this one should not be read as a step toward it.

---

## Open questions for the language project

Genuine ones; this library has no basis to decide them.

1. **The spelling.** `Str` overloaded, or a separate name? Overloading keeps
   one renderer and needs no new identifier, which is why it is proposed —
   but `Str` is currently "any value, rendered" [RT-006] and this makes it
   "any value, rendered, and a Double rendered *so*", which is a slightly
   different promise.
2. **`NaN` and `Infinity`.** `Str` answers `NaN` and `Infinity` today. Should
   `Str (NaN, 2)` answer `NaN`, pad it to `Width`, or refuse? Padding reads
   best in a column; refusing is defensible; silently producing digits is not.
3. **Should `Decimals` of zero yield a trailing point?** `Str (2.5, 0)` as `2`
   or `2.`? Pascal's `:0` gave no point, which seems right.
4. **Is a `Width` form wanted at all**, or is padding better left to a
   `strings` routine? Alignment of already-rendered text is pure string work
   and needs nothing from the language — only the precision half does. Taking
   just the two-argument overload would be a smaller change that closes the
   whole correctness gap, with the column-building left to a library. This
   library would be content with that.
5. **Integers and `Decimals`.** `Str (5, 2)` is refused under this proposal.
   Should it instead mean `5.00`, promoting as a mixed expression would
   [EXP-005]? That reads naturally and reopens the arity ambiguity above.

---

## What this proposal does not ask for

- A format-string function. See the risks.
- Thousands separators, currency, locales, or anything that varies by where
  the program is run. A library can do those on top once the digits are right.
- Any change to `Val`, to one-argument `Str`, or to `Write`'s variadic form.
- Anything about Integer rendering, which is already exact and needs no
  precision.

---

## What this library would do with it

- **Retire `time`'s `Pad`.** A private helper that zero-pads a year and a
  minute, written because nothing else could.
- **Offer `PadLeft`/`PadRight` in `strings`** for the alignment half, which is
  honest string work, and stop there — rather than offering a `Fixed` that
  would have to be wrong in the ways measured above.
- **Say so in `REFERENCE.md`.** `Str (X, 2)` is what a reader looking for
  Turbo Pascal's `:8:2` should find, and right now there is nothing to point
  them at.

---

## Appendix: how the evidence was gathered

Everything measured here came from running the language and a four-line C
program, not from reading either. The `Str` table is the output of a loop over
six literals; the `Fixed` table is `shroom-slots`' own helper quoted verbatim
into a program and run over the same six; the `%.2f` column is `printf` on the
same values with `%.17g` alongside to show what the Doubles actually hold. The
`Round (X * 100) / 100` row was run against `math`.

This library has never explained something about the language by looking at how
it is built, and this proposal keeps to that.
