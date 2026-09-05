# The design of `graph`

The unit's methods arrive assignment by assignment, but they land against this
design, which was settled in conversation before the first drawing routine was
written. A future method that fights anything here is wrong until this document
says otherwise.

## The vision

**There is no separation between text mode and graphics.** `graph` feels like
text mode — a cursor, cheap writing, scrolling, `ClrScr` — and you can draw
graphics in it. The 1980s made a programmer choose between the two because text
cells and bitmaps were different hardware; the machine everyone actually wanted
had both at once, and that constraint died with the hardware. This unit is that
machine.

Turbo Pascal's names are kept where they fit, as ever. Its *split* — Crt for
text, Graph for pixels, two rulebooks — is exactly the limitation not being
inherited.

## The architecture

One window, composed bottom to top as **one ordered stack of surfaces**, of
two kinds:

```
   background                the window's own surface, global pixel verbs
   the root Window           the screen-wide grid, Order 0 -- the desktop
   surfaces, by Order        Windows and ViewPorts, interleaved freely
```

- **A Window is celled**: a rectangle of text cells on cell boundaries,
  `X1, Y1, X2, Y2` inclusive in the root grid's cell space -- Turbo Pascal's
  `Window (1, 1, 80, 25)` grown into a class. It carries the whole text
  vocabulary as methods -- `Print`, `GotoXY`, `ClrScr`, the colors, the
  videos, `Blink` -- with its own cursor, its own colors, its own blink ink,
  and its own scrolling: its `PrintLn` at its own bottom row scrolls its
  cells and nothing else's.
- **A ViewPort is pixels**: a positioned, sized drawing surface with its own
  pixels and transparency -- what this document first called Canvas, renamed
  to the Turbo Pascal word for a clipped drawing region, which is what it is.
- **The root grid is the bottom Window**, screen-wide at Order 0, which no
  other surface may claim. The global text verbs delegate to it, so every
  program written before surfaces existed still means what it meant.

### Why two kinds, and not layers

A single text plane can only be entirely above or entirely below any given
graphic. The TurboVision case that breaks it: window A holds a pie chart
drawn *over* its text; window B drags *over* both. That is text, graphic,
text -- interleaved -- and no fixed layering can say it. With text itself a
stackable surface, B covers the chart because B is above it in the one
stack; no special case, just Order.

```
   root grid            the desktop's celled text        (bottom)
   Window A             a text window, Order 1
   ViewPort chart       A's pie chart, Order 2
   Window B             dragged over both, Order 3       (top)
```

Dragging B is assignment to its position; the chart follows A the same way;
`B.ClrEol` erases B's prose and cannot touch the chart, which belongs to a
different surface. The one rule survives sharpened: **text verbs act on the
receiving surface's text.**

### The one rule

**On a grid, text behaves like text mode; off the grid, text is a graphic.**
Every interaction question resolves by asking which surface a thing lives
on. `ClrEol` erases the celled text and the chart beneath shows through,
intact -- erasing a graphic is a graphics act, on the ViewPort that holds
it.

### The screen counts from one

Cells and pixels alike: `(1, 1)` is the top-left — of the grid, of the
window, of a canvas — and the count is the last index, so the bottom-right
cell is `TextCols (), TextRows ()` and the last pixel column is `GetMaxX ()`
= the width. Decided for the storybook read of `(1, 1, 80, 25)` and because
one dialect for the whole screen kills both classic off-by-ones at once.
The language's strings and Buffers stay 0-based — that seam exists wherever
the base is drawn, and it lives at the memory boundary: a cursor column
meets a string index as `Text[Col - 1]`.

### The two kinds of text

| | **celled** (the grid) | **free** (a canvas) |
| --- | --- | --- |
| placed by | cursor, in columns and rows | pixels, any X,Y |
| cursor, wrap, scroll | yes | no |
| erased by `ClrEol` / `ClrScr` | yes | no — text verbs ignore it |
| background color | yes, per cell; transparent by default | no — ink only |
| magnification | none; the cell is the size | any whole number, by `SetTextStyle` |
| orientation | upright only | 0° / 90° / 180° / 270°, by `SetTextStyle` |
| written by | `Write`, `WriteLn` | `OutText`, `OutTextXY` |

Two of those rows are forced rather than chosen. A cell is half as wide as it
is tall, so a glyph rotated 90° cannot sit on the grid — rotation is not
withheld from celled text, it is inexpressible there. And a background is a
cell concept — it fills the cell behind the glyph; a free label wanting a
backing draws a rectangle first, which is what graphics verbs are for.

