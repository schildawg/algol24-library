# Defects and hazards found while building the library

Problems found by using Algol-24. Nothing here is fixed in this repository.

Three kinds of thing, and the distinction matters:

| | What it is | What to do |
| --- | --- | --- |
| **Defect** | the language behaves *wrongly* | record here, tell the user, who fixes it in the compiler project |
| **Gap** | the language does not offer it | answer with an `external`, no ceremony, not recorded here |
| **Hazard** | the language behaves *as specified*, and the specified behavior is sharp | record here so the library defends against it; not the compiler's to fix |

---

## Defects

*None open.* L-4, L-5 and L-6 all closed in 0.3.2; see below.

## Closed

### L-6 — `New` did not check its argument count, and the two processors disagreed

**Status:** **CLOSED in 0.3.2**, verified 2026-11-12. Found on 0.3.1 while
verifying the reflection release against `REPORT-CONCURRENCY.md`.

⚠️ **The fix matches the written form in both shapes, which is more than
"gives an error now".** [RT-034]'s promise is that `New` answers exactly what
a written construction answers, and that is two different sentences depending
on whether the class has one constructor or an overload set. Both now agree,
interpreted and compiled:

| | `New`, interpreted | written form, interpreted |
| --- | --- | --- |
| one constructor, no arguments | `Expected 1 arguments but got 0.` | `Expected 1 arguments but got 0.` |
| one constructor, two arguments | `Expected 1 arguments but got 2.` | `Expected 1 arguments but got 2.` |
| an overload set, count matching neither | `No matching signature for Needs.` | `No matching signature for Needs.` |

**And selection still works through it**, which a fix that merely counted
arguments could have broken: a class with `Init (A)` and `Init (A, B)` answers
105 through `New (Two, [5])` and 1056 through `New (Two, [5, 6])`, the same as
the written calls. A wrong argument *type*, a non-List `Args` and a type
rather than a class are all still refused with their own sentences, and the
other five built-ins are byte-identical to their 0.3.1 output. What follows is
the original report.

[RT-034] says of `New` and `SetMember` that "**both answer exactly what the
written form answers**", and that `New` "selects among the constructors on the
argument values like a written construction [FUN-013]". The interpreter does
not apply the arity check to `New`. Reproduction, a class whose only
constructor takes one argument:

```console
$ algc l6.a24                                   # interpreted
Index 0 out of range 0..-1.
1
$ algc --compile --out=out l6.a24 && cc -std=c11 -O2 -o l6 out/*.c -lffi && ./l6
Expected 1 arguments but got 0.
Expected 1 arguments but got 2.
```

| `New (Needs, …)` where `constructor Init (Arg : Integer)` | interpreted | compiled |
| --- | --- | --- |
| `[]` — too few | **`Index 0 out of range 0..-1.`** | `Expected 1 arguments but got 0.` |
| `[1, 2]` — too many | **answers an instance, `W` = 1** | `Expected 1 arguments but got 2.` |
| `['x']` — wrong type | `No matching signature for Needs.` | `No matching signature for Needs.` |

**Two faults, and the second is the worse one.** Too few arguments leaks an
internal subscript error in place of the specified sentence — the message is a
subscript of an empty range, `0..-1` being the shape an empty `Args` has, which
is as much as can be said without reading the implementation. Too many
arguments is **not refused at all** interpreted: the extra is discarded and a
plausible instance comes back, which is the shape of fault that reports
success. Only the wrong-*type* case goes through selection, so `New` is
checking types and not counts.

**It is `New` alone, not constructor arity generally.** The written form is
correct interpreted, and so is `Invoke`, which is the same question asked of a
method:

| interpreted, one-argument target | answers |
| --- | --- |
| `Needs ()` — written | `Expected 1 arguments but got 0.` |
| `Needs (1, 2)` — written | `Expected 1 arguments but got 2.` |
| `Invoke (N, 'M', [])` | `Expected 1 arguments but got 0.` |
| `Invoke (N, 'M', [1, 2])` | `Expected 1 arguments but got 2.` |
| `New (Needs, [])` | **`Index 0 out of range 0..-1.`** |
| `New (Needs, [1, 2])` | **silently answers an instance** |

So the check exists in the interpreter and `New` is the one path that does not
reach it.

