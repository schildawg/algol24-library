/* graphffi.c -- the one C primitive the `graph` unit needs for speed.
 *
 * A glyph file is hex text, and the interpreter decoding it a digit at a time
 * was measured at ~8 seconds for the shipped 189 KB font -- all of it spent in
 * per-character calls.  Decoding is not logic, it is transcoding, so it is the
 * right thing to buy from C; everything that understands the format (headers,
 * widths, ink versus picture) stays in the unit.
 */

#include <math.h>
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

/* ------------------------------------------------------------ the grid -- */
/*
 * Stamping one glyph into a text cell is the hot path of text mode: a full
 * 80 x 25 screen is two thousand cells, and the interpreted blend loop was
 * measured at ~18ms per glyph -- half a minute per screenful.  So the cell
 * blit lives here.  The unit still owns the format knowledge and the layout:
 * it says which glyph, which mode, what ink, what background, and where.
 *
 * A foreign call takes at most eight arguments, so the target framebuffer is
 * set once rather than passed each time.
 */

static int32_t *target       = 0;
static int64_t  target_w     = 0;
static int64_t  target_h     = 0;

/* Style for the next stamps: a whole-number magnification and a quarter
 * turn counterclockwise.  State rather than parameters because a foreign
 * call takes at most eight arguments and the stamp already uses all eight.
 * The grid never sets it, so cells stay 1:1 and upright; a viewport sets it
 * around its own drawing and puts it back.
 */
static int64_t stamp_scale = 1;
static int64_t stamp_rot   = 0;

void alg_stamp_style (int64_t scale, int64_t rotation)
{
    stamp_scale = scale > 0 ? scale : 1;

    rotation = ((rotation % 360) + 360) % 360;
    stamp_rot = (rotation / 90) * 90;
}

void alg_stamp_target (void *pixels, int64_t w, int64_t h)
{
    target   = (int32_t *) pixels;
    target_w = w > 0 ? w : 0;
    target_h = h > 0 ? h : 0;
}

static int32_t mixed (int64_t ground, int64_t color, int64_t cover)
{
    int64_t gr = (ground >> 16) & 255, gg = (ground >> 8) & 255, gb = ground & 255;
    int64_t ir = (color  >> 16) & 255, ig = (color  >> 8) & 255, ib = color  & 255;

    int64_t r = gr + (ir - gr) * cover / 255;
    int64_t g = gg + (ig - gg) * cover / 255;
    int64_t b = gb + (ib - gb) * cover / 255;

    return (int32_t) (0xFF000000u | (r << 16) | (g << 8) | b);
}

/* Stamp one glyph's cell at pixel x, y.
 *
 * glyph is the unit's decoded word Buffer -- one word per row for mode 0
 * (bits, high bit leftmost), one coverage word per pixel for mode 1, one
 * RRGGBBAA word per pixel for mode 2 -- or NULL for a blank cell.  A bg of
 * -1 leaves uncovered pixels transparent; any other bg paints the whole cell
 * opaque and blends the ink into it, which is what a text-mode cell is.
 *
 * Total: no target, a senseless size, or a wholly off-target cell writes
 * nothing.
 */
void alg_stamp_cell (int64_t x, int64_t y, void *glyph,
                     int64_t gw, int64_t gh,
                     int64_t mode, int64_t ink, int64_t bg)
{
    const int32_t *words = (const int32_t *) glyph;

    if (target == 0 || gw <= 0 || gh <= 0) return;

    for (int64_t r = 0; r < gh; r++)
        for (int64_t c = 0; c < gw; c++)
        {
            int64_t cover = 0;
            int64_t color = ink;

            if (words != 0)
            {
                if (mode == 0)
                    cover = ((uint32_t) words[r] >> (gw - 1 - c)) & 1 ? 255 : 0;
                else if (mode == 1)
                    cover = (uint32_t) words[r * gw + c];
                else
                {
                    uint32_t px = (uint32_t) words[r * gw + c];

                    cover = px & 255;
                    color = px >> 8;
                }
            }

            /* Where this source pixel lands, before magnification.  A quarter
             * turn counterclockwise sends the glyph's top edge to the left
             * edge, which is what a chart's Y-axis label wants. */
            int64_t dx = c;
            int64_t dy = r;

            if (stamp_rot == 90)       { dx = r;          dy = gw - 1 - c; }
            else if (stamp_rot == 180) { dx = gw - 1 - c; dy = gh - 1 - r; }
            else if (stamp_rot == 270) { dx = gh - 1 - r; dy = c;          }

            for (int64_t sy = 0; sy < stamp_scale; sy++)
            {
                int64_t ty = y + dy * stamp_scale + sy;

                if (ty < 0 || ty >= target_h) continue;

                for (int64_t sx = 0; sx < stamp_scale; sx++)
                {
                    int64_t tx = x + dx * stamp_scale + sx;

                    if (tx < 0 || tx >= target_w) continue;

                    int32_t *out = target + ty * target_w + tx;

                    if (bg >= 0)
                        *out = mixed (bg, color, cover);
                    else if (cover >= 255)
                        *out = (int32_t) (0xFF000000u | (uint32_t) color);
                    else if (cover > 0)
                        *out = (int32_t) (((uint32_t) cover << 24) | (uint32_t) color);
                    else
                        *out = 0;
                }
            }
        }
}

/* Scroll the target up by dy pixel rows, filling the vacated band.
 *
 * fill is a whole ARGB word, passed as int64 because the sign bit of an
 * int32 would mangle an opaque color.  Total: a dy past the height clears
 * the whole target; zero or less does nothing.
 */
