# Plan: the `strings` unit

**Status:** **built**, 2026-10-24. `strings.a24` carries `Trim`, `Upper`,
`Lower`, `Repeat`, `Split` and `Join` — the four named plus the case pair — with
27 tests and six `REFERENCE.md` entries. Written against `algc 0.1.5`, which
brought the extension functions `PROPOSAL-EXTENSION-FUNCTIONS.md` asked for.

Everything measured below was measured by running the language, not by reading
it. The plan is kept rather than deleted because the two decisions it framed
were decided, and the record of why is worth having.

### What shipped, against what this planned

- **The name is `strings`**, as argued below.
- **Six methods, not four.** `Trim`, `Repeat`, `Split` and `Join` were named;
  `Upper` and `Lower` came with the ASCII decision below.
- **No C**, as planned, so the unit is portable as written.
- **ASCII-only case**, with everything else unchanged — the recommendation
  below, taken. It is asserted and not merely documented: a test checks
  `Upper ('café')` answers `CAFé`.
- **`Repeat` takes a `String`**, and a Char is accepted where one is declared.
  Measured before choosing, which is what avoided an arity-identical overload
  pair and the L-4 warning that would have come with it.
- **The private helper uses `private`** [MOD-006], which no other unit here
  does.
- **The collision audit came back empty** for the six settled names, and a
  consumer importing `strings` beside `graph` prints only its own output.
- **IndexOf and Substring were not written**, the core's `Pos` and `Copy`
  already answering them.

### Still open

- **Retiring `Fill` ×3** from `examples/ide.a24`, `examples/ide-crt.a24` and
  `examples/surfaces.a24` in favour of `Repeat`. Deliberately a separate pass,
  for the reason given under the second decision below.
- **Whether `graph`, `crt` and `sound` adopt it internally**, which buys their
  consumers nothing. Unchanged recommendation: decide per unit, later.
- **The inside of `sound`'s `NumberAt` and `DigitsAt`**, which a general digit
  scanner would simplify but not replace.

---

## The name: `strings`

Lowercase and plural, and the specification settles it rather than taste:
[MOD-015] uses the name in its own prose while explaining visibility.

> a unit that imports `strings` for its own use does not give *its* importers
> `S.Trim ()`

Three reasons behind that, in order of weight:

1. **`String` is a type.** A unit named `string` would put a type and a unit one
   capital apart, and `uses string;` reads as importing the type rather than a
   set of verbs over it. The plural says "operations on strings", which is what
   the unit is.
2. **Unit names here are lowercase**, so neither `String` nor `Strings` is
   available as written.
3. **The plural names what is operated on**, which is what `graph` and `sound`
   already do. `math` and `file` are singular, but neither collides with a type
   name — that is the distinguishing fact, not the grammatical number.

---

## What the unit is

- **Free functions, every one an `extension`.** Both spellings therefore work
  always — `Trim (S)` and `S.Trim ()` — which is what [FUN-015] permits and
  what makes composition read outward instead of inward.
- **Pure Algol-24, and no C at all.** The language supplies indexing, `Copy`,
  `Pos`, `Length`, `Ord`, `Char` and `Buffer`; nothing proposed below needs more
  than those. That makes `strings` the second unit after `testing` with no
  `*ffi.c`, and so **the first unit that is portable with no work** — no
  `.dylib` in a declaration, nothing for a per-platform build to rewrite.
- **No state.** Like `math`, `uses strings` has no consequences and its tests
  can assert exact values.

---

## What it absorbs, checked rather than assumed

The proposal claimed three duplications in the tree. Two are real, one is not,
and the count was wrong:

| Claimed | Found | Verdict |
| --- | --- | --- |
| repeated-character fill, "twice, in both IDEs" | **three** copies — `examples/ide.a24:13`, `examples/ide-crt.a24:31`, `examples/surfaces.a24:13`, identical `Fill (Ch : Char, N : Integer)` | real, and worse than stated |
| digit scanning in `sound`'s MML parser | `NumberAt` and `DigitsAt`, `sound.a24:246` and `:271` | partial — both carry parser state (a position to scan from, a fallback), so a general scanner replaces their middles and not the routines |
| byte extraction in `crt` | `ByteAt`, `crt.a24:1027` | **not a strings concern.** It reads a byte out of a *Buffer*, and its own doc says so. The language has since given `Buffer` byte indexing (`B[4]`), so it is superseded by the core, not by this unit |

So the honest case for the unit is `Fill` ×3 and the inside of two MML
routines, plus whatever a reader would otherwise write by hand.

---

## Proposed surface, for you to name

⚠️ **This is a proposal, not a decision.** `CLAUDE.md` reserves the method list
to you; what follows is a candidate set to cut from, grouped so that a group can
be taken or dropped whole.

Deliberately **not** proposed, because the core already has them: `Length`,
`Copy`, `Pos`, `Str`, indexing, `Ord`, `Char`. A unit that re-spells the core is
a unit that has to be kept in step with it.

| Group | Candidates | Why |
| --- | --- | --- |
| Trimming | `Trim`, `TrimLeft`, `TrimRight` | the commonest thing missing; every parser in the tree does it by hand |
| Case | `Upper`, `Lower` | needs a decision, see below |
| Asking | `StartsWith`, `EndsWith`, `Contains` | `Pos (S, X) >= 0` is the idiom today and reads as arithmetic |
| Building | `Repeat`, `PadLeft`, `PadRight` | `Repeat` retires `Fill` ×3; the pads are what every column of a text screen wants |
| Splitting | `Split`, `Join`, `Lines` | `Split` answers a `List`, `Join` takes one — the language has a List, so this is the shape it should take |
| Editing | `Replace` | the one verb with no reasonable hand-rolling |

