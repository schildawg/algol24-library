/* crtffi.c -- what the `crt` unit cannot ask the language for.
 *
 * A terminal is not a window: its size belongs to whoever resized it, its
 * input arrives a byte at a time only once the line discipline is out of the
 * way, and none of that is expressible in Algol-24.  So this file is the
 * boundary, and it is deliberately thin -- it moves bytes and flips terminal
 * modes.  Everything that understands what the bytes MEAN, which escape
 * sequence is which key and which cell wants repainting, stays in the unit
 * where it is readable and testable.
 *
 * Every function here is total: called with the terminal closed, or with no
 * terminal at all, each answers something defined rather than failing.  That
 * matters more than usual because the library's own tests run with no tty.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>

/* Whether standard output is a terminal at all.
 *
 * The tests run with it redirected, and `crt` uses this to keep quiet about
 * raw mode rather than failing: a program piped to a file should still run.
 */
int64_t alg_crt_istty (void)
{
    return isatty (1) ? 1 : 0;
}

/* The terminal's width and height in cells, as it is right now.
 *
 * Asked afresh each time rather than cached, so a window resized between two
 * calls answers the new size -- which is what lets the unit notice a resize
 * without a signal handler.  With no terminal, the classic 80 by 25, so a
 * redirected run still has a sane geometry to lay out against.
 */
static void winsize_now (int64_t *cols, int64_t *rows)
{
    struct winsize ws;

    *cols = 80;
    *rows = 25;

    if (ioctl (1, TIOCGWINSZ, &ws) == 0)
    {
        if (ws.ws_col > 0) *cols = ws.ws_col;
        if (ws.ws_row > 0) *rows = ws.ws_row;
    }
}

int64_t alg_crt_cols (void)
{
    int64_t c, r;

    winsize_now (&c, &r);

    return c;
}

int64_t alg_crt_rows (void)
{
    int64_t c, r;

    winsize_now (&c, &r);

    return r;
}

/* Whether the terminal has said it can do 24-bit color.
 *
 * COLORTERM is the only thing that says so with any reliability -- TERM
 * describes a terminfo entry, and terminfo has no truecolor capability that
 * is widely filled in.  Answering 0 is never wrong, only duller: the unit
 * falls back to the 256-color cube, which every terminal in use understands.
 */
int64_t alg_crt_truecolor (void)
{
    const char *c = getenv ("COLORTERM");

    if (c == NULL) return 0;

    return (strcmp (c, "truecolor") == 0 || strcmp (c, "24bit") == 0) ? 1 : 0;
}

/* ------------------------------------------------------------- raw mode -- */
/*
 * ReadKey wants one keystroke, unechoed, without waiting for a newline, and
 * KeyPressed wants to ask whether one is there without blocking at all.  Both
 * need the line discipline out of the way, which is termios and nothing else.
 *
 * The old settings are kept here so that leaving raw mode restores exactly
 * what was found rather than an approximation of a sane terminal.  A program
 * that dies without calling CloseCrt leaves the terminal raw; that is the
 * shell's `reset` to fix, and is why CloseCrt is worth calling from an
 * exception handler.
 */

static struct termios saved_mode;
static int            saved_valid = 0;

int64_t alg_crt_raw (int64_t on)
{
    if (!isatty (0)) return 0;

    if (on)
    {
        if (saved_valid) return 1;               /* already raw, idempotent */
        if (tcgetattr (0, &saved_mode) != 0) return 0;

        struct termios raw = saved_mode;

        /* Character at a time, no echo, and no signal or flow-control
         * interception -- Ctrl-C arrives as a byte, which is what lets a
         * full-screen program decide for itself what it means. */
        raw.c_lflag &= ~(unsigned long) (ICANON | ECHO | ISIG | IEXTEN);
        raw.c_iflag &= ~(unsigned long) (IXON | ICRNL | INLCR | ISTRIP);
        raw.c_oflag &= ~(unsigned long) (OPOST);
        raw.c_cc[VMIN]  = 0;
        raw.c_cc[VTIME] = 0;

        if (tcsetattr (0, TCSAFLUSH, &raw) != 0) return 0;

        saved_valid = 1;

        return 1;
    }

    if (!saved_valid) return 0;

    tcsetattr (0, TCSAFLUSH, &saved_mode);
    saved_valid = 0;

    return 1;
}

