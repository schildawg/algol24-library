# Proposal: a class's fields, listable at run time

**From:** the `algol24-library` project, as a user of the language

**Status:** **ACCEPTED, answered with more than it asked for, and still being
built.** The read half is specified — [RT-031] `Fields` and [RT-032] `TypeOf`,
with conformance cases 0218, 0219 and 0220 — and the core replied with its own
`PROPOSAL-REFLECTION.md`, which reverses this document's boundary in two places
and says so. ⚠️ **The work is in progress and there will be a release when it
is done.** `algc 0.3.0` answers `Undefined variable 'Fields'.`, so nothing here
can be built on any of it yet, and the figures and spellings below may still
move. The original ask follows, unedited. What changed is recorded next.

**Concerns:** one listable table and one type answer — **read-only**, no new
machinery, and nothing a program can use to change its own shape

---

## What was decided, against what this asked

**The enumeration this called "the only hole" is one call, and it carries the
type.** [RT-031] `Fields (X)` answers a **Map** from each field's name to the
type written on it — `''` where none was — so the list and the type arrive
together rather than as the two separate asks below. An instance answers its
class's fields, a class answers its own, and a value that is neither answers an
**empty Map** rather than raising, on the same bargain `5 is Dog` makes.

**[RT-032] goes well past this document.** `TypeOf (X)` answers a type as a
value, interned so `TypeOf (1)` and `TypeOf (2)` are identical and comparable
the way a class handle is, and **not constructible** — `TypeOf (42) ()` is
refused. A class answers its `Name`, its `Parent`, its `Fields`, and its
`Methods` and `Properties`, each a Map from name to a **List** of signatures,
a List because two methods of one name are an overload set [FUN-013].

⚠️ **`Fields` answers the whole inheritance chain where `Methods` answers only
the class's own, and the asymmetry is deliberate.** An instance holds every
inherited field as its own state, so a record of it needs all of them; an
overridden method *replaces* the one above it [CLS-011], so merging the chain
would put an override and an overload under one name indistinguishably.
`Parent` walks.

### All six open questions, answered

| this document asked | the answer |
| --- | --- |
| do private fields appear? — *"the hardest one"* | **yes.** `private` governs what a program may *write* and is settled before it runs; nothing records it at run time, so hiding them would be new machinery in all three processors. Stated as a trapdoor rather than left to be found |
| what order? | **declaration order, parent-first** — the order a class seeds its fields [CLS-005], reproducible because collections iterate in insertion order [COL-007] |
| are inherited fields included? | **yes**, the whole chain — see the asymmetry above |
| does a `property` appear? | **no**, it computes rather than stores [CLS-017], so a record built from the list can be read back. Listed separately as `Properties`, which a describer wants and a serializer must not write through |
| what is a type, as a value? | an interned value per built-in type, compared by identity; a class answers itself |
| a member or a built-in? | **a built-in, with the sugar left to this library as an extension** — which is what this document's own [EXP-022] risk argued for, since a member named `Fields` would silently shadow a class's own |

⚠️ **One citation here is wrong and is left standing as written.** Open
question 1 cites [MOD-006] for `private` inside a class; that rule is
`private` on a *module's* top-level declaration, and visibility sections
inside a class are **[DCL-011]**. The core caught it. `CLAUDE.md`'s own use of
[MOD-006], for `strings`' private helper, is correct.

### Where the core reversed this document, and why

This document excluded setting by computed name and invoking a method by name
as *"where a statically-checked language stops being checkable"* — "deliberately
excluded rather than deferred". **Both are now proposed**, and the argument is
that the premise does not hold here:

- overload selection is **already** at run time [EXP-013], the type system
  being gradual and an argument's declared type possibly `Any`;
- `alg_invoke` **already** dispatches by name, and is how every method call in
  the compiled back end works.

So a call by a computed name reaches the same machinery a written one does. The
line this document drew still holds for the one thing neither document asks
for: **building or altering a class at run time.** That boundary was drawn too
wide, not in the wrong place.

