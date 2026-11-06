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
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

/* ------------------------------------------------- the strings SDL lends -- */

/* NO FUNCTION HERE ANSWERS A POINTER, which is why these two exist.
 *
 * SDL_GetError and SDL_GetWindowTitle both hand back a `const char *` into
 * SDL's own storage. A String returned from C BORROWS those bytes rather than
 * copying them -- DEFECTS.md H-2 -- and SDL's error string in particular is a
 * per-thread static that any later SDL call may overwrite. The unit used to
 * declare both as answering a String directly, and two of its call sites held
 * the result across further SDL calls, one of them across the whole of
 * CloseGraph: a diagnostic that could be rubbish by the time it was raised,
 * and at the worst possible moment.
 *
 * These copy into a caller-owned buffer and answer a length, the discipline
 * fileffi.c and dosffi.c already keep.
 */

/* Copies the current SDL error into a caller's buffer and answers its length.
 *
 * Answers the length even when the buffer is too small to hold it, writing
 * nothing in that case, so a caller can size and retry. The unit asks the
 * length first.
 */
int64_t alg_graph_error (void *out, int64_t max)
{
    const char *msg = SDL_GetError ();

    if (msg == NULL) return 0;

    size_t len = strlen (msg);

    if (out != NULL && max >= (int64_t) len)
        memcpy (out, msg, len);

    return (int64_t) len;
}

/* The same, for a window's title. Answers 0 for a null window rather than
 * asking SDL about it. */
int64_t alg_graph_window_title (void *win, void *out, int64_t max)
{
    if (win == NULL) return 0;

    const char *title = SDL_GetWindowTitle ((SDL_Window *) win);

    if (title == NULL) return 0;

    size_t len = strlen (title);

    if (out != NULL && max >= (int64_t) len)
        memcpy (out, title, len);

    return (int64_t) len;
}

/* ----------------------------------------------- SDL, through this file -- */

/* The unit used to declare SDL's own entry points directly, naming
 * /opt/homebrew/lib/libSDL2.dylib in each of twenty-four `external` clauses.
 * That worked and cost two things.
 *
 * It was macOS-only, and not for a reason the library could fix by renaming
 * its own shims: an `in` clause takes a string literal, and that literal was
 * an absolute path to somebody else's library. SDL is /usr/lib/<triple>/
 * libSDL2-2.0.so.0 on Debian, /usr/lib64/... on Fedora, elsewhere again on
 * Alpine and MSYS2. Declaring SDL from C instead moves the question to build
 * time, where `sdl2-config` already answers it.
 *
 * And every one of those declarations was unchecked. [FUN-014] says a
 * mismatched foreign signature is undefined behavior -- DEFECTS.md H-1 -- so
 * twenty-four hand-copied prototypes were twenty-four chances at a plausible
 * wrong number. These wrappers are compiled against SDL's real headers, so a
 * wrong signature is a compile error here instead.
 *
 * They are deliberately thin. Nothing below decides anything; the unit still
 * says which window, which texture, what colour and when. DESIGN.md asks that
 * this file carry transcoding and blitting rather than logic, and a
 * passthrough carries neither.
 *
 * Pointers cross as void *, which is what the unit's `Pointer` already is.
 * SDL_GetError and SDL_GetWindowTitle are NOT here -- they answer strings, and
 * so are handled above by copying rather than lending.
 */

int64_t alg_sdl_init (int64_t flags)
{
    return SDL_Init ((Uint32) flags);
}

void *alg_sdl_create_window (const char *title, int64_t x, int64_t y,
                             int64_t w, int64_t h, int64_t flags)
{
    return SDL_CreateWindow (title == NULL ? "" : title,
                             (int) x, (int) y, (int) w, (int) h,
                             (Uint32) flags);
}

void alg_sdl_destroy_window (void *win)
{
    if (win != NULL) SDL_DestroyWindow ((SDL_Window *) win);
}

void *alg_sdl_create_renderer (void *win, int64_t index, int64_t flags)
{
    if (win == NULL) return NULL;

    return SDL_CreateRenderer ((SDL_Window *) win, (int) index, (Uint32) flags);
}

void alg_sdl_destroy_renderer (void *ren)
{
    if (ren != NULL) SDL_DestroyRenderer ((SDL_Renderer *) ren);
}

int64_t alg_sdl_set_render_draw_color (void *ren, int64_t r, int64_t g,
                                       int64_t b, int64_t a)
{
    if (ren == NULL) return -1;

    return SDL_SetRenderDrawColor ((SDL_Renderer *) ren, (Uint8) r, (Uint8) g,
                                   (Uint8) b, (Uint8) a);
}

