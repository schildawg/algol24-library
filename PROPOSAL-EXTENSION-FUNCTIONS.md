# Proposal: Extension Functions

**From:** the `algol24-library` project, as a user of the language
**Status:** proposal, for the language project to accept, refuse or reshape
**Concerns:** call syntax only — no change to types, scoping or semantics

---

## Summary

Let a top-level function opt in to being callable with method syntax on its
first parameter.

```algol24
extension function Upper (S : String) : String;
begin
    …
end
```

```algol24
Upper (S)         // both spellings, always
S.Upper ()
```

`extension` means exactly one thing: **this function may also be called with
method syntax on its first parameter.** It is permission, not a new kind of
declaration. The function is an ordinary top-level function in every other
respect.

---

## Why this library is asking

**So that `String` can grow in a library instead of in the core.**

The language's `String` is deliberately small — `.Length`, `Length`, `Copy`,
`Pos`, `Str`, `+`, and indexing. Growing it means one of three things, and two
of them are bad.

### (a) Grow the core

Every addition — `Split`, `Trim`, `Upper`, `Replace`, `Repeat`, `StartsWith` —
becomes a language change. The core swells, and the library can never help.

### (b) A rich wrapper class, on Groovy's `GString` model

The hope would be that a `ToString` makes the seam invisible. **It does not,
and this was measured rather than assumed.** Given a class holding `'hello'`
with a `ToString`:

| | answers | |
| --- | --- | --- |
| `Str (R)` | `hello` | honours `ToString` |
| `'' + R` | `Rich instance` | **silently wrong** |
| `Length (R)` | `13` | **silently wrong** — the length of `Rich instance` |
| `R = 'hello'` | `false` | |
| `R in SomeMap` | `false` | |

`ToString` is honoured by `Str` alone, and the two failures that matter most
answer plausible wrong numbers rather than raising. `Length` of a
five-character string answering `13` is the kind of fault that survives
testing.

Nor can it be fixed by honouring `ToString` more widely. Groovy's seam bites
hardest at **equality and map keys** — and `ToString` is a one-way conversion
where equality is symmetric. Closing it properly would mean making the wrapper
a genuine subtype of `String`: `GString` again, traps included.

There is also already one string seam in the language, and it has already cost
this library a bug. A one-character literal is a `Char`, never a `String`:

```algol24
'x' in M          // true   -- a Char key
Str ('x') in M    // false  -- the one-character String is a different key
```

`sound`'s note table is keyed by `Char`; the parser looked up `Str (C)` and
silently never matched. A second string seam is not wanted.

### (c) A `strings` unit of free functions

No second type, so no seam. This is already the library's most successful
pattern: `math` offers `Abs`, `Sqrt` and `Round` as functions over the
primitive `Double` rather than methods on a wrapper, and it has caused no
trouble in the life of the project.

Option (c) is right. Its **only** cost is that composition reads inside-out:

```algol24
Upper (Trim (Copy (S, 0, 8)))
```

Extension functions remove that cost:

```algol24
S.Copy (0, 8).Trim ().Upper ()
```

**With them, (c) is strictly better than a wrapper class. Without them, the
choice is between an awkward calling convention and a Groovy seam.**

---

## The proposal

### Declaration

An ordinary top-level function, marked:

```algol24
extension function Trim (S : String) : String;
extension procedure Grow (L : List, By : Integer);
```

The first parameter is the receiver. It must have a **written type** — see the
open questions.

### Calling

Both forms, always:

```algol24
Trim (S)
S.Trim ()
```

This is not a concession; it follows from what the declaration says. Because
Algol-24 has top-level functions, an extension *is* a function whose first
parameter is written out — so refusing `Trim (S)` would contradict the line
that declares it. Kotlin can refuse the prefix form because its receiver is not
a parameter (`fun String.trim()` has no `S` to pass); ours is.

Three consequences worth having:

- **Units read alike.** `math` is prefix throughout. A method-only `strings`
  would make two units of the same kind read differently for no reason.
- **Migration is free.** A plain function may become an extension, or stop
  being one, without breaking a call site.
- **No new safety cost.** Prefix calling was always available to every
  function. The opt-in governs *method* syntax, which is the only new part.

### Resolution

When the compiler meets `x.f (a₁ … aₙ)`:

1. If `x`'s type has a member named `f`, that member is called, exactly as
   today. **Members always win.**
2. Otherwise, if an **extension** function `f` is visible in the current file
   whose first parameter accepts `x` and whose remaining arity matches, the
   call means `f (x, a₁ … aₙ)`.
3. Otherwise it is the error it is today.

Rule 1 is what makes this backward compatible: no existing `x.f (…)` can change
meaning, because every one of them already resolves to a member.

A function **not** marked `extension` is never reachable by method syntax.

### Visibility

A rewritten call is found only among extensions **visible in the calling
file** — declared there, or brought in by that file's own `uses`.

This is where Algol-24 is better placed than Groovy. Groovy's metaclass
patching lets any code change what any type responds to, everywhere. Here
`S.Upper ()` compiles only in a file that says `uses strings`: explicit, local,
and visible at the top of the file.