⚠️ **This is the first divergence between processors inside reflection, which
is the thing this library argued could not happen.**
`PROPOSAL-REFLECTION.md` made the structural case for reflection over
coroutines on exactly this ground — reflection "lives in the shared **object
model**", so "one implementation, which all three inherit" — and measured a
property failure byte-identical between the two processors to show it. That
argument holds for the other five built-ins: `Fields`, `TypeOf`, `Member`,
`Invoke` and `SetMember` were compared across a class with inheritance, an
overload set, a property, a private method and an untyped field, and the
interpreted and compiled output is byte-identical. `New`'s argument handling
is the exception, and it sits in the call machinery rather than in the object
model, which is consistent with the argument rather than against it.

**Where the library would meet it.** A deserializer built on [RT-034] calls
`New (T, [])` to make a blank instance before filling it with `SetMember`.
Where the class declares a constructor taking arguments, that call is the
too-few case — so the diagnostic a library would show a user for "this class
cannot be made blank" is currently an internal subscript error interpreted and
the right sentence compiled. Nothing here is built on it yet; the conformance
corpus covers a wrong type (`0222`) and a non-List (`New (Thing, 3)`) but not
a count mismatch, which is why it passes.

### L-5 — `System.WriteLn` took one argument compiled and any number interpreted

**Status:** **CLOSED in 0.3.2**, verified 2026-11-12. Found on 0.3.0 while
porting a star chart into `examples/stars.a24`, and unchanged on 0.3.1.

The C back end now takes the qualified spelling variadically, so [RT-001] and
[MOD-011] hold on both processors. Measured across the counts the old boundary
cut at, plus mixed types, with `System.Write` and the unqualified spelling
alongside — **identical interpreted and compiled**:

```console
                          # System.WriteLn ();
one                       # System.WriteLn ('one');
ab                        # System.WriteLn ('a', 'b');
a42b                      # System.WriteLn ('a', 42, 'b');
x12.5truey                # System.WriteLn ('x', 1, 2.5, True, 'y');
ab                        # System.Write ('a', 'b'); System.Write (#10);
unqualified still fine
w1
```

⚠️ **This is what unblocks compiling a `graph` program that reports to the
console.** `graph` shadows `Write`/`WriteLn`, so the qualified spelling is the
only console route such a program has, and it was the one the back end
refused. `examples/stars.a24` still concatenates its one console line into a
single argument — that is now a style rather than a workaround, and the
comment naming this entry should go when anything else touches the file. What
follows is the original report.

The qualified spelling of a variadic built-in refuses more than one argument
in the C back end, and runs interpreted:

```console
$ algc q.a24                          # System.WriteLn ('a', 42, 'b');
a42b
$ algc --compile --out=out q.a24
Uncaught: A call to 'System.WriteLn' is not supported by the C back end yet.
```

Measured on 0.3.0, the boundary being exactly one argument:

| | interpreted | compiled |
| --- | --- | --- |
| `System.WriteLn ()` | ✓ | ✓ |
| `System.WriteLn ('a')` | ✓ | ✓ |
| `System.WriteLn ('a', 'b')` | ✓ | **refused** |
| `System.WriteLn ('a', 42, 'b')` | ✓ | **refused** |
| `System.Write ('a', 'b')` | ✓ | **refused** |
| `WriteLn ('a', 'b')` — unqualified | ✓ | ✓ |

**The specification says both halves of this should work.** [RT-001] says
`Write` and `WriteLn` "take any number of values" and so "can never fail this
way at all", and [MOD-011] says the qualified spelling always reaches the
built-in — "which is what makes taking a name over safe: nothing is put out of
reach." The interpreter obeys both. The C back end puts exactly that out of
reach, so the processors disagree and the interpreter is the one matching the
specification. The conformance corpus has the construct already:
`0187-declaring-a-builtins-name.a24` calls `System.Write` and
`System.WriteLn ()`, but never with two arguments, which is why it passes.

⚠️ **It lands on `graph` programs specifically, and that is not a
coincidence.** `graph` shadows `Write` and `WriteLn`, so `CLAUDE.md` requires a
graph program to reach the console through `System.WriteLn` — which makes this
the one spelling such a program has, and the one the compiler refuses. A
`graph` program that reports more than a single value to the console therefore
runs interpreted and cannot be compiled at all. `graph`'s *own* variadic
`WriteLn ('a', 'b')` compiles fine; it is a unit-declared subprogram rather
than a built-in, so only the console route is affected.

