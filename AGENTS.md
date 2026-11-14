# AGENTS.md

**The guidance for this repository is in [`CLAUDE.md`](CLAUDE.md). Read it
before working here; it governs.**

This file deliberately holds no copy of it. It used to hold a full one, and
that copy went stale within a week: it still said "Windows is not supported
and is not close" and "Windows compiles and has never been run" after every
unit's suite had been run on a Windows 11 machine, so the two files disagreed
about one of the larger facts in the project. A reader had no way to tell
which was current.

⚠️ **That is the same argument this project uses everywhere else.** A second
number formatter was refused because it duplicated knowledge the core owned;
`strings`' `Upper` and `Lower` were reimplemented by mistake and now delegate
to the core's `ToUpper`/`ToLower` with a test holding them together;
`PROPOSAL-REFLECTION.md` set aside a working source scraper for exactly this
reason -- "it duplicates knowledge that something else owns". A thousand-line
second copy of the project's own instructions is the same mistake in the same
shape, and the only version that cannot drift is the one that is not there.

A symlink would serve too, and is deliberately not used: Git for Windows
checks symlinks out as plain text files holding the target's path unless
`core.symlinks` is on, and this repository is now built and tested on Windows.

## The three things worth knowing before you open CLAUDE.md

So that a reader who stops here is not dangerously uninformed:

- ⚠️ **This project is a *user* of the language, not part of it.** The
  compiler lives in a separate repository and is deliberately not vendored.
  `algc` is a tool on `PATH`: do not build it, do not read its sources to work
  out how something behaves, and do not run its harnesses. Pulling a copy of
  the specification to work from is fine -- the spec is reference material,
  the implementation is not.
- ⚠️ **The user names the unit, and the methods within it.** Do not choose the
  next unit, expand a unit's surface beyond the methods named, or derive scope
  from any document. Wait to be told.
- ⚠️ **A defect is not a gap.** Where the language behaves *wrongly*, stop,
  record it in `DEFECTS.md` and say so -- never fix it here, never work around
  it silently, and never open the compiler to diagnose it. Where the language
  simply does not offer something, write an `external` and carry on.

Everything else -- the house style, the reference's format, the C conventions,
what has actually been measured on which platform, and the language points
that bite -- is in `CLAUDE.md`.