A smaller first cut that still retires the duplication would be `Trim`,
`Repeat`, `Split`, `Join` — four methods, one of which deletes three copies of
`Fill`.

---

## Two decisions I cannot make for you

### 1. What `Upper` and `Lower` do above ASCII

Three options, and the middle one is a trap:

- **ASCII only**, documented in `# Safety` as leaving every other code point
  unchanged. Six lines, exact, testable, and honest.
- **`towupper`/`towlower` through `external`.** Rejected on sight: the answer
  depends on the C locale, which makes the unit's behavior depend on the
  environment, and it is H-1 territory for the signature besides. It would also
  cost `strings` its no-C property, which is the unit's best feature.
- **Full Unicode case mapping**, which is a table of over a thousand entries
  with special cases that are not one-to-one.

**Recommendation: ASCII only, said plainly in the doc block.** It matters that
the doc says it, because `graphfont.hex` has emoji and users will pass non-ASCII
text; the first sentence should state that those come back unchanged. Full
mapping stays available later without breaking a caller, since widening what a
function handles is not a signature change.

### 2. Whether the library's own units adopt it

Retiring `Fill` ×3 in `examples/` is free. Using `strings` *inside* `graph`,
`crt` or `sound` is not, for a reason that is easy to miss:

⚠️ **Extension visibility is per-file and non-transitive** [MOD-015]. A unit
that says `uses strings` internally does **not** give its importers `S.Trim ()`.
Measured: a program importing a unit that imports the extension reaches the
ordinary function fine and answers `Undefined property 'Upper'.` for the method
spelling. So adopting it internally buys those units tidier code and buys their
consumers nothing — and adds an inter-unit dependency that moves every
`--test` count again.

**Recommendation: ship `strings` standing alone first.** Decide adoption as a
separate pass, per unit, after the unit exists and is documented.

---

## What the measurements say about naming

Three findings that constrain the method list, all run against 0.1.5:

**The hiding warning is receiver-type aware, which is the good news.** A
`Contains` extension declared on `String` does **not** disturb `Map.Contains`:

```algol24
extension function Contains (S : String, What : String) : Boolean;
…
WriteLn (M.Contains ('k'));        // a Map -- no warning
WriteLn ('hello'.Contains ('ell'));
```
```console
true
true
```

Nothing on stdout but the output. So `Contains` is safe to take, and the
candidate list does not have to avoid every member name in the language.

**No candidate collides with this library's own classes.** All 91 class member
names across the seven units were checked against 32 candidate verbs; the
intersection is empty. Worth re-running when the list is settled, because this
is the check that keeps L-4 from firing in a consumer's output.

**But `Length` must not be taken.** `String` has `Length` as a property, and a
property is read without a call, so declaring `extension function Length (S :
String)` splits the two spellings apart in silence:

```console
5        // S.Length   -- the property still
999      // Length (S) -- the extension
```

No warning, because `S.Length` never enters the call path. This is ordinary
shadowing of a built-in rather than anything extensions introduced — a plain
`function Length` behaves identically, measured — but it is a good reason to
leave the name alone.

---

## The obligations that come with it

The standing expectations from `CLAUDE.md`, plus two the extension changes:

- **Full per-method test coverage**, inline at the foot of the unit.
- ⚠️ **Both spellings tested, per method.** The permission is the new thing, and
  a test of `Trim (S)` does not test `S.Trim ()`. One assertion each way.
- **A `REFERENCE.md` entry per method**, alphabetical across the whole library,
  with the `extension` keyword shown in **Declaration** and the real parameter
  names — the language has named arguments, so a reader may write
  `Repeat (Ch: '-', N: 40)` from the reference.
- ⚠️ **Every reference example showing method syntax must carry `uses strings`
  itself**, or it will not compile — [MOD-015] again. This is the one way these
  examples differ from every other entry in the file.
- **The Char versus String seam.** `CLAUDE.md` records it costing three bugs
  already, always as `X = 'a'` answering False where `X` is a String. A unit
  about strings will meet it more than any other; assertions write
  `Str ('a')` on the literal side, and `Repeat` taking a `Char` rather than a
  `String` is a decision to make deliberately.

---

## Risks

1. **L-4, the warning on stdout.** Defence: the collision audit above, re-run
   once the list is settled. It is a defence against a symptom; the entry in
   `DEFECTS.md` stays open.
2. **0.1.5 becomes the minimum compiler version.** Today the library builds and
   tests on 0.1.4. The moment one `extension` is written, it does not —
   `README.md` and `CLAUDE.md` both say only `brew install algol24`, and would
   need to say which version.
3. **The unit is cheap, so its surface will want to grow.** `math` has held its
   shape because arithmetic has a bottom; strings do not. The method list being
   yours to name is the protection, and worth keeping deliberately short at
   first.

---

## Order of work

1. **You name the unit's methods** — or cut the list above.
2. `strings.a24`: doc blocks in house style, the methods, inline `test` blocks,
   no C.
3. `./test.sh strings.a24`, and the whole suite after it.
4. `REFERENCE.md` entries, every example run and its output pasted from the run.
5. The version note in `README.md` and `CLAUDE.md`, and the Status table row.
6. **Separately, afterwards:** retire `Fill` ×3 from `examples/`, each change
   with the suite re-run, since that touches working programs.
