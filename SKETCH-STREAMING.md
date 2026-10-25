# Sketch: streaming, and what sits on top of it

**Status:** sketch, awaiting the unit and the method names. Nothing here is
built. Written 2026-10-25.

The whole-file surface in `file` works and should stay. What it should stop
being is the *bottom* of the stack.

---

## Why this is not just a missing feature

`ReadAll` was built on `stat` plus one read, and that is unsound for anything
that is not a regular file holding still. Measured, before the fix:

```console
$ yes abcdefghijklmnopqrstuvwxyz | head -c 1048576 | algc big.a24
read 65536 of 1048576 bytes
```

A megabyte piped in came back as one pipe buffer, **reported as success**.
`stat` answers the bytes currently buffered for a pipe, zero for most
character devices, and a stale figure for a file that grows between the two
calls.

That is fixed: `ReadAll` now holds a handle internally and reads to end of
file, growing its buffer. The fix is worth noting here because of what it
proves — **the streaming primitive was needed to make the whole-file call
correct.** A pipe can be neither re-opened nor seeked, so reading one fully
means holding the stream open across several reads. The handle already exists
in `fileffi.c`; it is simply private.

So this is not "add streaming for completeness". The stack is upside down, and
righting it is mostly exposing what is already there.

---

## What the original reasoning said, and what became of it

From the commit that introduced `file`:

> There is no handle to hold, no cursor to keep and nothing to close, which
> covers everything this library does with files and almost everything a
> program does with a text file. Streaming is a door left open rather than a
> gap: it needs a way to hand back a line without handing back a borrowed
> pointer, and that is worth designing when something wants it.

Two reasons, and they have aged differently.

**The technical one is solved.** "A way to hand back a line without handing
back a borrowed pointer" is now a settled pattern in this library, used three
times: `ReadAll` itself, `dos.GetEnv`, and `graph`'s SDL error reader. Ask C
for a length, size a Buffer to it, read it back. A `ReadLine` would be the
same shape with no new idea in it.

**The scope one is the arguable half.** "Almost everything a program does with
a text file" is the claim that put the convenience layer at the bottom, and it
is not how anything else is built:

| | primitive | whole-file convenience |
| --- | --- | --- |
| Rust | `Read` / `Write` | `fs::read_to_string` |
| Go | `io.Reader` / `io.Writer` | `os.ReadFile` |
| Python | `open ()` | `.read ()` |
| Java | streams and channels | `Files.readAllBytes` |
| C# | `FileStream` | `File.ReadAllText` |
| C | `FILE *` | *none* |

Everywhere the stream is the primitive. It is not a coincidence: whole-file is
expressible in terms of streaming and streaming is not expressible in terms of
whole-file, which is exactly the asymmetry the pipe bug demonstrated.

One thing the original got right and this sketch keeps: **the whole-file calls
are what most programs should use.** Nothing proposed below changes `ReadAll`,
`ReadLines`, `WriteAll`, `AppendAll` or `WriteLines`, and a program that never
needs a handle should never see one.

---

## The surface, for you to name

⚠️ **A proposal, not a decision.** `CLAUDE.md` reserves the unit and its
methods to you. Groups are separable — any one can be taken or dropped whole.

### 1. The handle itself

```algol24
var F := InFile ('notes.txt');      // open for reading
var G := OutFile ('out.txt');       // open for writing, emptying it
var A := AppendFile ('log.txt');    // open for writing at the end
```

Classes rather than an Integer handle, so the type says what the thing can do
— an `InFile` has no `Write` to call by mistake — and so the three opening
modes are three constructors rather than a mode argument. That is the core's
own split in `TextFile` [RT-024], which has `Reset`, `Rewrite` and `Append`,
reached the way this language reaches things.

**Something has to be closed, and the language has already decided how.**

There is no `try`/`finally` — measured, `finally` does not parse — and that is
deliberate rather than missing. [RT-023] names the answer and calls it a
bargain: **release is idempotent**, so

> a handler can release on the way out without knowing how far the program got.

`Buffer.Free` is "the one member a freed Buffer still accepts", and [RT-024]
says `Close` makes the same promise. Measured: `Free` twice and `Close` twice
both survive, and releasing from inside an `except` handler works.

So the pattern is settled and `Close` here must follow it exactly — idempotent,
accepted on an already-closed handle, and safe to call from a handler:

```algol24
var F := InFile ('notes.txt');

try
    while not F.AtEnd () do Process (F.ReadLine ());
except on E : String do
begin
    F.Close ();                  // on the way out, however far it got
    raise E;
end
end

F.Close ();                      // and again, a no-op
```

⚠️ **Do not invent a second mechanism for this.** A reader that closed itself
at end of file would make the common loop safe and every other path
surprising, and it would diverge from how `Buffer` and `TextFile` already
behave. The cost of a handle is one idempotent call, which is the cost the
core already asks for twice.

**If `finally` is added to the language later, none of this surface changes.**
It is a statement form and not an API shape: the methods are the same, and
`Close` stays idempotent because its siblings in the core are. What would
change is the idiom shown above and in the reference entries — documentation
to re-run, not logic to move.

