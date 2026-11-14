/* POSIX and XSI declarations are hidden under a strict -std=c11 on glibc and
 * musl, where Apple's headers expose them whatever the standard asked for.
 * Measured: without this, Linux cannot see strdup.
 *
 * _XOPEN_SOURCE 700 implies POSIX.1-2008 and turns on XSI, which is where the
 * drand48 family lives. On macOS asking for it makes Apple's headers STRICTER
 * and would hide their own additions -- _NSGetEnviron and TIOCGWINSZ among
 * them -- so _DARWIN_C_SOURCE puts those back. Both lines must precede every
 * include.
 */
#define _XOPEN_SOURCE 700
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif

/* dosffi.c -- disk space, the environment, and running a program.
 *
 * NO FUNCTION HERE ANSWERS A POINTER. A String returned from C borrows the
 * bytes rather than copying them, so a C function that reuses its buffer
 * corrupts a String still in hand -- silently. See DEFECTS.md H-2. Every
 * reader here fills a caller-owned buffer and answers a count, the same
 * discipline fileffi.c keeps. Going the other way is safe: a String passed
 * INTO C is read during the call and not kept.
 *
 * `alg_` is the compiler runtime's own prefix and a common noun after it will
 * collide -- fileffi.c first used `alg_file_exists`, which the runtime already
 * defines, and it surfaced only in examples/build.sh --static. Everything here
 * is `alg_libdos_`.
 *
 * POSIX, not C99: statvfs and posix_spawn. So unlike fileffi.c this does not
 * build anywhere -- it builds on macOS, Linux and the BSDs, and Windows would
 * need GetDiskFreeSpaceEx and CreateProcess behind the same three functions.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#include <windows.h>
#include <wchar.h>

#define alg_strdup  _strdup

/* ⚠️ The language's text is UTF-8 and Windows' narrow calls are not: they
 * decode by the ANSI code page, so a non-ASCII path, variable name or value
 * meant something else on the way in and came back mangled. Exec lost
 * 'café ✓' that way, measured; every call below that takes or answers text
 * goes through these two and the W entry point instead. */

/* UTF-8 as UTF-16, malloc'd, or NULL if it is not valid UTF-8. */
static wchar_t *wide_of (const char *utf8)
{
    int need = MultiByteToWideChar (CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1,
                                    NULL, 0);
    if (need <= 0) return NULL;

    wchar_t *wide = malloc ((size_t) need * sizeof *wide);
    if (wide == NULL) return NULL;

    if (!MultiByteToWideChar (CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1,
                              wide, need))
    {
        free (wide);
        return NULL;
    }

    return wide;
}

/* UTF-16 as UTF-8, malloc'd, or NULL. */
static char *utf8_of (const wchar_t *wide)
{
    int need = WideCharToMultiByte (CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);
    if (need <= 0) return NULL;

    char *utf8 = malloc ((size_t) need);
    if (utf8 == NULL) return NULL;

    if (!WideCharToMultiByte (CP_UTF8, 0, wide, -1, utf8, need, NULL, NULL))
    {
        free (utf8);
        return NULL;
    }

    return utf8;
}

#else

#include <spawn.h>
#include <sys/statvfs.h>
#include <sys/wait.h>

#define alg_strdup strdup

#ifdef __APPLE__
#include <crt_externs.h>
#define ALG_ENVIRON (*_NSGetEnviron ())
#else
extern char **environ;
#define ALG_ENVIRON environ
#endif

#endif

/* ------------------------------------------------------------ disk space -- */

/* Free or total bytes on the filesystem holding `path`, or -1.
 *
 * `total` picks which. One helper rather than two near-identical bodies,
 * because the Windows call answers both at once and splitting it would mean
 * asking twice.
 */
#ifdef _WIN32

/* GetDiskFreeSpaceEx wants a directory, where the unit's contract allows any
 * file on the filesystem. GetVolumePathName maps either to its mount point,
 * which is what makes `DiskFree ('./dos.a24')` answer for the whole volume as
 * it does on POSIX.
 */
static int64_t disk_bytes (const char *path, int total)
{
    if (path == NULL) return -1;

    /* GetVolumePathName asks the filesystem nothing: it maps ANY path to the
     * volume it would be on, so '/no/such/path/anywhere' answered C:\'s free
     * space where statvfs refuses it. The unit documents a refusal for a path
     * that is not there, so ask whether this one is before mapping it. */
    wchar_t *wide = wide_of (path);
    if (wide == NULL) return -1;

    wchar_t        volume[MAX_PATH + 1];
    ULARGE_INTEGER available;   /* to this caller, quotas included */
    ULARGE_INTEGER capacity;

    int ok = GetFileAttributesW (wide) != INVALID_FILE_ATTRIBUTES
          && GetVolumePathNameW (wide, volume, MAX_PATH + 1)
          && GetDiskFreeSpaceExW (volume, &available, &capacity, NULL);

    free (wide);

    if (!ok) return -1;

    return (int64_t) (total ? capacity.QuadPart : available.QuadPart);
}