⚠️ **Asking for the declared type turned out to be what makes the write side
safe, which this document did not foresee.** A written field assignment widens
[VAL-001]; a by-name write must too, or it becomes the one way to put an
Integer where a class promised a String. The field table carrying the declared
type is what gives that check something to check against. The type was asked
for here to tell an Integer from a String in a serializer — it earns its place
twice over.

### Still open, and what remains library work

Calling by name, writing a field by name and constructing by name are
**proposed, not specified**. When they land, the unit over these built-ins is
this library's to write — extension functions, so the names stay out of every
program that does not `uses` it [MOD-015], and a receiver written `Any` is
legal and reaches an instance, an Integer and a String alike [FUN-016].

---

## Summary

Every instance already answers `ClassName` [CLS-008], and the runtime already
reaches fields **by name**. What is missing is the ability to ask *which*
fields a class has:

```algol24
for var F in P.Fields do
    WriteLn (F, ' = ', Str (P[F]));          // or whatever the spelling is
```

Two additions, both small:

1. **A class's field list, readable from Algol-24.** The table exists; nothing
   lists it.
2. **A declared type alongside each name** — or a `TypeOf` that answers for any
   value, since today only instances can be asked.

⚠️ **Read-only, and that is the whole proposal.** No setting by computed name,
no invoking methods by name, no building classes at run time. Those are where a
statically-checked language stops being checkable, and they are deliberately
excluded rather than deferred.

---

## The demand, measured

Three things in this library, none hypothetical.

**1. An instance has no useful diagnostic.**

```console
Str (P)  ->  Prop instance
```

That is what a failing test prints. For a language whose stated principle is
that code should read beautifully, it is a poor thing to be shown at the moment
something is wrong.

**2. Content equality has to be hand-written, repeatedly.** `CLAUDE.md` has
carried this for a month:

> **Lists and Maps compare by identity, not by content** — `AssertEqual` on two
> equal-looking Lists always fails, so a comparison has to walk them.

Measured on an instance, the same holds:

```console
A = B by identity: false
A = B by content:  true          (with a hand-written field comparison)
```

Those walks have been written by hand several times in this repository's own
tests, once per shape, and each is a thing that can drift from the class it
compares.

**3. The concurrency story needs it.** `PROPOSAL-CONCURRENCY.md` refuses typed
channels because they want "a copy protocol for arbitrary values". That is this
proposal: with a field list, a **library** can serialize any class, and a task
can hand back a value rather than bytes. Without it, every user writes a codec
per class, twice, and keeps the pair in step by hand.

⚠️ **The demand is real but moderate, and this document should not pretend
otherwise.** Each of the three has a workaround that works. What they share is
that the workaround is *per class* and goes stale silently when a field is
added — which is the shape of cost worth removing, rather than the size of it.

---

## Access by name already works. Only the LIST is missing.

This is the narrowest the ask gets, and it is worth stating before the rest:
**everything except enumeration already exists.** A callable can be held and
called later, three ways, and put in a Map keyed by name:

| | |
| --- | --- |
| a top-level function as a value | ✓ |
| a nested function as a value, capturing mutable locals | ✓ |
| a **bound method**, carrying its own `this` | ✓ |
| a `Map` of any of those, called later by name | ✓ |

Fields join that picture through getters, and the difference between a getter
and a stored value is the one to notice:

```console
before:  method=worth 40  value=40  getter=40
after:   method=worth 99  value=40  getter=99
                           ^^^^^^ a snapshot, taken when the Map was built
```

A method in a Map stays **live**, because the call goes through `this`. A
field's *value* is a copy. A field's *getter* is live again. So a class can
hand-write its own registry today and get access by name for both.

⚠️ **But every one of those Maps is hand-written, and that is the whole
problem.** The names come from whoever typed them, so adding a field and
forgetting the registry is silent — which is the staleness this proposal exists
to remove, not a workaround for it.

So the core is not being asked for reflection. It is being asked for **the
list**. Access is solved.

---

## Why this library cannot produce the list

Two routes, both measured shut.

**The runtime's own machinery is unreachable.** The emitted C shows exactly
what exists:

```c
alg_class_field (k_prop, "Name");                      /* the table, per class */
Value alg_property     (Value receiver, const char *name);
Value alg_set_property (Value receiver, const char *name, Value value);
```