/* One byte of input, or -1 when none is waiting.
 *
 * Never blocks.  The unit polls this to assemble escape sequences and to
 * answer KeyPressed, so a blocking read here would make KeyPressed a lie.
 */
int64_t alg_crt_getch (void)
{
    unsigned char c;

    if (!isatty (0)) return -1;

    ssize_t n = read (0, &c, 1);

    return n == 1 ? (int64_t) c : -1;
}

/* Sleep for a few milliseconds, so a wait for input is not a spin.
 *
 * ReadKey has to wait for a key that may be a minute away, and the language
 * has no sleep of its own -- a bare polling loop would burn a core doing
 * nothing.  graph delays through SDL; this unit has no SDL, so the delay
 * comes from here.
 *
 * Ten milliseconds is a hundred wakeups a second, which is far below anything
 * a person can perceive at a keyboard and far above anything that shows on a
 * CPU meter.
 *
 * Total: a negative or senseless delay sleeps not at all rather than
 * forever.
 */
void alg_crt_nap (int64_t millis)
{
    if (millis <= 0) return;
    if (millis > 1000) millis = 1000;

    struct timespec t;

    t.tv_sec  = (time_t) (millis / 1000);
    t.tv_nsec = (long) ((millis % 1000) * 1000000L);

    nanosleep (&t, NULL);
}

/* -------------------------------------------------------------- output -- */
/*
 * The painter builds one string per frame and hands it over here.  Going
 * through C rather than the language's own Write is not about speed but about
 * CONTROL: this writes exactly the bytes given, with no newline appended and
 * no buffering left to chance, which a screen painter cannot do without.
 *
 * Total: a null string writes nothing.
 */
void alg_crt_write (const char *text)
{
    if (text == NULL) return;

    size_t len  = strlen (text);
    size_t sent = 0;

    while (sent < len)
    {
        ssize_t n = write (1, text + sent, len - sent);

        if (n <= 0) break;                  /* a closed pipe, and no more */

        sent += (size_t) n;
    }
}

/* -------------------------------------------------------------- cells -- */
/*
 * A cell is four int32 words: the codepoint, the ink, the background, and a
 * flag word carrying Blink.  A codepoint of 0 means the cell holds no glyph,
 * which is different from holding a space -- a space paints its background
 * over what is beneath, an empty cell does not.
 *
 * The two runs below are here for the same reason graph's cell stamper is:
 * they touch every cell of a screen, and the interpreter cannot afford to.
 * Everything that decides anything -- which surface is where, what order they
 * stack in, which glyph a character has -- stays in the unit.
 */

#define CELL_WORDS 4

/* Empty every cell of a screen: no glyph, default ink, transparent ground.
 *
 * The compositor lays surfaces over this each frame, so it has to start from
 * nothing or the last frame shows through the gaps.  Two thousand cells is
 * eight thousand words, which is a run and so belongs here.
 *
 * Total: a null buffer, or a senseless count, clears nothing.
 */
void alg_crt_clear (void *screen, int64_t cells, int64_t fg)
{
    int32_t *c = (int32_t *) screen;

    if (c == 0 || cells <= 0) return;

    for (int64_t i = 0; i < cells; i++)
    {
        c[i * CELL_WORDS]     = 0;      /* no glyph */
        c[i * CELL_WORDS + 1] = (int32_t) fg;
        c[i * CELL_WORDS + 2] = -1;     /* Transparent */
        c[i * CELL_WORDS + 3] = 0;
    }
}

/* Lay one surface's cells into the screen, bottom-up.
 *
 * Called for each surface in Order, lowest first, so the topmost writer of a
 * cell wins.  An opaque background CLEARS the glyph under it before the
 * surface's own glyph is laid, which is what makes a window with a background
 * hide the text it covers rather than letting it show through the gaps.
 *
 * Coordinates are one-based, as everything on this screen is.  A surface
 * hanging off the edge is clipped rather than refused.
 *
 * Total: a null buffer, or a senseless size, lays nothing.
 */