#else

static int64_t disk_bytes (const char *path, int total)
{
    struct statvfs fs;

    if (path == NULL || statvfs (path, &fs) != 0)
        return -1;

    /* f_bavail rather than f_bfree: the second counts blocks the superuser
     * could still have, which is not what a program asking "may I write
     * this" wants to know. */
    uint64_t blocks = total ? (uint64_t) fs.f_blocks : (uint64_t) fs.f_bavail;

    return (int64_t) (blocks * (uint64_t) fs.f_frsize);
}

#endif

/* Bytes available to an unprivileged caller on the filesystem holding `path`.
 *
 * f_bavail rather than f_bfree: the second counts blocks the superuser could
 * still have, which is not what a program asking "may I write this" wants to
 * know.
 *
 * Turbo Pascal's DiskFree took a drive NUMBER and answered a LongInt, so it
 * went wrong above 2 GB. A path is the portable question and an Algol-24
 * Integer is unbounded, so neither limit is inherited.
 *
 * Answers -1 where the filesystem could not be asked, which the unit turns
 * into a raise. Multiplication is done in uint64 and the product can exceed
 * int64 only for a filesystem larger than 8 exabytes.
 */
int64_t alg_libdos_disk_free (const char *path)
{
    return disk_bytes (path, 0);
}

/* Total bytes on the filesystem holding `path`. -1 where it could not be
 * asked. f_blocks counts every block, the reserved ones included, which is
 * what "size" means. */
int64_t alg_libdos_disk_size (const char *path)
{
    return disk_bytes (path, 1);
}

/* ----------------------------------------------------------- environment -- */

/* Copies a variable's value into a caller's buffer and answers its length.
 *
 * Answers -1 where the variable is not set, which is distinct from a variable
 * set to the empty string answering 0 -- the unit keeps that distinction and
 * the caller can ask it.
 *
 * Answers the length a variable needs even when the buffer is too small to
 * hold it, writing nothing in that case, so a caller can size and retry. The
 * unit does not retry: it asks the length first.
 */
/* Copies UTF-8 text into a caller's buffer under the size-and-retry contract
 * below, and answers its length. */
static int64_t copy_out (const char *text, void *out, int64_t max)
{
    size_t len = strlen (text);

    if (out != NULL && max >= (int64_t) len)
        memcpy (out, text, len);

    return (int64_t) len;
}

#ifdef _WIN32

/* The environment block's entries one by one, in the C library's sense of
 * "the environment": an entry starting with '=' is one of Windows' hidden
 * per-drive directories, '=C:=C:\work', which _environ never listed and
 * Environment therefore never has. Answers the entry at `index` as UTF-8,
 * malloc'd, or NULL past the end; with index -1 it answers NULL and leaves
 * the count in *count.
 *
 * The block rather than _wenviron, which msvcrt leaves NULL until something
 * has asked for the wide environment -- an empty Environment for no reason. */
static char *env_entry (int64_t index, int64_t *count)
{
    wchar_t *block = GetEnvironmentStringsW ();
    int64_t  n     = 0;
    char    *found = NULL;

    if (block == NULL) { if (count) *count = 0; return NULL; }

    for (wchar_t *at = block; *at != L'\0'; at += wcslen (at) + 1)
    {
        if (*at == L'=') continue;

        if (n == index) { found = utf8_of (at); break; }

        n++;
    }

    FreeEnvironmentStringsW (block);

    if (count) *count = n;
    return found;
}

int64_t alg_libdos_getenv (const char *name, void *out, int64_t max)
{
    if (name == NULL)
        return -1;

    wchar_t *wide = wide_of (name);
    if (wide == NULL) return -1;

    /* Asked once for the size and once for the value, which is the API's own
     * contract; 0 from the first means the variable is not set. */
    DWORD need = GetEnvironmentVariableW (wide, NULL, 0);

    if (need == 0) { free (wide); return -1; }

    wchar_t *value = malloc ((size_t) need * sizeof *value);
    DWORD    got   = value ? GetEnvironmentVariableW (wide, value, need) : 0;

    free (wide);

    if (value == NULL || got >= need)
    {
        free (value);
        return -1;
    }

    char *utf8 = utf8_of (value);
    free (value);

    if (utf8 == NULL) return -1;

    int64_t len = copy_out (utf8, out, max);

    free (utf8);
    return len;
}

/* How many NAME=VALUE entries the environment holds. */
int64_t alg_libdos_env_count (void)
{
    int64_t n = 0;

    env_entry (-1, &n);

    return n;
}