⚠️ **"Transparent" needs a spelling of its own.** Colors are RGB integers and
0 is a legitimate black, so the transparent background is a named constant
(value −1), never a color that happens to mean it.

⚠️ **Rotation is not CJK vertical writing.** Spinning rotates the glyphs,
which is what a chart's axis label wants. Chinese set vertically keeps each
glyph upright and stacks them down a column — a different feature, deliberately
not conflated, addable later without touching orientation.

## Surfaces

Settled for both kinds, the first five by the original Canvas decisions:

- **Every verb is a method on its surface.** `Chart.OutTextXY (…)`,
  `B.Write (…)` — where a thing draws or writes is unaskable, because the
  receiver says. The globals are the root Window's methods, kept as the
  words programs already use, and every method is *also* a surface-first
  free function — `GotoXY (W, 3, 2)` — because verb-first is how a Turbo
  Pascal program reads. The aliases add no behavior; they became possible
  in 0.1.4, which settles an overload on arity when arity is enough.
- **Coordinates are local.** (1,1) is the surface's own top-left — cell for
  a Window, pixel for a ViewPort — which is what makes its contents
  portable: move the surface and everything on it moves, nothing redraws.
  Local is also the clip: a surface cannot scribble outside itself, which
  keeps the old `SetViewPort` promise without a clipping rectangle to
  maintain.
- **Translucency is 0..255**, property `Alpha`, default 255. A byte, because
  every color channel and every coverage value in the pipeline is already a
  byte, and no Real comparison wanted anywhere. Per-pixel transparency needs
  no dial at all: an unpainted pixel is transparent, which is the natural
  state of a fresh canvas.
- **`Order` is a nonzero signed integer** — negative under the root grid,
  positive over it — and `Order` and position are settable after creation:
  reordering is bring-to-front, moving is dragging. Two surfaces with the
  same Order stack by creation, earlier beneath later. Order 0 is the root
  grid's alone.
- **`CloseGraph` invalidates every surface.** A surface belongs to the window
  it was made in; reopening starts clean.
- **A Window is placed in cells, a ViewPort in pixels — of one shared
  logical space.** Both count from 1, both scale to the physical window as
  one, and `CellWidth`/`CellHeight` convert between them, so a chart placed
  inside a window's frame stays inside it at every window size. Placing a
  ViewPort in raw screen pixels was tried first and was wrong twice over: it
  neither lined up with the cells nor scaled with them.

## Why this costs nothing at present time

Each canvas is its own Buffer and its own SDL texture; the text grid and the
background likewise. `Present` copies them in stack order and **the GPU does
the compositing** — translucency via the texture's alpha mod, per-pixel
transparency via the alpha channel (the pixel format grows an alpha byte;
color numbers stay `0xRRGGBB`).

⚠️ **Per-pixel compositing in the interpreter is forbidden, and this is
measured, not felt.** The interpreted pixel loop runs at ~35µs per pixel, so
compositing one 640×480 frame in Algol-24 would cost ~10 seconds per present.
The whole design stands on SDL doing that instead; a present is a handful of
texture copies whatever the canvas count, and 200 full-frame uploads measured
at 0.03s.

## The worked use case

A TurboVision-style text window with a pie chart inside it:

- The window — frame, title, shadow, its text — is **celled text in the
  grid**, exactly as TurboVision drew into the text screen of a real machine.
- The chart is a **canvas at positive Order**, sized to the window's client
  area, positioned inside it, drawn once in its own local coordinates.
- Dragging the window repaints the frame in the grid as text always repainted,
  and the chart follows by setting `X` and `Y` — nothing redraws.
- Stacked windows are stacked canvases, `Order` matching the windows' order.
  `ClrEol` inside the window erases prose and never the chart.

## Color, and the palette that is not a register file

Designed, not built. Nothing here has been implemented; it is written down so
that when it is, the shape is already settled.

**A color is a 24-bit RGB Integer, and that is the whole of it.** `Blue` is
`170`, `Red` is `11141120`, `Transparent` is `-1`, and `Blink` is bit 24 added
to an ink. There is no index space: `4` means RGB `0x000004`, a near-black, and
not "entry four".

### Why Turbo Pascal's palette is not ported

`SetPalette` was a wrapper over the EGA and VGA palette registers, and the
shape of the API says so — numbered entries, `GetPaletteSize`, `MaxColors`,
`GetDefaultPalette`. Those are questions about a chip. Asked here, `MaxColors`
answers 16,777,216 and `GetPaletteSize` answers the same, which tells a reader
nothing at all. They are not kept.

