/* timeffi.c -- the calendar, which the language does not carry.
 *
 * `clock ()` [RT-012] answers the seconds since the epoch as a Double, and
 * nothing turns that into a year. The arithmetic could be hand-rolled -- leap
 * years are a closed rule -- but the TIME ZONE cannot: it lives in the host's
 * database and changes by legislation. That is the whole of what this file is
 * for.
 *
 * NO FUNCTION HERE ANSWERS A POINTER. A String returned from C borrows the
 * bytes rather than copying them, so a C function that reuses its buffer
 * corrupts a String still in hand -- silently. See DEFECTS.md H-2. These fill
 * a caller-owned buffer and answer a count, the same discipline fileffi.c
 * keeps.
 *
 * `alg_` is the compiler runtime's own prefix and a common noun after it will
 * collide, so everything here is `alg_libtime_`. A shared library hides such a
 * clash -- the interpreter resolves ours first -- and it surfaces only in
 * examples/build.sh --static, where everything links into one executable.
 */

#include <stdint.h>
#include <string.h>
#include <time.h>

/* The seven fields of a broken-down time, into a caller's buffer.
 *
 * Written as seven int32 words so the unit can read them back with
 * Buffer.GetInt, which indexes by byte. The order is the one the DateTime
 * class reads, and the two must agree:
 *
 *     0  year        (1900 + tm_year, so 2026 rather than 126)
 *     4  month       (1..12, where tm_mon is 0..11)
 *     8  day         (1..31)
 *    12  hour        (0..23)
 *    16  minute      (0..59)
 *    20  second      (0..60, a leap second being possible)
 *    24  day of week (0 = Sunday, as tm_wday has it)
 *
 * `local` chooses localtime over gmtime, which is the one thing the host knows
 * and a program cannot work out.
 *
 * Answers 1 on success and 0 where the conversion failed, which it can for a
 * time outside the host's representable range. A failure leaves the buffer
 * untouched rather than half-filled.
 */
int64_t alg_libtime_fields (double epoch, int64_t local, void *out, int64_t max)
{
    if (out == NULL || max < 28)
        return 0;

    /* A Double carries the seconds; the fraction is the unit's business, not
     * the calendar's. Truncating toward zero rather than rounding keeps the
     * second from advancing before it has elapsed. */
    time_t when = (time_t) epoch;

    struct tm parts;
    struct tm *got = local ? localtime_r (&when, &parts)
                           : gmtime_r (&when, &parts);

    if (got == NULL)
        return 0;

    int32_t f[7];

    f[0] = (int32_t) (parts.tm_year + 1900);
    f[1] = (int32_t) (parts.tm_mon + 1);
    f[2] = (int32_t) parts.tm_mday;
    f[3] = (int32_t) parts.tm_hour;
    f[4] = (int32_t) parts.tm_min;
    f[5] = (int32_t) parts.tm_sec;
    f[6] = (int32_t) parts.tm_wday;

    memcpy (out, f, sizeof f);

    return 1;
}

/* The host's offset from UTC in seconds, at the given time.
 *
 * Positive east of Greenwich. Worked out by asking for the same instant both
 * ways and differencing, rather than by reading tm_gmtoff, which is not in
 * C99 and is absent on some hosts. `timegm` would be the direct route and is
 * equally non-standard, so the difference is taken through mktime on a UTC
 * breakdown -- which is portable and exact for the offsets in use.
 *
 * Answers 0 where either conversion fails, which is also the right answer for
 * a host running on UTC.
 */
int64_t alg_libtime_utc_offset (double epoch)
{
    time_t when = (time_t) epoch;

    struct tm utc;
    struct tm loc;

    if (gmtime_r (&when, &utc) == NULL)
        return 0;
    if (localtime_r (&when, &loc) == NULL)
        return 0;

    /* mktime reads the local zone, so feeding it the UTC breakdown answers the
     * instant that UTC wall-clock would be locally -- and the difference from
     * the real instant is the offset. isdst is left to mktime to determine. */
    utc.tm_isdst = -1;

    time_t as_local = mktime (&utc);

    if (as_local == (time_t) -1)
        return 0;

    return (int64_t) (when - as_local);
}

/* Whether the host says daylight saving is in force at the given time.
 *
 * 1 for yes, 0 for no, and 0 where the conversion failed -- tm_isdst can
 * answer negative for "unknown", which is folded into no rather than invented
 * into a third state the unit would have to carry.
 */
int64_t alg_libtime_is_dst (double epoch)
{
    time_t when = (time_t) epoch;

    struct tm parts;

    if (localtime_r (&when, &parts) == NULL)
        return 0;

    return parts.tm_isdst > 0 ? 1 : 0;
}
