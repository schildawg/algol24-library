/* POSIX and XSI declarations are hidden under a strict -std=c11 on glibc and
 * musl, where Apple's headers expose them whatever the standard asked for.
 * Measured: without this, Linux cannot see srand48, drand48 and clock_gettime.
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

/* randomffi.c -- the two things the `random` unit needs that no C function
 * offers with the signature wanted.
 *
 * The generator lives here too, since 0.1.5. The unit used to declare libc's
 * `drand48` directly, which is exactly the shape wanted -- and which Windows
 * does not have at all. POSIX specifies the algorithm, so owning it changes
 * no stream; see alg_random_next.
 */

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

/* Seed the generator from the whole of `seed`.
 *
 * `srand48` takes a long but keeps only its low 32 bits -- measured, not
 * assumed: seeds 42, 42 + 2^32 and 42 + 2^33 all produce the same stream.  An
 * Algol-24 Integer is unbounded, so passing one straight through would silently
 * discard most of a large seed and make distinct seeds collide.
 *
 * Folding the high half onto the low half with XOR lets every bit contribute.
 * It is not a hash and makes no claim to be one; it only ensures that two seeds
 * differing anywhere are unlikely to produce the same stream, which pass-through
 * cannot promise.
 */
/* The generator itself, which used to be the host's.
 *
 * The unit declared libc's `drand48` directly, with no `in` clause, so it
 * resolved out of the running program -- fine wherever libc carries the
 * drand48 family and impossible on Windows, which does not. Owning it here is
 * what lets `random` build on every platform.
 *
 * ⚠️ IT IS THE SAME SEQUENCE, not a replacement for one. POSIX specifies this
 * generator exactly rather than leaving it to an implementation:
 *
 *     X(n+1) = (a * X(n) + c) mod 2^48,  a = 0x5DEECE66D,  c = 0xB
 *
 * and srand48 seeds it with X = (seed << 16) | 0x330E. Measured against the
 * platform's own drand48 before this was adopted: 4000 draws over four seeds,
 * bit-identical, zero mismatches. So no stream any program has relied on
 * changes, and a seed now answers the same stream on every platform rather
 * than only on the ones with an XSI libc.
 */
#define ALG_A    0x5DEECE66DULL
#define ALG_C    0xBULL
#define ALG_MASK 0xFFFFFFFFFFFFULL         /* 2^48 - 1 */

static uint64_t state = (0x1234ABCDULL << 16) | 0x330EULL;   /* srand48 (1) */

void alg_random_seed (int64_t seed)
{
    uint64_t bits = (uint64_t) seed;

    /* srand48 keeps only the low 32 bits of its argument -- measured, seeds
     * 42, 42 + 2^32 and 42 + 2^33 all gave one stream -- and an Algol-24
     * Integer is unbounded, so the high half is folded onto the low with XOR
     * rather than discarded. Not a hash and no claim to be one; it only means
     * two seeds differing anywhere are unlikely to collide, which
     * pass-through could not promise. */
    uint32_t folded = (uint32_t) ((bits ^ (bits >> 32)) & 0xFFFFFFFFu);

    state = ((uint64_t) folded << 16) | 0x330EULL;
}

/* The next value, at least zero and below one. */
double alg_random_next (void)
{
    state = (ALG_A * state + ALG_C) & ALG_MASK;

    return (double) state / 281474976710656.0;      /* 2^48 */
}

/* A seed from the clock, for `Randomize`.
 *
 * `time(NULL)` would be the obvious source and is the wrong one: it advances
 * once a second, so two programs started in the same second get identical
 * sequences.  Nanoseconds fix that for separate runs, and the process id
 * separates two processes that start within the same nanosecond.
 */
int64_t alg_random_clock_seed (void)
{
    struct timespec now;

    clock_gettime (CLOCK_REALTIME, &now);

    return (int64_t) now.tv_sec * 1000000000
         + (int64_t) now.tv_nsec
         + (int64_t) getpid () * 2654435761;
}
