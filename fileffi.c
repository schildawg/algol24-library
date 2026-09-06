/* fileffi.c -- reading and writing files, which the language leaves to us.
 *
 * The core's TextFile exists for the compiler, which has its own reasons. This
 * library reads one file -- a glyph file -- and had been borrowing that class
 * to do it. Owning the primitive instead costs a hundred lines of C and buys
 * a `file` unit that can grow without ever asking the language for anything.
 *
 * PLAIN C99, DELIBERATELY
 *
 * This is the library's first foreign code that builds anywhere. soundffi.c is
 * CoreAudio and macOS alone; crtffi.c is POSIX and rules out Windows. Nothing
 * here is worse than fopen, so `file` should port wherever a C compiler goes.
 *
 * NOTHING HERE RETURNS A POINTER TO ALGOL-24
 *
 * That is the whole shape of this file, and it was measured rather than
 * assumed. A `const char *` handed back becomes a String that BORROWS the
 * bytes -- the language does not copy them. So a function returning a reused
 * buffer corrupts a String the program is still holding, silently and with no
 * error:
 *
 *     var A := ReadAll ('a.txt');
 *     var B := ReadAll ('b.txt');     // A is now garbage
 *
 * Every function below therefore answers a count or a status, and writes its
 * bytes into a buffer the caller owns and keeps. The unit sizes that buffer
 * from alg_libfile_size first, which is one extra call and no ambiguity.
 *
 * Every function is total: a path that does not exist, is a directory, or
 * cannot be opened answers a defined failure rather than doing anything.
 *
 * THE alg_libfile_ PREFIX IS NOT DECORATION
 *
 * The first draft of this file used alg_file_, and `examples/build.sh
 * --static` would not link: the compiler's own runtime already defines
 * alg_file_exists, and a self-contained binary links both.  A shared library
 * hides the clash -- the interpreter resolves ours first -- so it appears only
 * when everything is linked into one executable, which is the one build the
 * suite makes last.
 *
 * The lesson is that alg_ is the RUNTIME's prefix and a common noun after it
 * is asking for a collision.  Library C should be named so that it cannot be
 * anything else.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Whether a path exists at all, directory or not. */
int64_t alg_libfile_exists (const char *path)
{
    struct stat st;

    if (path == NULL) return 0;

    return stat (path, &st) == 0 ? 1 : 0;
}

/* Whether a path is a directory.
 *
 * Worth asking separately: opening a directory as a file succeeds on some
 * systems and then reads nothing, which looks like an empty file rather than
 * a mistake.
 */
int64_t alg_libfile_isdir (const char *path)
{
    struct stat st;

    if (path == NULL) return 0;
    if (stat (path, &st) != 0) return 0;

    return S_ISDIR (st.st_mode) ? 1 : 0;
}

/* How many bytes a file holds, or -1 if it is not a readable file.
 *
 * The unit calls this before reading, to size the buffer exactly -- which
 * matters because a Buffer with trailing zeros has no Text.
 */
int64_t alg_libfile_size (const char *path)
{
    struct stat st;

    if (path == NULL) return -1;
    if (stat (path, &st) != 0) return -1;
    if (S_ISDIR (st.st_mode)) return -1;

    return (int64_t) st.st_size;
}

/* Reads a file into a buffer the caller owns, answering how many bytes came
 * back, or -1 if it could not be read.
 *
 * Reads at most `max`, so a file that grew between the size call and this one
 * cannot run past the buffer.
 */
int64_t alg_libfile_read (const char *path, void *into, int64_t max)
{
    if (path == NULL || into == NULL || max < 0) return -1;

    FILE *f = fopen (path, "rb");

    if (f == NULL) return -1;

    size_t got = fread (into, 1, (size_t) max, f);

    fclose (f);

    return (int64_t) got;
}

/* Writes text to a file, replacing it or adding to the end.
 *
 * Answers 1 for written and 0 for not. The text arrives as the String's own
 * bytes and is used only during the call, so there is no lifetime question
 * going this way.
 */
int64_t alg_libfile_write (const char *path, const char *text, int64_t append)
{
    if (path == NULL || text == NULL) return 0;

    FILE *f = fopen (path, append ? "ab" : "wb");

    if (f == NULL) return 0;

    size_t want = strlen (text);
    size_t put  = fwrite (text, 1, want, f);

    int closed = fclose (f);

    return (put == want && closed == 0) ? 1 : 0;
}

/* Removes a file. Answers 1 for gone and 0 for not. */
int64_t alg_libfile_erase (const char *path)
{
    if (path == NULL) return 0;

    return remove (path) == 0 ? 1 : 0;
}

/* Renames or moves a file. Answers 1 for moved and 0 for not. */
int64_t alg_libfile_rename (const char *from, const char *to)
{
    if (from == NULL || to == NULL) return 0;

    return rename (from, to) == 0 ? 1 : 0;
}

/* Copies a file, a block at a time.
 *
 * Not built from read-then-write in the unit, because that would hold the
 * whole file in memory to move it -- and copying a large file is exactly when
 * that matters.
 */
int64_t alg_libfile_copy (const char *from, const char *to)
{
    if (from == NULL || to == NULL) return 0;

    FILE *in = fopen (from, "rb");

    if (in == NULL) return 0;

    FILE *out = fopen (to, "wb");

    if (out == NULL) { fclose (in); return 0; }

    char    block[65536];
    int64_t ok = 1;

    while (1)
    {
        size_t got = fread (block, 1, sizeof block, in);

        if (got == 0) break;

        if (fwrite (block, 1, got, out) != got) { ok = 0; break; }
    }

    if (ferror (in)) ok = 0;
    if (fclose (out) != 0) ok = 0;

    fclose (in);

    return ok;
}
