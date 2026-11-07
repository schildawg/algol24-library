# Proposal: Bitwise Operators

**From:** the `algol24-library` project, as a user of the language
**Status:** **ACCEPTED and shipped in 0.1.6** as `&`, `|`, `^`, `~`, `<<` and `>>` [EXP-023, EXP-024], with a warning when `and` or `or` is handed an Integer. What this document asked for and what landed are compared in `DEFECTS.md` H-3, which also records that the native `^` measured four times faster than the `alg_xor` foreign call this proposal cited — so `graph`'s C is gone rather than merely unnecessary. The original ask follows, unedited.
**Concerns:** Integer operators only — no change to types, scoping or the
numeric model

---

## Summary

The language has **no bitwise operations of any kind**. Add them as operators on
Integer, defined over the unbounded Integer the language already has.

```algol24
Flags bor Mask            // set bits
Flags band not Mask       // clear bits
Pixel bxor Pen            // the XorPut line mode
(R shl 16) bor (G shl 8) bor B
```

The spelling above is illustrative, not the ask. See *The spelling question*.

---

## What is missing, measured

Nothing exists under any spelling. Tested against `algc 0.1.5`:

| Written | Result |
| --- | --- |
| `6 & 3`, `6 \| 3`, `6 ^ 3`, `~0` | `Error: Unexpected character` — not even lexed |
| `1 << 4`, `256 >> 4` | `Expect expression!` |
| `6 xor 3`, `6 band 3`, `6 bitxor 3` | parse errors |
| `Xor (6, 3)`, `Shl (1, 4)`, `bitand (6, 3)` | `Undefined variable` |

The word "bitwise" does not appear in the specification.

**And `and`/`or` are a trap rather than an absence.** They are specified to test
truthiness and yield an *operand* [EXP-010], so a reader coming from Pascal
writes the obvious thing and gets a plausible wrong number with no diagnostic:

```algol24
WriteLn (6 and 3);     // 3   -- bitwise would be 2
WriteLn (6 or 3);      // 6   -- bitwise would be 7
```

This is specified behavior and the proposal does not ask for it to change — see
*What does not change*. It is recorded here because it is why the absence is
sharp rather than merely inconvenient, and it is `DEFECTS.md` H-3.

---

## Why this library is asking

Because it has already paid for the gap, twice, in the two largest units.

### The workaround that exists today

`graph` reached into C for a single exclusive or, and said why in the doc block:

```algol24
/// Private to this unit. The language has no bitwise operators at all -- its
/// `and` and `or` are logical, answering an operand rather than combining
/// bits -- so this is a genuine gap rather than a shortcut.
function XorWords (A : Integer, B : Integer) : Integer;
    external 'alg_xor' in 'libgraphffi.so';
```

**And that workaround cannot generalize, which is the heart of the ask.** The C
behind it is:

```c
int64_t alg_xor (int64_t a, int64_t b)
{
    return (int64_t) (int32_t) ((uint32_t) a ^ (uint32_t) b);
}
```

It truncates to 32 bits, combines as unsigned, and sign-extends back. That is
exactly right for a pixel word and silently wrong for an Integer, and the
declaration — `(Integer, Integer) : Integer` — cannot say so. A library is
obliged to pick a width here; the language is not.

### The duplication the absence has produced

| Hand-rolled | Where |
| --- | --- |
| `XorWords`, a foreign call for one operation | `graph.a24:169` |
| `ByteAt`, byte extraction by a `div`/`mod` loop with a running multiplier | `crt.a24:1027` |
| RGB channel splitting — `div 65536`, `div 256 mod 256`, `mod 256` | `graph.a24` and `crt.a24`, several sites each |
| The `HighVideo`/`LowVideo` brightening, a three-line channel expression | duplicated in both units |
| `Blink := 16777216` — bit 24 as a magic decimal | declared in both units |
| `if X < 0 then X := X + 4294967296` — undoing `Buffer.GetInt`'s sign | both units |

