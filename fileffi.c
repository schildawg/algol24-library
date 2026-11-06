/* fileffi.c -- reading and writing files, which the language leaves to us.
 *
 * The core's TextFile exists for the compiler, which has its own reasons. This
 * library reads one file -- a glyph file -- and had been borrowing that class
 * to do it. Owning the primitive instead costs a hundred lines of C and buys
 * a `file` unit that can grow without ever asking the language for anything.
 *
 * PLAIN C99, DELIBERATELY
 *
 * This is the library's first foreign code that builds anywhere. soundffi.c
 * and graphffi.c want SDL2; crtffi.c and dosffi.c are POSIX and rule out
 * Windows. Nothing here is worse than fopen, so `file` should port wherever a
 * C compiler goes.
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

/* The one thing here that is not plain C99. POSIX rather than ISO -- but
 * mingw-w64 ships a dirent that wraps FindFirstFile, so this file still
 * cross-compiles for Windows; verified with x86_64-w64-mingw32-gcc before
 * directory reading was put in this unit rather than in `dos`. */
#include <dirent.h>
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
/* ------------------------------------------------ reading to end of file -- */
/*
 * A whole-file read cannot be done with stat and one fread, which is what
 * this file did at first and what was measured wrong: `stat` answers the
 * bytes currently BUFFERED for a pipe, zero for most character devices, and a
 * stale figure for a regular file that grows between the two calls. A
 * megabyte piped into ReadAll came back as 65536 bytes -- one pipe buffer --
 * reported as success.
 *
 * Reading to end of file instead needs the stream held open across several
 * reads, because a pipe can be neither re-opened nor seeked: its bytes are
 * gone once read. So there is a handle here now, even though the unit's own
 * surface is still whole-file.
 *
 * The handle is a small integer into a table rather than a pointer, which is
 * the shape `sound` already uses for a loaded clip: a stale integer answers a
 * defined failure where a stale pointer would not.
 */

#define ALG_MAX_FILES 32

static FILE *handles[ALG_MAX_FILES];

/* Opens a file and answers a handle, or -1.
 *
 * Mode 0 reads, 1 writes and empties first, 2 writes at the end -- which is
 * Reset, Rewrite and Append, the three the core's own TextFile offers
 * [RT-024].
 *
 * Mode 3 is read AND write without emptying, which none of those three can
 * do and which a record file needs: updating record 5 must leave records 0
 * to 4 where they are. Mode 4 is the same but creates or empties, for a
 * record file being started rather than opened.
 *
 * Anything else is refused rather than guessed at.
 */
int64_t alg_libfile_open (const char *path, int64_t mode)
{
    if (path == NULL) return -1;

    const char *how = mode == 0 ? "rb"
                    : mode == 1 ? "wb"
                    : mode == 2 ? "ab"
                    : mode == 3 ? "r+b"
                    : mode == 4 ? "w+b"
                    : NULL;

    if (how == NULL) return -1;

    int slot = -1;

    for (int i = 0; i < ALG_MAX_FILES; i++)
        if (handles[i] == NULL) { slot = i; break; }

    if (slot < 0) return -1;

    FILE *f = fopen (path, how);

    if (f == NULL) return -1;

    handles[slot] = f;

    return slot;
}

/* Reads up to `max` bytes into `into` at `offset`, answering how many came
 * back and 0 at end of file.
 *
 * The offset is a parameter because a Pointer cannot be offset in the
 * language -- the unit passes one Buffer and the position within it, rather
 * than arithmetic it has no operator for.
 *
 * Answers -1 for a handle that is not open, which the unit raises on.
 */
int64_t alg_libfile_read_into (int64_t handle, void *into, int64_t offset,
                               int64_t max)
{
    if (handle < 0 || handle >= ALG_MAX_FILES) return -1;
    if (handles[handle] == NULL) return -1;
    if (into == NULL || offset < 0 || max <= 0) return 0;

    size_t got = fread ((char *) into + offset, 1, (size_t) max,
                        handles[handle]);

    return (int64_t) got;
}

