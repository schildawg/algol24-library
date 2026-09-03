/* randomffi.c -- the two things the `random` unit needs that no C function
 * offers with the signature wanted.
 *
 * The generator itself is C's `drand48` family, declared directly by the unit:
 * `double drand48(void)` is exactly the shape wanted and needs no wrapper.
 * These two do not have such a shape, for different reasons.
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
void alg_random_seed (int64_t seed)
{
    uint64_t bits = (uint64_t) seed;

    srand48 ((long) ((bits ^ (bits >> 32)) & 0xFFFFFFFFu));
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