**The library is not working around this silently.** `examples/stars.a24`
builds its one console line by concatenation —
`System.WriteLn (Str (A) + ' … ' + Str (B))` — because a single argument
compiles, and says so in a comment naming this entry. Nothing else here calls
the qualified spelling with more than one argument; checked across all ten
units and every example.

### L-4 — a warning went to stdout, where it mixed with the program's output

**Status:** **CLOSED in 0.3.2**, verified 2026-11-12. Reported 2026-09-04 and
open across 0.1.5, 0.1.6, 0.2.4, 0.3.0 and 0.3.1 — the longest-lived entry
here, and by the end it had four triggers.

⚠️ **All four moved, and the fix is narrow: every warning is still emitted,
with the same text and the same line number, on stderr instead.** A fix that
silenced any of them would have been worse than the defect. Measured with the
streams separated:

| trigger | stdout | stderr |
| --- | --- | --- |
| an overload settled at run time | `int` / `str` | `[WARN] …:6: 'Same' selects among 2 overloads at run time.` |
| a member hiding a visible extension | `99` | `[WARN] …:12: 'Size' is a member of Box, …` |
| an Integer handed to `and` | `3` | `[WARN] …:1: an operand of 'and' is an Integer, where '&', '|' and '~' …` |
| an `Any`-receiver extension against a member | `my own` | `[WARN] …:12: 'Describe' is a member of Report, …` |

**Two consequences worth recording, because both were live constraints.**

1. ⚠️ **The reflection unit's naming is no longer governed by this.** Its
   surface is all extensions [MOD-015], so any exported name a consumer also
   used as a member printed a line in *their* stdout — which ruled out
   `Describe`, `Equals` and the other obvious names. The warning still fires,
   correctly, but on stderr, so a clash now costs a diagnostic rather than a
   corrupted program. `NOTES-REFLECTION.md` §3 records the change.
2. ⚠️ **`check-reference.py` and `examples/check.sh` are no longer exposed.**
   Both compare a program's output byte for byte, so a single legitimately
   ambiguous overload anywhere in a unit would have broken them. That is why
   the entry mattered to this repository beyond tidiness.

**H-3's last defect-shaped edge goes with it.** 0.1.6's Integer-to-`and`
warning closed most of that hazard and its stream was the residue; the hazard
itself stands, `6 and 3` still being **3** by [EXP-010] rather than 2, which is
specified behavior the library defends against rather than anything to fix.

What follows is the original report, kept for the four triggers' detail.

⚠️ **The fourth trigger, measured on 0.3.1** — a consumer's own member of the
same name as a visible extension, with the streams separated:

```console
$ algc clash.a24 2>/dev/null
[WARN] clash.a24:14: 'Describe' is a member of Report, so the extension of that name is not reached.
member wins: my own
$ algc clash.a24 2>&1 >/dev/null
                                   # stderr is empty
```

This one is the library's problem rather than a consumer's mistake: the
reflection unit's whole surface is extensions [MOD-015], so any name it
exports that somebody uses as a member prints a line in *their* program.
`NOTES-REFLECTION.md` §3 records it as the governing naming constraint.

⚠️ **0.1.6 added a THIRD trigger**, and a common one: handing an Integer to
`and` or `or` warns, naming the new bitwise operators, and that warning goes
to stdout like the others. The warning itself is a good addition and closes
most of H-3; its stream is this defect. No unit here triggers it, checked by
compiling each of the ten.

**Still open on 0.2.4**, re-verified by running `WriteLn (6 and 3)` with the
two streams captured separately: the warning arrives on **stdout** with `3`
after it, and **stderr is empty**. Unchanged across 0.1.5, 0.1.6 and 0.2.4.

An overload the compiler cannot settle statically still warns — correctly,
since [ERR-010]'s dynamic selection really does happen when argument *types*
decide — but the warning is written to **stdout**:

```console
$ algc q.a24 2>/dev/null
[WARN] q.a24:2: 'Same' selects among 2 overloads at run time.
[WARN] q.a24:3: 'Same' selects among 2 overloads at run time.
int
str
```