So access is **by name at run time, throughout**. But an instance cannot cross
the foreign boundary to reach it:

| | |
| --- | --- |
| `instance.Address` | `Undefined property 'Address'.` |
| `Ord (instance)` | `Ord failed: 'Prop instance' has no ordinal.` |
| `external 'alg_property'`, interpreted | `No foreign symbol 'alg_property'.` |
| `external 'alg_property'`, compiled | resolves, then `Only instances have properties.` |

The last row is the interesting one: in a compiled binary the symbol *is*
found, the call lands, and the instance arrives as something that is not an
instance — the marshalling has no representation for one. That is [FUN-014]
undefined behaviour caught by a type check rather than a crash, and it is not a
route this library would take even if it worked.

**A source scraper works and should not be built.** A generator that reads
`.a24` sources and emits field accessors was written and run:

```algol24
// Generated from model.a24. Do not edit.

function PropFields (It : Any) : Map;
begin
    Exit ['Name' : It.Name, 'Worth' : It.Worth, 'Fixed' : It.Fixed];
end
```

It solved all three problems above. It is still the wrong answer, for the
reason this project rejected a second number formatter: **it duplicates
knowledge that something else owns.** A library parser would have to agree with
the compiler's about every declaration forever, and would need updating for
`property`, visibility sections, inheritance, and anything added later. The
same argument, the same conclusion.

---

## The proposal

### Where the data comes from

**The AST, distilled at build time — not the AST, retained.**

Every processor already has the declarations at the point where it emits: the
tree walker walks them, the VM compiles them, the C emitter writes
`alg_class_field` calls from them. So each distils into the **same shared
table**, and the language-level answer is one thing rather than three.

⚠️ **Retaining the AST would be the wrong trade, and this project has already
measured why.** `PROPOSAL-CONCURRENCY.md` records the shadow stack at **+5.8%**
on `Fib (27)`, and `alg_root_top` read and written inline *because a call per
block boundary was too expensive*. An AST held alive for the life of a program
is thousands of nodes kept for a few dozen bytes of metadata — a cost every
program pays so that some can reflect. Java and C# both faced this and both
distilled; `.class` files and metadata tables carry declarations and the AST is
discarded.

**And a distilled table cannot go stale**, which a manifest beside the source
could: it is produced by the same pass that produces the code.

### What it would add

```
alg_class_field (klass, "Name")  ->  also carries the declared type
                                     (the AST has it; the call is already there)

a way to list that table from Algol-24
a way to read a field by a name held in a variable   (alg_property, exists)
```

The third already exists in the runtime and is the only one with no work in it.

### Why it is uniform across all three processors, unlike coroutines

This is the structural argument, and it is the reason this proposal is not the
one `PROPOSAL-CONCURRENCY.md` refuses.

| | lives in | consequence |
| --- | --- | --- |
| coroutines | each processor's **execution model** — C locals, the C stack, an IP and a stack slice | a feature one processor has is not a feature of the language |
| reflection | the shared **object model** | one implementation, which all three inherit |

Measured: a property failure is byte-identical interpreted and compiled —

```console
missing field: Undefined property 'Nope'.      # interpreted
missing field: Undefined property 'Nope'.      # compiled
```

— and `alg_property` is declared in `algol.h`, which the emitted directory
carries with it. The object model is already common. So this satisfies "the
core works for all three" by construction rather than by effort.

### Why the type is needed as well as the name

A name alone is not enough for the serialization case, because **only instances
can be asked their type**:

| | |
| --- | --- |
| `instance.ClassName` | `Prop` |
| `(42).ClassName` | `Undefined property 'ClassName'.` |
| `('x').ClassName` | `Only instances have properties.` |
| `[1, 2].ClassName` | `Undefined property 'ClassName'.` |

And [TYP-009] makes the collection case a **rule** rather than an omission: "A
collection is **not** a class instance. It has no `ClassName`." So a generic
writer holding a field's value cannot tell an Integer from a String, and
`Str (…)` round-trips through `Val` only by losing which it was.

Either the field table carries the declared type, or `TypeOf` answers for any
value. The second is more generally useful and is the smaller surface; the
first is free, since the AST already knows and the emitter is already writing
that call.