void alg_scroll_up (int64_t dy, int64_t fill)
{
    if (target == 0 || dy <= 0) return;
    if (dy > target_h) dy = target_h;

    int64_t keep = (target_h - dy) * target_w;

    for (int64_t i = 0; i < keep; i++)
        target[i] = target[i + dy * target_w];

    for (int64_t i = keep; i < target_h * target_w; i++)
        target[i] = (int32_t) (uint32_t) fill;
}

/* Fill a rectangle of the stamp target with an eight-by-eight pattern.
 *
 * The unit decides everything that is a decision -- which rectangle, which
 * way round its corners were given, which pattern, which colour, and whether
 * the fill is empty at all.  What is left is a run of pixels, and a run is
 * what C is for: the interpreted loop was measured at 7.5 seconds for one
 * 300 x 200 bar, of which a bare 60,000 buffer writes are 0.5.
 *
 * Coordinates are one-based and inclusive, as everything on this screen is,
 * and the pattern tiles from the SURFACE's origin rather than the
 * rectangle's -- which is what makes two bars side by side share one weave.
 *
 * Total: no target, no pattern, or a rectangle wholly outside writes nothing.
 */
void alg_fill_rect (int64_t x1, int64_t y1, int64_t x2, int64_t y2,
                    void *pattern, int64_t color)
{
    const int32_t *rows = (const int32_t *) pattern;

    if (target == 0 || rows == 0) return;

    int32_t ink = (int32_t) (0xFF000000u | (uint32_t) color);

    for (int64_t y = y1; y <= y2; y++)
    {
        if (y < 1 || y > target_h) continue;

        uint32_t row  = (uint32_t) rows[(y - 1) % 8];
        int32_t *line = target + (y - 1) * target_w;

        for (int64_t x = x1; x <= x2; x++)
        {
            if (x < 1 || x > target_w) continue;

            if ((row >> (7 - (x - 1) % 8)) & 1) line[x - 1] = ink;
        }
    }
}

/* Fill an elliptical sector of the stamp target with an eight-by-eight
 * pattern.
 *
 * The companion to alg_fill_rect, and there for the same reason: a slice of
 * any size is thousands of pixels and the interpreted loop cannot afford
 * them.  The unit still decides the centre, the radii, the angles, the
 * pattern and the colour.
 *
 * Two radii rather than one, so that PieSlice is the equal-radii case of
 * Sector exactly as Circle is of Ellipse -- one primitive, and the two verbs
 * cannot drift into computing different shapes.
 *
 * Angles are DEGREES, zero at three o'clock, counterclockwise, and the sweep
 * is measured counterclockwise from the start.  Because a screen's Y grows
 * downward the test flips it, so the mathematics is in the ordinary
 * orientation and only the read of the pixel is upside down.
 *
 * Whether a point lies within the sweep is asked WITHOUT an arctangent, which
 * the library does not have in two-argument form: a ray at angle A divides
 * the plane, and the cross product says which side a point falls on.  For a
 * sweep of half a turn or less the point must be on the counterclockwise side
 * of the start AND the clockwise side of the end; for more than half a turn
 * the two half-planes overlap and it is either.  Exact, and no transcendental
 * per pixel.
 *
 * The angle is the PARAMETRIC one, as Ellipse's is, which is why the point is
 * divided by the radii before the test: on a stretched circle the parametric
 * angle is the circle's, and dividing undoes the stretch.
 *
 * Total: no target, no pattern, a radius below zero, or a sector wholly
 * outside writes nothing.
 */
void alg_fill_sector (int64_t cx, int64_t cy, int64_t xr, int64_t yr,
                      int64_t start_deg, int64_t sweep_deg,
                      void *pattern, int64_t color)
{
    const int32_t *rows = (const int32_t *) pattern;

    if (target == 0 || rows == 0 || xr < 0 || yr < 0) return;
    if (xr == 0 || yr == 0) return;

    int32_t ink = (int32_t) (0xFF000000u | (uint32_t) color);

    double s = (double) start_deg * 3.14159265358979323846 / 180.0;
    double e = (double) (start_deg + sweep_deg) * 3.14159265358979323846 / 180.0;

    double sx = cos (s), sy = sin (s);
    double ex = cos (e), ey = sin (e);

    int whole = sweep_deg >= 360 || sweep_deg <= -360;
    int wide  = sweep_deg > 180;

    int64_t bound = xr > yr ? xr : yr;

    for (int64_t y = cy - yr; y <= cy + yr; y++)
    {
        if (y < 1 || y > target_h) continue;

        uint32_t row  = (uint32_t) rows[(y - 1) % 8];
        int32_t *line = target + (y - 1) * target_w;

        for (int64_t x = cx - xr; x <= cx + xr; x++)
        {
            if (x < 1 || x > target_w) continue;

            int64_t px = x - cx;
            int64_t py = cy - y;              /* the screen's Y is upside down */

            /* Inside the ellipse: (px/xr)^2 + (py/yr)^2 <= 1, multiplied out
             * so the test stays in integers. */
            if (px * px * yr * yr + py * py * xr * xr > xr * xr * yr * yr)
                continue;

            if (!whole)
            {
                /* Undo the stretch before asking the angle, the parametric
                 * angle being the circle's. */
                double nx = (double) px / (double) xr;
                double ny = (double) py / (double) yr;

                double from_start = sx * ny - sy * nx;
                double to_end     = ex * ny - ey * nx;

                if (wide)
                {
                    if (!(from_start >= 0.0 || to_end <= 0.0)) continue;
                }
                else if (!(from_start >= 0.0 && to_end <= 0.0)) continue;
            }

            if ((row >> (7 - (x - 1) % 8)) & 1) line[x - 1] = ink;
        }
    }

    (void) bound;
}
