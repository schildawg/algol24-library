# Report: concurrency, measured from the library side

**From:** the `algol24-library` project, as a user of the language
**Responds to:** the core's `PROPOSAL-CONCURRENCY.md`, v0.2.5
**Status:** a report, not an ask. Nothing here requests a language change.

The proposal asks for exactly one thing from this side:

> What would reopen the built-in question is a measurement the library cannot
> answer: a `Tasks` unit built over `dosffi.c` […] that is *still* unable to do
> the job. That report, with its numbers, is what the core should respond to.

**This report answers in the negative direction: the library can do the job, so
the built-in question should stay closed.** Its conclusion agrees with the
proposal's. Four of the proposal's supporting claims, however, are wrong or
mis-sized, and one sentence in the rule would cost somebody a bad afternoon.

⚠️ **What this is not.** No `Tasks` unit has been built. These are
measurements of the *primitives* a unit would rest on, taken against v0.2.4,
and they bear on the proposal's reasoning rather than closing its question. The
Inn has not been re-measured.

---

## 1. Re-entry is not "the one thing a library genuinely cannot do"

The proposal's sharpest deferral:

> ⚠️ **A RE-ENTRY CONVENTION IS THE ONE THING A LIBRARY GENUINELY CANNOT DO.**
> A task taking a *subprogram* rather than a program — `Task (LoadSheet, Path)`
> — needs the child to run this program's own code.

It works today, with no core change. `ParamStr (0)` [RT-013] plus an argv
dispatch convention, run as a compiled binary:

```console
parent is .../reentry
  child for sheet-a.png exited 0
  child for sheet-b.png exited 0
  child for sheet-c.png exited 0
  result: loaded sheet-a.png in pid-child
  result: loaded sheet-b.png in pid-child
  result: loaded sheet-c.png in pid-child
```

The child re-executed the same program, dispatched on its first argument to the
named subprogram, and the parent read the results back. That is
`Task (LoadSheet, Path)` in all but syntax, and it is Erlang's
`spawn (Module, Function, Args)` shape rather than program granularity.

**What makes it work is that `ParamStr (0)` differs by processor in the
helpful direction:**

| | `ParamStr (0)` answers | re-entry |
| --- | --- | --- |
| compiled | **the binary itself** | `Exec (ParamStr (0), …)` directly |
| interpreted | the `.a24` source path | needs `algc` located, which `Exec` will not `PATH`-search |

**A dispatch table needs no `if`-chain either**, because a bound method is a
first-class value carrying its own receiver:

```console
sheet -> sheet from art/a.dat
sound -> sound from audio/a.dat
dispatched: sound from audio/b.dat
```

That is a `Map` of name to bound method. Combined with the above, the library
has both halves of a worker's entry point already.

---

## 2. The recompile figure that deferred it is off by four orders of magnitude

The proposal defers on cost:

> It is deferred by a number in this document: a child that re-executes the
> program **recompiles** it, and the Inn takes 27.6 s to start.

That 27.6 s is the *interpreted* start-up of a program doing ~26 s of its own
art loading. It is not the spawn cost. Measured, a compiled binary
re-executing itself 50 times:

| | |
| --- | --- |
| first (cold) run | 0.98 ms per child |
| three warm runs | **0.80, 0.80, 0.78 ms** per child |

So for a compiled binary the child *is* the binary and nothing recompiles. The
recompile objection applies only to the interpreted case — which is also the
case `ParamStr (0)` makes awkward, so the two limitations coincide rather than
compound.

⚠️ **This is a small binary's floor.** A large program's own start-up is its
own problem, which is the argument *for* a pool rather than against the
approach.

---

## 3. `fork` is named approvingly and is hazardous here

The proposal says of re-entry that "`fork` gives that free on a Unix". For this
library, fork is the dangerous option. Its eight shims hold **25 static
objects**, and under fork they are neither shared nor independent — they are
**copied, including live operating-system handles**:

