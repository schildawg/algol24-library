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
#include <spawn.h>
#include <sys/statvfs.h>
#include <sys/wait.h>

#ifdef __APPLE__
#include <crt_externs.h>
#define ALG_ENVIRON (*_NSGetEnviron ())
#else
extern char **environ;
#define ALG_ENVIRON environ
#endif

/* ------------------------------------------------------------ disk space -- */

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
    struct statvfs fs;

    if (path == NULL || statvfs (path, &fs) != 0)
        return -1;

    return (int64_t) ((uint64_t) fs.f_bavail * (uint64_t) fs.f_frsize);
}

/* Total bytes on the filesystem holding `path`. -1 where it could not be
 * asked. f_blocks counts every block, the reserved ones included, which is
 * what "size" means. */
int64_t alg_libdos_disk_size (const char *path)
{
    struct statvfs fs;

    if (path == NULL || statvfs (path, &fs) != 0)
        return -1;

    return (int64_t) ((uint64_t) fs.f_blocks * (uint64_t) fs.f_frsize);
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
int64_t alg_libdos_getenv (const char *name, void *out, int64_t max)
{
    if (name == NULL)
        return -1;

    const char *value = getenv (name);

    if (value == NULL)
        return -1;

    size_t len = strlen (value);

    if (out != NULL && max >= (int64_t) len)
        memcpy (out, value, len);

    return (int64_t) len;
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

    size_t len = strlen (entry);

    if (out != NULL && max >= (int64_t) len)
        memcpy (out, entry, len);

    return (int64_t) len;
}

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

    char *copy = strdup (arg);

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
