#!/bin/sh
#
# The library's own C, as a shared library the units reach through
# `external '...' in '...'`.
#
# A unit's foreign declaration names the file by its platform spelling, so the
# extension below and the one written in the .a24 sources have to agree.

set -eu

CC=${CC:-cc}
CFLAGS=${CFLAGS:--std=c11 -O2 -Wall -Wextra}

case "$(uname -s)" in
    Darwin) SO=dylib; SHARED=-dynamiclib ;;
    *)      SO=so;    SHARED='-shared -fPIC' ;;
esac

for SRC in ./*ffi.c; do
    NAME=$(basename "$SRC" .c)
    # shellcheck disable=SC2086
    $CC $CFLAGS $SHARED -o "lib$NAME.$SO" "$SRC"
    echo "lib$NAME.$SO"
done
