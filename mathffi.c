/* mathffi.c -- C primitives the `math` unit needs and libm does not offer.
 *
 * An Algol-24 `external` declaration states the C function's signature, and
 * nothing checks it: a declaration that misdescribes one is undefined behaviour
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
 * out-of-range double to an integer type is undefined behaviour, and a
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