/* Writes a String's bytes, answering how many went out or -1.
 *
 * The text arrives as the String's own bytes and is used only during the
 * call, so this direction has no lifetime question -- it is coming back the
 * other way that borrows.
 */
int64_t alg_libfile_write_text (int64_t handle, const char *text)
{
    if (handle < 0 || handle >= ALG_MAX_FILES) return -1;
    if (handles[handle] == NULL) return -1;
    if (text == NULL) return 0;

    size_t len = strlen (text);

    if (len == 0) return 0;

    return (int64_t) fwrite (text, 1, len, handles[handle]);
}

/* Writes Count bytes from a buffer at Offset, answering how many went out. */
int64_t alg_libfile_write_bytes (int64_t handle, const void *from,
                                 int64_t offset, int64_t count)
{
    if (handle < 0 || handle >= ALG_MAX_FILES) return -1;
    if (handles[handle] == NULL) return -1;
    if (from == NULL || offset < 0 || count <= 0) return 0;

    return (int64_t) fwrite ((const char *) from + offset, 1, (size_t) count,
                             handles[handle]);
}

/* Pushes buffered bytes at the operating system. Answers 1 for done. */
int64_t alg_libfile_flush (int64_t handle)
{
    if (handle < 0 || handle >= ALG_MAX_FILES) return -1;
    if (handles[handle] == NULL) return -1;

    return fflush (handles[handle]) == 0 ? 1 : 0;
}

/* ------------------------------------------- bytes, for the line reader -- */
/*
 * These two exist for SPEED and carry no decisions. Finding a newline and
 * copying a run of bytes are both things the unit could do with its own loop
 * and `B[I]`, and both are the shape this project has measured as far too
 * slow interpreted: graphffi.c carries a hex decoder for exactly this reason,
 * the interpreted version having taken ~8 seconds for one font file.
 *
 * So the unit still decides what a line is, where it ends and what to do with
 * a carriage return. It just does not walk the bytes one call at a time.
 */

/* The index of `byte` in [from, from + len), or -1. */
int64_t alg_libfile_scan (const void *bytes, int64_t from, int64_t len,
                          int64_t byte)
{
    if (bytes == NULL || from < 0 || len <= 0) return -1;

    const unsigned char *found =
        memchr ((const unsigned char *) bytes + from, (int) (byte & 255),
                (size_t) len);

    if (found == NULL) return -1;

    return (int64_t) (found - (const unsigned char *) bytes);
}

/* Copies `len` bytes from src + src_off to dst + dst_off, answering the
 * count. The regions must not overlap, and in this unit they never do --
 * they are always two different buffers. */
int64_t alg_libfile_copy_bytes (void *dst, int64_t dst_off, const void *src,
                                int64_t src_off, int64_t len)
{
    if (dst == NULL || src == NULL) return 0;
    if (dst_off < 0 || src_off < 0 || len <= 0) return 0;

    memcpy ((char *) dst + dst_off, (const char *) src + src_off,
            (size_t) len);

    return len;
}

/* --------------------------------------------------------- directories -- */
/*
 * Reading a directory, which the core offers no way to do: it has MkDir,
 * RmDir, ChDir and GetDir [RT-026] and nothing that LISTS one.
 *
 * An iterator rather than a function answering everything, for the reason the
 * rest of this file learned the hard way -- the incremental form is the
 * primitive and the all-at-once form is sugar on top, never the other way
 * round. `readdir` is itself incremental, so this is the shape the operating
 * system already has.
 *
 * `.` and `..` are skipped here rather than by the caller. Every caller would
 * filter them and the ones that forgot would recurse forever; Python's
 * listdir and Go's ReadDir both make the same choice.
 */

#define ALG_MAX_DIRS 16

static DIR *dirs[ALG_MAX_DIRS];

/* Opens a directory and answers a handle, or -1. */
int64_t alg_libdir_open (const char *path)
{
    if (path == NULL) return -1;

    int slot = -1;

    for (int i = 0; i < ALG_MAX_DIRS; i++)
        if (dirs[i] == NULL) { slot = i; break; }

    if (slot < 0) return -1;

    DIR *d = opendir (path);

    if (d == NULL) return -1;

    dirs[slot] = d;

    return slot;
}

