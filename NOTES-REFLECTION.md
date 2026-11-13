# Notes: reflection shipped in 0.3.1, and what finalizing it here needs

**Status:** notes, not a plan. Nothing here chooses a unit or a surface — the
user names those. What follows is what was measured against `algc 0.3.1` and
re-verified on **0.3.2**, plus the decisions the unit cannot be written
without.

⚠️ **0.3.2 closed all three defects this work found or depended on** — L-4,
L-5 and L-6 — so `DEFECTS.md` has nothing open for the first time. Two of them
mattered here: **L-6 blocked anything that constructs**, and **L-4 governed
the unit's naming**. Both constraints are gone, and §3 and §5 are revised
accordingly.

`PROPOSAL-REFLECTION.md` has the history against what this library asked for.
This file is the working side: what is true, what bites, and what is still a
question.

---

## 1. What shipped, and that it works

Seven things, six built-ins and one method on a type value:

| | rule | verified |
| --- | --- | --- |
| `Fields (X)` | [RT-031] | a Map, name to declared type, parent-first; `''` for an untyped field |
| `TypeOf (X)` | [RT-032] | a type as a value; interned; a class answers `Name`, `Parent`, `Fields`, `Methods`, `Properties` |
| `Member (X, Name)` | [RT-033] | reads a field, runs a property, reaches a **private** member |
| `Invoke (X, Name, Args)` | [RT-033] | calls a method, selects among an overload set at run time, reaches a private one |
| `New (T, Args)` | [RT-034] | constructs from a type held as a value; selects on count and type since 0.3.2 |
| `SetMember (X, Name, V)` | [RT-034] | writes a field by name, finds an inherited one, folds the name |
| `T.Accepts (X)` | [RT-035] | the dynamic form of `is`, including on a class fetched out of a Map |

⚠️ **Byte-identical interpreted and compiled**, measured against one class
carrying inheritance, an overload set, a property, a private method and an
untyped field:

```console
Fields:      [Tag:String, Name:String, Worth:Integer, Ratio:Double, Loose:]
TypeOf name: Prop parent Base
Methods:     [Init:[[]], Scaled:[[Integer], [Double, Boolean]], Secret:[[]]]
Properties:  [Rich:[[]]]
Member:      widget true
Invoke:      126 private 43
SetMember:   11 now 11
Accepts:     true false
overload:    33 / 0
inherited:   1 parent methods [Init:[[]], Inherited:[[]]]
```

That is the structural claim `PROPOSAL-REFLECTION.md` made — reflection lives
in the shared object model, so all three processors inherit one implementation
— now measured on the shipped thing rather than argued. There is no `--vm`
flag on `algc 0.3.1`, so "both processors" here means interpreted and compiled
C; the bytecode machine is not separately reachable from the command line.

## 2. The defect this found, closed in 0.3.2

`DEFECTS.md` **L-6**, found here and fixed. On 0.3.1, `New (T, [])` against a
constructor wanting one argument leaked `Index 0 out of range 0..-1.`
interpreted, and `New (T, [1, 2])` was **not refused at all** — the extra was
discarded and a plausible instance came back. Compiled was correct on both.

**0.3.2 matches the written form in both shapes**, which is more than a count
check would have given: a class with one constructor answers
`Expected 1 arguments but got 0.` exactly as the written call does, and an
overload set answers `No matching signature`. Selection still works through
`New` — `Init (A)` and `Init (A, B)` answer 105 and 1056 for `[5]` and
`[5, 6]`, matching the written calls — and the other five built-ins are
byte-identical to their 0.3.1 output, so the fix was narrow.

⚠️ **So a deserializer can construct now.** `New (T, [])` to make a blank
instance is still only possible where the class's constructors accept no
arguments; where one demands them, the unit gets a catchable sentence to
re-raise rather than an internal subscript error. That is the difference
between a decent diagnostic and a confusing one, and it is why the read half
is no longer the only half worth starting.

## 3. Two things that will bite whoever writes the unit, and one that no longer does

