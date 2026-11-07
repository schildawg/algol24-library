# finished

Documents whose work is done. They are kept rather than deleted because each
one records *why* a thing is the way it is, and that reasoning outlives the
decision — several of these were re-read months later to answer "was this
argued or assumed?"

Nothing here is current. For what the library is now, see `../CLAUDE.md`,
`../REFERENCE.md` and `../DEFECTS.md`.

| | What it asked for | What became of it |
| --- | --- | --- |
| `PROPOSAL-EXTENSION-FUNCTIONS.md` | a top-level routine callable on its first argument | **accepted**, shipped in compiler 0.1.5 as [FUN-015] and [EXP-022]; `strings` is built on it |
| `PROPOSAL-BITWISE-OPERATORS.md` | bit operations, which the language had none of | **accepted**, shipped in 0.1.6 as `&` `\|` `^` `~` `<<` `>>`; `graph`'s `alg_xor` was deleted, the native operator measuring four times faster |
| `PROPOSAL-NUMBER-FORMATTING.md` | `Str` with a decimal count, because a library cannot round correctly from `Str`'s own output | **accepted**, shipped in 0.1.6; both of its open questions resolved as the document suggested |
| `PLAN-STRINGS.md` | the `strings` unit's surface | **built**; its analysis missed that the core already had `ToUpper`/`ToLower`, which is recorded in the document and in `CLAUDE.md` |
| `SKETCH-STREAMING.md` | file handles, reading, writing, seeking, records | **built**, all five groups; group 5 was `Buffer.PutDouble`, which this library declined to write so the core could specify the representation |

⚠️ **A proposal here is not a record of what shipped.** Each says what this
library *asked for*; the compiler decided. Where the two differ the document
says so at the top, and `DEFECTS.md` carries the measurements taken after the
fact. `PROPOSAL-NUMBER-FORMATTING.md` also carries a correction to its own
evidence, which is worth knowing before quoting its tables.
