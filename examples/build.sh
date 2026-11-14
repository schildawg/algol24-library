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
# A declaration written `external 'alg_trunc' in 'libmathffi.so'` dlopens
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
    sed -i.bak "s/ in 'lib[a-z]*ffi\.so'//g" "$SRC"/*.a24
    rm -f "$SRC"/*.bak

    mkdir -p "$SRC/emitted"
    ( cd "$SRC" && algc --compile --out=emitted statistics.a24 ) > /dev/null

    # soundffi.c and graphffi.c both reach SDL2, so a self-contained binary
    # that links the library's C needs SDL's headers and library too. Since 0.1.5
    # that is SDL rather than CoreAudio -- the framework existed on macOS
    # alone, and this unit is meant to make a noise anywhere.
    AUDIO=$(sdl2-config --cflags --libs)

    # Resolving "from the executable" means the executable's EXPORTS on
    # Windows, where the runtime asks GetProcAddress of the program's own
    # module -- and a mingw executable exports nothing unless told to, so every
    # shim linked in answered `No foreign symbol`. vm/build.sh in the compiler
    # repository passes the same flag for the same reason.
    #
    # ⚠️ ELF NEEDS ONE TOO, and the sentence here used to say it did not: "ELF
    # and Mach-O need nothing: dlsym on the program finds its symbols as they
    # are." That is true of Mach-O and false of ELF. A linked-in shim is in the
    # executable, and dlsym(RTLD_DEFAULT, ...) searches DYNAMIC symbol tables --
    # which an executable's own symbols do not enter unless it is linked to
    # export them. So the self-contained binary built and then answered
    # `No foreign symbol 'alg_random_seed'` at its first call, on Linux alone.
    # -rdynamic is the driver's spelling of it and works on both GNU
    # toolchains; --export-all-symbols is PE-only and does not link here.
    EXPORT=''
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*) EXPORT='-Wl,--export-all-symbols' ;;
        Linux)                EXPORT='-rdynamic' ;;
    esac

    # ⚠️ -lm BECAUSE A -DALG_FFI BUILD REFERENCES MATH WHETHER THIS PROGRAM
    # DOES OR NOT. The runtime holds a reference to libm under that switch so
    # that a bare-named `external` can reach it -- `external 'atan'` with no
    # `in` clause resolves from the running program, and on Linux libm is a
    # library of its own that --as-needed would otherwise drop. Without this
    # the link fails as `undefined reference to symbol 'atan'`; before compiler
    # 0.4.1 it failed here on `sincos` instead, from graphffi.c's arcs, which
    # is the same missing flag found from the other side. A no-op on macOS.
    # shellcheck disable=SC2086
    $CC $CFLAGS -DALG_FFI -o "$OUT/statistics" \
        "$SRC"/emitted/*.c ./*ffi.c -lffi -lm $AUDIO $EXPORT

    echo "$OUT/statistics  (self-contained)"
else
    mkdir -p "$OUT/emitted"
    algc --compile --out="$OUT/emitted" examples/statistics.a24 > /dev/null

    # -lm for the reason the static branch above gives, and it is NEW here:
    # this path links none of the library's own C, so it was fine until the
    # runtime itself began referencing math under -DALG_FFI.
    # shellcheck disable=SC2086
    $CC $CFLAGS -DALG_FFI -o "$OUT/statistics" "$OUT"/emitted/*.c -lffi -lm

    # dlopen resolves a bare library name against the working directory, so the
    # shims have to sit beside the binary and it has to be run from there.
    cp ./*ffi.so "$OUT"

    echo "$OUT/statistics  (run it from $OUT, which holds the .so shims)"
fi