/* Copies the next entry's name into a caller's buffer and answers its length.
 *
 * Answers 0 when the directory is exhausted, and -1 for a bad handle or a
 * name too long for the buffer. A name is bounded by the filesystem -- 255
 * bytes on every one this runs on -- and the unit passes a buffer far larger,
 * so -1 here means something genuinely strange rather than a small buffer.
 */
int64_t alg_libdir_next (int64_t handle, void *into, int64_t max)
{
    if (handle < 0 || handle >= ALG_MAX_DIRS) return -1;
    if (dirs[handle] == NULL) return -1;
    if (into == NULL || max <= 0) return -1;

    for (;;)
    {
        struct dirent *e = readdir (dirs[handle]);

        if (e == NULL) return 0;                    /* nothing left */

        /* Skip the two that are not really entries. */
        if (strcmp (e->d_name, ".") == 0) continue;
        if (strcmp (e->d_name, "..") == 0) continue;

        size_t len = strlen (e->d_name);

        if ((int64_t) len > max) return -1;

        memcpy (into, e->d_name, len);

        return (int64_t) len;
    }
}

/* Closes a handle. A no-op for one already closed, on the same terms as a
 * file's. */
void alg_libdir_close (int64_t handle)
{
    if (handle < 0 || handle >= ALG_MAX_DIRS) return;
    if (dirs[handle] == NULL) return;

    closedir (dirs[handle]);

    dirs[handle] = NULL;
}

/* ------------------------------------------------------------ position -- */
/*
 * What a record file is built on. Everything above indexes by RECORD; these
 * three index by byte, because the language has no sizeof and so no width the
 * C could know about.
 *
 * None of them works on a pipe, which can only go forwards. fseek answers
 * failure there rather than doing something surprising, and the unit turns
 * that into a raise naming the reason -- a quiet -1 would read as "position
 * zero" to a caller who did not check.
 */

/* Moves to `offset` bytes from the start. Answers 1 for moved, 0 for not. */
int64_t alg_libfile_seek (int64_t handle, int64_t offset)
{
    if (handle < 0 || handle >= ALG_MAX_FILES) return 0;
    if (handles[handle] == NULL) return 0;
    if (offset < 0) return 0;

    return fseek (handles[handle], (long) offset, SEEK_SET) == 0 ? 1 : 0;
}

/* Where the handle is, in bytes from the start, or -1. */
int64_t alg_libfile_tell (int64_t handle)
{
    if (handle < 0 || handle >= ALG_MAX_FILES) return -1;
    if (handles[handle] == NULL) return -1;

    long at = ftell (handles[handle]);

    return at < 0 ? -1 : (int64_t) at;
}

/* How many bytes the OPEN file holds, or -1.
 *
 * Asked of the handle rather than of the path, because a writer's bytes may
 * still be in a buffer -- `stat` would answer the file as it was before the
 * last few writes. So this flushes first, then measures by moving to the end
 * and back, leaving the position where it found it.
 */
int64_t alg_libfile_length (int64_t handle)
{
    if (handle < 0 || handle >= ALG_MAX_FILES) return -1;
    if (handles[handle] == NULL) return -1;

    FILE *f = handles[handle];

    if (fflush (f) != 0) return -1;

    long here = ftell (f);

    if (here < 0) return -1;                       /* a pipe, most likely */
    if (fseek (f, 0, SEEK_END) != 0) return -1;

    long end = ftell (f);

    if (fseek (f, here, SEEK_SET) != 0) return -1;

    return end < 0 ? -1 : (int64_t) end;
}

/* Closes a handle. A no-op for one already closed, so a handler can release
 * on the way out without knowing how far the program got. */
void alg_libfile_close (int64_t handle)
{
    if (handle < 0 || handle >= ALG_MAX_FILES) return;
    if (handles[handle] == NULL) return;

    fclose (handles[handle]);

    handles[handle] = NULL;
}

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
