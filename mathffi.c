/* mathffi.c -- C primitives the `math` unit needs and libm does not offer.
 *
 * An Algol-24 `external` declaration states the C function's signature, and
 * nothing checks it: a declaration that misdescribes one is undefined behavior
 * in the ordinary C sense, which the language specification says outright.  So
 * a foreign declaration can only ever name a C function whose real signature is
 * the one wanted.
 *
 * libm has `trunc`, but it returns a `double`.  Declaring it `: Integer` to get
 * an integer back is exactly the misdescription above -- it compiles, it runs,
 * and it answers 0 for every input.  There is no `lltrunc` to reach for
 * instead.  So the function wanted is written here rather than approximated
 * there.
 *
 * Built into a shared library by ./build.sh, and reached through
 * `external '...' in '...'`.
 */

#include <math.h>
#include <stdint.h>

/* Truncate toward zero, answering an integer.
 *
 * math.a24's Trunc rejects NaN, the infinities and anything at or beyond 2^63
 * before calling, so the guards below are never what produces the answer.  They
 * are here so that this function is total on its own: a C cast of an
 * out-of-range double to an integer type is undefined behavior, and a
 * primitive that a wrong caller can turn into UB is not one worth having.
 */
int64_t alg_trunc (double x)
{
    if (isnan (x))                       return 0;
    if (x >=  9223372036854775808.0)     return INT64_MAX;   /* 2^63 is one past the top */
    if (x <   -9223372036854775808.0)    return INT64_MIN;   /* but -2^63 itself fits */

    return (int64_t) x;
}

/* Every finite double is exactly `mantissa * 2^exponent` for an integral
 * mantissa below 2^53.  These two answer that pair, and together they let
 * math.a24 build the exact Integer for a value too large for the 64-bit
 * conversion above -- which Algol-24 can hold, having unbounded Integers, and C
 * cannot.
 *
 * Split into two calls because a foreign call returns one value and cannot take
 * an out-parameter.  Both are cheap: `frexp` is a handful of bit operations.
 */
int64_t alg_float_mantissa (double x)
{
    int exponent;
    double fraction = frexp (x, &exponent);   /* x = fraction * 2^exponent, 0.5 <= |fraction| < 1 */

    /* Scaling by 2^53 makes it integral without losing a bit, because a double
     * carries 53 bits of significand and no more. */
    return (int64_t) ldexp (fraction, 53);
}

int64_t alg_float_exponent (double x)
{
    int exponent;

    frexp (x, &exponent);

    return exponent - 53;                     /* to match the scaling above */
}

/* libm's own `trunc` and `llrint`, carried here so `math` does not have to
 * find them in the running program.
 *
 * Both were declared with no `in` clause, which resolves out of whatever the
 * process already holds -- fine on macOS and Linux, where libm is there, and
 * fatal on Windows: the runtime looks in the program and in msvcrt.dll, and
 * mingw-w64 links these two C99 functions in statically rather than taking
 * them from msvcrt, so `Int` and `Round` answered `No foreign symbol` and took
 * `graph`, `sound` and `random` down with them.  `random` lost `drand48` the
 * same way and was fixed the same way.  `sqrt`, `exp`, `log`, `sin`, `cos`
 * and `atan` predate C99 and msvcrt exports them, so they stay as they are.
 */
double alg_libmath_int (double x)
{
    return trunc (x);
}

/* Rounds by the current direction -- to nearest, ties to even, since nothing
 * here changes it.  `llrint` of NaN or of a value outside int64 is
 * unspecified, so those answer a defined value instead; math.a24's Round
 * refuses them before calling, and this only keeps a wrong caller out of UB.
 */
int64_t alg_libmath_round (double x)
{
    if (isnan (x))                       return 0;
    if (x >=  9223372036854775808.0)     return INT64_MAX;
    if (x <   -9223372036854775808.0)    return INT64_MIN;

    return (int64_t) llrint (x);
}