int64_t alg_sdl_render_clear (void *ren)
{
    if (ren == NULL) return -1;

    return SDL_RenderClear ((SDL_Renderer *) ren);
}

void alg_sdl_render_present (void *ren)
{
    if (ren != NULL) SDL_RenderPresent ((SDL_Renderer *) ren);
}

/* SDL answers through two out-pointers, and the unit passes two one-int
 * buffers because a Pointer there cannot be offset. */
void alg_sdl_get_window_size (void *win, void *w, void *h)
{
    if (win == NULL) return;

    SDL_GetWindowSize ((SDL_Window *) win, (int *) w, (int *) h);
}

int64_t alg_sdl_get_desktop_display_mode (int64_t index, void *mode)
{
    if (mode == NULL) return -1;

    return SDL_GetDesktopDisplayMode ((int) index, (SDL_DisplayMode *) mode);
}

void alg_sdl_pump_events (void)
{
    SDL_PumpEvents ();
}

void *alg_sdl_create_texture (void *ren, int64_t format, int64_t access,
                              int64_t w, int64_t h)
{
    if (ren == NULL) return NULL;

    return SDL_CreateTexture ((SDL_Renderer *) ren, (Uint32) format,
                              (int) access, (int) w, (int) h);
}

void alg_sdl_destroy_texture (void *tex)
{
    if (tex != NULL) SDL_DestroyTexture ((SDL_Texture *) tex);
}

int64_t alg_sdl_update_texture (void *tex, void *rect, void *pixels,
                                int64_t pitch)
{
    if (tex == NULL) return -1;

    return SDL_UpdateTexture ((SDL_Texture *) tex, (const SDL_Rect *) rect,
                              pixels, (int) pitch);
}

int64_t alg_sdl_render_copy (void *ren, void *tex, void *src, void *dst)
{
    if (ren == NULL) return -1;

    return SDL_RenderCopy ((SDL_Renderer *) ren, (SDL_Texture *) tex,
                           (const SDL_Rect *) src, (const SDL_Rect *) dst);
}

int64_t alg_sdl_set_texture_blend_mode (void *tex, int64_t mode)
{
    if (tex == NULL) return -1;

    return SDL_SetTextureBlendMode ((SDL_Texture *) tex, (SDL_BlendMode) mode);
}

int64_t alg_sdl_set_texture_alpha_mod (void *tex, int64_t alpha)
{
    if (tex == NULL) return -1;

    return SDL_SetTextureAlphaMod ((SDL_Texture *) tex, (Uint8) alpha);
}

int64_t alg_sdl_set_hint (const char *name, const char *value)
{
    if (name == NULL || value == NULL) return 0;

    return SDL_SetHint (name, value);
}

int64_t alg_sdl_poll_event (void *e)
{
    return SDL_PollEvent ((SDL_Event *) e);
}

int64_t alg_sdl_push_event (void *e)
{
    if (e == NULL) return -1;

    return SDL_PushEvent ((SDL_Event *) e);
}

void alg_sdl_start_text_input (void)
{
    SDL_StartTextInput ();
}

void alg_sdl_delay (int64_t ms)
{
    SDL_Delay (ms > 0 ? (Uint32) ms : 0);
}

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
static int64_t stamp_nx  = 1;      /* magnification as a ratio per axis, */
static int64_t stamp_dx  = 1;      /* so a half-height glyph is 1 over 2  */
static int64_t stamp_ny  = 1;
static int64_t stamp_dy  = 1;
static int64_t stamp_rot = 0;

static void set_rotation (int64_t rotation)
{
    rotation = ((rotation % 360) + 360) % 360;
    stamp_rot = (rotation / 90) * 90;
}

void alg_stamp_style (int64_t scale, int64_t rotation)
{
    stamp_nx = stamp_ny = scale > 0 ? scale : 1;
    stamp_dx = stamp_dy = 1;

    set_rotation (rotation);
}

/* The general form: a separate ratio for each axis, so text can be stretched
 * or squeezed rather than only doubled.  A zero or negative term is refused
 * into 1 rather than dividing by it.
 */
