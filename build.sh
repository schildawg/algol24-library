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

# Most of the C here needs nothing but libc. soundffi.c is the exception: it
# reaches CoreAudio, which on macOS means frameworks rather than a -l flag.
# Deliberately not SDL -- `crt` beeps through this too, and owes a window
# system nothing.
frameworks_for () {
    case "$1" in
        ./soundffi.c)
            case "$(uname -s)" in
                Darwin) echo "-framework AudioToolbox -framework AudioUnit -framework CoreFoundation" ;;
                *)      echo "" ;;
            esac
            ;;
        *) echo "" ;;
    esac
}

for SRC in ./*ffi.c; do
    NAME=$(basename "$SRC" .c)
    LINK=$(frameworks_for "$SRC")
    # shellcheck disable=SC2086
    $CC $CFLAGS $SHARED -o "lib$NAME.$SO" "$SRC" $LINK
    echo "lib$NAME.$SO"
done