Nothing at all arrives on stderr. A diagnostic conventionally goes there, and
here it matters concretely: this repository's `check-reference.py` and
`examples/check.sh` compare a program's output byte for byte, so a single
legitimately ambiguous overload anywhere in a unit would break them — and
would corrupt the output of any program that pipes or diffs its own.

#### 0.1.5 gave it a second trigger, and this one the library will meet

Extension functions [FUN-015] brought a new warning with them: where a member
hides an extension that is visible, [ERR-010] names the class that took it.
That diagnostic is the right one to have — it closes the silent capture a class
could otherwise perform by adding a member in a later version — and it is
written to **stdout**, the same way:

```console
$ algc e2.a24 2>/dev/null
[WARN] e2.a24:19: 'Size' is a member of Box, so the extension of that name is not reached.
99
```

Nothing on stderr, measured the same way as the overload warning above.

**How much likelier, measured.** Narrower than it first looked. The check is
**receiver-type aware**, not name-only: a `Contains` extension declared on
`String` does not disturb `Map.Contains`, and prints nothing.

```algol24
extension function Contains (S : String, What : String) : Boolean;
…
WriteLn (M.Contains ('k'));          // a Map -- silent
WriteLn ('hello'.Contains ('ell'));
```
```console
true
true
```

So the collision has to be on the *same* receiver type — an extension on
`String` against a member of `String`, not against any member anywhere. That is
a much smaller target than the first reading of [EXP-022] suggested, and it is
why `strings` could take `Trim`, `Upper`, `Lower`, `Repeat`, `Split` and `Join`
without provoking anything: none is a member of `String` or of `List`, and all
91 class member names across the library were checked against the six.

It remains worse than the original trigger, because the original needs a
program to declare two same-arity overloads of one name — which this library
does not do at all — where this one needs only a name a future version of some
class might add as a method. That is exactly the capture the diagnostic exists
to announce, so the diagnostic is right and only its stream is wrong.

It bites this repository twice over, because `check-reference.py` and
`examples/check.sh` both compare a program's output byte for byte. A reference
example that happened to collide would not merely look untidy; it would fail
the suite, and the failure would read as a wrong answer rather than as a
diagnostic on the wrong stream.

The library's defence: keep an extension's name clear of the members of its own
receiver type, and audit rather than discover. Done for `strings` and recorded
in `finished/PLAN-STRINGS.md`; a consumer importing `strings` beside `graph`, and using
`List`, `Map` and `Buffer` members throughout, prints only its own output.

That is a defence against a symptom, which is why the entry stays open. A
diagnostic belongs on stderr, and until it is there, every program built on an
extension-exporting unit carries the risk that a later version of some class
starts narrating into its output.

---

### L-2 — overload selection warned even when arity made it unambiguous

**Fixed in 0.1.4**, verified 2026-09-04.

Arity now settles the choice statically and silently:

```algol24
procedure Bump (By : Integer);              // global
procedure Bump (T : Thing, By : Integer);   // object first
```
```console
global bump 5
7
```

Not a warning anywhere, where 0.1.3 warned at every call site. And the fix is
the narrow one that was wanted rather than a blanket silencing — an overload
whose arities match, so that argument types really do decide at run time,
still warns exactly as [ERR-010] requires:

```console
[WARN] q.a24:2: 'Same' selects among 2 overloads at run time.
```

What it unblocks: the Pascal-style free-function aliases, `Print (W, 'text')`
beside the screen-wide `Print ('text')`, which work perfectly and were merely
too loud to ship.

The half of the entry about warnings reaching stdout is not fixed and
continues as L-4.

### L-3 — two methods of one name and arity were accepted, and the first silently won

**Fixed in 0.1.4**, verified 2026-09-04.

Refused now, at check time, with the message the situation deserves:

```console
Uncaught: 'Twin' is already defined.
```

Both halves are covered — a duplicated method and a duplicated top-level
function each answer it — where 0.1.3 accepted either in silence and called
the first declaration forever. This is the defect that would have made the
`MoveTo` collision invisible: a pen that quietly moved the viewport instead
of drawing from it.

The harmless case is untouched, as it should be: a method and a top-level
function of one name still coexist, `O.Ping` reaching the method and a bare
`Ping` the function. Inside a class the bare name reaches the *top-level*
one, and `this.Ping` names the method.