The language's `uses` is already non-transitive, and this library depends on
that. It should carry over unchanged — a unit that uses `strings` internally
does not give *its* consumers `S.Upper ()`.

---

## What does not change

Checked against this repository, the language's largest user:

- **No existing call changes meaning.** Members win, and every `x.f (…)` in the
  library resolves to a member today.
- **Arity-based overloading is untouched.** `graph` leans on it —
  `GotoXY (Col, Row)` is the screen, `GotoXY (W, Col, Row)` is that window.
- **Nothing already written needs marking.** Un-marked functions behave
  exactly as now.

### And one thing it explicitly does not solve

This library has **59 hand-written delegate functions** (49 in `graph`, 10 in
`crt`, 295 lines with their doc blocks) that give every surface verb a
free-function spelling:

```algol24
procedure Line (V : ViewPort, X1 : Integer, Y1 : Integer,
                X2 : Integer, Y2 : Integer);
begin
    V.Line (X1, Y1, X2, Y2);
end
```

**Extension functions do not remove these,** and the proposal does not claim
they do. Those delegates run method-to-function; extensions run
function-to-method. Removing them would need the opposite direction, and the
verbs cannot simply be re-declared as extensions because they maintain
invariants across several fields at once — `Pen` against `PenAsked`,
`FillInk` against `FillAsked`, `Filling`, `FillPat`, `FillBuf`. Writing one of
those from outside desynchronizes the object:

```algol24
V.SetColor (LightCyan);
V.Pen := Red;
V.GetColor ()        // still LightCyan -- GetColor answers PenAsked
```

They belong to the class. The delegates stay, and that is fine: they are
already written, tested and documented, and they cause no faults — only
maintenance.

---

## Risks

**Recapture.** Code writes `X.Foo ()` resolving to an extension; a later
version of `X`'s class adds a `Foo` member; the call silently means something
else. This is the standard criticism of the family, and no opt-in scheme fixes
it — opting in does not stop a class from gaining members.

The right answer is a **diagnostic**, not syntax: warn when a newly added
member would previously have resolved to a visible extension. That catches the
actual failure mode at no cost in language surface.

**Discoverability.** "What can I call on a `String`?" becomes "the builtins,
plus every visible extension." Worse for tooling and for a reference document.
The library's answer is to keep such functions grouped in units, which is
already how `math` reads.

**Diagnostics.** A failed `x.f ()` currently means "no such member." It will
come to mean "no such member, and no visible extension either" — and naming
near-misses is the difference between a good message and a bad one. Worth
budgeting for.

---

## Open questions for the language project

Genuine ones; this library has no basis to decide them.

1. **Must the receiver have a written type?** Given gradual typing, an
   extension with an untyped first parameter would become a method on
   everything. Requiring a written type looks right, but note the library
   already uses untyped first-parameter dispatch deliberately, in
   `MoveTo (Surface, …)` and `SetPalette (Surface, …)`, precisely because two
   declarations of one arity would put the choice on the argument's type. If
   the requirement is imposed, those stay as they are — they are not
   extensions and do not want to be.

2. **May extensions overload each other?** If `Trim (S : String)` and
   `Trim (S : String, What : Char)` both exist, `S.Trim ()` should find the
   first. Confirm arity resolution applies unchanged after the rewrite.

3. **Is the receiver evaluated once?** `Next ().Trim ()` must not call `Next`
   twice. Presumably obvious; worth stating.

4. **Should a zero-argument extension be reachable without parentheses?**
   `S.Length` is a property today. The library has no need for
   `extension function Foo (S : String)` to be callable as `S.Foo` and would
   not miss it.

5. **Extensions on primitives and on classes alike?** The motivating case is
   `String`, but `List`, `Map`, `Buffer`, `Integer` and `Double` would all
   benefit, and so would the library's own classes. No reason to distinguish,
   but worth being deliberate.

---

## What this proposal does not ask for

- Full bidirectional UFCS. The opposite direction has a working solution
  already — top-level functions — and this library uses it.
- Any change to `String`'s contents. That is a separate decision this feature
  merely makes tractable.
- `ToString` being honoured more widely. Worth considering on its own merits;
  `'' + Obj` answering `Rich instance` is a poor default whatever happens here.
- Operator overloading, changes to gradual typing, or anything about `is`.

---

## What this library would do with it

- **A `strings` unit.** The library has hand-rolled repeated-character fill
  (twice, in both IDEs), digit scanning in `sound`'s MML parser, and byte
  extraction in `crt`. All would move to `strings`, which could then grow
  without ever touching the language again.
- **Chained reads** where they help: `X.Round ()`, `L.Sorted ()`.

Nothing urgent. The reason to consider the feature now is that **the shape of
`String` is being decided now**, and this is what makes the good answer
available.

---

## Appendix: how the evidence was gathered

Everything measured here came from running the language, not from reading its
source. The `ToString` table is the output of a short program; the `Char`
versus `String` map-key result likewise; the delegate counts come from a script
over `graph.a24` and `crt.a24` that finds free functions taking a `Window` or
`ViewPort` first whose body is a single forwarding call.

This library has never explained something about the language by looking at how
it is built, and this proposal keeps to that.
