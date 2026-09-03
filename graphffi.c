/* graphffi.c -- the one C primitive the `graph` unit needs for speed.
 *
 * A glyph file is hex text, and the interpreter decoding it a digit at a time
 * was measured at ~8 seconds for the shipped 189 KB font -- all of it spent in
 * per-character calls.  Decoding is not logic, it is transcoding, so it is the
 * right thing to buy from C; everything that understands the format (headers,
 * widths, ink versus picture) stays in the unit.
 */

#include <stdint.h>
#include <stddef.h>

static int hex_value (char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;

    return 0;   /* match the unit's old HexDigit: junk counts as zero */
}

/* Decode `hex` in groups of `group` digits, one int32 word per group, into
 * `out`.  Answers how many words were written.  A trailing partial group is
 * ignored, which is what dividing the length by the group size promised.
 *
 * A group of 8 can carry a value past the int32 sign bit; it is stored as its
 * bit pattern, and the caller adds 2^32 back when a signed read answers a
 * negative.  Total on purpose: a NULL or a senseless group answers 0 rather
 * than touching anything.
 */
int64_t alg_unhex_words (const char *hex, int64_t group, int32_t *out)
{
    if (hex == NULL || out == NULL || group <= 0 || group > 8) return 0;

    int64_t written = 0;

    while (1)
    {
        uint32_t value = 0;

        for (int64_t i = 0; i < group; i++)
            if (hex[i] == '\0') return written;
            else value = value * 16 + (uint32_t) hex_value (hex[i]);

        out[written++] = (int32_t) value;
        hex += group;
    }
}