---

## Hazards

### H-4 — unbounded recursion ends the process with a signal, and takes buffered output with it

**Status:** recorded 2026-11-12 on 0.3.2, while designing `reflect`. A hazard
rather than a defect: the specification says nothing about stack exhaustion,
and a language that keeps live state on the C stack in two of its three
processors has nothing obvious to promise instead. The library defends itself.

**Why it is recorded now.** Before reflection a generic walk over somebody
else's data could not be written at all, so this was only reachable by writing
an obviously non-terminating function. `Fields` and `Member` make a generic
recursive walk four lines long, and a cyclic object graph is ordinary data —
a parent pointer, a doubly linked list, two nodes that know each other. So the
hazard went from theoretical to the first thing a describer meets.

**Measured.** Two instances pointing at each other, walked by the obvious
recursive describer:

```algol24
var A := Node ('a');
var B := Node ('b');
A.Peer := B;
B.Peer := A;

function Naive (X : Any) : String;
begin
    if X = nil then Exit 'nil';

    var Out := TypeOf (X).Name + '(';
    for var F in Fields (X) do Out := Out + F + '=' + Naive (Member (X, F)) + ' ';
    Exit Out + ')';
end
```

```console
$ algc cycle.a24 ; echo "exit: $?"
exit: 139
```

⚠️ **Exit 139 is SIGSEGV, and stdout came back EMPTY.** Three `WriteLn` calls
ran to completion before the recursion started and **none of their output
appeared** — the buffer died with the process. Nothing on stderr either. That
is the sharp part: not that infinite recursion fails, which is unremarkable,
but that it fails with no diagnostic and destroys the record of how far the
program got. A `try … except` cannot catch it, so a library cannot turn it into
a raised error.

#### How the library defends against it

`reflect` carries the set of containers it has already entered, and both its
routines stop on arriving somewhere twice. Instances and collections are
identity-comparable and usable as `Set` members, deduping by identity, which is
what makes the guard cheap and exact:

```console
cycle-safe:  Node(Tag='a' Peer=Node(Tag='b' Peer=<Node seen>))
self-ref:    Node(Tag='s' Peer=<Node seen>)
```

⚠️ **Any future generic walk here needs the same guard, and no test will tell
you it is missing.** Every natural fixture is acyclic, so an unguarded walker
passes its whole suite and takes down the first consumer that owns a back
pointer. `reflect`'s `Describe Terminates On A Cycle`,
`Describe Terminates On A Self Reference`,
`Describe Terminates On A Collection Holding Itself`,
`SameContent Terminates On Two Cycles Of The Same Shape` and
`SameContent Terminates On A Self Reference` exist to keep that from rotting.

### H-3 — `and` and `or` look like bitwise operators and are not

**Status:** **mostly closed by 0.1.6**, which added `&`, `|`, `^`, `~`, `<<`
and `>>` [EXP-023, EXP-024] and a warning when `and` or `or` is handed an
Integer. Recorded 2026-10-24 as a hazard; what remains is narrow and is
described at the foot of this entry. Specified by [EXP-010], checked before
being written up.

The hazard as recorded, which 0.1.5 and earlier had in full: the language had
**no bitwise operations of any kind**, and the two operators
that look like they would do the job quietly do something else. `and` and `or`
test truthiness and yield an *operand* rather than a Boolean, so the obvious
thing a reader writes compiles and answers a plausible wrong number:

```algol24
WriteLn (6 and 3);     // 3   -- bitwise would be 2
WriteLn (6 or 3);      // 6   -- bitwise would be 7
```

Nothing is wrong here. [EXP-010] says both operators test truthiness, and
`conformance/0048-logical-operators.a24` asserts the operand-yielding result in
terms. The hazard is that the *absence* of bitwise operators is concealed by the
presence of two operators wearing their names — so the failure is a wrong number
rather than `Undefined`.

**And on 0.1.5 every other spelling was absent**, which is what made the trap
worth recording rather than merely noting:

| Written | Result |
| --- | --- |
| `6 & 3`, `6 \| 3`, `6 ^ 3`, `~0` | `Error: Unexpected character` — not lexed |
| `1 << 4`, `256 >> 4` | `Expect expression!` |
| `6 xor 3`, `6 band 3`, `6 bitxor 3` | parse errors |
| `Xor (6, 3)`, `Shl (1, 4)` | `Undefined variable` |

