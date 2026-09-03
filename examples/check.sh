#!/bin/sh
#
# The example must produce the same output three ways: interpreted, as a
# self-contained binary, and as a binary beside its shims.
#
# This is the only check in the repository that exercises the whole story --
# that the library's units compile into an application, that the C shims reach
# it under either linking, and that a compiled program agrees with the
# interpreter. Everything else here tests one unit at a time.
#
# It is also why the example seeds the generator instead of randomizing: three
# runs of a program that chose its own seed could not be compared at all.

set -eu

test -f examples/statistics.a24 || {
    echo 'examples/check.sh: run me from the repository root' >&2
    exit 2
}

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

algc examples/statistics.a24 > "$WORK/interpreted.txt"

examples/build.sh --static > /dev/null
cp examples/build/statistics "$WORK/standalone"
( cd "$WORK" && ./standalone ) > "$WORK/static.txt"

examples/build.sh > /dev/null
( cd examples/build && ./statistics ) > "$WORK/dynamic.txt"

FAILED=0

if ! diff -q "$WORK/interpreted.txt" "$WORK/static.txt" > /dev/null; then
    echo 'example: the self-contained binary disagrees with the interpreter'
    diff "$WORK/interpreted.txt" "$WORK/static.txt" || true
    FAILED=1
fi

if ! diff -q "$WORK/interpreted.txt" "$WORK/dynamic.txt" > /dev/null; then
    echo 'example: the dynamically-linked binary disagrees with the interpreter'
    diff "$WORK/interpreted.txt" "$WORK/dynamic.txt" || true
    FAILED=1
fi

if [ "$FAILED" = 0 ]; then
    echo "example: $(wc -l < "$WORK/interpreted.txt" | tr -d ' ') lines, identical interpreted and both ways compiled"
fi

exit "$FAILED"