void alg_stamp_ratio (int64_t num_x, int64_t den_x, int64_t num_y,
                      int64_t den_y, int64_t rotation)
{
    stamp_nx = num_x > 0 ? num_x : 1;
    stamp_dx = den_x > 0 ? den_x : 1;
    stamp_ny = num_y > 0 ? num_y : 1;
    stamp_dy = den_y > 0 ? den_y : 1;

    set_rotation (rotation);
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

    /* The glyph's extent once turned.  A quarter turn counterclockwise sends
     * the glyph's top edge to the left edge, which is what a chart's Y-axis
     * label wants, and swaps its width and height along with it. */
    int64_t turned = (stamp_rot == 90 || stamp_rot == 270);
    int64_t rw = turned ? gh : gw;
    int64_t rh = turned ? gw : gh;

    /* And its extent once scaled.  Walking the DESTINATION and sampling back
     * is what lets the ratio be a fraction: replicating each source pixel a
     * whole number of times, as this once did, can only ever magnify. */
    int64_t dw = rw * stamp_nx / stamp_dx;
    int64_t dh = rh * stamp_ny / stamp_dy;

    if (dw < 1) dw = 1;      /* squeezed past nothing still marks its place */
    if (dh < 1) dh = 1;

    for (int64_t oy = 0; oy < dh; oy++)
    {
        int64_t ty = y + oy;

        if (ty < 0 || ty >= target_h) continue;

        int64_t ry = oy * stamp_dy / stamp_ny;

        if (ry >= rh) ry = rh - 1;

        for (int64_t ox = 0; ox < dw; ox++)
        {
            int64_t tx = x + ox;

            if (tx < 0 || tx >= target_w) continue;

            int64_t rx = ox * stamp_dx / stamp_nx;

            if (rx >= rw) rx = rw - 1;

            /* Undo the turn to find which source pixel this one shows. */
            int64_t r, c;

            if (stamp_rot == 90)       { r = rx;          c = gw - 1 - ry; }
            else if (stamp_rot == 180) { c = gw - 1 - rx; r = gh - 1 - ry; }
            else if (stamp_rot == 270) { r = gh - 1 - rx; c = ry;          }
            else                       { c = rx;          r = ry;          }

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

/* Scroll the target up by dy pixel rows, filling the vacated band.
 *
 * fill is a whole ARGB word, passed as int64 because the sign bit of an
 * int32 would mangle an opaque color.  Total: a dy past the height clears
 * the whole target; zero or less does nothing.
 */
/* Move a band of the stamp target up or down within itself, filling what it
 * vacates.
 *
 * The general form of the scroller.  A whole grid scrolling at its bottom is
 * one band -- all of it, moving up -- and deleting or inserting a line is
 * another: the rows from the cursor down, moving one row either way.  Both
 * are a run of pixels, so both live here.
 *
 * Rows are one-based and inclusive, as everything on this screen is.  A
 * positive dy moves the band's contents UP and fills at its foot, which is
 * what deleting a line does; a negative dy moves them DOWN and fills at its
 * head, which is what inserting one does.  The copy runs forward for the
 * first and backward for the second, the regions overlapping either way.
 *
 * Total: no target, a band outside the surface, a dy of zero, or a shift at
 * least as large as the band all do something defined -- nothing, or a band
 * wholly filled.
 */
void alg_scroll_band (int64_t top, int64_t bottom, int64_t dy, int64_t fill)
{
    if (target == 0) return;

    if (top < 1) top = 1;
    if (bottom > target_h) bottom = target_h;
    if (top > bottom || dy == 0) return;

    int32_t f    = (int32_t) (uint32_t) fill;
    int64_t rows = bottom - top + 1;
    int64_t d    = dy < 0 ? -dy : dy;

    if (d >= rows)
    {
        /* Everything in the band is shifted clean out of it. */
        for (int64_t y = top; y <= bottom; y++)
            for (int64_t x = 0; x < target_w; x++)
                target[(y - 1) * target_w + x] = f;

        return;
    }

    if (dy > 0)
    {
        for (int64_t y = top; y <= bottom - d; y++)
            for (int64_t x = 0; x < target_w; x++)
                target[(y - 1) * target_w + x] =
                    target[(y - 1 + d) * target_w + x];

        for (int64_t y = bottom - d + 1; y <= bottom; y++)
            for (int64_t x = 0; x < target_w; x++)
                target[(y - 1) * target_w + x] = f;
    }
    else
    {
        for (int64_t y = bottom; y >= top + d; y--)
            for (int64_t x = 0; x < target_w; x++)
                target[(y - 1) * target_w + x] =
                    target[(y - 1 - d) * target_w + x];

        for (int64_t y = top; y < top + d; y++)
            for (int64_t x = 0; x < target_w; x++)
                target[(y - 1) * target_w + x] = f;
    }
}

/* Scroll the whole target up, which is the band that is all of it.
 *
 * Kept as its own name because scrolling a grid at its foot is the common
 * case and reads better without three arguments that never vary.
 */
void alg_scroll_up (int64_t dy, int64_t fill)
{
    if (target == 0 || dy <= 0) return;

    alg_scroll_band (1, target_h, dy, fill);
}

/* Fill a rectangle of the stamp target with an eight-by-eight pattern.
 *
 * The unit decides everything that is a decision -- which rectangle, which
 * way round its corners were given, which pattern, which color, and whether
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
 * them.  The unit still decides the center, the radii, the angles, the
 * pattern and the color.
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

/* Fill the region around a seed with an eight-by-eight pattern, stopping at
 * pixels of the border color.
 *
 * The first primitive here that READS the surface.  Every other figure knows
 * which pixels it covers from its parameters alone; a flood has to look, and
 * looking is the whole cost -- a region is thousands of pixels and each one
 * must be examined whether or not it is painted.  That is the same bargain
 * alg_fill_rect struck, so it is struck the same way.  The unit still decides
 * everything that is a decision: the seed, the border, the pattern, the
 * color, and whether an empty fill should look at all.
 *
 * A BOUNDARY fill, as Turbo Pascal's was: the region is every pixel reachable
 * from the seed without crossing one of the border color -- not every pixel
 * matching the seed's own color.  That is the one an outlined figure wants,
 * and an outlined figure is what a flood is usually aimed at.
 *
 * Colors arrive as this screen's own: an RGB from zero up, or Transparent as
 * a negative, which is the untouched pixel's stored zero.
 *
 * Painted pixels cannot serve as the record of where the flood has been,
 * because a patterned fill leaves most of them unpainted -- so a visited byte
 * per pixel is kept alongside.  Spans rather than pixels go on the stack, one
 * per contiguous run of the neighboring row, which is what keeps it bounded.
 *
 * Total: no target, no pattern, a seed off the surface, a seed already on the
 * border, or no memory writes nothing.
 */
void alg_flood_fill (int64_t sx, int64_t sy, int64_t border,
                     void *pattern, int64_t color)
{
    const int32_t *rows = (const int32_t *) pattern;

    if (target == 0 || rows == 0) return;
    if (sx < 1 || sy < 1 || sx > target_w || sy > target_h) return;

    int32_t edge = border < 0
                 ? 0
                 : (int32_t) (0xFF000000u | (uint32_t) border);

    if (target[(sy - 1) * target_w + (sx - 1)] == edge) return;

    int64_t count = target_w * target_h;

    unsigned char *seen = (unsigned char *) calloc ((size_t) count, 1);

    if (seen == 0) return;

    int64_t  cap   = 256;
    int64_t  top   = 0;
    int64_t *stack = (int64_t *) malloc ((size_t) cap * 2 * sizeof (int64_t));

    if (stack == 0) { free (seen); return; }

    int32_t ink = (int32_t) (0xFF000000u | (uint32_t) color);

    stack[top * 2] = sx; stack[top * 2 + 1] = sy; top++;

    while (top > 0)
    {
        top--;

        int64_t x = stack[top * 2];
        int64_t y = stack[top * 2 + 1];

        int64_t base = (y - 1) * target_w;

        if (seen[base + x - 1] || target[base + x - 1] == edge) continue;

        /* The run this seed sits in, walked out to both edges. */
        int64_t lx = x;
        int64_t rx = x;

        while (lx > 1 && !seen[base + lx - 2] && target[base + lx - 2] != edge)
            lx--;

        while (rx < target_w && !seen[base + rx] && target[base + rx] != edge)
            rx++;

        uint32_t row = (uint32_t) rows[(y - 1) % 8];

        for (int64_t i = lx; i <= rx; i++)
        {
            seen[base + i - 1] = 1;

            if ((row >> (7 - (i - 1) % 8)) & 1) target[base + i - 1] = ink;
        }

        /* One seed per contiguous run of each neighboring row, rather than
         * one per pixel, so the stack stays a fraction of the region. */
        for (int64_t d = -1; d <= 1; d += 2)
        {
            int64_t ny = y + d;

            if (ny < 1 || ny > target_h) continue;

            int64_t nbase = (ny - 1) * target_w;
            int64_t i     = lx;

            while (i <= rx)
            {
                while (i <= rx
                       && (seen[nbase + i - 1] || target[nbase + i - 1] == edge))
                    i++;

                if (i > rx) break;

                if (top == cap)
                {
                    int64_t  grown = cap * 2;
                    int64_t *bigger = (int64_t *) realloc
                        (stack, (size_t) grown * 2 * sizeof (int64_t));

                    if (bigger == 0) { free (stack); free (seen); return; }

                    stack = bigger;
                    cap   = grown;
                }

                stack[top * 2] = i; stack[top * 2 + 1] = ny; top++;

                while (i <= rx
                       && !seen[nbase + i - 1] && target[nbase + i - 1] != edge)
                    i++;
            }
        }
    }

    free (stack);
    free (seen);
}

/* Fill the whole stamp target with one raw pixel value.
 *
 * The one fill that alg_fill_rect cannot do, because that one forces the
 * alpha byte opaque -- deliberately, since a figure being filled is being
 * painted.  Clearing is the opposite act, and its natural value is a fully
 * transparent zero, so it takes the pixel as written rather than an RGB to
 * make opaque.
 *
 * Total: no target writes nothing.
 */
void alg_clear_target (int64_t value)
{
    if (target == 0) return;

    int32_t v = (int32_t) value;
    int64_t n = target_w * target_h;

    for (int64_t i = 0; i < n; i++) target[i] = v;
}

/* ------------------------------------------------------------- images -- */
/*
 * Copy a rectangle of the stamp target out into a caller's buffer.
 *
 * A rectangle of any size is thousands of pixels, which is the same bargain
 * every other run here strikes.  The unit decides the rectangle and owns the
 * buffer; this only moves the words.
 *
 * Coordinates are one-based and inclusive, as everything on this screen is.
 * A rectangle reaching past the surface reads transparent zeros there rather
 * than reading out of bounds, so an image lifted from the edge is defined and
 * puts back what it took.
 *
 * Total: no target, no destination, or a rectangle the wrong way round
 * writes nothing.
 */
void alg_get_image (void *dst, int64_t x1, int64_t y1, int64_t x2, int64_t y2)
{
    int32_t *out = (int32_t *) dst;

    if (target == 0 || out == 0 || x2 < x1 || y2 < y1) return;

    int64_t at = 0;

    for (int64_t y = y1; y <= y2; y++)
        for (int64_t x = x1; x <= x2; x++)
            if (x < 1 || y < 1 || x > target_w || y > target_h)
                out[at++] = 0;
            else
                out[at++] = target[(y - 1) * target_w + (x - 1)];
}

/* Draw a caller's buffer onto the stamp target, combining as the mode says.
 *
 * The modes are Turbo Pascal's five, plus one its hardware had no need of.
 * They combine the WHOLE 32-bit pixel, alpha included, which is what makes
 * XorPut's defining property hold exactly: writing the same image twice
 * restores what was underneath, because (a ^ b) ^ b is a whatever the bits
 * mean.  It also makes an opaque sprite show over a transparent surface,
 * where combining only the colour would leave the alpha at zero and the
 * sprite invisible.
 *
 * NotPut is the exception: it inverts the colour and keeps the source's
 * alpha, because the inverse of a picture is a picture, not an absence of
 * one.
 *
 * TransparentPut is the one Turbo Pascal lacked, its images being opaque
 * indices throughout.  It skips wholly transparent source pixels, which is
 * what a sprite over a background needs and what CopyPut cannot do.
 *
 * Pixels falling outside the surface are dropped, the edge being the clip
 * here as everywhere.
 *
 * Total: no target, no source, or a senseless size draws nothing.
 */
void alg_put_image (const void *src, int64_t w, int64_t h,
                    int64_t x, int64_t y, int64_t mode)
{
    const int32_t *in = (const int32_t *) src;

    if (target == 0 || in == 0 || w <= 0 || h <= 0) return;

    for (int64_t row = 0; row < h; row++)
    {
        int64_t ty = y + row;

        if (ty < 1 || ty > target_h) continue;

        for (int64_t col = 0; col < w; col++)
        {
            int64_t tx = x + col;

            if (tx < 1 || tx > target_w) continue;

            int32_t  s  = in[row * w + col];
            int32_t *d  = &target[(ty - 1) * target_w + (tx - 1)];

            switch (mode)
            {
                case 1:  *d = (int32_t) ((uint32_t) *d ^ (uint32_t) s); break;
                case 2:  *d = (int32_t) ((uint32_t) *d | (uint32_t) s); break;
                case 3:  *d = (int32_t) ((uint32_t) *d & (uint32_t) s); break;
                case 4:  *d = (int32_t) (((uint32_t) s & 0xFF000000u)
                              | (~(uint32_t) s & 0x00FFFFFFu));         break;
                case 5:  if ((uint32_t) s & 0xFF000000u) *d = s;        break;
                default: *d = s;                                        break;
            }
        }
    }
}