| | |
| --- | --- |
| `fileffi.c` | `FILE *handles[32]`, `DIR *dirs[16]` — duplicated buffers, shared file offsets |
| `soundffi.c` | an open `SDL_AudioDeviceID`, clips, voices |
| `graphffi.c` | the SDL window, renderer and texture |
| `crtffi.c` | the saved termios mode — a child restoring it fights the parent |
| `randomffi.c` | the `drand48` state |

The last is demonstrable rather than theoretical. `randomffi.c`'s generator,
run verbatim, three draws taken, then forked:

```console
child  next three: 0.446583 0.318693 0.886428
parent next three: 0.446583 0.318693 0.886428
```

**Identical streams, silently.** The classic fork-and-PRNG fault, and this
library is exposed to it precisely because 0.1.6's portability work moved the
generator into our C — the right call for Windows, and it put the state where a
fork copies it.

⚠️ **So the rule should say a child is a fresh program.** One sentence, and it
makes the argv re-entry above the *safe* route as well as the portable one —
the two findings agree.

---

## 4. "Communicate in bytes" is sufficient, and better than it sounds

The proposal is right that the Inn's art is bytes, and the reason is stronger
than stated: `graph`'s `Image` already **is** a flat byte array —

```algol24
constructor Image (W : Integer, H : Integer);

W, H : Integer          the size in pixels
Bits : Buffer           the pixels, four bytes each, row by row
```

— so a worker writes `Sheet.Bits` out and the parent reads it back with no
serialization at all. Demonstrated end to end:

```console
worker produced 30x30, 3600 bytes
handed over 3608 bytes
parent read 30x30
pixels identical: true
and PutImage accepted it unchanged
```

The eight extra bytes are a header carrying width and height. And the transfer
is free relative to the work — 64 MB through a pipe:

| | |
| --- | --- |
| cold | 9340 MB/s |
| warm | **9834 MB/s** |

Sixteen 1024×1024 RGBA sheets in about 7 ms, against ~26 s of decoding. The
copy costs roughly **0.03%** of what the loading costs.

⚠️ **The limit is object graphs, not volume.** Art is bytes already; a `List`
of instances is not, and nothing in the library will make it so — there is no
reflection, only type introspection on instances [CLS-008], and
`(42).ClassName` and `[1,2].ClassName` both fail by rule [TYP-009]. So the
proposal's refusal of typed channels is right, and
`PROPOSAL-REFLECTION.md` in this repository is the narrower ask that would
close it.

> **Note added 2026-11-12, after this report was sent.** That last paragraph's
> conclusion stands and its reason no longer does. `PROPOSAL-REFLECTION.md` was
> accepted and answered with more than it asked: [RT-031] `Fields` and [RT-032]
> `TypeOf` are specified, so a **library** serializer over an arbitrary class
> becomes writable and "nothing in the library will make it so" stops being
> true. The core's reply also proposes writing and constructing by name, which
> is what a *de*serializer needs — a field list alone would only get a value
> out, not back in. So the copy protocol this section leaves open is on a path
> to closing, and a task could hand back a value rather than bytes.
>
> ⚠️ **It changes nothing about what this report recommends.** The three
> primitives in §"What the library will build" are unaffected, bytes remain the
> right transport for art, and none of it is in a release — 0.3.0 answers
> `Undefined variable 'Fields'.` and the core is still building. Typed channels
> staying out of the *language* is still the right call; the copy protocol was
> always library work, and now it is library work that can be written.

---

## 5. Coroutines are not absent, which changes the refusal's framing

The proposal's reasoning for refusing them stands — two of three processors
keep live state on the C stack, and a feature one processor has is not a
feature of the language. But the document reads as though the capability is
missing, and it is available in two weaker forms:

| | cost per resume | suspends arbitrary control flow |
| --- | --- | --- |
| a nested function capturing mutable locals | a function call | no — the caller restructures |
| a process blocked on a pipe | **3.16–3.20 µs** warm, 3.81 cold | **yes**, with no transformation |
| a core coroutine | a function call | yes |

Generators work today, on all three processors:

```console
fibs: 0 1 1 2 3 5 8 13 21 34
```