The word "bitwise" did not appear in the specification.

#### What 0.1.6 changed, measured

`finished/PROPOSAL-BITWISE-OPERATORS.md` was accepted. Verified on 0.1.6:

| Written | Answers |
| --- | --- |
| `6 & 3`, `6 \| 3`, `6 ^ 3`, `~6` | `2`, `7`, `5`, `-7` |
| `1 << 4`, `256 >> 4` | `16`, `16` |
| `1 << 70` | `1180591620717411303424` — grows rather than wrapping [EXP-007] |
| `-7 >> 1` | `-4`, the floor — the shift is **arithmetic** |
| `-7 div 2` | `-3`, truncated toward zero, so a shift is not a division |
| `1.5 & 2`, `True & True`, `~1.5` | refused: `& expects Integers.` |
| `6 and 3` | still `3`, and now **warns** and names `&`, `\|` and `~` |

⚠️ **The remaining hazard is the warning's stream, not the arithmetic.** The
new warning is written to **stdout**, so a unit using `and` on an Integer would
add a line to every consumer's output. That is L-4, not a separate fault.
Measured: no unit in this library triggers it, because this entry already had
them avoiding the construct.

⚠️ **A right shift is still not a division**, which is the one trap that
survives. The old note that `div` truncates where a shift floors now matters
*more* rather than less, because both spellings are available and they disagree
on exactly the negative values where it is easy not to look.

#### How the library defended against it before 0.1.6, none of which remains

1. **Reach for C, and say so.** `graph` declares
   `XorWords … external 'alg_xor'` with a doc block naming the gap. That is the
   right shape for a gap the language does not offer — an `external`, without
   ceremony.
2. **Know that the C answer is narrow.** `alg_xor` is
   `(int64_t)(int32_t)((uint32_t) a ^ (uint32_t) b)` — a **32-bit** operation
   declared as `(Integer, Integer) : Integer`. Correct for a pixel word, wrong
   for an Integer, and the declaration cannot say which. Deliberate narrowing
   rather than H-1's accident, but it means the helper must stay private to the
   unit that needs that width.
3. **Do not write a general `bits` unit.** A pure Algol-24 exclusive or is
   writable and costs 0.7 ms per call — 13.94 s for 20,000 of them, measured —
   so it would be unusable in the pixel loop that wants it most, and a
   foreign one would carry `alg_xor`'s width problem into a public surface.
4. **Arithmetic, where the quantity is small.** `crt`'s `ByteAt` picks a byte
   with `div` and `mod`, and both units split colors into channels the same
   way. Correct, slow, and duplicated — which is the cost this hazard actually
   imposed.

#### What has been removed since

**Defences 1, 2 and 3 are gone**, `alg_xor` with them. The measurement that
decided it, three runs, bare loop subtracted:

| | per call |
| --- | --- |
| `alg_xor`, the foreign call | 7.58 µs |
| native `^` | **1.87 µs** |
| the hand-rolled `div`/`mod` version | 547 µs |

So the C was not a necessary evil but **four times slower** than the operator
that replaced it — a foreign call's overhead dwarfing a one-instruction
operation. Note 3's warning against a pure Algol-24 exclusive or was right
about the hand-rolled loop and says nothing about a native operator.

It also fixed a fault. `alg_xor` narrowed to 32 bits and sign-extended, so an
impossible color silently drew a wrong pixel under `XorPut` where `CopyPut`
already raised — `Buffer.PutInt` refusing anything outside int32. Measured
before the change:

```console
CopyPut with a huge color:
  raised: Integer 4278190079 is too large to use here.
XorPut with the same color:
  drew without complaint
```

`SetColor` is now bounded at `White + Blink`, so such a color is refused at
the call rather than deep in a draw, and the two write modes agree.

**Defence 4 is gone too**, and with it the last of this entry's cost. `crt`'s
`ByteAt` is one line:

```algol24
Exit (B.GetInt ((N div 4) * 4) >> (8 * (N mod 4))) & 255;
```