/* Copies entry `index` into a caller's buffer and answers its length, or -1
 * for an index outside the environment. Same size-and-retry contract as
 * alg_libdos_getenv. */
int64_t alg_libdos_env_at (int64_t index, void *out, int64_t max)
{
    if (index < 0) return -1;

    char *entry = env_entry (index, NULL);

    if (entry == NULL) return -1;

    int64_t len = copy_out (entry, out, max);

    free (entry);
    return len;
}

#else

int64_t alg_libdos_getenv (const char *name, void *out, int64_t max)
{
    if (name == NULL)
        return -1;

    const char *value = getenv (name);

    if (value == NULL)
        return -1;

    return copy_out (value, out, max);
}

/* How many NAME=VALUE entries the environment holds. */
int64_t alg_libdos_env_count (void)
{
    char **env = ALG_ENVIRON;

    if (env == NULL)
        return 0;

    int64_t n = 0;

    while (env[n] != NULL)
        n++;

    return n;
}

/* Copies entry `index` into a caller's buffer and answers its length, or -1
 * for an index outside the environment. Same size-and-retry contract as
 * alg_libdos_getenv. */
int64_t alg_libdos_env_at (int64_t index, void *out, int64_t max)
{
    char **env = ALG_ENVIRON;

    if (env == NULL || index < 0)
        return -1;

    for (int64_t i = 0; i < index; i++)
        if (env[i] == NULL)
            return -1;

    const char *entry = env[index];

    if (entry == NULL)
        return -1;

    return copy_out (entry, out, max);
}

#endif

/* -------------------------------------------------- running a program ----- */

/* The argument vector being built for the next spawn.
 *
 * State in C rather than a packed buffer from Algol-24, because packing would
 * mean agreeing on a separator and no byte is safe: a path may contain any
 * byte but NUL, and NUL cannot cross the boundary as part of a String. Pushing
 * one argument per call avoids the question entirely.
 *
 * The vector is owned here and freed by the spawn, so a reset is always safe
 * and an abandoned push leaks nothing beyond the next reset.
 */
#define ALG_MAX_ARGS 1024

static char   *args[ALG_MAX_ARGS];
static int64_t arg_count = 0;

static void args_clear (void)
{
    for (int64_t i = 0; i < arg_count; i++)
    {
        free (args[i]);
        args[i] = NULL;
    }

    arg_count = 0;
}

/* Begins an argument vector, discarding any half-built one. */
void alg_libdos_arg_reset (void)
{
    args_clear ();
}

/* Appends one argument. Answers 1 on success, 0 if the vector is full or the
 * copy failed -- the unit raises on 0 rather than spawning a truncated
 * command, which is the one outcome worth designing against here. */
int64_t alg_libdos_arg_push (const char *arg)
{
    if (arg == NULL || arg_count >= ALG_MAX_ARGS - 1)
        return 0;

    char *copy = alg_strdup (arg);

    if (copy == NULL)
        return 0;

    args[arg_count++] = copy;

    return 1;
}

/* Runs `path` with the pushed arguments and waits for it.
 *
 * NO SHELL. posix_spawn takes the argument vector directly, so a path or an
 * argument containing a space, a quote, a semicolon or a newline is passed
 * through as one argument and cannot become syntax. Turbo Pascal's Exec took a
 * command LINE, which is where that hazard lives; system() would reintroduce
 * it.
 *
 * argv[0] is the path itself, as a program expects, with the pushed arguments
 * after it. The environment is inherited.
 *
 * Answers the exit status for a program that exited, 128 + the signal number
 * for one that was killed -- the shell's convention, and distinguishable
 * because a status of its own cannot exceed 255. Answers -1 where the program
 * could not be started at all, which the unit turns into a raise: a command
 * that never ran is not a command that failed.
 *
 * The vector is cleared either way, so a failed spawn does not leave arguments
 * to contaminate the next one.
 */
#ifdef _WIN32

/* Windows takes a command LINE, which is the irony of this port.
 *
 * The whole reason Exec takes an argument list is that a command line makes
 * quoting the caller's problem and a semicolon into syntax. CreateProcess
 * offers no vector form, so the line has to be built here -- and built so
 * that the child's own CommandLineToArgvW takes it apart into exactly the
 * arguments that went in. Getting that wrong is how a space in a path becomes
 * two arguments.
 *
 * The rules are the ones CommandLineToArgvW documents, and they are not
 * obvious: inside quotes a run of backslashes is literal unless a quote
 * follows, in which case each backslash must be doubled and the quote
 * escaped. An argument needing no quoting is appended bare, which keeps the
 * common line readable.
 *
 * Answers 0 if the argument will not fit. Windows caps a command line at
 * 32767 characters and a truncated command is the one outcome worth refusing
 * outright.
 */