⚠️ **`TypeOf (nil)` is `nil`, so every entry point needs a guard.** [RT-032]
is explicit about it — `nil is X` is false for every `X`, so a type value for
it would be a thing to compare that the language says is not there. The
consequence is that the natural first line of a describer raises:

```console
TypeOf(nil).Name:   Only instances have properties.
```

`Fields (nil)` is fine and answers an empty Map, as `Fields (42)` does.

⚠️ **`SetMember` does NOT enforce the field's declared type, so the unit
must.** This is specified rather than wrong — [RT-034] says it is "exactly as
permissive at run time as `X.Field := <untyped>` already is" — but it inverts
what the proposal expected. Measured: a String written into a field declared
`Integer` lands, and the field afterwards holds a String.

```console
String into Integer: oops -> W is now oops a String
out of subrange:     99 is not in Digit.
into a property:     'Rich' is a property of Thing and cannot be assigned.
```

So the declared type `Fields` answers is what a deserializer checks
**against**; it is not checked **for** it. A subrange and a property are
refused; a plain type mismatch is not.

⚠️ **Naming WAS the real constraint, and 0.3.2 removed it.** On 0.3.1 a
member of the name won [EXP-022] and the compiler said so on **stdout**
(`DEFECTS.md` L-4), so any name this unit exported that a consumer also used
as a member printed a line in *their* program's output. That ruled out the
obvious names — `Describe`, `Equals`, `Clone` and the rest. Measured again on
0.3.2, with the streams separated:

```console
$ algc t4.a24 2>/dev/null
my own                             # stdout is the program's own output
$ algc t4.a24 2>&1 >/dev/null
[WARN] t4.a24:12: 'Describe' is a member of Report, so the extension of that name is not reached.
```

The warning still fires, correctly — it is what announces the capture a class
performs by adding a member in a later version — but on stderr. **So a clash
now costs a diagnostic rather than a corrupted program**, and the unit may be
named for what reads best.

⚠️ **Two names are still unavailable, for a different reason.** [RT-032] makes
`Name`, `Parent`, `Fields`, `Methods`, `Properties` and `Accepts` members of a
class *value*, and a member wins, so an extension of one of those names could
not be called on a class — the specification notes this costs "an extension
called `Name` or `Accepts` the call on a class". That is a real surface
collision rather than an output one, and it survives the L-4 fix.

## 4. What a copy protocol needs, and the one hole left

`REPORT-CONCURRENCY.md` §4 left the copy protocol open. It closes. A generic
`Encode`/`Decode` pair of about 50 lines, naming no field of anything, round
trips a class with a String, an Integer, a Double, a Boolean and a nested
instance:

```console
wire:   Rec|Name=widget|Worth=42|Ratio=1.5|Flag=true|Kid=(Inner|N=9)
again:  Rec|Name=widget|Worth=42|Ratio=1.5|Flag=true|Kid=(Inner|N=9)
equal:  true
typed:  Rec, kid a real Inner
fresh:  false (a copy, not the same object)
```

Identical interpreted and compiled. What the encoder needs is `TypeOf (X).Name`,
`Fields (X)` for the names and declared types, and `Member` per field; what the
decoder needs is `New`, `Fields` again to learn what to parse each piece into,
and `SetMember`.

⚠️ **The hole: nothing turns a type's NAME back into a type.** `TypeOf`
answers a type from a *value*, and `TypeOf ('Rec')` answers the `String` type,
correctly and uselessly. So a decoder hand-writes `['Rec' : Rec, 'Inner' :
Inner]` to turn the name in a record into something `New` accepts. A class
survives being held in a `var`, a `List` and a `Map` and comes back
constructible, so the registry itself works fine.

**That is one table per program where the proposal counted a codec per class**,
and it goes stale *loudly* — a missing key fails the decode rather than
dropping a field in silence. Worth noting and not worth asking the core about
until something here needs it.

### Two more limits a serializer meets, both at the collection boundary