**What the palette actually bought was not naming.** The framebuffer stored
*indices*, so writing one register recolored every pixel already drawn,
instantly and with no redraw — which is what fades, color cycling, fire and
water were all made of. That capability is genuinely given up here: a
ViewPort's buffer holds final ARGB and goes straight to `SDL_UpdateTexture`,
so there is nothing left to remap. The design does not pretend otherwise, and
a paletted surface that could do it is a door left open below rather than
something this section quietly implies.

### What is kept: the sixteen names, rebound per surface

A palette here **rebinds what the sixteen color names mean on one surface,
and it is consulted when you draw**. Programs written to `LightCyan` and
`Brown` keep working unchanged; a theme recolors them wholesale.

```algol24
var Amber := [DarkGray: 4276224, LightGray: 12550144, White: 16768256];

V.SetPalette (Amber);
V.SetColor (LightGray);      // draws amber, because that is what LightGray means here
V.SetColor (LightCyan);      // unlisted, so it still means LightCyan
```

A palette is a Map from one of the sixteen constants to the RGB it should
mean. `SetPalette` replaces the surface's mapping entirely, and **a name the
map does not list means itself** — so a partial palette is the ordinary case
and rebinding one color does not require restating the other fifteen.
`GetPalette` answers the mapping in force. Whether to also keep Turbo Pascal's
per-entry `SetPalette (Entry, Color)` as a merge is a detail for the
assignment that builds this.

### The limitation, which is the first thing any doc comment must say

**A palette governs what is drawn next, not what is already drawn.** Set one
after painting a screen and the screen does not change; re-theming means
re-drawing. This is exactly the half of Turbo Pascal's behavior the pixel
model cannot give, and stating it anywhere but first would let a reader
discover it by being surprised.

### Where it applies

Exactly five places take a color from a caller and keep it, and the lookup
belongs at those five and nowhere else:

| | verb |
| --- | --- |
| `Window.Ink` | `TextColor` |
| `Window.Back` | `TextBackground` |
| `ViewPort.Pen` | `SetColor` |
| `ViewPort.FillInk` | `SetFillStyle`, `SetFillPattern` |

Free text follows the pen, so `OutText` and `OutTextXY` are themed without
knowing about it. `PutPixel` is deliberately **not** in the list: it writes a
color directly and bypasses the pen already, and the two exceptions should be
the same exception.

Two values must survive the lookup untouched. `Transparent` is not a color
and has no palette entry. `Blink` is bit 24 *added* to an ink, so
`Yellow + Blink` must have that bit stripped before the lookup and put back
after — miss this and blinking text is the one thing on the screen that
ignores the theme.

`GetColor` answers the name it was given rather than what the name resolved
to, so save-and-restore round-trips. `GetPixel` answers the resolved color,
because that is what is actually on the surface.

### The one open question

Under this scheme `SetColor (V, 11141120)` and `SetColor (V, Red)` are
indistinguishable, so an arbitrary RGB that happens to equal one of the
sixteen gets rebound too. Sixteen values out of 16.7 million is a small
surface, and **the remap is inert until a palette is set**, so a program that
never asks for a theme can never be bitten — but it is magic, and it should be
ruled on rather than discovered.

The recommendation is to accept it and state the rule plainly: a color equal
to one of the sixteen is a *name*, anything else is a literal.

The alternative considered was tagging the sixteen constants into a reserved
range, the way `Blink` uses bit 24, so that names and raw RGBs are genuinely
different spaces. It is rejected because the constants **are** the CGA RGB
values, which is both useful and true, and because `Yellow + Blink` is
arithmetic on a color that the library already invites — tagging would break
the idiom to fix a collision that costs less than the fix.

## Doors left open, deliberately

Nothing below is designed yet; nothing above blocks any of it.

- **The background as the root ViewPort.** The window's own surface may one
  day be simply a ViewPort at the bottom of the stack, and the global pixel
  verbs its methods, completing the symmetry the root Window began.
- ~~`WriteLn` onto the grid~~ — arrived in 0.1.4, which lets a unit overload
  a built-in. The celled writers are `Write` and `WriteLn`, variadic as
  [RT-015] has them, and they shadow the built-ins for any program that says
  `uses graph` — which is the intent. `System.Write` and `System.WriteLn`
  reach the console, so nothing is lost by the shadowing; it only has to be
  named. `Print` and `PrintLn` were the placeholders, and are gone.