void alg_crt_blit (void *screen, int64_t scols, int64_t srows,
                   void *src, int64_t wcols, int64_t wrows,
                   int64_t x1, int64_t y1)
{
    int32_t *dst = (int32_t *) screen;
    const int32_t *s = (const int32_t *) src;

    if (dst == 0 || s == 0) return;
    if (scols <= 0 || srows <= 0 || wcols <= 0 || wrows <= 0) return;

    for (int64_t r = 0; r < wrows; r++)
    {
        int64_t ty = y1 + r;

        if (ty < 1 || ty > srows) continue;

        for (int64_t c = 0; c < wcols; c++)
        {
            int64_t tx = x1 + c;

            if (tx < 1 || tx > scols) continue;

            const int32_t *in  = s   + (r * wcols + c) * CELL_WORDS;
            int32_t       *out = dst + ((ty - 1) * scols + (tx - 1)) * CELL_WORDS;

            if (in[2] >= 0)              /* an opaque background */
            {
                out[2] = in[2];
                out[0] = 0;              /* and it hides what it covers */
            }

            if (in[0] != 0)              /* a glyph of its own */
            {
                out[0] = in[0];
                out[1] = in[1];
                out[3] = in[3];
            }
        }
    }
}

/* ------------------------------------------------------------ painting -- */

static void put_str (char **at, const char *s)
{
    while (*s) *(*at)++ = *s++;
}

static void put_num (char **at, int64_t n)
{
    char tmp[24];
    int  i = 0;

    if (n == 0) { *(*at)++ = '0'; return; }

    while (n > 0) { tmp[i++] = (char) ('0' + n % 10); n /= 10; }
    while (i > 0) *(*at)++ = tmp[--i];
}

/* The nearest xterm-256 index to an RGB, for terminals without truecolor.
 *
 * The cube is 6x6x6 from index 16 with the levels 0, 95, 135, 175, 215, 255,
 * and a 24-step gray ramp from 232 that the grays land on far more closely
 * than the cube would take them.
 */
static int64_t nearest_256 (int64_t rgb)
{
    int64_t r = (rgb >> 16) & 255, g = (rgb >> 8) & 255, b = rgb & 255;

    static const int lev[6] = {0, 95, 135, 175, 215, 255};

    int ri = 0, gi = 0, bi = 0;

    for (int i = 1; i < 6; i++)
    {
        if (labs ((long) lev[i] - (long) r) < labs ((long) lev[ri] - (long) r)) ri = i;
        if (labs ((long) lev[i] - (long) g) < labs ((long) lev[gi] - (long) g)) gi = i;
        if (labs ((long) lev[i] - (long) b) < labs ((long) lev[bi] - (long) b)) bi = i;
    }

    int64_t cube = 16 + 36 * ri + 6 * gi + bi;
    int64_t cd   = (lev[ri] - r) * (lev[ri] - r) + (lev[gi] - g) * (lev[gi] - g)
                 + (lev[bi] - b) * (lev[bi] - b);

    /* The gray ramp, which a near-gray color usually lands closer to. */
    int64_t avg = (r + g + b) / 3;
    int64_t gi2 = (avg - 8) / 10;

    if (gi2 < 0)  gi2 = 0;
    if (gi2 > 23) gi2 = 23;

    int64_t gl = 8 + gi2 * 10;
    int64_t gd = (gl - r) * (gl - r) + (gl - g) * (gl - g) + (gl - b) * (gl - b);

    return gd < cd ? 232 + gi2 : cube;
}

static void put_color (char **at, int64_t rgb, int fg, int64_t truecolor)
{
    put_str (at, "\033[");

    if (rgb < 0)                              /* Transparent: the default */
    {
        put_str (at, fg ? "39m" : "49m");

        return;
    }

    if (truecolor)
    {
        put_str (at, fg ? "38;2;" : "48;2;");
        put_num (at, (rgb >> 16) & 255); *(*at)++ = ';';
        put_num (at, (rgb >> 8) & 255);  *(*at)++ = ';';
        put_num (at, rgb & 255);
    }
    else
    {
        put_str (at, fg ? "38;5;" : "48;5;");
        put_num (at, nearest_256 (rgb));
    }

    *(*at)++ = 'm';
}