static int quote_onto (char *out, size_t max, size_t *at, const char *arg)
{
    size_t need_quotes = (*arg == '\0' || strpbrk (arg, " \t\n\v\"") != NULL);

    if (*at + 1 < max && *at > 0) out[(*at)++] = ' ';

    if (!need_quotes)
    {
        size_t len = strlen (arg);

        if (*at + len >= max) return 0;

        memcpy (out + *at, arg, len);
        *at += len;

        return 1;
    }

    if (*at + 1 >= max) return 0;

    out[(*at)++] = '"';

    for (const char *p = arg; ; p++)
    {
        size_t slashes = 0;

        while (*p == '\\') { slashes++; p++; }

        if (*p == '\0')
        {
            /* Double them: they precede the closing quote. */
            if (*at + slashes * 2 + 1 >= max) return 0;

            for (size_t i = 0; i < slashes * 2; i++) out[(*at)++] = '\\';

            break;
        }

        if (*p == '"')
        {
            /* Double them and escape the quote they precede. */
            if (*at + slashes * 2 + 2 >= max) return 0;

            for (size_t i = 0; i < slashes * 2; i++) out[(*at)++] = '\\';

            out[(*at)++] = '\\';
            out[(*at)++] = '"';

            continue;
        }

        if (*at + slashes + 1 >= max) return 0;

        for (size_t i = 0; i < slashes; i++) out[(*at)++] = '\\';

        out[(*at)++] = *p;
    }

    if (*at + 1 >= max) return 0;

    out[(*at)++] = '"';

    return 1;
}

int64_t alg_libdos_spawn (const char *path)
{
    if (path == NULL)
    {
        args_clear ();
        return -1;
    }

    static char line[32768];
    size_t      at = 0;

    line[0] = '\0';

    int ok = quote_onto (line, sizeof line, &at, path);

    for (int64_t i = 0; ok && i < arg_count; i++)
        ok = quote_onto (line, sizeof line, &at, args[i]);

    args_clear ();

    if (!ok) return -1;

    line[at] = '\0';

    /* ⚠️ WIDE, because the language's text is UTF-8 and the A entry point is
     * not. CreateProcessA decodes by the ANSI code page, so 'café ✓' reached
     * the child as 'cafÃ© âœ“' -- each UTF-8 byte its own cp1252 character.
     * Measured with a child reporting CommandLineToArgvW's split: every ASCII
     * case arrived exact, quotes and backslash runs included, and only the
     * non-ASCII one did not. The line is built in UTF-8 as before and widened
     * once, path and all. */
    static wchar_t wide_line[32768];
    static wchar_t wide_path[32768];

    if (!MultiByteToWideChar (CP_UTF8, MB_ERR_INVALID_CHARS, line, -1,
                              wide_line, 32768)
        || !MultiByteToWideChar (CP_UTF8, MB_ERR_INVALID_CHARS, path, -1,
                                 wide_path, 32768))
        return -1;

    STARTUPINFOW        si;
    PROCESS_INFORMATION pi;

    ZeroMemory (&si, sizeof si);
    ZeroMemory (&pi, sizeof pi);

    si.cb = sizeof si;

    /* The path is passed as lpApplicationName as well as heading the line, so
     * Windows does not go searching and `C:\Program Files\x.exe` cannot be
     * read as `C:\Program` with an argument. */
    if (!CreateProcessW (wide_path, wide_line, NULL, NULL, FALSE, 0, NULL, NULL,
                         &si, &pi))
        return -1;

    WaitForSingleObject (pi.hProcess, INFINITE);

    DWORD status = 1;

    if (!GetExitCodeProcess (pi.hProcess, &status)) status = 1;

    CloseHandle (pi.hThread);
    CloseHandle (pi.hProcess);

    /* No signals here, so no 128 + n. A process killed by a fault answers the
     * NTSTATUS Windows gives it, which is a large number rather than a small
     * one -- the unit documents the POSIX convention and this platform simply
     * cannot keep it. */
    return (int64_t) status;
}

#else

int64_t alg_libdos_spawn (const char *path)
{
    if (path == NULL)
    {
        args_clear ();
        return -1;
    }

    char *argv[ALG_MAX_ARGS + 1];

    argv[0] = (char *) path;

    for (int64_t i = 0; i < arg_count; i++)
        argv[i + 1] = args[i];

    argv[arg_count + 1] = NULL;

    pid_t child = 0;
    int   err   = posix_spawn (&child, path, NULL, NULL, argv, ALG_ENVIRON);

    args_clear ();

    if (err != 0)
        return -1;

    int status = 0;

    while (waitpid (child, &status, 0) < 0)
        ;

    if (WIFEXITED (status))
        return (int64_t) WEXITSTATUS (status);

    if (WIFSIGNALED (status))
        return (int64_t) (128 + WTERMSIG (status));

    return -1;
}

#endif