And the ordering runs the other way round if anything. The library has **187**
`try` statements today and not one of them releases a resource — they are
tests asserting a raise, or a catch that re-raises with a better message. The
absence of `finally` has cost this library nothing, because nothing here is
held by a caller. **This handle would be the first**, which is what would make
a `finally` proposal concrete rather than theoretical, the way `graph`'s
`XorWords` made the bitwise one concrete.

### 2. Reading

```algol24
function  ReadLine () : String;       // one line, without its terminator
function  AtEnd () : Boolean;         // nothing left
function  ReadBytes (Into : Buffer, Count : Integer) : Integer;
```

`ReadLine` is the one that matters and the one that motivated the original
objection. It answers a String the unit owns, built the way `ReadAll` already
builds one.

The line-ending rule should be the one `ReadLines` already follows, which is
worth keeping identical rather than inventing twice: a trailing newline does
not invent an empty last line, and a carriage return before a newline belongs
to the newline, so a file written on Windows reads the same.

`ReadBytes` is the binary half, filling a caller's Buffer and answering a
count — the H-2 shape, unchanged.

### 3. Writing

```algol24
procedure Write (Text : String);
procedure WriteLine (Text : String);
procedure WriteBytes (From : Buffer, Count : Integer);
procedure Flush ();
```

⚠️ **`Write` as a method is safe where a free function would not be.**
`graph` and `crt` both export a free `Write` and `WriteLn`, shadowing the
built-ins, and a third would make a bare `Write` ambiguous in combinations
worth writing — the `Delay` problem `sound` already taught. `F.Write (…)` is a
method on a receiver and collides with nothing. **A free-function alias for
these would be a mistake**, which is worth writing down because every other
surface verb in `graph` and `crt` has one.

`WriteLine` rather than `WriteLn` for the same reason: no one should read
`WriteLn (F, …)` and wonder which unit answered.

### 4. Position, which is the `file of Record` story

```algol24
function  Position () : Integer;      // bytes from the start
procedure Seek (Offset : Integer);
function  Size () : Integer;          // bytes, of the open file
```

Byte-indexed, deliberately — there is no `sizeof` in this language and so no
record count to index by. Turbo Pascal's `Seek (F, N)` counted records because
the compiler knew their width; here the caller knows it and the arithmetic is
theirs.

⚠️ **Seeking is not available on everything, and the surface should say so
rather than fail oddly.** A pipe cannot seek. `Seek` on one wants a raise with
a message that names the reason, not an `ESPIPE` leaking through as a `-1`.

On top of this, the `RecordFile` view from the earlier discussion becomes a
small class rather than a new mechanism: a path, a record size given at
construction, `Count` in records, `Read (N)` and `Write (N, B)`.

### 5. The gap underneath all of it

⚠️ **`Buffer` has `PutInt`/`GetInt` and nothing for a Double**, so a record
holding a price has no encoding in pure Algol-24. Measured: `PutDouble` is
`Undefined property`.

This library should fill that itself rather than ask the language, because
unlike the number-formatting case a library can be exactly right here — eight
bytes of `memcpy`, no rounding and no representation question — and `file`'s C
already receives a Buffer's address. `Buffer.PutDouble` next to `PutInt` would
be the more natural home, but it is not worth a proposal for something that
has no wrong answer.

Worth knowing for any record format written this way: `PutInt` is
**little-endian** and accepts **unaligned offsets**, both measured.

---

## What I would not do

- **Not a `TextFile` replacement.** The core has one [RT-024] and it is being
  pared to what the compiler needs. This unit owns its own primitive for that
  reason and should keep doing so.
- **Not a seekable text abstraction.** Lines and byte offsets do not mix
  cleanly — a line number is not a position — and pretending otherwise is how
  file APIs grow confusing. One handle can do both, but a caller mixing
  `ReadLine` and `Seek` should expect exactly what the bytes say.
- **Not buffered-versus-unbuffered as a choice.** `FILE *` buffers; that is
  enough, and `Flush` is the escape hatch.
- **Not async, non-blocking, or watching a file for changes.** Different
  subjects.

---

## Order of work, if you want it

1. **You name the unit and the methods** — or cut the groups above. My
   recommendation is groups 1, 2 and 3 first, which is the whole of what
   "streaming" means, and to decide group 4 separately since it carries the
   seek-on-a-pipe question.
2. The C: `fileffi.c` already has `open`, `read_into` and `close` privately.
   Writing, seeking and a line reader are the additions.
3. The unit, with tests per method and both line-ending conventions covered.
4. `REFERENCE.md` entries, every example run.
5. **Reimplement `ReadAll` and friends on top**, which should change no test
   and no reference output. That is the proof the layering is right.
6. Only then `RecordFile`, if wanted.

---

## Appendix: what was measured for this

That `finally` does not parse, and that `Buffer.Free` and `TextFile.Close`
both survive being called twice, with a release from inside an `except`
handler. The pipe truncation, by `yes | head -c 1048576 | algc`, before and
after the fix — 65536 bytes, then 1048576. `FileSize` of a FIFO answering 0. `ReadAll`
of a FIFO answering the empty string, and the same FIFO reading correctly
afterwards. `Buffer.PutDouble` answering `Undefined property`. `PutInt`'s byte
order, by writing `0x01020304` and reading the four bytes back as `4 3 2 1`.
`PutInt` at an unaligned offset of 7, reading back correctly. The core's
`TextFile` members, from [RT-024], which has no `Seek` of any kind.
