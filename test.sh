#!/bin/sh
#
# Every unit's test blocks.
#
# Unit tests are the whole of the testing story here. Conformance cases,
# refusals and interpreted-versus-compiled comparison belong to the compiler
# project; this is a library written by a user of the language, and it is tested
# the way any Algol-24 program is.

set -eu

# The units reach the library's own C through a shared library, so a stale one
# would be tested instead of the source beside it.
./build.sh > /dev/null

UNITS=${*:-$(ls ./*.a24)}
FAILED=0

for UNIT in $UNITS; do
    printf '%s: ' "$(basename "$UNIT" .a24)"

    if OUTPUT=$(algc --test "$UNIT" 2>&1); then
        echo "$(printf '%s\n' "$OUTPUT" | grep -c 'Test:') tests passed"
    else
        echo 'FAILED'
        printf '%s\n' "$OUTPUT"
        FAILED=1
    fi
done

if ! ./check-reference.py; then
    FAILED=1
fi

if ! examples/check.sh; then
    FAILED=1
fi

exit "$FAILED"
