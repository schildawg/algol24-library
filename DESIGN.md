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

One window, composed from bottom to top:

```
   background                      the window's own surface
   canvases with negative Order    graphics under the text
   the text grid                   Order 0 -- celled text, the one grid
   canvases with positive Order    graphics over the text
```

- **The text grid** is the celled surface `WriteLn` will write to. There is
  exactly one and it sits at Order 0, which no canvas may claim. It is
  **logical** — 80 × 25 by default whatever the window, chosen by `TextMode`,
  and scaled to the window by the GPU at present time. A happy consequence:
  in a 4:3 window the cells display at 1:2.4, the authentic aspect of a text
  mode on a 4:3 tube.
- **A Canvas** is a class: a positioned, sized drawing surface with its own
  pixels, its own transparency, and a place in the stack. Every drawing verb
  is a method on it.

### The one rule

**On the grid, text behaves like text mode; off the grid, text is a graphic.**
Every interaction question resolves by asking which side of that line a thing
lives on. `ClrEol` erases the celled text and the pie chart under it shows
through, intact — erasing a graphic is a graphics act, on the canvas that
holds it.

### The two kinds of text

| | **celled** (the grid) | **free** (a canvas) |
| --- | --- | --- |
| placed by | cursor, in columns and rows | pixels, any X,Y |
| cursor, wrap, scroll | yes | no |
| erased by `ClrEol` / `ClrScr` | yes | no — text verbs ignore it |
| background color | yes, per cell; transparent by default | no — ink only |
| orientation | upright only | 0° / 90° / 180° / 270° |
| written by | `WriteLn` | `OutText`, `OutTextXY` |

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

## Canvas

Settled, all five by decision:

- **Every drawing verb is a Canvas method.** `Chart.Line (…)`,
  `Chart.OutTextXY (…)` — where a thing draws is unaskable, because the
  receiver says.
- **Coordinates are local.** (0,0) is the canvas's own top-left corner, which
  is what makes a drawing portable: move the canvas and the drawing moves,
  nothing redraws. It is also the clip — a canvas cannot scribble outside
  itself, which keeps the old `SetViewPort` promise without a clipping
  rectangle to maintain.
- **Translucency is 0..255**, property `Alpha`, default 255. A byte, because
  every color channel and every coverage value in the pipeline is already a
  byte, and no Real comparison wanted anywhere. Per-pixel transparency needs
  no dial at all: an unpainted pixel is transparent, which is the natural
  state of a fresh canvas.
- **`Order` is a nonzero signed integer** — negative under the text grid,
  positive over it — and `Order`, `X` and `Y` are settable after creation:
  reordering is bring-to-front, moving is dragging. Two canvases with the same
  Order stack by creation, earlier beneath later.
- **`CloseGraph` invalidates every canvas.** A canvas belongs to the window it
  was made in; reopening starts clean.

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

## Doors left open, deliberately

Nothing below is designed yet; nothing above blocks any of it.

- **The background as the root canvas.** The likely end state is that the
  window's own surface is simply a canvas at the bottom of the stack, and
  today's global `OutText` / `OutTextXY` either delegate to it or retire.
  Decided when the Canvas class lands.
- **`WriteLn` onto the grid** waits on the compiler growing overloading for
  built-ins. Until it lands, the celled writers are named **`Print` and
  `PrintLn`**, which `Write`/`WriteLn` absorb when the fix arrives; `OutText`
  carries free text throughout.
- **A per-canvas text grid** — a window that scrolls its own celled contents.
  The grid stays singular until something needs otherwise.
- **Canvas scaling** — a texture can be presented at other than 1:1, which is
  sprite scaling for free. `Width`/`Height` stay read-only until wanted.
- **CJK vertical writing**, as above.
- **`Blink`**, parked until input arrives. Turbo Pascal spelled it
  `TextColor (LightRed + Blink)` with `Blink = 128` — the CGA attribute
  byte's bit 7, the same bit BASIC's `COLOR 16`+ wrote. The spelling can
  survive here as bit 24 (`Blink := 16777216`), since 128 is a legitimate
  navy in 24-bit color; blinking cells would stamp into a fourth texture the
  GPU shows and hides by phase. What it waits on is a clock: something must
  present periodically, and the event loop that would naturally drive it
  belongs to the input assignment.

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