### And the library hands users bit-packed values already

A color is a packed integer, `Blink` is bit 24, and `SetLineStyle (UserBitLn,
61455, …)` takes a sixteen-bit pattern. The library gives users bit-packed
integers and the language gives them no way to take one apart. Closing that is
not something the library can do for them.

### A library answer was considered and is worse

A pure Algol-24 `bxor` is writable — a loop over `div 2` and `mod 2` — and was
written and measured. It is correct, and it costs **0.7 ms per call**: 13.94
seconds for 20,000 exclusive ors of 32-bit words.

`graph`'s XOR write mode needs one per pixel *written*, which is what makes that
unusable rather than merely slow. A rubber-band line of a thousand pixels, the
figure `SetWriteMode` exists for, would cost about 0.7 s to draw and 0.7 s again
to erase — so a line that followed the mouse would manage under one frame a
second. A filled figure in XOR mode is worse by its area.

That is why `graph` went to C rather than writing the helper, and it is why a
`bits` unit would be a trap: slow in pure form, width-lying in foreign form, and
needing deprecation the moment operators arrived.

---

## The proposal

### The operations

Six, on Integer operands: **and**, **or**, **exclusive or**, **not**, **shift
left**, **shift right**.

### The semantics: infinite two's complement

This is the part only the language can get right, and the reason the proposal
is worth making at all.

Over an unbounded Integer there is no width to appeal to — which looks like an
obstacle and is not. Treat every Integer as two's complement extended
infinitely to the left: a non-negative value has infinitely many leading zeros,
a negative value infinitely many leading ones. Every operation is then **total
and closed**, with no width named anywhere:

| Expression | Answers | Why |
| --- | --- | --- |
| `6 band 3` | `2` | ordinary |
| `6 bor 3` | `7` | ordinary |
| `6 bxor 3` | `5` | ordinary |
| `not 0` | `-1` | `…1111` |
| `not (-1)` | `0` | |
| `-1 band 5` | `5` | `-1` is all ones |
| `-2 bor 1` | `-1` | |
| `1 shl 70` | `1180591620717411303424` | the result grows, as [EXP-007] already promises |

No operand is out of range, no result overflows, and nothing has to ask how wide
an Integer is. This is the same model Python uses for its arbitrary-precision
integers, and it has been consistent there for decades.

**It also composes with [EXP-007].** `1 shl 70` growing rather than wrapping is
not a special case for this proposal; it is the existing rule that "a result too
large for the machine's width grows to hold it."

### Shift right is arithmetic, and is *not* `div`

Worth stating plainly because the obvious reading is wrong. `div` truncates
toward zero — measured, `-7 div 2` is `-3` — while an arithmetic right shift
floors:

```
-7 shr 1   ->  -4        (floor)
-7 div 2   ->  -3        (truncate toward zero)
```

They agree whenever the shifted-out bits are zero and disagree otherwise, so
`shr` cannot be specified as division by a power of two. It should be specified
as the two's-complement shift, which is what makes it the inverse of `shl` for
values that have room.

A negative shift count has no meaning and should be refused rather than read as
a shift the other way.

### The spelling question, which is the language project's

`and` and `or` are taken, and **this proposal explicitly does not ask for them
to be reused.** `6 and 3` is `3` today; making it `2` would silently change the
meaning of every program that relies on [EXP-010], which is the one outcome
worth designing against.

So new spellings are needed, and the library has no standing to choose them.
Two directions, with what each costs:

- **Word operators** — `band`, `bor`, `bxor`, `bnot`, `shl`, `shr`. Consistent
  with `div`, `mod`, `in` and `is`, which is the language's established style
  for operators that are words. Not beautiful.
- **Symbols** — `&`, `|`, `^`, `~`, `<<`, `>>`. Familiar from C, and a
  departure: none of those characters is lexed at all today, so this is a lexer
  change as well as a style decision.

A third option worth naming only to dismiss: making `and`/`or` bitwise *when
both operands are Integers*. It would break `if Count and Found` for every
program that writes it, and the breakage would be silent.