static void put_utf8 (char **at, int64_t cp)
{
    if (cp < 0x80) { *(*at)++ = (char) cp; return; }

    if (cp < 0x800)
    {
        *(*at)++ = (char) (0xC0 | (cp >> 6));
        *(*at)++ = (char) (0x80 | (cp & 0x3F));

        return;
    }

    if (cp < 0x10000)
    {
        *(*at)++ = (char) (0xE0 | (cp >> 12));
        *(*at)++ = (char) (0x80 | ((cp >> 6) & 0x3F));
        *(*at)++ = (char) (0x80 | (cp & 0x3F));

        return;
    }

    *(*at)++ = (char) (0xF0 | (cp >> 18));
    *(*at)++ = (char) (0x80 | ((cp >> 12) & 0x3F));
    *(*at)++ = (char) (0x80 | ((cp >> 6) & 0x3F));
    *(*at)++ = (char) (0x80 | (cp & 0x3F));
}

/* Paint the cells that differ from what was last painted, and remember them.
 *
 * The diff is the whole point: a full screen is two thousand cells and around
 * forty kilobytes of escapes, where a typical frame changes a handful.  Only
 * changed cells are emitted, the cursor is moved only when the next cell is
 * not the one after the last, and a color is re-stated only when it changes
 * -- which is what keeps a repaint to a few hundred bytes.
 *
 * `out` is a caller's scratch buffer; the unit sizes it from the screen, and
 * the worst case is bounded by cells x the longest per-cell run.
 *
 * Answers how many cells were actually repainted, which the unit's tests read
 * back -- a frame that changed nothing must emit nothing.
 *
 * Total: a null buffer, or a senseless size, paints nothing.
 */
int64_t alg_crt_paint (void *cells, void *prev, void *out,
                       int64_t cols, int64_t rows, int64_t truecolor)
{
    const int32_t *now = (const int32_t *) cells;
    int32_t *was = (int32_t *) prev;
    char    *buf = (char *) out;

    if (now == 0 || was == 0 || buf == 0 || cols <= 0 || rows <= 0) return 0;

    char   *at      = buf;
    int64_t painted = 0;

    int64_t cur_fg = -2, cur_bg = -2, cur_fl = -1;
    int64_t at_col = -1, at_row = -1;

    for (int64_t r = 1; r <= rows; r++)
        for (int64_t c = 1; c <= cols; c++)
        {
            int64_t i = ((r - 1) * cols + (c - 1)) * CELL_WORDS;

            if (now[i] == was[i] && now[i+1] == was[i+1]
                && now[i+2] == was[i+2] && now[i+3] == was[i+3])
                continue;

            if (r != at_row || c != at_col)
            {
                put_str (&at, "\033[");
                put_num (&at, r);
                *at++ = ';';
                put_num (&at, c);
                *at++ = 'H';
            }

            int64_t fg = now[i+1], bg = now[i+2], fl = now[i+3];

            if (fl != cur_fl)
            {
                put_str (&at, (fl & 1) ? "\033[5m" : "\033[25m");
                cur_fl = fl;
            }

            if (fg != cur_fg) { put_color (&at, fg, 1, truecolor); cur_fg = fg; }
            if (bg != cur_bg) { put_color (&at, bg, 0, truecolor); cur_bg = bg; }

            put_utf8 (&at, now[i] == 0 ? ' ' : now[i]);

            was[i] = now[i]; was[i+1] = now[i+1];
            was[i+2] = now[i+2]; was[i+3] = now[i+3];

            at_col = c + 1;
            at_row = r;

            if (at_col > cols) { at_col = -1; at_row = -1; }

            painted++;
        }

    *at = '\0';

    /* Deliberately does NOT write.  The unit decides whether anything
     * reaches the terminal, which is what lets the tests paint a screen and
     * read the escapes back without a byte of it landing on stdout. */
    return painted;
}