And a lazy tree walk — yielding from mid-recursion, the case that separates a
generator from a coroutine — works with the recursion turned into an explicit
stack held in the closure:

```console
walked lazily: root a b c d
```

So a core coroutine would be an **optimisation of something that already
works**, not a new capability. Worth saying, because "deserves its own
proposal" reads as a larger gap than it is.

---

## 6. A `Tasks` unit should block, not sleep

Not in the proposal, and it bears on the three primitives it lists. Measured
CPU against 0.4 s of waiting:

| | |
| --- | --- |
| blocking `waitpid` | **0.0001 s** |
| polled with `WNOHANG`, no nap | **0.3997 s** — a whole core |
| polled with a 10 ms nap | **0.0003 s** |

So `Result ()` should block — the kernel parks the process for free, which is
what `dos.Exec` already does. A nap is needed only for `Done ()` inside a loop
with other work, and there it is worth a factor of about 1300.

⚠️ **And `tasks` must not export its own `Delay`.** `graph` and `crt` each have
one, and `CLAUDE.md` records that `sound` was refused a third because "a third
copy made a bare `Delay` ambiguous in the two combinations worth writing." A
fourth is worse. `crt.Delay` is reachable without a terminal or `InitCrt` —
it is plain `CrtNap` — so a headless program can already nap; it just reads
oddly to import a terminal unit for it.

---

## 7. For a distributed ecosystem, the coarse granularity stops mattering

Thousands of agents across many machines is dozens per machine, where 0.8 ms to
start is invisible. And if agents communicate across machines at all, the
transport is bytes over a wire regardless — so isolation is not a cost being
accepted but the condition being required. That is how Erlang's own node
boundary works: cheap processes within a node, copied terms over TCP between
them, and this design takes the second half.

⚠️ **One forward-compatibility note, cheap now and expensive later.** A
`tasks` surface should keep the **channel** abstract rather than the spawn. A
`Result ()` that reads from a pipe is local forever; one that reads from a
*stream* can be backed by a socket later — and `file`'s `InFile` already
abstracts a stream that cannot be re-opened or seeked, which is a socket as
much as a pipe.

The missing piece for real distribution is **sockets**. `dos` has none, and
that is new C when the time comes rather than a surprise to discover.

---

## What the library will build, and what it still needs

**Needs, all in `dosffi.c`, beside 470 lines that already spawn on both
families:** `posix_spawn` without the `waitpid`, answering the child's id;
`waitpid` with `WNOHANG`; and a pipe.

⚠️ **One sizing correction to the proposal's "small and sits beside code that
does the hard part":** those three give fire-and-collect. A *pool* — which is
what amortises start-up — also needs a framing protocol over the pipe, because
a persistent worker taking successive jobs needs message boundaries. Still
library work, but a unit with real design in it rather than a thin wrapper.

**Needs nothing from the core.** Not for spawning, not for re-entry, not for
dispatch, and not for waiting.

---

## Appendix: how the evidence was gathered

Every figure came from running something, on macOS/arm64 against `algc 0.2.4`.
Spawn and switch costs are quoted warm with the cold first run noted, because
they differ by about 20%.

The programs: a compiled Algol-24 binary re-executing itself 50 times with a
no-op child; a C parent and child exchanging one byte 20,000 times; the same
pair passing 64 MB in 1 MB writes; `getrusage` around a blocking wait, a
`WNOHANG` spin and a `WNOHANG` loop with a 10 ms nap; `randomffi.c`'s
generator quoted verbatim into a C program that forks after three draws; an
Algol-24 program that lifts a `ViewPort` region with `GetImage`, writes
`Bits` through `OutFile`, reads it back through `InFile` into a fresh `Image`,
compares every byte and lays it down with `PutImage`; a closure-based
Fibonacci generator and a closure-based lazy tree walk; and a `Map` of bound
methods dispatched by name.

⚠️ **No compiler source was read.** This library's rules forbid reading the
implementation to work out how something behaves. Where runtime internals are
named here — `alg_property`, `alg_class_field` — they were read from the C
that `algc --compile` emitted for a program written for the purpose, which is
that program's own output.
