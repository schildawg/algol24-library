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

# The shims are named .so on EVERY platform, macOS included, and the units'
# declarations say .so throughout.
#
# `dlopen` reads a file's contents and not its extension -- measured: a Mach-O
# built with -dynamiclib and named libprobeffi.so loads on macOS and its symbol
# resolves. So one spelling serves macOS, Linux and the BSDs, where a .dylib
# literal would have needed the sources rewritten per platform. A foreign
# declaration's `in` clause takes a string literal and cannot ask what host it
# is on, which is what made that rewrite the only alternative.
#
# Only the FLAG differs, the two toolchains spelling "build a shared library"
# differently. `file lib*.so` will say Mach-O here and ELF on Linux.
case "$(uname -s)" in
    Darwin) SHARED=-dynamiclib ;;
    *)      SHARED='-shared -fPIC' ;;
esac

# Most of the C here needs nothing but libc. soundffi.c is the exception: it
# reaches SDL2's audio device, so it needs SDL's headers to compile and its
# library to link. `sdl2-config` answers both and is what Homebrew's sdl2
# installs, so the path is not hardcoded here the way `graph`'s own
# declarations have to hardcode it -- a foreign declaration's `in` clause
# takes a string literal and a build script does not.
#
# SDL rather than CoreAudio since 0.1.5: CoreAudio is a macOS framework and
# exists nowhere else, so the unit it backed could only ever have made a noise
# on one operating system. SDL_INIT_AUDIO opens no window and needs no display
# server, so this costs a dependency and not a window system.
flags_for () {
    case "$1" in
        ./soundffi.c|./graphffi.c) sdl2-config --cflags --libs ;;
        *)                         echo "" ;;
    esac
}

for SRC in ./*ffi.c; do
    NAME=$(basename "$SRC" .c)
    LINK=$(flags_for "$SRC")
    # shellcheck disable=SC2086
    $CC $CFLAGS $SHARED -o "lib$NAME.so" "$SRC" $LINK
    echo "lib$NAME.so"
done