⚠️ **A field declared `List of Integer` answers `List`**, so the declared type
loses the element type:

```console
Q1 declared types: [Kid:Inner, Items:List, Maybe:Inner, Plain:]
```

[RT-032] says this plainly for a signature — "a parameter of `List of Integer`
reads as `List`, which is what a field of that type answers too" — so it is
specified rather than missing. The consequence for a decoder is that it cannot
learn from the field table what to parse a list's elements into; it has to
either encode each element's own type beside it, or be told.

⚠️ **`Member` does not reach a collection's members, though the written form
does.** `[1, 2].Length` is 2; `Member ([1, 2], 'Length')` is
`Undefined property 'Length'.` A collection is not a class instance [TYP-009]
and `Fields` of one is an empty Map by [RT-031], so this is consistent with
the rest — but it sits oddly against [RT-033]'s "both answer exactly what the
written form answers", which is framed around a class. Not recorded as a
defect, because the specification's framing is about declared classes
throughout; worth a question to the core if a unit here ever wants it. The
practical answer is that a generic writer **iterates** a collection field and
does not reflect on it.

## 5. Decisions the unit needs, which are the user's

1. **The unit's name.** `reflect` is the obvious one and collides with
   nothing in the ten units.
2. **Which of the three motivating uses it covers.**
   `PROPOSAL-REFLECTION.md` named a describer, a deep equality and a
   serializer. The first two are a few lines each over `Fields` and `Member`;
   the serializer is the one with real design in it, and the one the
   concurrency story wants.
3. **The exported names.** Much freer than it was: §3's stdout constraint is
   gone, so the only names actually unavailable are the six [RT-032] puts on a
   class value. Pick for readability.
4. **Whether the serializer belongs here at all.** It needs a wire format,
   which is a choice this library has not had to make before, and a registry
   the *consumer* owns rather than the unit. A describer and a comparer need
   neither.
5. ~~**Whether to wait on L-6.**~~ Moot — L-6 is fixed, so the constructing
   half is as writable as the read half. Nothing here is blocked on the
   compiler any more.

## 6. What it costs

⚠️ **The first unit here to call any of the seven raises the library's
compiler floor from 0.1.6 to 0.3.2** for every consumer, not just for programs
that use reflection — `./test.sh` runs every unit. **0.3.1 is where the
built-ins exist and 0.3.2 is where `New` selects correctly interpreted**, so a
unit that only reads could claim 0.3.1 and one that constructs could not;
carrying two floors to save one patch release is not worth it, so the number
is 0.3.2.

That is the whole cost. There is no per-program run-time cost — the tables are
per class and already built, which is the test the shadow-stack measurement
set — and nothing a program that never reflects pays for.

## Appendix: how this was gathered

Everything above came from running `algc 0.3.1`, interpreted and compiled to C
with `cc -std=c11 -O2 … -lffi`. The probes: all seven exercised against a
two-level class with an overload set, a property, a private method and an
untyped field, diffed between processors; `Fields` and `TypeOf` against `nil`,
an Integer, a String and a List; `Member` and `Invoke` against non-instances
and a collection; `SetMember` against a wrong type, a subrange, a property, an
undeclared name and an untyped field; `New` against a stored class, a wrong
argument type and a wrong argument count; an `extension function … (X : Any)`
in a real unit called both spellings from a consumer; a member/extension clash
with stdout and stderr separated; and the `Encode`/`Decode` round trip.

Re-verified on 0.3.2: the seven-built-in probe byte-identical to its 0.3.1
output and across processors; the round trip unchanged; the `nil`,
`SetMember` and collection-boundary edges unchanged, all three being specified
behavior rather than faults; L-4's four triggers with the streams separated;
L-5 across the counts its old boundary cut at; and L-6 against a
single-constructor class, an overload set and the written form of each.

⚠️ **No compiler source was read.** The specification was pulled for
[RT-031] through [RT-035], which is reference material; behavior was
established by running programs written for the purpose.
