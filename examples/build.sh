#!/bin/sh
#
# Builds examples/statistics.a24 into a native executable, with the library's
# Algol-24 units compiled in.
#
# Run from the repository root:
#
#     examples/build.sh              # -> examples/build/statistics
#     examples/build.sh --static     # ... depending on nothing but libc
#
# WHAT GETS LINKED IN, EITHER WAY
#
# `algc --compile` emits one .c per unit the program reaches, so math.a24,
# random.a24 and testing.a24 become math.c, random.c and testing.c and are
# compiled into the binary along with the program's own. The Algol-24 half of
# the library is genuinely linked in and there is nothing to ship beside it.
#
# THE C HALF IS THE AWKWARD PART, AND THE TWO MODES DIFFER ONLY THERE
#
# A declaration written `external 'alg_trunc' in 'libmathffi.dylib'` dlopens
# that library at run time -- even when the same symbol is already linked into
# the executable, which was measured rather than assumed. Written without the
# `in` clause the symbol is looked for in the running program instead, so
# linking mathffi.c in makes the binary self-contained.
#
# The catch is that the second spelling cannot be run by the interpreter at all:
# `algc math.a24` then answers `No foreign symbol 'alg_trunc'.`, because the
# symbol would have to be inside `algc` itself. Preloading with
# DYLD_INSERT_LIBRARIES does not rescue it either.
#
# So the units keep the `in` clause -- the library's own tests run interpreted,
# and that has to keep working -- and --static rewrites it out of a throwaway
# copy of the sources. Nothing under version control is touched.

set -eu

CC=${CC:-cc}
CFLAGS=${CFLAGS:--std=c11 -O2}
STATIC=no

case "${1:-}" in
    --static) STATIC=yes ;;
    '')       ;;
    *)        echo "usage: examples/build.sh [--static]" >&2; exit 2 ;;
esac

case "$(uname -s)" in
    Darwin) SO=dylib ;;
    *)      SO=so ;;
esac

test -f examples/statistics.a24 || {
    echo 'examples/build.sh: run me from the repository root' >&2
    exit 2
}

./build.sh > /dev/null

OUT=examples/build
rm -rf "$OUT"
mkdir -p "$OUT"

if [ "$STATIC" = yes ]; then
    SRC=$(mktemp -d)
    trap 'rm -rf "$SRC"' EXIT

    cp ./*.a24 examples/statistics.a24 "$SRC"

    # Drop the library clause so the symbols resolve from the executable.
    sed -i.bak "s/ in 'lib[a-z]*ffi\.$SO'//g" "$SRC"/*.a24
    rm -f "$SRC"/*.bak

    mkdir -p "$SRC/emitted"
    ( cd "$SRC" && algc --compile --out=emitted statistics.a24 ) > /dev/null

    # shellcheck disable=SC2086
    $CC $CFLAGS -DALG_FFI -o "$OUT/statistics" \
        "$SRC"/emitted/*.c ./*ffi.c -lffi

    echo "$OUT/statistics  (self-contained)"
else
    mkdir -p "$OUT/emitted"
    algc --compile --out="$OUT/emitted" examples/statistics.a24 > /dev/null

    # shellcheck disable=SC2086
    $CC $CFLAGS -DALG_FFI -o "$OUT/statistics" "$OUT"/emitted/*.c -lffi

    # dlopen resolves a bare library name against the working directory, so the
    # shims have to sit beside the binary and it has to be run from there.
    cp ./*ffi."$SO" "$OUT"

    echo "$OUT/statistics  (run it from $OUT, which holds the .$SO shims)"
fi