---

## What does not change

- **Static access is untouched.** `P.Name` stays what it is, checked as it is.
  Nothing here is a new way to write ordinary code.
- **No per-instance cost.** The table is per class and already built.
- **A program that does not reflect pays nothing**, which is the test the
  shadow-stack measurement sets.
- **The type checker keeps its reasoning.** Reading a field by a computed name
  is dynamic, but it cannot *create* a field, so nothing becomes unprovable
  that was provable.
- **Private stays a question, not an assumption** — see below.

---

## Risks

**The slope to intercession.** Introspection is reading; intercession is
changing. Python has both — `setattr`, attribute hooks, metaclasses,
`type (name, bases, dict)` — and that is how statically-shaped programs become
undebuggable. Go stops at mostly-reading and is better for it. This proposal
asks for reading and the refusal below is meant to be load-bearing.

**Generic code reads dynamically.** `Value (P, F)` is less legible than
`P.Worth`, and that is a real loss. It is confined to code that is *inherently*
generic — serializers, comparers, describers — and ordinary code never writes
it. But the loss is not zero and should not be sold as such.

**A renamed field breaks at run time rather than at compile time.** A generated
accessor would have failed the build. This is the one genuine advantage the
scraper had, and it is given up knowingly.

---

## Open questions

Each of these is a language decision this library has no standing to make.

1. **Do private fields [MOD-006] appear?** The hardest one. If they do,
   encapsulation is readable through the back door. If they do not, a class
   with private state cannot be serialized or compared generically — which is
   half the demand. A third answer is that they appear only to code inside the
   class, which is consistent with how `private` already works but harder to
   specify.
2. **What order?** Declaration order is the useful answer for a serializer,
   because it makes a written record reproducible. Any order at all makes one
   non-deterministic.
3. **Are inherited fields included?** `class D (B)` gives `D` the parent's
   fields [CLS-011], and a serializer wants them. A describer might want to
   distinguish them.
4. **Does a `property` appear?** It is a member that computes rather than
   stores. A describer would want it; a serializer must not write it, since
   there is nothing to write back into.
5. **What is a type, as a value?** A String name is simplest and weakest. A
   first-class type value is more useful and a much larger question.
6. **Spelling, and whether it is a member or a built-in.** `P.Fields` reads
   well but puts a name on every instance, which [EXP-022] says would win
   against a class's own member called `Fields`.

---

## What this proposal does not ask for

- **Setting a field by a computed name.** `alg_set_property` exists in the
  runtime; this proposal deliberately does not surface it.
- **Invoking a method by name.**
- **Creating or altering classes at run time**, metaclasses, attribute hooks.
- **Frames, stacks or source** — Python's `inspect`. Nothing here needs them.
- **AST retention**, for the measured reason above.
- **Anything about collections' internals.** A List already iterates.

---

## What this library would do with it

- **A generic `Describe`**, so an instance in a failing test prints its fields
  rather than `Prop instance`.
- **A generic deep equality**, retiring the hand-written walks and the note in
  `CLAUDE.md` that explains why they exist.
- **A generic serializer**, which closes the copy-protocol question
  `PROPOSAL-CONCURRENCY.md` leaves open — so a task can answer a value rather
  than bytes, and no user writes a codec per class.

None of those is a new unit's worth of surface, and all three are ordinary
Algol-24 once the field list exists.

---

## Appendix: how the evidence was gathered

Everything measured here came from running the language, or from reading the C
that `algc --compile` emitted for a program written for the purpose. ⚠️ **No
compiler source was read.** This library's rules forbid reading the
implementation to work out how something behaves, and the distinction that made
this investigation possible is that *emitted* C is a program's own output
rather than the compiler's source.

The probes: `ClassName` against an instance, an Integer, a String and a List;
`Fields`, `Members`, `TypeOf` and subscripting against an instance; iterating
an instance; `Address` and `Ord` against an instance; `external 'alg_property'`
declared by hand and called under both the interpreter and a compiled binary;
a property failure compared byte for byte between the two; and a source
scraper written, run, and used to solve all three motivating problems before
being set aside.