- ~~A per-canvas text grid~~ — arrived as the Window class: a window that
  scrolls its own celled contents is exactly what a Window is.
- **ViewPort scaling** — a texture can be presented at other than 1:1, which
  is sprite scaling for free. Sizes stay fixed at creation until wanted.
- **Paletted surfaces** — a ViewPort whose buffer holds indices, expanded
  through a table at `Present` into the texture. This is the half of Turbo
  Pascal's palette the section above gives up: fades, color cycling and an
  instant re-theme of a finished screen, none of which need a redraw. The
  expansion is a per-pixel run and so belongs in `graphffi.c`, but the cost
  lands on every primitive — `PutPixel` and the three C fills all write
  `0xFF000000 | color` today and would need an index path, `GetPixel` becomes
  ambiguous between index and color, and transparency needs a reserved entry.
  A fork in the surface model, and priced accordingly.
- **The rest of the drawing vocabulary** — `Bar`, `Circle`, `Ellipse`, `Arc`,
  `PieSlice`, `FloodFill`, `GetImage`/`PutImage`. The pen arrived with
  `Line`; these join it on the ViewPort, and the fill styles are the open
  question they bring.
- **CJK vertical writing**, as above.
- ~~`Blink`~~ — arrived early, because the clock it waited on was the
  language's own `clock ()` [RT-012] all along. The spelling is Turbo
  Pascal's with the flag moved to bit 24; blinking ink lives on its own
  texture that `Present` shows and hides at the VGA's 267ms cadence. What
  remained true until input arrived: the phase advances only when something
  presents. `ReadKey` and `KeyPressed` now present once a frame while they
  wait, so the idle `Print ('')` tick is retired — a program waiting on
  input blinks for free.

## Speed, and the options if it ever matters

Measured on the IDE demo, which paints two chart windows over a full text
screen:

| | interpreted | compiled |
| --- | --- | --- |
| the whole screen | **4.53 s** | **0.18 s** |

Interpreted, that divides into 0.84 s decoding the 630 KB glyph file, 1.46 s
for the text half, and 3.07 s for the two charts — the line drawing, where
every pixel is an interpreted `PutPixel`.

The curves cost more again, and for a reason worth knowing: `Arc` and
`Ellipse` call `Sin` and `Cos` per point, and each is a foreign call. A
five-slice pie chart of radius 150 draws in about half a second, which is
fine; forty full discs of radius 190 took thirteen. Replacing the
power-of-two loop that tested the line pattern with a lookup table took a
fifth off every line, arc and ellipse and cost nothing in clarity — the
remaining time is the trig, and the answer to that, if it is ever wanted, is
the same as everywhere else: draw the curve in C, or compile.

Compiled is 25× faster and instant, which is what `--compile` is for; the
cost is only ever felt while developing. **Accepted as it stands.** If it
ever needs addressing, in increasing order of what it spends:

- **A binary glyph cache.** The 0.84 s is pure decode of text that never
  changes: dump the decoded buffers once and reload them with one read.
  Costs nothing in design — no logic moves.
- **`alg_line` in `graphffi.c`.** Bresenham with the pattern and thickness,
  perhaps forty lines, collapsing the 3 s to nothing. The cost is the one
  the standing rule guards: line geometry is *logic*, and logic belongs in
  Algol-24 where it is readable and testable. `alg_trunc` is four lines and
  a primitive; a line renderer is neither.
- ~~A C blitter for whole runs~~ — arrived with `Bar`, which is the first
  figure to fill: 7.5 seconds interpreted for one 300 × 200 rectangle, free
  through `alg_fill_rect`. The line drawn is the one the rest of the file
  argues for — the unit decides the rectangle, the corners' order, the
  pattern, the color and whether the fill is empty; C receives a run.

## What was measured on the way here

- A unit-defined `WriteLn` **replaces** the built-in rather than overloading
  it — `WriteLn ('x')` then answers `Expected 2 arguments but got 1.` — which
  is why the text verbs are `OutText`/`OutTextXY` today and why the grid's
  `WriteLn` waits on a compiler change.
- Glyph blending is interpreter-priced at ~18ms per antialiased glyph; fine
  for labels, and what `--compile` is for otherwise. The hex decode that cost
  8 seconds per font load is already C (`graphffi.c`); the blit may follow it
  someday, the format knowledge and layout logic never do.
- macOS keeps its menu bar in borderless fullscreen, so a fullscreen window is
  the desktop's width but short of its height — `GetMaxX`/`GetMaxY` report
  the window that actually opened, and that is the general rule: state
  describes what is, not what was asked for.
