#!/usr/bin/env python3
"""Run every example in REFERENCE.md and check it prints what the entry claims.

The reference is what a user trusts when the source is not to hand, so an
example that does not compile -- or that claims output it does not produce -- is
worse than no example.

An entry pairs a program with its output:

    ```algol24
    ... the program ...
    ```

    ```console
    ... what it prints ...
    ```

A ```algol24 block NOT followed by a ```console block is a Declaration excerpt
rather than a program, and is skipped.

An example that can only run on a POSIX host -- `Exec` naming `/bin/echo` --
carries `<!-- check-reference: posix-only -->` on the line before its program,
and is skipped on Windows. A skip is counted and printed, never silent: the run
says how many examples it did not check.

Every file is read and written as UTF-8, which is what REFERENCE.md and the
units are. Python's default is the locale's encoding, cp1252 on Windows, where
the first non-ASCII byte in the reference stopped the run before any example.

This script exits non-zero if any example is wrong, and also if it finds fewer
examples than there are ```console blocks -- a check that silently verifies
nothing is worse than no check, which this script learned the hard way.
"""

import os
import re
import shutil
import subprocess
import sys
import tempfile

ANSI = re.compile(r'\x1b\[[0-9;]*m')
TEST_NAME = re.compile(r"^test '(.*?)';", re.M)

ROOT = os.path.dirname(os.path.abspath(__file__))
REFERENCE = os.path.join(ROOT, 'REFERENCE.md')

FENCE = re.compile(r'```(algol24|console)\n(.*?)^```\n', re.S | re.M)

POSIX_ONLY = '<!-- check-reference: posix-only -->'
ON_WINDOWS = os.name == 'nt' or sys.platform in ('win32', 'cygwin', 'msys')


def examples(text):
    """Each (program, expected, posix_only) triple: an algol24 block
    immediately followed by a console one, and whether it is marked as
    needing a POSIX host."""
    blocks = [(m.group(1), m.group(2), m.start()) for m in FENCE.finditer(text)]

    for (kind, body, at), (next_kind, next_body, _) in zip(blocks, blocks[1:]):
        if kind == 'algol24' and next_kind == 'console':
            before = text[:at].rstrip('\n').rsplit('\n', 1)[-1]
            yield body, next_body, before.strip() == POSIX_ONLY


def main():
    text = open(REFERENCE, encoding='utf-8').read()
    expected_count = text.count('```console\n')
    pairs = list(examples(text))

    if len(pairs) != expected_count:
        print(f'{REFERENCE}: found {len(pairs)} examples but {expected_count} '
              f'console blocks -- the extractor and the file disagree', file=sys.stderr)
        return 1

    work = tempfile.mkdtemp()
    try:
        for name in os.listdir(ROOT):
            if name.endswith(('.a24', '.hex', '.wav')) or '.dylib' in name or name.endswith('.so'):
                shutil.copy(os.path.join(ROOT, name), work)

        failed = 0
        skipped = 0
        for i, (program, expected, posix_only) in enumerate(pairs):
            if posix_only and ON_WINDOWS:
                skipped += 1
                continue

            path = os.path.join(work, f'example{i}.a24')
            with open(path, 'w', encoding='utf-8', newline='\n') as f:
                f.write(program)

            # An example using the built-in assertions only runs under --test:
            # Fail and AssertEqual are undefined in an ordinary program.
            in_test_mode = re.search(r"^test '", program, re.M) is not None
            argv = ['algc', '--test', path] if in_test_mode else ['algc', path]

            # graph examples open windows; the dummy driver opens them nowhere.
            env = dict(os.environ, SDL_VIDEODRIVER='dummy', ALG_SOUND='dummy')
            result = subprocess.run(argv, capture_output=True, text=True,
                                    encoding='utf-8', errors='replace',
                                    cwd=work, env=env)

            got = result.stdout
            if in_test_mode:
                # --test is transitive over `uses`, so the run also reports every
                # test in every unit the example imports -- and the banner names
                # the temporary file. Neither is what the entry illustrates, and
                # both would shift as unrelated units gain tests. Keep only the
                # lines for the tests this example itself declares.
                mine = TEST_NAME.findall(program)
                got = '\n'.join(line for line in ANSI.sub('', got).splitlines()
                                 if any(f'Test: {name} ' in line for name in mine))
                got = got + '\n' if got else got

            if got != expected:
                failed += 1
                print(f'--- example {i} does not print what it claims ---\n'
                      f'{program}\nexpected:\n{expected}got:\n{got}'
                      f'{result.stderr}', file=sys.stderr)

        note = f', {skipped} skipped as POSIX-only on this host' if skipped else ''
        print(f'reference: {len(pairs) - skipped} examples run, {failed} wrong{note}')
        return 1 if failed else 0
    finally:
        shutil.rmtree(work)


if __name__ == '__main__':
    sys.exit(main())