---

## What does not change

- **`and` and `or` keep their meaning**, [EXP-010] included. Nothing in this
  proposal touches truthiness, short-circuiting, or the operand-yielding
  result.
- **No change to the numeric model.** [EXP-007] already says Integer results
  grow; this proposal relies on that rather than amending it.
- **Doubles are not involved.** A bitwise operator on a Double should be
  refused, not reinterpreted. The bit pattern of a Double is a separate
  question, and not one this proposal touches: `math` decomposes one privately
  for `Trunc`'s exact path and exposes nothing, which is where that should stay
  until something asks otherwise.
- **No existing program changes meaning**, since every spelling proposed here
  is currently a parse error or an undefined name.

---

## Risks

**A new keyword can collide.** Word operators are keywords, and any program
using `shl` or `band` as an identifier would break. [LEX-011] shows the
language's answer to exactly this problem — `unit`, `test`, `on` and
`extension` are recognized by position rather than reserved — and whether that
technique extends to an infix operator is a question for the project.

**Precedence is a decision with no obvious default.** C's choice, where `&`
binds looser than `==`, is widely regarded as a mistake and is the subject of a
compiler warning in every modern C. Putting the bitwise operators between the
comparisons and the arithmetic would avoid inheriting it.

**Shifts invite a width assumption.** Under this proposal `1 shl 64` is a large
number rather than zero or undefined behavior, which is the right answer and
will surprise anyone arriving from C. It wants saying in the specification
rather than leaving to discovery.

---

## Open questions for the language project

Genuine ones; this library has no basis to decide them.

1. **The spelling**, as above. Words, symbols, or something else.
2. **Precedence**, and whether the bitwise operators sit above or below the
   comparisons.
3. **Is `not` reusable for bitwise complement?** `not` is the logical negation
   today. Overloading it on Integer would read well — `Flags band not Mask` —
   but it means one spelling testing truthiness for some operands and
   complementing bits for others. A separate `bnot` avoids the question at the
   cost of another keyword.
4. **A negative shift count**: refuse, or shift the other way? This library
   would refuse, on the grounds that a count arrived at by subtraction and gone
   negative is a bug the program wants to hear about.
5. **Should `Buffer` gain anything alongside?** The `if X < 0 then X := X +
   4294967296` fixup in two units exists because `Buffer.GetInt` answers a
   signed word. An unsigned reader would retire those sites independently of
   this proposal, and may be the better place to solve that half.

---

## What this proposal does not ask for

- Bitwise operations on Doubles, or any reinterpretation of a Double's bits.
- A fixed-width integer type. The unbounded Integer is the right model and this
  proposal is written to fit it, not around it.
- Any change to `and`, `or`, truthiness or short-circuiting.
- Rotation, population count, or bit-scan operations. They are expressible once
  the six exist, and none of them has come up in this library.
- Removal of `graph`'s `alg_xor`. It would stay until the pixel loop was
  measured against the operator, which is the library's business and not the
  language's.

---

## What this library would do with it

- **Retire `XorWords` and its C**, if the operator measures well enough in the
  XorPut path — one fewer foreign call, and one fewer place where a declaration
  cannot say what the callee does.
- **Retire `crt`'s `ByteAt`** and the duplicated RGB splitting, and give
  `Blink`'s bit 24 a spelling that says it is bit 24.
- **Not write a `bits` unit.** With operators there is nothing left for one to
  hold; without them, anything it held would be slow, width-lying, or both.

---

## Appendix: how the evidence was gathered

Everything measured here came from running the language, not from reading it.
The absent-spelling table is the output of ten one-line programs; the `and`/`or`
results, the `div` rounding and the 0.7 ms figure likewise; the duplication
inventory is a search over this repository's own units. `alg_xor`'s definition
is quoted from this repository's `graphffi.c`, which is application code this
project owns.

This library has never explained something about the language by looking at how
it is built, and this proposal keeps to that.