where it used to add 2^32 to bring a negative word into range, then build a
multiplier in a loop and divide by it — because `div` truncates toward zero
and so is not a shift. **The mask makes the correction unnecessary**: `& 255`
discards the sign-extended bits, so no `+ 4294967296` survives anywhere in
either unit.

Seven sites in all: `ByteAt`, `graph`'s `Mix`, its RRGGBBAA pixel path, its
font colour probe, and `HighVideo`/`LowVideo` in both units.

⚠️ **Equivalence was proven before any of it was changed**, not after. The old
and new forms were run against each other over the int32 range — all four byte
positions for `ByteAt`, every colour a bounded setter now admits for the
channel split, and the signed pixel words including `-2147483648` — at zero
disagreements. Then the tests: each converted path has a named one, the
RRGGBBAA path being covered by `An Emoji Glyph Arrives In Its Own Colors`
rather than by the formula alone.

**This one has a proposal rather than only a defence.**
`finished/PROPOSAL-BITWISE-OPERATORS.md` asks the language project for the six
operations over the unbounded Integer, where they can be total in a way no
library can manage: infinite two's complement needs no width, so `not 0` is
`-1` and `1 shl 70` grows by [EXP-007] rather than wrapping.

### H-2 — a String returned from C borrows the bytes, and does not copy them

**Status:** hazard, not a defect. Recorded 2026-09-05 while building `file`.
The library's defence is below; whether the language wants to say anything
about it is the compiler project's call.

**Still present on 0.2.4**, re-verified rather than assumed: a C function that
reuses one static buffer was declared to answer a String, called twice, and the
String held from the first call changed under the second —

```console
held: alpha
after a second call, the first String is: omega
```

— so the buffer-and-count discipline this library keeps is still load-bearing
and not a precaution against something since fixed.

A foreign function declared to answer a `String` hands back a `const char *`,
and the String that arrives **points at the C's own memory**. It is not a
copy. So a C function that reuses or frees its buffer corrupts a String the
program is still holding — silently, with no error, and at a distance:

```algol24
function ReadAll (Path : String) : String;
    external 'alg_file_readall' in 'libfileffi.so';

var A := ReadAll ('a.txt');
var B := ReadAll ('b.txt');     // the C reused its buffer

WriteLn (A);                     // garbage: A pointed at what B overwrote
```

Measured, not assumed. `A` came back as `first file` before the second call
and as binary rubbish after it, with `A = 'first file' + #10` answering False.

This is arguably the same ground [FUN-014] already covers — the language
defines the call and not the callee, and a C function's memory is the callee's
business. It is recorded separately because the failure is *not* a signature
mismatch: the declaration is right, the types agree, and the call is
well-formed. What is wrong is a lifetime, which nothing in the declaration can
express.

**The library's defence: no foreign function here answers a pointer.**

`fileffi.c` says so at the top and every function obeys it. A read fills a
buffer the *caller* owns and answers a count:

```algol24
var N := RawSize (Path);
var B := Buffer (N);            // sized exactly -- see below

RawRead (Path, B.Address, N);

Exit B.Text;
```

Going the other way is safe and needs no care: a `String` passed **into** C
arrives as bytes the C reads during the call and does not keep.

**A neighboring trap, found the same day.** The library's C is named
`alg_something`, which is also the compiler runtime's prefix. `fileffi.c`
first used `alg_file_exists` -- a name the runtime already defines. Nothing
failed while the C was a shared library, because the interpreter resolves the
library's copy first; it failed only when `examples/build.sh --static` linked
everything into one executable. Not a language fault, but worth knowing that
the two namespaces overlap and only one build shows it.

**A Buffer must be sized exactly, which helps.** `Buffer.Text` refuses a
buffer holding a zero byte — `A Buffer holding a zero byte has no Text.` — so
a buffer larger than its contents fails on its own trailing zeros rather than
answering a truncated String. That turns a silent fault into a loud one, and
is why `file` calls `alg_file_size` before every read.

### H-1 — a foreign declaration's signature is never checked, and a mismatch is silent

**Status:** specified behavior, not a defect. Recorded 2026-09-02,
reclassified the same day on reading [FUN-014], and deliberately left alone
in 0.1.4 — there is nothing here for the compiler to fix, and the library's
defences below are the answer.

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
`0x070605ffffffffff`, which is uninitialized memory.

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
