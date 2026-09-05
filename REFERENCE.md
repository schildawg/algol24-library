# The Algol-24 Library Reference

Every routine and constant the library offers, alphabetically, after the pattern
of Turbo Pascal's own reference: what it does, how it is declared, what is worth
knowing about it, what else to look at, and a program that uses it.

Every example in this file has been run, and the output shown beneath it is what
it printed.

Each example assumes the unit is reachable — run from the directory holding the
`.a24` files, or from one beside them.

## Index

| | | |
| --- | --- | --- |
| [`Abs`](#abs) | function | `math` |
| [`Aliases`](#aliases) | convention | `graph` |
| [`Arc`](#arc) | procedure, alias | `graph` |
| [`ArcCoords`](#arccoords) | class | `graph` |
| [`ArcTan`](#arctan) | function | `math` |
| [`AssertNear`](#assertnear) | procedure | `testing` |
| [`Bar`](#bar) | procedure, alias | `graph` |
| [`Bar3D`](#bar3d) | procedure, alias | `graph` |
| [`Blink`](#blink) | constant | `graph` |
| [`CellWidth` `CellHeight`](#cellwidth) | functions | `graph` |
| [`Circle`](#circle) | procedure, alias | `graph` |
| [`Clear`](#clear) | method | `graph` |
| [`ClearViewPort`](#clearviewport) | procedure, alias | `graph` |
| [`CloseGraph`](#closegraph) | procedure | `graph` |
| [`ClrEol`](#clreol) | procedure | `graph` |
| [`ClrScr`](#clrscr) | procedure | `graph` |
| [`Black` … `White`](#colors) | constants | `graph` |
| [`Cos`](#cos) | function | `math` |
| [`DelLine` `InsLine`](#delline) | procedures | `graph` |
| [`DrawPoly`](#drawpoly) | procedure, alias | `graph` |
| [`Ellipse`](#ellipse) | procedure, alias | `graph` |
| [`Exp`](#exp) | function | `math` |
| [`FillEllipse`](#fillellipse) | procedure, alias | `graph` |
| [`FillPoly`](#fillpoly) | procedure, alias | `graph` |
| [`FillSettings` `LineSettings`](#fillsettings) | classes | `graph` |
| [`FillStyles`](#fillstyles) | constants | `graph` |
| [`FloodFill`](#floodfill) | procedure, alias | `graph` |
| [`Frac`](#frac) | function | `math` |
| [`GetArcCoords`](#getarccoords) | function, alias | `graph` |
| [`GetAspectRatio`](#getaspectratio) | function | `graph` |
| [`GetColor`](#getcolor) | function, alias | `graph` |
| [`GetFillPattern`](#getfillpattern) | function, alias | `graph` |
| [`GetFillSettings` `GetLineSettings`](#getfillsettings) | functions, aliases | `graph` |
| [`GetMaxX`](#getmaxx) | function | `graph` |
| [`GetMaxY`](#getmaxy) | function | `graph` |
| [`GetPixel`](#getpixel) | function, alias | `graph` |
| [`GetX`](#getx) | function, alias | `graph` |
| [`GetY`](#gety) | function, alias | `graph` |
| [`GotoXY`](#gotoxy) | procedure | `graph` |
| [`HighVideo`](#highvideo) | procedure | `graph` |
| [`Infinity`](#infinity) | constant | `math` |
| [`InitGraph`](#initgraph) | procedure | `graph` |
| [`InstallUserFont`](#installuserfont) | procedure | `graph` |
| [`Int`](#int) | function | `math` |
| [`IsInfinite`](#isinfinite) | function | `math` |
| [`IsNaN`](#isnan) | function | `math` |
| [`KeyPressed`](#keypressed) | function | `graph` |
| [`KeyUp` … `KeyClose`](#keys) | constants | `graph` |
| [`Line`](#line) | procedure, alias | `graph` |
| [`LineRel`](#linerel) | procedure, alias | `graph` |
| [`SolidLn` … `ThickWidth`](#linestyles) | constants | `graph` |
| [`LineTo`](#lineto) | procedure, alias | `graph` |
| [`Ln`](#ln) | function | `math` |
| [`LowVideo`](#lowvideo) | procedure | `graph` |
| [`Max`](#max) | function | `math` |
| [`Min`](#min) | function | `math` |
| [`MoveTo`](#moveto) | procedure, alias | `graph` |
| [`NaN`](#nan) | constant | `math` |
| [`NormVideo`](#normvideo) | procedure | `graph` |
| [`Odd`](#odd) | function | `math` |
| [`OutText`](#outtext) | procedure | `graph` |
| [`OutTextXY`](#outtextxy) | procedure | `graph` |
| [`PenRel`](#penrel) | procedure, alias | `graph` |
| [`PenTo`](#pento) | procedure, alias | `graph` |
| [`Pi`](#pi) | constant | `math` |
| [`PieSlice`](#pieslice) | procedure, alias | `graph` |
| [`PutPixel`](#putpixel) | procedure, alias | `graph` |
| [`Random`](#random) | function | `random` |
| [`RandomInteger`](#randominteger) | function | `random` |
| [`Randomize`](#randomize) | procedure | `random` |
| [`RandomReal`](#randomreal) | function | `random` |
| [`ReadKey`](#readkey) | function | `graph` |
| [`Rectangle`](#rectangle) | procedure, alias | `graph` |
| [`Round`](#round) | function | `math` |
| [`ScreenHeight`](#screenheight) | function | `graph` |
| [`ScreenWidth`](#screenwidth) | function | `graph` |
| [`Sector`](#sector) | procedure, alias | `graph` |
| [`SetBkColor` `GetBkColor`](#setbkcolor) | procedures, aliases | `graph` |
| [`SetBlinkRate`](#setblinkrate) | procedure | `graph` |
| [`SetColor`](#setcolor) | procedure, alias | `graph` |
| [`SetFillStyle`](#setfillstyle) | procedures, aliases | `graph` |
| [`SetLineStyle`](#setlinestyle) | procedure, alias | `graph` |
| [`SetSeed`](#setseed) | procedure | `random` |
| [`SetTextJustify`](#settextjustify) | procedure, alias | `graph` |
| [`SetTextStyle`](#settextstyle) | method | `graph` |
| [`Show`](#show) | procedure, alias | `graph` |
| [`Sin`](#sin) | function | `math` |
| [`Sqr`](#sqr) | function | `math` |
| [`Sqrt`](#sqrt) | function | `math` |
| [`TextBackground`](#textbackground) | procedure | `graph` |
| [`TextColor`](#textcolor) | procedure | `graph` |
| [`TextCols`](#textcols) | function | `graph` |
| [`LeftText` … `TopText`](#textjustify) | constants | `graph` |
| [`TextMode`](#textmode) | procedure | `graph` |
| [`TextRows`](#textrows) | function | `graph` |
| [`TextWidth` `TextHeight`](#textwidth) | functions | `graph` |
| [`Trunc`](#trunc) | function | `math` |
| [`ViewPort`](#viewport) | class | `graph` |
| [`WhereX`](#wherex) | function | `graph` |
| [`WhereY`](#wherey) | function | `graph` |
| [`Window`](#window) | class | `graph` |
| [`Write` `WriteLn`](#write) | procedures | `graph` |

---

## Abs

*function* — unit `math`

**Function**

Returns the absolute value of its argument.

**Declaration**

```algol24
function Abs (X);
```

**Remarks**

`X` is an Integer or a Double, and the result has the same type as `X`. That
follows from `0 - X` taking its type from its operand rather than from anything
this routine arranges, which is also why the function is written untyped.

Because an Integer is unbounded, the negation cannot overflow. `Abs` of −2¹²⁷
answers 2¹²⁷, where C's `abs(INT_MIN)` is undefined behavior.

Negative zero is normalized: `Abs (-0.0)` is `0.0`, not `-0.0`. The comparison
is `<=` rather than `<` for exactly this reason — `-0.0 < 0` is false, so `<`
would hand `-0.0` straight back.

Being untyped, the result cannot initialize a variable of a written type:
`var N : Integer := Abs (-5);` is refused, while `var N := Abs (-5);`,
`Abs (-5) + 1` and `Take (Abs (-5))` are all accepted.

Raises `Operands must be numbers.` for an argument the subtraction cannot take,
including a String and `nil`.

**See also**

[`Max`](#max), [`Min`](#min), [`Sqr`](#sqr)

**Example**

```algol24
uses math;

WriteLn (Abs (-42));
WriteLn (Abs (-2.5));
WriteLn (Abs (-170141183460469231731687303715884105728));
```

```console
42
2.5
170141183460469231731687303715884105728
```

---

## Aliases

*a convention* — unit `graph`

**Function**

Every surface method is also a free function taking the surface first.

**Declaration**

```algol24
GotoXY (W, 3, 2)        is      W.GotoXY (3, 2)
Line (V, 1, 1, 40, 20)  is      V.Line (1, 1, 40, 20)
```

**Remarks**

Verb-first is how a Turbo Pascal program reads, and this library's vocabulary
is Turbo Pascal's — so each of the fifty-eight surface methods has a
free-function twin. Each is a one-line delegate adding no behavior whatever,
and each has its own entry in this reference, marked *alias of* the method it
reaches.

They work on **arity**: `GotoXY (Col, Row)` is the screen and
`GotoXY (W, Col, Row)` is that window. Before 0.1.4 every such call warned at
its site; the compiler now settles a choice on arity when arity is enough,
silently.

Two are shaped around the language rather than declared twice:

⚠️ **[`Write`](#write) and `WriteLn` are one variadic each**, inspecting their
first value — a leading [`Window`](#window) is the surface written *to*, not a
value written. A variadic already matches every arity, so a second declaration
could only be ambiguous against it.

⚠️ **[`MoveTo`](#moveto) takes an untyped surface** and asks what it is. Two
declarations of one arity would put the choice back on the argument's *type*,
which a gradual type system settles at run time and warns about — correctly.

The screen-wide verbs are untouched: a bare `Write` still means the screen,
and `System.Write` still means the console.

**See also**

[`ViewPort`](#viewport), [`Window`](#window)

**Example**

See any alias entry — [`Line`](#line), [`GotoXY`](#gotoxy),
[`SetColor`](#setcolor) — or `examples/ide.a24`, which uses both spellings.

---

## Arc

*procedure, alias of `ViewPort.Arc`* — unit `graph`

**Function**

Draws a circular arc on a viewport.

**Declaration**

```algol24
procedure Arc (V : ViewPort, X : Integer, Y : Integer, StartAngle : Integer,
               EndAngle : Integer, Radius : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `Arc (V, …)` is `V.Arc (…)`.

Angles are **degrees**, zero at three o'clock, counting **counterclockwise** —
Turbo Pascal's convention, and the one `Arc (V, 100, 100, 0, 90, 50)` reads as:
a quarter turn upward.

⚠️ **Counterclockwise on a screen means Y gets smaller.** A screen's Y grows
downward, so the four cardinal angles land like this:

| angle | where | |
| --- | --- | --- |
| 0° | three o'clock | `X + R`, `Y` |
| 90° | twelve o'clock | `X`, **`Y − R`** |
| 180° | nine o'clock | `X − R`, `Y` |
| 270° | six o'clock | `X`, `Y + R` |

Getting that sign backwards mirrors every curve in the library, and nothing
but a picture would catch it — which is why the tests assert all four.

The sweep runs from `StartAngle` to `EndAngle` **the counterclockwise way**, so
`Arc (V, X, Y, 350, 10, R)` is the twenty degrees across three o'clock rather
than the three hundred and forty the other way round. Equal angles draw a
point; `0, 360` is the whole circle.

Drawn in the pen's [color](#setcolor), patterned by the current
[line style](#setlinestyle), and thickened **along the radius** — the only
direction that keeps a thick arc the same shape.

⚠️ **The pen is not moved.** An arc is a figure, not a journey, so a following
[`LineTo`](#lineto) still draws from wherever the pen was.

Raises `Arc needs a radius of zero or more.` and `CloseGraph has closed this
surface.`

**See also**

[`GetArcCoords`](#getarccoords), [`Line`](#line), [`SetColor`](#setcolor),
[`SetLineStyle`](#setlinestyle), [`ViewPort`](#viewport)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'arc', False);

var V := ViewPort (100, 60, 300, 300, 1);

// A full circle, then a quarter of one drawn thick over it.
SetColor (V, LightGray);
Arc (V, 150, 150, 0, 360, 100);

SetColor (V, Yellow);
SetLineStyle (V, SolidLn, 0, ThickWidth);
Arc (V, 150, 150, 0, 90, 100);
Show (V);

// Ninety degrees is twelve o'clock: a SMALLER Y than the center.
System.WriteLn (V.Buf.GetInt ((49 * 300 + 149) * 4) <> 0);
System.WriteLn (V.Buf.GetInt ((149 * 300 + 149) * 4) = 0);

CloseGraph ();
```

```console
true
true
```

---

## ArcCoords

*class* — unit `graph`

**Function**

Where the last arc on a surface was centered, began and ended.

**Declaration**

```algol24
constructor ArcCoords (X : Integer, Y : Integer, XStart : Integer,
                       YStart : Integer, XEnd : Integer, YEnd : Integer);

// readable
X, Y : Integer                  the center the arc was drawn about
XStart, YStart : Integer        where the curve began
XEnd, YEnd : Integer            where it ended
```

**Remarks**

Turbo Pascal's `ArcCoordsType`, with its six field names kept whole so a
program written to them still reads.

⚠️ **It is answered, not filled in.** Turbo Pascal's `GetArcCoords` took a
`var` record and wrote into it; [`GetArcCoords`](#getarccoords) here returns
one of these. That is the same record with the awkward half of Pascal's
calling convention left out — there is nothing to declare before the call,
and nothing to leave uninitialized if the call is skipped.

⚠️ **The six are a snapshot.** A later arc makes a new `ArcCoords` rather than
changing an existing one, so a value held on to still describes the figure it
came from.

Constructing one directly is legal and occasionally useful — for stashing the
geometry of a figure drawn some other way — but the usual source is
`GetArcCoords`.

**See also**

[`Arc`](#arc), [`GetArcCoords`](#getarccoords), [`ViewPort`](#viewport)

**Example**

See [`GetArcCoords`](#getarccoords).

---

## ArcTan

*function* — unit `math`

**Function**

Returns the arctangent of its argument, in radians.

**Declaration**

```algol24
function ArcTan (X : Double) : Double;
```

**Remarks**

The result lies between −π/2 and π/2. `ArcTan` is an odd function, so negating
the argument negates the result.

It is a foreign declaration onto libm's `atan`, which is the whole of the
implementation.

There is no two-argument form, so a full-circle angle is not available from this
routine alone.

**See also**

[`Cos`](#cos), [`Pi`](#pi), [`Sin`](#sin)

**Example**

Four times the arctangent of one is π — the classic way of arriving at it.

```algol24
uses math;

WriteLn (ArcTan (1.0) * 4);
WriteLn (ArcTan (0.0));
```

```console
3.141592653589793
0.0
```

---

## AssertNear

*procedure* — unit `testing`

**Function**

Fails a test unless two Doubles agree to within a tolerance.

**Declaration**

```algol24
procedure AssertNear (Expected : Double, Actual : Double, Tolerance : Double);
```

**Remarks**

For a floating-point result whose exact bits are not guaranteed. IEEE 754
requires the arithmetic operations and [`Sqrt`](#sqrt) to be correctly rounded,
so those can and should be asserted exactly with `AssertEqual`; it requires
nothing of [`Sin`](#sin), [`Cos`](#cos), [`Exp`](#exp), [`Ln`](#ln) and
[`ArcTan`](#arctan), whose last bit may differ between one platform's libm and
another's. Those are what this is for.

The tolerance is **inclusive**, and the comparison is symmetric in the first two
arguments. A tolerance of zero asks for exact agreement, which is a clearer way
of saying so than `AssertEqual` when the values are Doubles.

⚠️ **`NaN` always fails, in either position.** Every comparison against `NaN` is
false, so a test written with `>` would let one through — a routine that answered
`NaN` did not land near anything. The infinities fail too, unless the tolerance
is itself infinite.

⚠️ **Only usable inside a `test` block, under `algc --test`.** It reports through
the built-in `Fail`, which does not exist in an ordinary program: calling
`AssertNear` outside a test answers `Undefined variable 'Fail'.`

A failure reads `Failed.  Expected 1.22 to within 0.001 but got 1.5.` — `Fail`
supplies the first word.

The unit deliberately depends on nothing, `math` included, so that `math`'s own
tests may use it without the two importing one another.

**See also**

[`Cos`](#cos), [`Exp`](#exp), [`Ln`](#ln), [`Sin`](#sin), [`Sqrt`](#sqrt)

**Example**

```algol24
uses testing;

test 'A measurement lands where it should';
begin
    AssertNear (1.22, 1.2201, 0.001);
    AssertNear (3.14159, 3.14160, 0.001);
end
```

```console
[INFO] Test: A measurement lands where it should .................... [ PASS ]
```

---

## Bar

*procedure, alias of `ViewPort.Bar`* — unit `graph`

**Function**

Fills a rectangle with the current fill pattern and color.

**Declaration**

```algol24
procedure Bar (V : ViewPort, X1 : Integer, Y1 : Integer, X2 : Integer, Y2 : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `Bar (V, …)` is `V.Bar (…)`.

The corners may be given either way round.

⚠️ **No outline is drawn.** Turbo Pascal's `Bar` is the fill alone, so a
bordered bar is a `Bar` and a [`Rectangle`](#rectangle) over it — which is
also how it gets two different colors, the fill pen and the drawing pen being
separate. See [`SetFillStyle`](#setfillstyle).

The pattern is tiled from the **surface's** origin rather than the bar's, so
bars side by side share one weave. `EmptyFill` paints nothing at all, leaving
what is beneath showing through.

The pen is not moved, a bar being a figure rather than a journey.

⚠️ **This one is drawn by C**, uniquely among the figures so far. A
300 × 200 bar is sixty thousand pixels and the interpreted loop measured 7.5
seconds; the same fill through `alg_fill_rect` is free — fifty full-surface
bars cost what one does. The unit still decides everything that is a
decision: which rectangle, which way round its corners came, which pattern,
which color, and whether the fill is empty at all. What went to C is the run.

Raises `CloseGraph has closed this surface.` once the window has gone.

**See also**

[`FillStyles`](#fillstyles), [`Rectangle`](#rectangle), [`SetFillStyle`](#setfillstyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'bar', False);

var V := ViewPort (60, 60, 400, 260, 1);

// A chart's bars, each filled and then outlined -- two colors, because the
// fill pen and the drawing pen are separate.
var Heights := [180, 120, 210, 90, 150];

SetColor (V, White);

for var I := 0; I < 5; I := I + 1 do
begin
    var Left := 30 + I * 70;

    SetFillStyle (V, [SolidFill, HatchFill, XHatchFill,
                      InterleaveFill, SlashFill][I], LightGreen);

    Bar (V, Left, 240 - Heights[I], Left + 50, 240);
    Rectangle (V, Left, 240 - Heights[I], Left + 50, 240);
end

Show (V);

System.WriteLn (GetFillColor (V) = LightGreen);
System.WriteLn (GetColor (V) = White);

CloseGraph ();
```

```console
true
true
```

---

## Bar3D

*procedure, alias of `ViewPort.Bar3D`* — unit `graph`

**Function**

Fills a bar and draws it in three dimensions.

**Declaration**

```algol24
procedure Bar3D (V : ViewPort, X1 : Integer, Y1 : Integer, X2 : Integer,
                 Y2 : Integer, Depth : Integer, Top : Boolean);
```

**Remarks**

An [alias](#aliases) of the method: `Bar3D (V, …)` is `V.Bar3D (…)`.

The front face is filled with the current fill pattern and outlined with the
current line style and pen — [`Bar`](#bar) and [`Rectangle`](#rectangle) in one
call, which is what Turbo Pascal's `Bar3D` was. `Depth` then projects it up and
to the right at forty-five degrees, drawing the right face and, when `Top` is
true, the top one.

⚠️ **`Top` exists for stacking.** A column of bars wants the cap on the topmost
alone; drawn with `Top` false, the ones beneath get their right face but no lid
cutting across the stack. The right face is always drawn — it is what makes the
bar look solid.

⚠️ **Only the front face carries the pattern.** The other faces are outlines,
which is what keeps a patterned bar legible in three dimensions.

Turbo Pascal suggested a `Depth` of a quarter the bar's width, which still
reads well. A depth of zero is exactly a `Bar` with a `Rectangle` over it —
there is a test asserting the two are pixel-identical.

The pen is not moved: a figure is not a journey, which is the rule
[`Arc`](#arc), [`Circle`](#circle), `Bar` and [`Rectangle`](#rectangle) keep as
well. Only [`Line`](#line), [`LineTo`](#lineto) and [`LineRel`](#linerel) move
it, because moving it is what they are for.

Raises `Bar3D needs a depth of zero or more.` and `CloseGraph has closed this
surface.`

**See also**

[`Bar`](#bar), [`Rectangle`](#rectangle), [`SetFillStyle`](#setfillstyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'bar3d', False);

var V := ViewPort (60, 60, 420, 280, 1);

// A row of solid bars, each capped -- the depth is a quarter of the width,
// which is what Turbo Pascal suggested and still reads well.
var Heights := [180, 120, 210, 90];

SetColor (V, White);

for var I := 0; I < 4; I := I + 1 do
begin
    var Left := 40 + I * 90;

    SetFillStyle (V, [SolidFill, HatchFill, SlashFill, XHatchFill][I], LightCyan);
    Bar3D (V, Left, 250 - Heights[I], Left + 60, 250, 15, True);
end

// And a stack: only the topmost is capped, so no lid cuts across it.
SetFillStyle (V, SolidFill, LightRed);
Bar3D (V, 380, 190, 400, 250, 12, False);
Bar3D (V, 380, 130, 400, 190, 12, True);

Show (V);

System.WriteLn (GetX (V));
System.WriteLn (GetColor (V) = White);

CloseGraph ();
```

```console
1
true
```

---

## Blink

*constant* — unit `graph`

**Function**

The blink flag, added onto a [`TextColor`](#textcolor).

**Declaration**

```algol24
const Blink := 16777216;
```

**Remarks**

`TextColor (LightRed + Blink)` makes the ink blink — 267 milliseconds on,
267 off by default, the VGA's own cadence of sixteen frames each way;
[`SetBlinkRate`](#setblinkrate) chooses another. The flag
is per-color, as Turbo Pascal's was: the next `TextColor` without it writes
steady text, and a steady stamp over a blinking cell ends its blinking.
[`NormVideo`](#normvideo) and [`TextMode`](#textmode) end it too.

Only the letters vanish; the cell's background stays, exactly as the
adapter had it.

Turbo Pascal's value was 128 — the CGA attribute byte's bit 7, the same bit
BASIC's `COLOR 16` and up wrote. Here 128 is a legitimate navy, so the flag
rides bit 24 and the spelling survives unchanged.

The phase advances whenever something presents, on the language's own
`clock ()`. A program drawing or sitting in a loop that ticks `Write ('')`
blinks; one that draws once and halts shows whichever phase it landed in.

**See also**

[`Colors`](#colors), [`NormVideo`](#normvideo), [`TextColor`](#textcolor)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'blink', False);

TextColor (LightRed + Blink);
Write ('this text blinks');

NormVideo ();
WriteLn (' and this does not');

System.WriteLn (WhereY ());

CloseGraph ();
```

```console
2
```

---

## CellWidth

*functions* — unit `graph`

**Function**

The size of one text cell, in the logical space surfaces live in.

**Declaration**

```algol24
function CellWidth () : Integer;
function CellHeight () : Integer;
```

**Remarks**

Cells are half as wide as they are tall — 16 × 32 for the shipped font — and
a wide (CJK) glyph takes two of them.

⚠️ **Both surface kinds share one logical space**, and these are its ruler.
A [`Window`](#window) is placed in cells and a [`ViewPort`](#viewport) in
that space's pixels, so cell `Col, Row` begins at
`(Col - 1) * CellWidth (), (Row - 1) * CellHeight ()` — which is exactly what
[`Window.PixelLeft`](#window) and `PixelTop` answer. A viewport placed there
stays there at **every** window size, because the whole space scales to the
physical window as one.

The physical window's own pixels are a different ruler:
[`GetMaxX`](#getmaxx) measures those, and the background that the global
[`OutTextXY`](#outtextxy) draws on lives in them.

An `InstallUserFont` of a different cell size changes both answers.

**See also**

[`GetMaxX`](#getmaxx), [`ViewPort`](#viewport), [`Window`](#window)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'cells', False);

var W := Window (4, 5, 38, 20, 1);

// A chart one cell inside the window's frame, in the same logical space.
var Chart := ViewPort (W.PixelLeft () + CellWidth (),
                       W.PixelTop () + CellHeight (),
                       (W.Cols - 2) * CellWidth (),
                       (W.Rows - 2) * CellHeight (), 2);

System.WriteLn (CellWidth ());
System.WriteLn (CellHeight ());
System.WriteLn (Chart.X);
System.WriteLn (Chart.Y);

CloseGraph ();
```

```console
16
32
64
160
```

---

## Circle

*procedure, alias of `ViewPort.Circle`* — unit `graph`

**Function**

Draws a circle on a viewport.

**Declaration**

```algol24
procedure Circle (V : ViewPort, X : Integer, Y : Integer, Radius : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `Circle (V, …)` is `V.Circle (…)`.

[`Arc`](#arc) swept the whole way round, and everything said there holds — the
pen's [color](#setcolor), the [line style](#setlinestyle)'s pattern, thickness
spreading along the radius, and the pen left where it was.

The **outline** only. A filled disc waits on the figures that take a fill;
[`SetFillStyle`](#setfillstyle) is the state they will use.

Raises `Circle needs a radius of zero or more.` and `CloseGraph has closed
this surface.`

**See also**

[`Arc`](#arc), [`Ellipse`](#ellipse), [`GetArcCoords`](#getarccoords),
[`GetAspectRatio`](#getaspectratio), [`SetFillStyle`](#setfillstyle),
[`SetLineStyle`](#setlinestyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'circle', False);

var V := ViewPort (100, 60, 300, 300, 1);

SetColor (V, LightCyan);
Circle (V, 150, 150, 120);

SetColor (V, Yellow);
SetLineStyle (V, DottedLn, 0, ThickWidth);
Circle (V, 150, 150, 60);
Show (V);

// Three o'clock on the outer circle is lit; the middle is hollow.
System.WriteLn (V.Buf.GetInt ((149 * 300 + 269) * 4) <> 0);
System.WriteLn (V.Buf.GetInt ((149 * 300 + 149) * 4) = 0);

CloseGraph ();
```

```console
true
true
```

---

## Clear

*method of [`Window`](#window) and [`ViewPort`](#viewport)* — unit `graph`

**Function**

Clears a surface, whichever kind it is.

**Declaration**

```algol24
procedure Clear ();     // on a Window, and on a ViewPort
```

**Remarks**

The one clearing verb both kinds of surface answer to. Turbo Pascal had two
unrelated names for it — `ClrScr` in Crt and `ClearViewPort` in Graph — because
text and pixels were different worlds there. They are one world here, so the
verb is one verb, and the Turbo Pascal names are kept as
[aliases](#aliases): [`ClrScr (W)`](#clrscr) and
[`ClearViewPort (V)`](#clearviewport).

⚠️ **What a surface is cleared *to* differs, and follows the one rule.** A
grid is text, so its cells take the current
[`TextBackground`](#textbackground); a canvas is a graphic, so it goes back to
the [`Transparent`](#colors) it was born as and whatever lies beneath shows
through again.

That difference is the point rather than an inconsistency: painting a viewport
black would hide what is under it, which is the opposite of clearing it.
Filling one with a color is [`Bar`](#bar) over the whole of it.

The cursor, or the pen and the free-text position, go home to `1, 1`.

⚠️ **The settings are not touched.** [`TextColor`](#textcolor) and
[`TextBackground`](#textbackground) on a window; [`SetColor`](#setcolor),
[`SetLineStyle`](#setlinestyle) and [`SetFillStyle`](#setfillstyle) on a
viewport. This clears the surface, not what you drew it with — a redraw after
a clear should not have to say everything over again.

Every other surface is left alone, as ever.

Raises `CloseGraph has closed this surface.` once the window has gone.

**See also**

[`Bar`](#bar), [`ClearViewPort`](#clearviewport), [`ClrEol`](#clreol),
[`ClrScr`](#clrscr), [`ViewPort`](#viewport), [`Window`](#window)

**Example**

See [`ClearViewPort`](#clearviewport).

---

## ClearViewPort

*procedure, alias of `ViewPort.Clear`* — unit `graph`

**Function**

Clears a viewport back to transparent.

**Declaration**

```algol24
procedure ClearViewPort (V : ViewPort);
```

**Remarks**

An [alias](#aliases) of the method: `ClearViewPort (V)` is `V.Clear ()`.
Turbo Pascal's name for it, kept; [`Clear`](#clear) is the verb.

⚠️ **It erases rather than paints.** The pixels go back to
[`Transparent`](#colors), so what lies beneath the viewport shows through
again. There is no color to pass, because a cleared surface and a surface
painted a color are different things — [`Bar`](#bar) over the whole viewport
is the second one.

Turbo Pascal cleared to the background color, having no transparency to go
back to. A stack of surfaces does, and transparent is what a fresh viewport
already is, so this returns one to how it looked when made.

The pen and the free-text position go home to `1, 1`, as a window's cursor
does under [`ClrScr`](#clrscr). The pen's color, the line style and the fill
style are left alone.

Raises `CloseGraph has closed this surface.` once the window has gone.

**See also**

[`Bar`](#bar), [`Clear`](#clear), [`ClrScr`](#clrscr),
[`ViewPort`](#viewport)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'clear', False);

var V := ViewPort (50, 50, 120, 80, 1);

SetFillStyle (V, SolidFill, Red);
Bar (V, 1, 1, 120, 80);
PenTo (V, 40, 60);

System.WriteLn ('painted:  ', GetPixel (V, 60, 40) = Red);

ClearViewPort (V);

// Back to transparent, not to a color -- so whatever lies beneath a viewport
// shows through it again.
System.WriteLn ('cleared:  ', GetPixel (V, 60, 40) = Transparent);

// The pen goes home, as a window's cursor does; what the pen was set to does
// not change.
System.WriteLn ('pen home: ', GetX (V), ' ', GetY (V));
System.WriteLn ('fill kept: ', GetFillStyle (V) = SolidFill);

CloseGraph ();
```

```console
painted:  true
cleared:  true
pen home: 1 1
fill kept: true
```

---

## CloseGraph

*procedure* — unit `graph`

**Function**

Closes the graphics window.

**Declaration**

```algol24
procedure CloseGraph ();
```

**Remarks**

Total: closing when nothing is open does nothing, so a handler or a test may
call it unconditionally to reach a known state. The window may be reopened
afterward with [`InitGraph`](#initgraph).

Only the window and its renderer are released. SDL's video subsystem stays up
for the life of the process, which is why [`ScreenWidth`](#screenwidth) still
answers after a close.

**See also**

[`GetMaxX`](#getmaxx), [`InitGraph`](#initgraph)

**Example**

```algol24
uses graph;

CloseGraph ();                       // nothing open: does nothing

InitGraph (320, 200, 'brief', False);
CloseGraph ();
CloseGraph ();                       // already closed: does nothing

System.WriteLn ('closed without complaint');
```

```console
closed without complaint
```

---

## ClrEol

*procedure* — unit `graph`

**Function**

Clears the grid from the cursor to the end of its row.

**Declaration**

```algol24
procedure ClrEol ();            // the screen
procedure ClrEol (W : Window);  // that window
```

**Remarks**

`ClrEol (W)` is an [alias](#aliases) of `W.ClrEol`, clearing that window's
row instead of the screen's.

The cleared cells take the current [`TextBackground`](#textbackground), and
the cursor does not move.

⚠️ **Only the grid is touched.** A graphic beneath the cleared cells — a
chart a window was sitting on — shows through, intact. Erasing a graphic is a
graphics act; this is a text verb, and text verbs act on text. That is the
design's one rule, and this routine is where it is most visible.

Raises `Graph is not open.` without a window.

**See also**

[`ClrScr`](#clrscr), [`Write`](#write), [`TextBackground`](#textbackground)

**Example**

```algol24
uses graph;

InitGraph (320, 200, 'eol', False);

Write ('doomed text');
GotoXY (1, 1);
ClrEol ();

System.WriteLn (WhereX ());

CloseGraph ();
```

```console
1
```

---

## ClrScr

*procedure* — unit `graph`

**Function**

Clears the whole grid and homes the cursor.

**Declaration**

```algol24
procedure ClrScr ();            // the screen
procedure ClrScr (W : Window);  // that window
```

**Remarks**

`ClrScr (W)` is an [alias](#aliases) of `W.Clear`, clearing that window
alone. [`Clear`](#clear) is the verb both kinds of surface answer to;
`ClrScr` is Turbo Pascal's name for it on a grid, and
[`ClearViewPort`](#clearviewport) is Turbo Pascal's name for it on a canvas.

The bare `ClrScr ()` is the base console layer — the root grid at Order 0 —
and clears nothing else in the stack.

Every cell takes the current [`TextBackground`](#textbackground) —
`TextBackground (Blue); ClrScr;` is how a classic screen paints its field in
two lines. With the background `Transparent` the grid becomes wholly
invisible and everything beneath shows.

Graphics are not touched, on the same rule as [`ClrEol`](#clreol).

Raises `Graph is not open.` without a window.

**See also**

[`Clear`](#clear), [`ClearViewPort`](#clearviewport), [`ClrEol`](#clreol),
[`TextBackground`](#textbackground)

**Example**

```algol24
uses graph;

InitGraph (320, 200, 'scr', False);

GotoXY (5, 3);
TextBackground (Blue);
ClrScr ();

System.WriteLn (WhereX ());
System.WriteLn (WhereY ());

CloseGraph ();
```

```console
1
1
```

---

## Colors

*constants* — unit `graph`

**Function**

The sixteen colors of a CGA text screen, and `Transparent`.

**Declaration**

```algol24
const Black        := 0;          const DarkGray     := 5592405;
const Blue         := 170;        const LightBlue    := 5592575;
const Green        := 43520;      const LightGreen   := 5635925;
const Cyan         := 43690;      const LightCyan    := 5636095;
const Red          := 11141120;   const LightRed     := 16733525;
const Magenta      := 11141290;   const LightMagenta := 16733695;
const Brown        := 11162880;   const Yellow       := 16777045;
const LightGray    := 11184810;   const White        := 16777215;

const Transparent  := -1;
```

**Remarks**

Named 24-bit values, so `TextColor (Yellow)` reads like 1989 while being RGB
underneath — and any other RGB Integer is just as welcome wherever these are.
The names and shades are the classic CGA palette, `Brown` and all.

`Transparent` is not a color but the absence of one, which needs its own
spelling because `0` is a legitimate black. Only
[`TextBackground`](#textbackground) accepts it; as an ink it raises.

**See also**

[`TextBackground`](#textbackground), [`TextColor`](#textcolor)

**Example**

```algol24
uses graph;

System.WriteLn (White);
System.WriteLn (Blue);
System.WriteLn (Transparent);
```

```console
16777215
170
-1
```

---

## Cos

*function* — unit `math`

**Function**

Returns the cosine of an angle given in radians.

**Declaration**

```algol24
function Cos (X : Double) : Double;
```

**Remarks**

`X` is in **radians**, not degrees. Multiply a degree measure by `Pi / 180`.

The result is exact at the angles whose cosine is a whole number, but a quarter
turn is not one of them: `Cos (Pi / 2)` answers `6.123233995736766E-17` rather
than zero, because `Pi / 2` is itself the nearest Double to a quarter turn and
not the quarter turn. Compare against a tolerance rather than against zero.

A foreign declaration onto libm's `cos`.

**See also**

[`ArcTan`](#arctan), [`Pi`](#pi), [`Sin`](#sin)

**Example**

```algol24
uses math;

for var Deg := 0; Deg <= 180; Deg := Deg + 90 do
    WriteLn (Str (Deg) + ' deg -> ' + Str (Cos (Deg * Pi / 180)));
```

```console
0 deg -> 1.0
90 deg -> 6.123233995736766E-17
180 deg -> -1.0
```

---

## DelLine

*procedures* — unit `graph`

**Function**

Deletes the cursor's row, or inserts a blank one.

**Declaration**

```algol24
procedure DelLine ();            // the screen
procedure DelLine (W : Window);  // that window

procedure InsLine ();            // the screen
procedure InsLine (W : Window);  // that window
```

**Remarks**

The window forms are [aliases](#aliases) of `W.DelLine` and `W.InsLine`. The
bare forms are the **base console layer** — the root grid at Order 0 — and
touch nothing else in the stack.

`DelLine` removes the row the cursor is on; every row below it moves **up**
one, and the last row is left blank. `InsLine` opens a blank row at the
cursor; every row below moves **down** one.

⚠️ **They act within the surface's own walls**, from the cursor's row to that
surface's last — a window's rows, never the screen's. Graphics are untouched,
on the same rule as [`ClrEol`](#clreol).

⚠️ **The cursor does not move**, in column or row. That is what makes
`DelLine` twice delete two rows rather than the same one twice, and what lets
`InsLine` and `DelLine` undo each other.

The blank row takes the current [`TextBackground`](#textbackground) — the one
in force when the verb runs, not the one the rest of the surface was cleared
to, so changing the background between a clear and a `DelLine` shows.

⚠️ **What `InsLine` pushes off the bottom is gone.** Nothing scrolls into
being kept, here or in Turbo Pascal: a window holds its rows and no more. So
the two are inverses only while nothing falls off the end.

Raises `CloseGraph has closed this surface.` on a window whose graph has gone,
and `Graph is not open.` for the bare forms without a window.

**See also**

[`Clear`](#clear), [`ClrEol`](#clreol), [`ClrScr`](#clrscr),
[`TextBackground`](#textbackground), [`Window`](#window)

**Example**

```algol24
uses graph;

// A Window holds pixels, not characters, so telling a blank row from a
// written one means asking whether any of its pixels carry ink.
function Written (W : Window, Row : Integer) : Boolean;
begin
    for var Y := (Row - 1) * CellHeight (); Y < Row * CellHeight (); Y := Y + 1 do
        for var X := 0; X < W.PxW; X := X + 1 do
            if W.Buf.GetInt ((Y * W.PxW + X) * 4) <> Blue - 16777216 then
                Exit True;

    Exit False;
end

InitGraph (640, 480, 'lines', False);

var W := Window (1, 1, 20, 4, 1);

TextBackground (W, Blue);
ClrScr (W);
Write (W, 'alpha');
GotoXY (W, 1, 2);
Write (W, 'beta');
GotoXY (W, 1, 3);
Write (W, 'gamma');

System.WriteLn ('written: ', Written (W, 1), ' ', Written (W, 2), ' ',
                Written (W, 3), ' ', Written (W, 4));

// Delete the first row: the rest come up, and the last goes blank.
GotoXY (W, 1, 1);
DelLine (W);

System.WriteLn ('deleted: ', Written (W, 1), ' ', Written (W, 2), ' ',
                Written (W, 3), ' ', Written (W, 4));

// Insert one back: the rest go down again. The cursor never moved, which is
// what lets the two undo each other.
InsLine (W);

System.WriteLn ('inserted:', Written (W, 1), ' ', Written (W, 2), ' ',
                Written (W, 3), ' ', Written (W, 4));
System.WriteLn ('cursor at ', WhereX (W), ', ', WhereY (W));

CloseGraph ();
```

```console
written: true true true false
deleted: true true false false
inserted:false true true false
cursor at 1, 1
```

---

## DrawPoly

*procedure, alias of `ViewPort.DrawPoly`* — unit `graph`

**Function**

Draws the lines joining a list of points.

**Declaration**

```algol24
procedure DrawPoly (V : ViewPort, Points : List);
```

**Remarks**

An [alias](#aliases) of the method: `DrawPoly (V, …)` is `V.DrawPoly (…)`.

`Points` is a List of two-number Lists:

```algol24
DrawPoly (V, [[10, 10], [80, 10], [45, 70]]);
```

Turbo Pascal took a pointer and a count, because Pascal's array had no length
of its own. The language has a List, so it is given one.

⚠️ **The figure is not closed.** Three points draw *two* lines; the way to
close a triangle is to repeat its first point as a fourth. That is Turbo
Pascal's rule and it is kept because it is the more general of the two — an
open polyline is a thing worth drawing, and a closing segment nobody asked for
cannot be taken back. `FillPoly`, when it arrives, will close of its own
accord, a filled shape having no open reading.

Two points draw exactly a [`Line`](#line) — there is a test asserting the two
paint the same pixels.

⚠️ **Every point is checked before any line is drawn**, so a malformed list
leaves the surface untouched rather than a figure broken off where the fault
was.

Drawn with the pen's [color](#setcolor) and the current
[line style](#setlinestyle), segment by segment. The pen is not moved.

Raises `DrawPoly wants at least two points.`, `DrawPoly wants points of two
numbers.`, and `CloseGraph has closed this surface.`

**See also**

[`FillPoly`](#fillpoly), [`Line`](#line), [`Rectangle`](#rectangle), [`SetLineStyle`](#setlinestyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'poly', False);

var V := ViewPort (60, 60, 400, 300, 1);

// A closed pentagon: the first point repeated as the last is what closes it.
SetColor (V, LightCyan);
DrawPoly (V, [[200, 30], [340, 130], [285, 270], [115, 270], [60, 130],
              [200, 30]]);

// And an open path, which is what leaving the repeat off is for.
SetColor (V, Yellow);
SetLineStyle (V, DashedLn, 0, ThickWidth);
DrawPoly (V, [[80, 200], [140, 150], [200, 210], [260, 140], [320, 190]]);
Show (V);

System.WriteLn (GetX (V));
System.WriteLn (GetColor (V) = Yellow);

CloseGraph ();
```

```console
1
true
```

---

## Ellipse

*procedure, alias of `ViewPort.Ellipse`* — unit `graph`

**Function**

Draws an elliptical arc on a viewport.

**Declaration**

```algol24
procedure Ellipse (V : ViewPort, X : Integer, Y : Integer, StartAngle : Integer,
                   EndAngle : Integer, XRadius : Integer, YRadius : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `Ellipse (V, …)` is `V.Ellipse (…)`.

⚠️ **Turbo Pascal's `Ellipse` takes angles**, so it is an elliptical *arc*: a
whole ellipse is the sweep `0, 360`, and anything less is a piece of one. The
angles mean what [`Arc`](#arc)'s mean — degrees, zero at three o'clock,
counterclockwise, which on a screen puts ninety degrees *above* the center.

With equal radii it is exactly [`Circle`](#circle): there is a test asserting
the two paint the same pixels, which is the strongest statement available that
they compute one curve.

⚠️ **The angle is the parametric one, not a bearing.** On a wide ellipse the
point at forty-five degrees is not forty-five degrees from the center as a
protractor would have it — it is where the circle's forty-five lands once the
circle is stretched. That is what Turbo Pascal drew, and it is what makes
`Ellipse (V, X, Y, 0, 90, A, B)` the quadrant a reader expects; but it is
worth knowing before computing an angle from a slope.

Drawn in the pen's [color](#setcolor), patterned by the
[line style](#setlinestyle), and thickened by drawing the neighboring
ellipses — which keeps a thick ellipse an ellipse rather than a ring of
uneven width. The pen is not moved.

The outline only; a filled one waits on `FillEllipse`.

Raises `Ellipse needs radii of zero or more.` and `CloseGraph has closed this
surface.`

**See also**

[`Arc`](#arc), [`Circle`](#circle), [`GetArcCoords`](#getarccoords),
[`SetLineStyle`](#setlinestyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'ellipse', False);

var V := ViewPort (60, 60, 400, 300, 1);

// A whole ellipse, and a quarter of a wider one drawn thick over it.
SetColor (V, LightCyan);
Ellipse (V, 200, 150, 0, 360, 160, 90);

SetColor (V, Yellow);
SetLineStyle (V, SolidLn, 0, ThickWidth);
Ellipse (V, 200, 150, 0, 90, 120, 60);
Show (V);

// Each radius governs its own axis: 160 across, 90 down.
System.WriteLn (V.Buf.GetInt ((149 * 400 + 359) * 4) <> 0);
System.WriteLn (V.Buf.GetInt ((59 * 400 + 199) * 4) <> 0);

CloseGraph ();
```

```console
true
true
```

---

## Exp

*function* — unit `math`

**Function**

Returns e raised to the power of its argument.

**Declaration**

```algol24
function Exp (X : Double) : Double;
```

**Remarks**

`Exp (1.0)` is e itself, `2.718281828459045`.

`Ln` undoes `Exp` exactly — `Ln (Exp (X))` answers `X` — but the reverse round
trip does not always hold: `Exp (Ln (3.0))` is `3.0000000000000004`. That is
ordinary floating-point rounding rather than anything either routine does.

A foreign declaration onto libm's `exp`.

**See also**

[`Ln`](#ln), [`Sqrt`](#sqrt)

**Example**

```algol24
uses math;

WriteLn (Exp (0.0));
WriteLn (Exp (1.0));
```

```console
1.0
2.718281828459045
```

---

## FillEllipse

*procedure, alias of `ViewPort.FillEllipse`* — unit `graph`

**Function**

Fills and outlines a whole ellipse on a viewport.

**Declaration**

```algol24
procedure FillEllipse (V : ViewPort, X : Integer, Y : Integer,
                       XRadius : Integer, YRadius : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `FillEllipse (V, …)` is
`V.FillEllipse (…)`.

[`Sector`](#sector) swept the whole way round, exactly as [`Circle`](#circle)
is [`Arc`](#arc) — filled with the current fill pattern and color, then
outlined with the pen and the current [line style](#setlinestyle). No radii are
drawn: a whole turn's two coincide.

It takes no angles, because Turbo Pascal's did not. A wedge is
[`Sector`](#sector)'s business.

⚠️ **A filled circle is this with equal radii.** Turbo Pascal has no
`FillCircle`, and neither does this — `FillEllipse (V, X, Y, R, R)` is it, and
so is `PieSlice (V, X, Y, 0, 360, R)`. There are tests asserting all three
agree pixel for pixel, which is the same statement `Circle` and `Ellipse` make
about their outlines.

With `EmptyFill` it is outlined and hollow. The fill and the outline are
separate pens, and the pen is not moved.

Raises `FillEllipse needs radii of zero or more.` and `CloseGraph has closed
this surface.`

**See also**

[`Ellipse`](#ellipse), [`GetArcCoords`](#getarccoords),
[`PieSlice`](#pieslice), [`Sector`](#sector), [`SetFillStyle`](#setfillstyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'fillellipse', False);

var V := ViewPort (60, 60, 420, 320, 1);

// Three ellipses, each filled in its own pattern and edged in white.
SetColor (V, White);

SetFillStyle (V, SolidFill, LightRed);
FillEllipse (V, 110, 160, 90, 60);

SetFillStyle (V, HatchFill, LightCyan);
FillEllipse (V, 240, 160, 60, 110);

// Equal radii: the filled circle Turbo Pascal never named.
SetFillStyle (V, InterleaveFill, Yellow);
FillEllipse (V, 350, 160, 55, 55);

Show (V);

// Wide reaches further across than down; the circle is as wide as it is tall.
System.WriteLn (V.Buf.GetInt ((159 * 420 + 189) * 4) <> 0);
System.WriteLn (V.Buf.GetInt ((59 * 420 + 109) * 4) = 0);

CloseGraph ();
```

```console
true
true
```

---

## FillPoly

*procedure, alias of `ViewPort.FillPoly`* — unit `graph`

**Function**

Fills and outlines the polygon through a list of points.

**Declaration**

```algol24
procedure FillPoly (V : ViewPort, Points : List);
```

**Remarks**

An [alias](#aliases) of the method: `FillPoly (V, …)` is `V.FillPoly (…)`.

`Points` is a List of two-number Lists, as [`DrawPoly`](#drawpoly) takes.

⚠️ **Unlike `DrawPoly`, the figure closes itself.** The last point joins the
first whether or not it was repeated, because a filled shape has no open
reading. Repeating it anyway is harmless — the closing edge is then of no
length and crosses nothing — and a test asserts the two spellings paint
identically.

Filled with the current fill pattern and color, then outlined with the pen and
the current [line style](#setlinestyle), so a polygon may be laid in one color
and edged in another. With `EmptyFill` it is outlined and hollow.

⚠️ **The fill is by the even-odd rule**, which is what a scanline fill means
and what Turbo Pascal did: a row is crossed by the edges, the crossings are
sorted, and the spans between the first and second, third and fourth, and so
on are painted. A **self-crossing polygon therefore fills in bands** — the
classic five-pointed star comes out with a hollow pentagon at its heart, which
is correct rather than a defect.

A rectangle given as four points is exactly [`Bar`](#bar) with a
[`Rectangle`](#rectangle) over it, pixel for pixel — which also pins the
half-open edge rule that keeps a shape from leaking at its corners.

The geometry is in Algol-24 — which row is crossed where is the logic — and
only the spans are handed to C, one call per span. That is a different split
from [`Bar`](#bar)'s and [`PieSlice`](#pieslice)'s, where the shape itself
went over; a polygon's shape is its points, and those are the caller's.

The pen is not moved.

Raises `FillPoly wants at least three points.`, `FillPoly wants points of two
numbers.`, and `CloseGraph has closed this surface.`

**See also**

[`Bar`](#bar), [`DrawPoly`](#drawpoly), [`SetFillStyle`](#setfillstyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'fillpoly', False);

var V := ViewPort (60, 40, 400, 400, 1);

// A filled arrow: closed of its own accord, so the first point is not
// repeated at the end.
SetColor (V, White);
SetFillStyle (V, SolidFill, LightCyan);
FillPoly (V, [[40, 140], [180, 140], [180, 90], [280, 175],
              [180, 260], [180, 210], [40, 210]]);

// And a star, which the even-odd rule leaves hollow at its heart.
SetFillStyle (V, HatchFill, Yellow);
FillPoly (V, [[200, 280], [224, 390], [112, 320], [288, 320], [176, 390]]);
Show (V);

// The arrow's shaft is filled; the star's middle is not.
System.WriteLn (V.Buf.GetInt ((174 * 400 + 99) * 4) <> 0);
System.WriteLn (V.Buf.GetInt ((339 * 400 + 199) * 4) = 0);

CloseGraph ();
```

```console
true
true
```

---

## FillSettings

*classes* — unit `graph`

**Function**

The fill and line state a viewport is holding, answered whole.

**Declaration**

```algol24
constructor FillSettings (Style : Integer, Color : Integer);

Style : Integer         one of the FillStyles
Color : Integer         the ink figures are filled in

constructor LineSettings (Style : Integer, Pattern : Integer,
                          Thickness : Integer);

Style : Integer         one of the LineStyles
Pattern : Integer       the sixteen bits walked along a line
Thickness : Integer     the width in pixels
```

**Remarks**

Turbo Pascal's `FillSettingsType` and `LineSettingsType`, answered by
[`GetFillSettings`](#getfillsettings) and `GetLineSettings` rather than
written into a `var` record — the same reasoning as
[`ArcCoords`](#arccoords).

⚠️ **The field names are this library's, not Turbo Pascal's.**
`FillSettings.Style` is Turbo Pascal's `Pattern` field, renamed so it matches
[`SetFillStyle`](#setfillstyle)`(Style, Color)`; calling it `Pattern` here
would invite a reader to expect the eight rows that
[`GetFillPattern`](#getfillpattern) answers. `LineSettings.Style` is Turbo
Pascal's `LineStyle`, on the same reasoning. **Every field is named for the
argument that sets it**, so a saved state goes back through the setter without
translation:

```algol24
SetLineStyle (V, Line.Style, Line.Pattern, Line.Thickness);
```

They are snapshots. A later `SetFillStyle` makes a new one rather than
changing an existing one, so a value held on to still describes the state it
came from.

**See also**

[`ArcCoords`](#arccoords), [`GetFillSettings`](#getfillsettings),
[`SetFillStyle`](#setfillstyle), [`SetLineStyle`](#setlinestyle),
[`ViewPort`](#viewport)

**Example**

See [`GetFillSettings`](#getfillsettings).

---

## FillStyles

*constants* — unit `graph`

**Function**

The fill patterns [`SetFillStyle`](#setfillstyle) takes.

**Declaration**

```algol24
const EmptyFill      := 0;     const HatchFill      := 7;
const SolidFill      := 1;     const XHatchFill     := 8;
const LineFill       := 2;     const InterleaveFill := 9;
const LtSlashFill    := 3;     const WideDotFill    := 10;
const SlashFill      := 4;     const CloseDotFill   := 11;
const BkSlashFill    := 5;     const UserFill       := 12;
const LtBkSlashFill  := 6;
```

**Remarks**

Turbo Pascal's thirteen names, kept whole because a program written to them
should still read. Each is an eight-by-eight bit pattern tiled across the
figure being filled, one bit to the pixel.

The bits themselves are this library's own — Turbo Pascal's exact patterns
were a BGI implementation detail rather than a promise, the same reasoning
that governs the [line styles](#linestyles).

⚠️ **`EmptyFill` paints nothing at all**, leaving whatever is beneath showing
through. Turbo Pascal filled with the background color there, which a surface
that is transparent by default has no equivalent for — leaving the pixels
alone is what *empty* means on a stack of surfaces.

⚠️ **`UserFill` is not chosen by name.** It is what
[`SetFillPattern`](#setfillstyle) sets, since a pattern without bits would be
meaningless; passing it to `SetFillStyle` raises.

**See also**

[`LineStyles`](#linestyles), [`SetFillStyle`](#setfillstyle)

**Example**

See [`SetFillStyle`](#setfillstyle).

---

## FloodFill

*procedure, alias of `ViewPort.FloodFill`* — unit `graph`

**Function**

Fills the region around a point, stopping at pixels of the border color.

**Declaration**

```algol24
procedure FloodFill (V : ViewPort, X : Integer, Y : Integer, Border : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `FloodFill (V, …)` is `V.FloodFill (…)`.

The only figure that **reads** the surface. Every other one knows the pixels
it covers from its parameters — [`Bar`](#bar) from its corners,
[`Sector`](#sector) from its radii and angles — while a flood has to look at
what is already drawn and follow it.

⚠️ **A boundary fill, as Turbo Pascal's was.** The region is every pixel
reachable from the seed without crossing one of the **border** color — not
every pixel matching the seed's own. That is the fill an outlined figure
wants, and an outlined figure is what a flood is usually aimed at:
[`Circle`](#circle) and then `FloodFill` inside it is the filled disc drawn
the long way round.

Follow that definition and two consequences fall out, both of them the point
rather than surprises. A **gap in the outline leaks** — the paint escapes
through it and floods whatever lies beyond, because the paint goes exactly
where the drawn border does not stop it. And an outline **divides the surface
in two**: flood inside it, flood outside it, and between them and the border
every pixel is accounted for.

`Border` may be [`Transparent`](#colors), which is the useful one on a fresh
surface — the untouched pixels are then the border, so the flood spreads
exactly as far as the paint already there and recolors a blob without
touching what surrounds it.

The current [fill pattern and color](#setfillstyle) are used, tiled from the
surface's origin as every fill is, so a patterned flood leaves most of the
region unpainted while still crossing all of it. `EmptyFill` paints nothing.
The pen is not moved, a flood being a figure rather than a journey.

A seed off the viewport, or one already standing on the border color, fills
nothing and does not raise — the edge is the clip, as it is for
[`PutPixel`](#putpixel).

Raises `FloodFill wants a color or Transparent.` and `CloseGraph has closed
this surface.`

**See also**

[`Bar`](#bar), [`Circle`](#circle), [`GetPixel`](#getpixel),
[`SetFillStyle`](#setfillstyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'flood', False);

var V := ViewPort (100, 60, 200, 200, 1);

// An outline, and then the paint poured inside it.
SetColor (V, LightCyan);
Circle (V, 100, 100, 80);

SetFillStyle (V, SolidFill, Blue);
FloodFill (V, 100, 100, LightCyan);
Show (V);

System.WriteLn (GetPixel (V, 100, 100) = Blue);        // the middle is filled
System.WriteLn (GetPixel (V, 100, 25) = Blue);         // and so is the top
System.WriteLn (GetPixel (V, 5, 5) = Transparent);     // outside is untouched

CloseGraph ();
```

```console
true
true
true
```

---

## Frac

*function* — unit `math`

**Function**

Returns the fractional part of its argument.

**Declaration**

```algol24
function Frac (X : Double) : Double;
```

**Remarks**

The result **carries the sign of `X`**. `Frac (-3.5)` is `-0.5`, not `0.5` and
not `0.3` — the definition is `X - Int (X)`, and [`Int`](#int) truncates toward
zero.

The identity that holds is `Int (X) + Frac (X) = X`. That the result is
non-negative is *not* one, and a program wanting a non-negative fraction has to
say so itself.

Accuracy is the subtraction's. `Frac (3.7)` is `0.7000000000000002`, because
`3.7` has no exact Double to begin with — that is arithmetic rather than
anything this routine does.

Above `2^52` a Double has no fractional bits left, so `Frac` is `0.0` from there
upward.

Written in Algol-24, over [`Int`](#int).

**See also**

[`Int`](#int), [`Round`](#round), [`Trunc`](#trunc)

**Example**

```algol24
uses math;

WriteLn (Frac (3.5));
WriteLn (Frac (-3.5));
WriteLn (Int (3.5) + Frac (3.5));
WriteLn (Frac (3.7));
```

```console
0.5
-0.5
3.5
0.7000000000000002
```

---

## GetArcCoords

*function, alias of `ViewPort.GetArcCoords`* — unit `graph`

**Function**

Where the last arc on a viewport was centered, began and ended.

**Declaration**

```algol24
function GetArcCoords (V : ViewPort) : ArcCoords;
```

**Remarks**

An [alias](#aliases) of the method: `GetArcCoords (V)` is
`V.GetArcCoords ()`. It answers an [`ArcCoords`](#arccoords).

What it is for is **joining something to a curve**: a line continuing from the
end of an [`Arc`](#arc), a label at the lip of a [`PieSlice`](#pieslice), a
second figure hung off the first.

⚠️ **Recomputing those points from the angles is the obvious alternative and
is subtly wrong.** Rounding puts the recomputed point a pixel away from the
drawn one often enough to show as a gap, and the drawing loop's rounding is
not something a caller should have to reproduce. The endpoints here are
computed the way the drawing loop computes its pixels, so what is answered is
a point the figure really reached.

**All six curve verbs record**, which is more than Turbo Pascal promised:

| verb | records because |
| --- | --- |
| [`Arc`](#arc) | it walks the curve |
| [`Ellipse`](#ellipse) | it walks the curve |
| [`Circle`](#circle) | drawn through `Arc` |
| [`PieSlice`](#pieslice) | outlined with `Arc` |
| [`Sector`](#sector) | outlined with `Ellipse` |
| [`FillEllipse`](#fillellipse) | drawn through `Sector`, so through `Ellipse` |

Turbo Pascal documented only the first three of those. Recording in the two
primitives rather than in the six verbs is what makes the other three free —
and is why they cannot drift apart from each other.

For a whole turn the two endpoints coincide, a closed figure having no two
ends. Figures that draw no curve — [`Line`](#line),
[`Rectangle`](#rectangle), [`Bar`](#bar), [`DrawPoly`](#drawpoly) — leave the
answer alone.

Each surface keeps its own, as it keeps its own pen.

Raises `No arc has been drawn on this surface.` before any of the six have
run — six meaningless numbers would be worse than a refusal, since nothing
about them would look wrong at the call site — and `CloseGraph has closed this
surface.` once the window has gone.

**See also**

[`Arc`](#arc), [`ArcCoords`](#arccoords), [`PieSlice`](#pieslice),
[`Sector`](#sector)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'arccoords', False);

var V := ViewPort (100, 60, 300, 300, 1);

SetColor (V, LightCyan);
Arc (V, 150, 150, 0, 120, 100);

var C := GetArcCoords (V);

System.WriteLn ('center ', C.X, ' ', C.Y);
System.WriteLn ('start  ', C.XStart, ' ', C.YStart);
System.WriteLn ('end    ', C.XEnd, ' ', C.YEnd);

// The end is a pixel the arc really drew, which is why a line started there
// meets the curve instead of landing beside it.
System.WriteLn ('on the arc: ', GetPixel (V, C.XEnd, C.YEnd) = LightCyan);

SetColor (V, Yellow);
PenTo (V, C.XEnd, C.YEnd);
LineRel (V, -50, -30);
Show (V);

CloseGraph ();
```

```console
center 150 150
start  250 150
end    100 63
on the arc: true
```

---

## GetAspectRatio

*function* — unit `graph`

**Function**

How much wider than tall a logical pixel appears on the screen.

**Declaration**

```algol24
function GetAspectRatio () : Double;
```

**Remarks**

The space [`Window`](#window)s and [`ViewPort`](#viewport)s are measured in is
fixed by the grid and the font — `TextCols () * CellWidth ()` by
`TextRows () * CellHeight ()`, which is **1280 by 800** for the default 80 by
25 — and it is scaled independently in X and Y to fill whatever window
actually opened.

⚠️ **When those two scales differ, a logical pixel is not square**, and a
[`Circle`](#circle) drawn with equal radii shows as an oval. This is Turbo
Pascal's problem arriving by a different road: there it was the display
hardware, here it is the stretch.

`1.0` is square. Above `1.0` a logical pixel shows wider than tall; below,
taller than wide. A circle that should *look* round is:

```algol24
Ellipse (V, X, Y, 0, 360, Round (R / GetAspectRatio ()), R);
```

The ratio follows the window's **shape, not its size** — every 4:3 window
answers `0.8333…`, because the logical space is 16:10 whatever the window.
[`TextMode`](#textmode) changes it too, by changing the logical space: 40
columns in the same window doubles it.

⚠️ **`Circle` does not correct itself, where Turbo Pascal's did.** The logical
space is the program's own coordinate system, and a radius meaning the same in
both directions there is what keeps `Circle (X, Y, R)`,
[`PieSlice`](#pieslice)` (X, Y, 0, 360, R)` and
[`FillEllipse`](#fillellipse)` (X, Y, R, R)` one shape. Correcting for the
screen is the caller's to ask for, and this is the asking.

Turbo Pascal answered two `Word`s with the vertical one pegged at 10000 —
fixed point standing in for a fraction the machine could not afford. The
language has Doubles, so this answers the fraction itself.

The background is not affected: its pixels are the window's own, one to one,
so they are square whatever this answers.

Raises `Graph is not open.` without a window.

**See also**

[`CellWidth`](#cellwidth), [`Circle`](#circle), [`Ellipse`](#ellipse),
[`GetMaxX`](#getmaxx), [`TextMode`](#textmode)

**Example**

```algol24
uses graph;
uses math;

InitGraph (640, 480, 'aspect', False);

// The logical space is fixed by the grid and the font. The window it is
// stretched into need not be the same shape, and here it is not.
System.WriteLn ('logical  ', TextCols () * CellWidth (), ' by ',
                TextRows () * CellHeight ());
System.WriteLn ('window   ', GetMaxX (), ' by ', GetMaxY ());
System.WriteLn ('aspect   ', GetAspectRatio ());

var V := ViewPort (100, 100, 400, 400, 1);
var R := 100;

// Equal radii are equal in the logical space, so this shows as an oval.
SetColor (V, DarkGray);
Circle (V, 200, 200, R);

// Cancelling the stretch makes it look round on the screen.
var XR := Round (R / GetAspectRatio ());

SetColor (V, Yellow);
Ellipse (V, 200, 200, 0, 360, XR, R);
Show (V);

System.WriteLn ('corrected x radius ', XR);

CloseGraph ();
```

```console
logical  1280 by 800
window   640 by 480
aspect   0.8333333333333334
corrected x radius 120
```

---

## GetColor

*function, alias of `ViewPort.GetColor`* — unit `graph`

**Function**

Returns the color a viewport's pen is drawing in.

**Declaration**

```algol24
function GetColor (V : ViewPort) : Integer;
```

**Remarks**

An [alias](#aliases) of the method: `GetColor (V)` is `V.GetColor ()`.

The pen's color governs everything the viewport draws — the
[`Line`](#line) family and the [`OutText`](#outtext) pair alike. A fresh
viewport draws in `White`.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`Line`](#line), [`SetColor`](#setcolor), [`ViewPort`](#viewport)

**Example**

See [`SetColor`](#setcolor), which sets it and reads it back.

---

## GetFillPattern

*function, alias of `ViewPort.GetFillPattern`* — unit `graph`

**Function**

The eight rows of the fill pattern in force.

**Declaration**

```algol24
function GetFillPattern (V : ViewPort) : List;
```

**Remarks**

An [alias](#aliases) of the method: `GetFillPattern (V)` is
`V.GetFillPattern ()`.

Eight Integers, each the eight bits of one row with the high bit leftmost —
**the same shape [`SetFillPattern`](#setfillstyle) takes**, so what comes out
of one viewport goes straight into another:

```algol24
SetFillPattern (W, GetFillPattern (V), LightCyan);
```

⚠️ **It answers the bits actually in force, whichever style set them.** Turbo
Pascal's `GetFillPattern` answered the last pattern given to
`SetFillPattern` and nothing else, so asking it after a built-in style told
you about a pattern that was no longer painting anything. Here
`InterleaveFill` answers its own checker.

⚠️ **The list is a copy.** Changing it changes nothing on the viewport, so
what is answered cannot be used to alter what a surface paints without asking.

Raises `CloseGraph has closed this surface.` once the window has gone.

**See also**

[`FillStyles`](#fillstyles), [`GetFillSettings`](#getfillsettings),
[`SetFillStyle`](#setfillstyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'pattern', False);

var V := ViewPort (50, 50, 100, 100, 1);

// A built-in style answers the bits it stands for, not merely its number.
SetFillStyle (V, InterleaveFill, Yellow);

for var Row in GetFillPattern (V) do
    System.Write (Row, ' ');

System.WriteLn ('');

// The same eight rows go straight back in, here into another surface.
var W := ViewPort (200, 50, 100, 100, 1);

SetFillPattern (W, GetFillPattern (V), LightCyan);

System.WriteLn ('W is UserFill: ', GetFillStyle (W) = UserFill);
System.WriteLn ('bits carried:  ', GetFillPattern (W)[0] = 170);

CloseGraph ();
```

```console
170 85 170 85 170 85 170 85 
W is UserFill: true
bits carried:  true
```

---

## GetFillSettings

*functions, aliases of `ViewPort.GetFillSettings` and `GetLineSettings`* — unit `graph`

**Function**

Reads back the fill or line state a viewport is holding.

**Declaration**

```algol24
function GetFillSettings (V : ViewPort) : FillSettings;
function GetLineSettings (V : ViewPort) : LineSettings;
```

**Remarks**

[Aliases](#aliases) of the methods, answering a
[`FillSettings`](#fillsettings) or a `LineSettings`.

What they are for is **saving a state and putting it back** — before a routine
that will change the pen, so it can be handed over unchanged afterward. Every
field is named for the argument that sets it, so a restore is one call and no
translation:

```algol24
var Line := GetLineSettings (V);
// ... something that changes the pen ...
SetLineStyle (V, Line.Style, Line.Pattern, Line.Thickness);
```

[`GetFillStyle`](#setfillstyle) and `GetFillColor` answer the two halves of
the fill separately and are shorter when only one is wanted.
`GetLineSettings` has no such pair: **it is the only way to read the line
state back**, there being no `GetLineStyle`.

⚠️ **`Pattern` is the bits actually walked, not the argument given.** A
built-in style answers what it stands for — `SolidLn` answers `65535`,
`DashedLn` answers `61680` — where a caller usually passes `0` beside it.
Turbo Pascal answered the argument, which said nothing at all for four of its
five styles. Feeding what is answered back into
[`SetLineStyle`](#setlinestyle) restores the same line either way, since the
style is restored alongside it.

Both are snapshots, not views onto the surface.

Raises `CloseGraph has closed this surface.` once the window has gone.

**See also**

[`FillSettings`](#fillsettings), [`GetFillPattern`](#getfillpattern),
[`SetFillStyle`](#setfillstyle), [`SetLineStyle`](#setlinestyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'settings', False);

var V := ViewPort (50, 50, 200, 100, 1);

SetFillStyle (V, XHatchFill, LightGreen);
SetLineStyle (V, DashedLn, 0, 5);

// Save both states before something that will change them.
var Fill := GetFillSettings (V);
var Line := GetLineSettings (V);

System.WriteLn ('fill style ', Fill.Style, ' color ', Fill.Color);
System.WriteLn ('line style ', Line.Style, ' pattern ', Line.Pattern,
                ' thickness ', Line.Thickness);

SetFillStyle (V, SolidFill, Red);
SetLineStyle (V, DottedLn, 0, 1);

// And put them back, the fields being the setters' own arguments.
SetFillStyle (V, Fill.Style, Fill.Color);
SetLineStyle (V, Line.Style, Line.Pattern, Line.Thickness);

System.WriteLn ('restored   ', GetFillStyle (V) = XHatchFill, ' ',
                GetLineSettings (V).Thickness = 5);

CloseGraph ();
```

```console
fill style 8 color 5635925
line style 3 pattern 61680 thickness 5
restored   true true
```

---

## GetMaxX

*function* — unit `graph`

**Function**

Returns the highest X a program can draw to — the window's width.

**Declaration**

```algol24
function GetMaxX () : Integer;
```

**Remarks**

The screen counts from 1, pixels and cells alike, so the count is the last
index: `for var X := 1; X <= GetMaxX ();` visits every column with no minus
one anywhere — the off-by-one Turbo Pascal's 0-based `GetMaxX` was famous for
does not exist here.

⚠️ **The answer describes the window that actually opened**, not the request.
Under a fullscreen [`InitGraph`](#initgraph) it is the desktop's width,
whatever size was asked for.

Raises `Graph is not open.` before `InitGraph` or after
[`CloseGraph`](#closegraph), when there is no window to measure.

For how big a window *could* be, before opening one, see
[`ScreenWidth`](#screenwidth) — that is a different question, and it has its
own name so that this one can keep meaning what drawing code needs.

**See also**

[`GetMaxY`](#getmaxy), [`InitGraph`](#initgraph), [`ScreenWidth`](#screenwidth)

**Example**

```algol24
uses graph;

InitGraph (100, 50, 'grid', False);

var Columns := 0;

for var X := 1; X <= GetMaxX (); X := X + 1 do
    Columns := Columns + 1;

System.WriteLn (Columns);

CloseGraph ();
```

```console
100
```

---

## GetMaxY

*function* — unit `graph`

**Function**

Returns the highest Y a program can draw to — the window's height.

**Declaration**

```algol24
function GetMaxY () : Integer;
```

**Remarks**

Everything said of [`GetMaxX`](#getmaxx) holds here, for rows — including that
the answer is the opened window's, which matters most under fullscreen on
macOS, where the menu bar stays and the window is short of the desktop's full
height.

Raises `Graph is not open.` when there is no window.

**See also**

[`GetMaxX`](#getmaxx), [`InitGraph`](#initgraph), [`ScreenHeight`](#screenheight)

**Example**

See [`GetMaxX`](#getmaxx) and [`InitGraph`](#initgraph), which show both.

---

## GetPixel

*function, alias of `ViewPort.GetPixel`* — unit `graph`

**Function**

The color of one pixel of a viewport.

**Declaration**

```algol24
function GetPixel (V : ViewPort, X : Integer, Y : Integer) : Integer;
```

**Remarks**

An [alias](#aliases) of the method: `GetPixel (V, …)` is `V.GetPixel (…)`.

The read [`PutPixel`](#putpixel) writes, and the only way to ask what a
surface already holds. Coordinates are the viewport's own, counting from 1.

⚠️ **`Transparent` and `Black` are different answers.** A pixel painted black
is opaque and answers [`Black`](#colors); a pixel never painted answers
[`Transparent`](#colors) and shows whatever surface lies beneath it. Nothing
else on a stack of surfaces could tell paint from the gap in it.

A position **off the viewport answers `Transparent`** rather than raising, the
edge being the clip here exactly as it is for `PutPixel`.

The value is what was written, not what the screen finally shows: a pixel of a
viewport that some other surface covers still answers its own color.

Raises `CloseGraph has closed this surface.` once the window it belongs to has
gone.

**See also**

[`Colors`](#colors), [`FloodFill`](#floodfill), [`PutPixel`](#putpixel),
[`ViewPort`](#viewport)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'read', False);

var V := ViewPort (50, 50, 40, 30, 1);

PutPixel (V, 1, 1, White);
PutPixel (V, 2, 1, Black);

System.WriteLn (GetPixel (V, 1, 1) = White);
System.WriteLn (GetPixel (V, 2, 1) = Black);          // paint, and opaque
System.WriteLn (GetPixel (V, 3, 1) = Transparent);    // never painted
System.WriteLn (GetPixel (V, 41, 1) = Transparent);   // past the edge

SetFillStyle (V, SolidFill, Red);
Bar (V, 10, 10, 20, 20);

System.WriteLn (GetPixel (V, 15, 15) = Red);

CloseGraph ();
```

```console
true
true
true
true
true
```

---

## GetX

*function, alias of `ViewPort.GetX`* — unit `graph`

**Function**

Returns the X of a viewport's pen.

**Declaration**

```algol24
function GetX (V : ViewPort) : Integer;
```

**Remarks**

An [alias](#aliases) of the method: `GetX (V)` is `V.GetX ()`.

In the viewport's own coordinates, counting from 1. A [`Line`](#line) leaves
the pen at its far end, so this is where the next [`LineTo`](#lineto) will
draw from.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`GetY`](#gety), [`Line`](#line), [`PenTo`](#pento)

**Example**

See [`Line`](#line), which reads the pen back after drawing.

---

## GetY

*function, alias of `ViewPort.GetY`* — unit `graph`

**Function**

Returns the Y of a viewport's pen.

**Declaration**

```algol24
function GetY (V : ViewPort) : Integer;
```

**Remarks**

An [alias](#aliases) of the method: `GetY (V)` is `V.GetY ()`.

Everything said of [`GetX`](#getx) holds here.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`GetX`](#getx), [`Line`](#line), [`PenTo`](#pento)

**Example**

See [`Line`](#line).

---

## GotoXY

*procedure* — unit `graph`

**Function**

Moves the cursor to a column and row of the grid.

**Declaration**

```algol24
procedure GotoXY (Col : Integer, Row : Integer);           // the screen
procedure GotoXY (W : Window, Col : Integer, Row : Integer);   // that window
```

**Remarks**

⚠️ **One-based, storybook**: the top-left cell is `1, 1` and the bottom-right
is `TextCols (), TextRows ()` — the count is the last index, and no cell
coordinate ever carries a minus one. The whole screen counts from 1, pixels
included, as Turbo Pascal's text mode and every terminal since have had it;
the one seam is that a cursor column meets the language's 0-based strings as
`Text[Col - 1]`.

A window has the same verb: `GotoXY (W, Col, Row)` is an [alias](#aliases) of `W.GotoXY`, moving that window's own cursor within its own cells.

A position off the grid raises `GotoXY is off the grid.` rather than being
clamped or ignored — the cursor is always somewhere real, and a wrong
computation is caught where it happened.

Raises `Graph is not open.` without a window.

**See also**

[`Write`](#write), [`WhereX`](#wherex), [`WhereY`](#wherey)

**Example**

```algol24
uses graph;

InitGraph (320, 200, 'goto', False);

GotoXY (10, 5);

System.WriteLn (WhereX ());
System.WriteLn (WhereY ());

CloseGraph ();
```

```console
10
5
```


---

## HighVideo

*procedure* — unit `graph`

**Function**

Brightens the ink celled text draws with.

**Declaration**

```algol24
procedure HighVideo ();            // the screen
procedure HighVideo (W : Window);  // that window
```

**Remarks**

`HighVideo (W)` is an [alias](#aliases) of `W.HighVideo`, brightening that
window's ink alone; `LowVideo` and `NormVideo` likewise.

A dim palette color becomes its bright partner — `Blue` to `LightBlue`,
`Red` to `LightRed` — and a bright one stays itself, which is the idempotence
Turbo Pascal's intensity bit had. The famous odd couple is preserved:
`Brown`'s bright partner is `Yellow`, because the original adapter's brown
was a special circuit and the bright version of that slot came out yellow.

⚠️ **Off the palette there is no intensity bit**, so any other RGB moves
halfway toward white — and calling again moves it further, which repeated
`LowVideo` mirrors toward black. A 16-color idea, read honestly in 24 bits.

The background is never touched; [`NormVideo`](#normvideo) restores both.

**See also**

[`Colors`](#colors), [`LowVideo`](#lowvideo), [`NormVideo`](#normvideo), [`TextColor`](#textcolor)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'video', False);

TextColor (Brown);
HighVideo ();
Write ('now yellow');

NormVideo ();
WriteLn (' and back to normal');

System.WriteLn (WhereY ());

CloseGraph ();
```

```console
2
```


---

## Infinity

*constant* — unit `math`

**Function**

The positive infinity, as a Double.

**Declaration**

```algol24
const Infinity := 1.0 / 0.0;
```

**Remarks**

A real value rather than a spelling: an overflowing operation answers it, and so
does dividing a non-zero value by zero. `1.0 / 0.0` is `Infinity` rather than a
raise — only `div` and `mod` refuse a zero divisor.

There is no separate constant for the negative infinity. `-Infinity` is it, and
compares and renders as expected.

It is ordered above every finite Double, which makes it the right seed for a
running minimum — the first value compared always replaces it. `-Infinity` seeds
a running maximum the same way.

Unlike [`NaN`](#nan), it compares usefully: `X = Infinity` is a sound test.
[`IsInfinite`](#isinfinite) says it more plainly and catches both signs at once.

Note that `Infinity - Infinity` is [`NaN`](#nan), as is `Infinity * 0.0`.

**See also**

[`IsInfinite`](#isinfinite), [`IsNaN`](#isnan), [`Max`](#max), [`Min`](#min), [`NaN`](#nan)

**Example**

```algol24
uses math;

WriteLn (Infinity);
WriteLn (-Infinity);
WriteLn (1.0E300 * 1.0E300 = Infinity);

// Seeded with Infinity, so the first value always replaces it.
var Smallest := Infinity;
for var V in [3.5, -1.25, 9.0] do
    Smallest := Min (Smallest, V);

WriteLn (Smallest);
```

```console
Infinity
-Infinity
true
-1.25
```

---

## InitGraph

*procedure* — unit `graph`

**Function**

Opens the graphics window.

**Declaration**

```algol24
procedure InitGraph (Width : Integer, Height : Integer, Title : String, Fullscreen : Boolean);
```

**Remarks**

The window is `Width` by `Height` pixels, centered on the desktop, with `Title`
in its title bar — Unicode included. It opens cleared to black.

With `Fullscreen` true the window instead covers the desktop at the desktop's
own resolution, and `Width` and `Height` are not used. This is the borderless
kind of fullscreen: it changes no display mode, and cannot leave the screen in
one if the program dies. On macOS the menu bar stays, so the fullscreen window
is the desktop's width but can be short of its height.

The size asked for and the size received can differ — fullscreen is the obvious
case — so [`GetMaxX`](#getmaxx) and [`GetMaxY`](#getmaxy) report what actually
opened rather than echoing the request.

Turbo Pascal's `InitGraph` took a driver, a mode and a path to BGI files; all
three existed to pick a video card and are gone. Failure **raises**, carrying
SDL's own message, rather than setting a result code for `GraphResult` to fetch
— there is no `GraphResult`.

Raises `Graph is already open.` for a second call before
[`CloseGraph`](#closegraph), and `InitGraph needs a positive size.` for a
`Width` or `Height` of zero or less.

The unit reaches SDL2 by its Homebrew path, so as written it wants
`brew install sdl2`.

**See also**

[`CloseGraph`](#closegraph), [`GetMaxX`](#getmaxx), [`GetMaxY`](#getmaxy), [`ScreenWidth`](#screenwidth)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'My Program', False);

System.WriteLn (GetMaxX ());
System.WriteLn (GetMaxY ());

CloseGraph ();
```

```console
640
480
```

---

## InstallUserFont

*procedure* — unit `graph`

**Function**

Installs a glyph file as the font all text draws with.

**Declaration**

```algol24
procedure InstallUserFont (Path : String);
```

**Remarks**

The format is a cell font, one glyph per line: a codepoint, a separator, and
hex rows. A `:` separator carries ink — one bit per pixel, or one byte of
coverage per pixel when the file opens with a `# gray8` header — and a `+`
separator carries a **picture**, RRGGBBAA per pixel, which is how emoji arrive
in their own colors. A glyph's width is the length of its row data, so a CJK
glyph is simply twice as wide as a Latin one and the format needs no width
table.

The library ships two:

| | |
| --- | --- |
| `graphfont.hex` | antialiased coverage with emoji; installed by default the first time text draws |
| `romfont.hex` | one bit per pixel — what a 1980s character ROM held |

The default's coverage is all printable ASCII plus a set of Turkish, Greek,
Chinese, Korean and Spanish glyphs and a handful of emoji. The example
repository's `makefont.py` generates files in this format from any font the
machine has, which is how coverage grows.

A path resolves against the working directory, as any data file does. A new
font replaces the old entirely; a font that cannot be opened **leaves the old
one installed** and raises `InstallUserFont: cannot open '...'.`

In Turbo Pascal this registered a `.CHR` stroked-font file for `SetTextStyle`
to select later; here the glyph file itself is the font, so installing is
selecting.

**See also**

[`OutText`](#outtext), [`OutTextXY`](#outtextxy)

**Example**

```algol24
uses graph;

InitGraph (400, 200, 'fonts', False);

OutTextXY (10, 10, 'antialiased, with emoji');

InstallUserFont ('romfont.hex');
OutTextXY (10, 60, 'one bit per pixel');

CloseGraph ();
System.WriteLn ('two fonts, one window');
```

```console
two fonts, one window
```

---

## Int

*function* — unit `math`

**Function**

Returns the integer part of its argument, as a Double.

**Declaration**

```algol24
function Int (X : Double) : Double;
```

**Remarks**

This is [`Trunc`](#trunc) without the conversion — the same truncation toward
zero, but the answer stays a Double. `Int (3.7)` is `3.0` where `Trunc (3.7)` is
`3`, and that difference is the whole reason both exist.

Truncation is toward zero, so `Int (-3.7)` is `-3.0` rather than `-4.0`.

Nothing is being converted, so each infinity and `NaN` answers itself rather
than raising — the two arguments [`Trunc`](#trunc) refuses. On every finite
Double the two agree, each in its own return type.

A Double keeps its sign through truncation, so a value between `0` and `-1`
answers **negative zero**: `Int (-0.9)` renders as `-0.0`. It compares equal to
`0.0`, so only the rendering — or the sign of `1.0 / …` — tells them apart.

libm's `trunc` is exactly this function, and declaring it with the return type C
really gives it is all it takes.

**See also**

[`Frac`](#frac), [`Round`](#round), [`Trunc`](#trunc)

**Example**

```algol24
uses math;

WriteLn (Int (3.7));
WriteLn (Int (-3.7));
WriteLn (Int (1.0E300));
```

```console
3.0
-3.0
1.0E300
```

---

## IsInfinite

*function* — unit `math`

**Function**

Returns True when its argument is either infinity.

**Declaration**

```algol24
function IsInfinite (X : Double) : Boolean;
```

**Remarks**

Both signs answer True.

⚠️ **A `NaN` answers False.** It is not a number, but it is not an infinity
either, and keeping the two apart is why there are two predicates rather than
one. A value is exactly one of `NaN`, infinite, or finite, and the three cases
are exclusive and exhaustive — which is what makes them safe to branch on.

`Infinity` compares equal to itself, so this could be written `X = Infinity or
X = -Infinity` at the call site. The predicate says it more plainly and cannot
be got half right.

**See also**

[`Infinity`](#infinity), [`IsNaN`](#isnan), [`NaN`](#nan)

**Example**

```algol24
uses math;

WriteLn (IsInfinite (1.0 / 0.0));
WriteLn (IsInfinite (-1.0 / 0.0));
WriteLn (IsInfinite (1.0E300 * 1.0E300));
WriteLn (IsInfinite (1.7976931348623157E308));
WriteLn (IsInfinite (Sqrt (-1.0)));
```

```console
true
true
true
false
false
```

---

## IsNaN

*function* — unit `math`

**Function**

Returns True when its argument is not a number.

**Declaration**

```algol24
function IsNaN (X : Double) : Boolean;
```

**Remarks**

⚠️ **This is the only test that works.** A `NaN` is the one value not equal to
itself, and every comparison against it is false — so `X = NaN` answers False
*even when `X` is a NaN*, and `X <> NaN` answers True for every `X` including a
NaN. Neither says what was meant, and both look entirely reasonable.

A NaN arrives from an operation with no answer rather than from no value:
[`Sqrt`](#sqrt) of a negative, [`Ln`](#ln) of a negative, zero divided by zero,
one infinity subtracted from another, or `Infinity * 0.0`.

Note that [`AssertNear`](#assertnear) already refuses a NaN, so a test need not
guard for one separately.

**See also**

[`Infinity`](#infinity), [`IsInfinite`](#isinfinite), [`NaN`](#nan)

**Example**

```algol24
uses math;

WriteLn (IsNaN (Sqrt (-1.0)));
WriteLn (IsNaN (0.0));
WriteLn (IsNaN (Infinity));

// The comparison a reader reaches for first, and why it cannot be used.
var Bad := Sqrt (-1.0);
WriteLn (Bad = NaN);
WriteLn (IsNaN (Bad));
```

```console
true
false
false
false
true
```

---

## KeyPressed

*function* — unit `graph`

**Function**

Returns True when a key is waiting.

**Declaration**

```algol24
function KeyPressed () : Boolean;
```

**Remarks**

Neither consumes the key nor blocks; [`ReadKey`](#readkey) answers it.

Polling this in a loop keeps the blink phase beating — the poll presents
when a frame is due — so `while not KeyPressed do` animates a screen the
way the era's programs did. When the loop has nothing else to do,
[`ReadKey`](#readkey) alone is the better idle, since it sleeps between
looks.

Raises `Graph is not open.` without a window.

**See also**

[`Keys`](#keys), [`ReadKey`](#readkey)

**Example**

See [`ReadKey`](#readkey), which shows the empty queue answered;
`examples/keys.a24` echoes every key as it arrives.

---

## Keys

*constants* — unit `graph`

**Function**

The keys that are not characters, each as one Char of its own.

**Declaration**

```algol24
const KeyUp     := #57344;    const KeyF1  := #57360;   const KeyF7  := #57366;
const KeyDown   := #57345;    const KeyF2  := #57361;   const KeyF8  := #57367;
const KeyLeft   := #57346;    const KeyF3  := #57362;   const KeyF9  := #57368;
const KeyRight  := #57347;    const KeyF4  := #57363;   const KeyF10 := #57369;
const KeyHome   := #57348;    const KeyF5  := #57364;   const KeyF11 := #57370;
const KeyEnd    := #57349;    const KeyF6  := #57365;   const KeyF12 := #57371;
const KeyPgUp   := #57350;
const KeyPgDn   := #57351;    const KeyClose := #57384;
const KeyInsert := #57352;
const KeyDelete := #57353;
```

**Remarks**

Values in Unicode's private-use area, so each special key is an ordinary
Char and `if ReadKey () = KeyUp` reads as it should. Enter, Escape, Tab and
Backspace are not here because they never needed to be: they arrive as their
real characters, `#13` `#27` `#9` `#8`, as they always did.

`KeyClose` is the window's close button, delivered through the same stream —
a window is part of this machine, and a program's one input loop should hear
it.

**See also**

[`KeyPressed`](#keypressed), [`ReadKey`](#readkey)

**Example**

See [`ReadKey`](#readkey).

---

## Line

*procedure, alias of `ViewPort.Line`* — unit `graph`

**Function**

Draws a straight line on a viewport.

**Declaration**

```algol24
procedure Line (V : ViewPort, X1 : Integer, Y1 : Integer, X2 : Integer, Y2 : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `Line (V, …)` is `V.Line (…)`.

Draws between two points **and leaves the pen at the far end**, so a series is
[`PenTo`](#pento) once and [`LineTo`](#lineto) after — which is how the chart
in `examples/ide.a24` is plotted.

Coordinates are the viewport's own, counting from 1, and its edges are the
clip: a line running far outside is drawn for the part that lands and
discarded for the rest, never raising.

The current [`SetLineStyle`](#setlinestyle) patterns it and thickens it, and
[`SetColor`](#setcolor) colors it. Drawing does not appear until something
presents; [`Show`](#show) is that.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`LineRel`](#linerel), [`LineStyles`](#linestyles), [`LineTo`](#lineto), [`PenTo`](#pento), [`Rectangle`](#rectangle), [`SetColor`](#setcolor)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'lines', False);

var V := ViewPort (100, 100, 400, 300, 1);

// A solid axis, then a dashed thick series plotted point to point.
SetColor (V, LightGray);
Line (V, 40, 20, 40, 260);
Line (V, 40, 260, 380, 260);

SetColor (V, LightGreen);
SetLineStyle (V, DashedLn, 0, ThickWidth);
PenTo (V, 40, 200);
LineTo (V, 140, 90);
LineTo (V, 240, 160);
LineTo (V, 380, 40);

SetColor (V, Yellow);
SetLineStyle (V, SolidLn, 0, NormWidth);
Rectangle (V, 40, 20, 380, 260);
Show (V);

// The pen is where the last LineTo left it: Rectangle does not move it.
System.WriteLn (GetX (V));
System.WriteLn (GetY (V));
System.WriteLn (GetColor (V) = Yellow);

CloseGraph ();
```

```console
380
40
true
```

---

## LineRel

*procedure, alias of `ViewPort.LineRel`* — unit `graph`

**Function**

Draws from a viewport's pen by a displacement.

**Declaration**

```algol24
procedure LineRel (V : ViewPort, DX : Integer, DY : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `LineRel (V, …)` is `V.LineRel (…)`.

A displacement where [`LineTo`](#lineto) takes a destination; the pen ends at
the far end either way. [`PenRel`](#penrel) is the same move without drawing.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`Line`](#line), [`LineTo`](#lineto), [`PenRel`](#penrel)

**Example**

See [`Line`](#line).

---

## LineStyles

*constants* — unit `graph`

**Function**

The line styles [`SetLineStyle`](#line) takes, and the two thicknesses.

**Declaration**

```algol24
const SolidLn   := 0;      const NormWidth  := 1;
const DottedLn  := 1;      const ThickWidth := 3;
const CenterLn  := 2;
const DashedLn  := 3;
const UserBitLn := 4;
```

**Remarks**

A style is a sixteen-bit pattern walked along the line, one bit to the pixel
and repeating: a set bit paints, a clear one skips. The four named patterns
are this library's own — Turbo Pascal's exact bits were a BGI implementation
detail rather than a promise — and `UserBitLn` says the pattern comes from
the caller's own `Pattern` argument instead.

The thicknesses are Turbo Pascal's two names for 1 and 3. They are ordinary
numbers, and `SetLineStyle` accepts any positive thickness, so a 7-pixel
rule needs no constant.

**See also**

[`Line`](#line)

**Example**

See [`Line`](#line); `examples/ide.a24` shows all five styles and three
thicknesses side by side.

---

## LineTo

*procedure, alias of `ViewPort.LineTo`* — unit `graph`

**Function**

Draws from a viewport's pen to a point.

**Declaration**

```algol24
procedure LineTo (V : ViewPort, X : Integer, Y : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `LineTo (V, …)` is `V.LineTo (…)`.

The pen ends where the line does, so successive calls plot a series — the
idiom a chart is drawn with. [`PenTo`](#pento) is the same move without
drawing.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`Line`](#line), [`LineRel`](#linerel), [`PenTo`](#pento)

**Example**

See [`Line`](#line), which plots four points this way.

---

## Ln

*function* — unit `math`

**Function**

Returns the natural logarithm of its argument.

**Declaration**

```algol24
function Ln (X : Double) : Double;
```

**Remarks**

`Ln (0.0)` answers `-Infinity` and a negative argument answers `NaN`. Neither
raises — both come from libm rather than from a check here — so a program that
cares must test the argument or the result itself. `NaN` is not equal to itself,
which is the usual way to detect it.

A foreign declaration onto libm's `log`. The name is `Ln` rather than C's `Log`
because `log` is ambiguous across languages — natural in C, base ten in others —
where `Ln` can only mean one thing.

**See also**

[`Exp`](#exp), [`Sqrt`](#sqrt)

**Example**

```algol24
uses math;

WriteLn (Ln (1.0));
WriteLn (Ln (Exp (1.0)));
WriteLn (Ln (0.0));
```

```console
0.0
1.0
-Infinity
```

---

## LowVideo

*procedure* — unit `graph`

**Function**

Dims the ink celled text draws with.

**Declaration**

```algol24
procedure LowVideo ();            // the screen
procedure LowVideo (W : Window);  // that window
```

**Remarks**

The mirror of [`HighVideo`](#highvideo): a bright palette color becomes its
dim partner and a dim one stays itself; any other RGB moves halfway toward
black, again further on each call.

**See also**

[`HighVideo`](#highvideo), [`NormVideo`](#normvideo)

**Example**

See [`HighVideo`](#highvideo); `examples/video.a24` shows the whole palette
both ways.


---

## Max

*function* — unit `math`

**Function**

Returns the larger of its two arguments.

**Declaration**

```algol24
function Max (A, B);
```

**Remarks**

The result is whichever argument was larger, in that argument's own type. A tie
answers `B`, which is visible only when the two differ in type: `Max (2, 2.0)`
is `2.0` and `Max (2.0, 2)` is `2`.

Like `Abs`, `Max` is untyped, so its result cannot initialize a variable of a
written type. Assigning to a variable whose type was *inferred* is fine, which
is what makes the running-maximum idiom in the example work.

Raises `Operands must be numbers.` for arguments the comparison cannot take.

**See also**

[`Abs`](#abs), [`Min`](#min)

**Example**

```algol24
uses math;

WriteLn (Min (3, 7));
WriteLn (Max (3, 7));

var Highest := 0;
for var N in [3, 17, 8] do
    Highest := Max (Highest, N);

WriteLn (Highest);
```

```console
3
7
17
```

---

## Min

*function* — unit `math`

**Function**

Returns the smaller of its two arguments.

**Declaration**

```algol24
function Min (A, B);
```

**Remarks**

The result is whichever argument was smaller, in that argument's own type. A tie
answers `B`, on the same reasoning as `Max`: `Min (2, 2.0)` is `2.0` and
`Min (2.0, 2)` is `2`.

Untyped, with the same consequence for a declared variable as `Abs` and `Max`.

Raises `Operands must be numbers.` for arguments the comparison cannot take.

**See also**

[`Abs`](#abs), [`Max`](#max)

**Example**

See [`Max`](#max), which shows both.

---

## MoveTo

*procedure, alias of `Window.MoveTo` and `ViewPort.MoveTo`* — unit `graph`

**Function**

Moves a surface, keeping its size.

**Declaration**

```algol24
procedure MoveTo (Surface, X : Integer, Y : Integer);
```

**Remarks**

An [alias](#aliases) of both methods: `MoveTo (W, …)` is `W.MoveTo (…)`.

A [`Window`](#window) moves in **cells** and a [`ViewPort`](#viewport) in the
logical space's **pixels** — each surface in the coordinates it was placed in.
Any position is allowed, off the grid included: a surface may be dragged
partly or wholly out of view, and what falls outside is clipped at present
time.

⚠️ **This one takes an untyped surface** rather than being declared once per
class. Two declarations of one arity would put the choice back on the
argument's *type*, which a gradual type system settles at run time and warns
about at every call site — correctly, since it cannot know. Asking `is Window`
inside costs one branch and no noise.

⚠️ **`MoveTo` moves the surface; [`PenTo`](#pento) moves a viewport's pen.**
Turbo Pascal spelled the pen's move `MoveTo`, but a surface is a thing that
can itself be moved, and that reading wins the shorter name.

Raises `MoveTo wants a Window or a ViewPort.` for anything else, and
`CloseGraph has closed this surface.` once the window has gone.

**See also**

[`PenTo`](#pento), [`ViewPort`](#viewport), [`Window`](#window)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'moveto', False);

var Panel := Window (2, 2, 20, 8, 1);
var Chart := ViewPort (100, 100, 60, 40, 2);

MoveTo (Panel, 30, 5);
MoveTo (Chart, 250, 180);

System.WriteLn (Panel.X1);
System.WriteLn (Panel.X2);
System.WriteLn (Chart.X);

CloseGraph ();
```

```console
30
48
250
```

---

## NaN

*constant* — unit `math`

**Function**

The quiet NaN, as a Double.

**Declaration**

```algol24
const NaN := 0.0 / 0.0;
```

**Remarks**

Useful for handing back a Double that means nothing, and for feeding a routine
under test.

⚠️ **It is not useful for comparison, and that is the whole of what makes it
awkward.** This constant is not equal to itself, and no value is equal to it.
`X = NaN` is always False and `X <> NaN` is always True, whatever `X` is.
[`IsNaN`](#isnan) is the only way to ask.

It renders as `NaN`, which is how a NaN shows up in output and in a failed
assertion's message.

**See also**

[`Infinity`](#infinity), [`IsInfinite`](#isinfinite), [`IsNaN`](#isnan)

**Example**

```algol24
uses math;

WriteLn (NaN);
WriteLn (NaN = NaN);
WriteLn (NaN <> NaN);
WriteLn (IsNaN (NaN));
```

```console
NaN
false
true
true
```

---

## NormVideo

*procedure* — unit `graph`

**Function**

Restores the ink and background a text mode wakes up in.

**Declaration**

```algol24
procedure NormVideo ();            // the screen
procedure NormVideo (W : Window);  // that window
```

**Remarks**

`LightGray` on `Transparent` — the defaults `InitGraph` and
[`TextMode`](#textmode) set — which is what Turbo Pascal's `NormVideo` did
with the attribute the program started under. The one of the three that
touches the background.

**See also**

[`HighVideo`](#highvideo), [`LowVideo`](#lowvideo), [`TextBackground`](#textbackground)

**Example**

See [`HighVideo`](#highvideo).

---

## Odd

*function* — unit `math`

**Function**

Returns True when its argument is odd.

**Declaration**

```algol24
function Odd (N : Integer) : Boolean;
```

**Remarks**

`N` must be an Integer. A Double raises `Operands must be integers.`, because
`mod` refuses one rather than truncating it.

Negative numbers answer correctly. This is worth stating because the obvious
implementation does not: `mod` keeps the sign of its left operand, so
`-3 mod 2` is `-1`, and a test written `N mod 2 = 1` would answer False for
every negative odd number. The test here is `<> 0`.

The argument is unbounded, so oddness of a number past the machine width is
answered as readily as any other.

**See also**

[`Abs`](#abs)

**Example**

```algol24
uses math;

for var N := 1; N <= 5; N := N + 1 do
    if Odd (N) then WriteLn (Str (N) + ' is odd');
```

```console
1 is odd
3 is odd
5 is odd
```

---

## OutText

*procedure* — unit `graph`

**Function**

Draws text at the current position and advances it by the text's width.

**Declaration**

```algol24
procedure OutText (Text : String);                 // the background
procedure OutText (V : ViewPort, Text : String);   // that viewport
```

**Remarks**

Successive calls continue one line, which is what the current position exists
for. The position starts at the window's top-left corner when `InitGraph`
opens it; [`OutTextXY`](#outtextxy) neither consults nor moves it.

Nothing wraps: text past the window's edge is clipped, not folded.

Any codepoint the installed font carries will draw — see
[`InstallUserFont`](#installuserfont) for what the shipped default covers.
Emoji arrive in their own colors. A codepoint the font lacks advances a blank
half-cell rather than raising, so a text with one exotic character still
lands. The ink is the background surface's white; a
[`ViewPort`](#viewport)'s own free text takes the color its
[`SetColor`](#line) was given.

`WriteLn` cannot serve here, and it was tried: a unit-defined `WriteLn`
**replaces** the built-in rather than overloading it, so a program using such
a unit would lose console output entirely — `WriteLn ('x')` answers
`Expected 2 arguments but got 1.` The Turbo Pascal names avoid breaking the
console to reach the window.

Raises `Graph is not open.` without a window.

**See also**

[`InstallUserFont`](#installuserfont), [`OutTextXY`](#outtextxy)

**Example**

```algol24
uses graph;

InitGraph (400, 200, 'text', False);

OutTextXY (10, 10, 'Hello 你好 🙂');
OutText ('one ');
OutText ('line, ');
OutText ('in pieces');

CloseGraph ();
System.WriteLn ('drawn');
```

```console
drawn
```

---

## OutTextXY

*procedure* — unit `graph`

**Function**

Draws text with its top-left corner at X, Y.

**Declaration**

```algol24
procedure OutTextXY (X : Integer, Y : Integer, Text : String);              // the background
procedure OutTextXY (V : ViewPort, X : Integer, Y : Integer, Text : String); // that viewport
```

**Remarks**

The current position is neither consulted nor moved, which is Turbo Pascal's
rule and the useful one: placed text is placed, and a following
[`OutText`](#outtext) carries on from wherever it was. Pixels count from 1 —
`1, 1` is the window's corner — like everything on this screen.

Coordinates may land partly or wholly outside the window; what falls outside
is clipped pixel by pixel rather than raising, so a label may slide off an
edge gracefully.

Everything said of [`OutText`](#outtext) about fonts, emoji, missing glyphs
and ink holds here.

Raises `Graph is not open.` without a window.

**See also**

[`InstallUserFont`](#installuserfont), [`OutText`](#outtext)

**Example**

See [`OutText`](#outtext) and [`InstallUserFont`](#installuserfont), which
place text with it.

---

## PenRel

*procedure, alias of `ViewPort.PenRel`* — unit `graph`

**Function**

Moves a viewport's pen by a displacement, without drawing.

**Declaration**

```algol24
procedure PenRel (V : ViewPort, DX : Integer, DY : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `PenRel (V, …)` is `V.PenRel (…)`.

`PenTo` and `PenRel` pair with [`LineTo`](#lineto) and
[`LineRel`](#linerel) — the same two destinations, one drawing and one not.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`LineRel`](#linerel), [`PenTo`](#pento)

**Example**

See [`Line`](#line).

---

## PenTo

*procedure, alias of `ViewPort.PenTo`* — unit `graph`

**Function**

Moves a viewport's pen to a point, without drawing.

**Declaration**

```algol24
procedure PenTo (V : ViewPort, X : Integer, Y : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `PenTo (V, …)` is `V.PenTo (…)`.

Where a series of [`LineTo`](#lineto) calls begins. Turbo Pascal spelled this
`MoveTo`, which here moves the viewport itself — see [`MoveTo`](#moveto).
`PenTo` and `PenRel` pair with `LineTo` and `LineRel`: the same two
destinations, one drawing and one not, which is a clarity Turbo Pascal's
naming did not have.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`GetX`](#getx), [`Line`](#line), [`LineTo`](#lineto), [`MoveTo`](#moveto), [`PenRel`](#penrel)

**Example**

See [`Line`](#line), which begins its series with it.

---

## Pi

*constant* — unit `math`

**Function**

The ratio of a circle's circumference to its diameter.

**Declaration**

```algol24
const Pi := 3.141592653589793;
```

**Remarks**

The value is the nearest Double to π, and is the same one C's `M_PI` names, so a
computation mixing it with a libm result through `Sin`, `Cos` or `ArcTan` stays
consistent.

It is a constant rather than a function, unlike Turbo Pascal's `Pi`, because
nothing here needs it to be a call.

Being the nearest Double and not π, it does not behave like the exact value
everywhere — see [`Cos`](#cos) for the quarter-turn case.

**See also**

[`ArcTan`](#arctan), [`Cos`](#cos), [`Sin`](#sin)

**Example**

```algol24
uses math;

var Radius := 3.0;

WriteLn ('circumference: ' + Str (2 * Pi * Radius));
WriteLn ('area:          ' + Str (Pi * Sqr (Radius)));
```

```console
circumference: 18.84955592153876
area:          28.274333882308138
```

---

## PieSlice

*procedure, alias of `ViewPort.PieSlice`* — unit `graph`

**Function**

Fills and outlines a pie slice on a viewport.

**Declaration**

```algol24
procedure PieSlice (V : ViewPort, X : Integer, Y : Integer, StartAngle : Integer,
                    EndAngle : Integer, Radius : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `PieSlice (V, …)` is `V.PieSlice (…)`.

The wedge between two angles: filled with the current fill pattern and color,
then outlined with the pen and the current line style — the arc, and the two
radii closing it to the center.

Angles are [`Arc`](#arc)'s: degrees, zero at three o'clock, counterclockwise,
ninety at twelve, and the sweep is the counterclockwise one.

⚠️ **A sweep of 360 is the whole disc, and draws no radii.** The two would
coincide, and a spoke across a full pie is a blemish rather than an edge.

⚠️ **Two pens.** The wedge is laid in the fill color and edged in the drawing
color — which is exactly what a labelled pie chart wants, and what
[`SetFillStyle`](#setfillstyle) is separate for. With `EmptyFill` the slice is
outlined and hollow, which is how to draw one without a color.

The pen is not moved.

The fill is drawn by C, as [`Bar`](#bar)'s is, and for the same reason. It
asks whether a point lies within the sweep by **cross product rather than
arctangent** — a ray divides the plane, and which side a point falls on
answers the question exactly, with no transcendental per pixel and no need
for a two-argument `ArcTan` the library does not have.

Raises `PieSlice needs a radius of zero or more.` and `CloseGraph has closed
this surface.`

**See also**

[`Arc`](#arc), [`Circle`](#circle), [`GetArcCoords`](#getarccoords),
[`Sector`](#sector), [`SetFillStyle`](#setfillstyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'pie', False);

var V := ViewPort (60, 40, 400, 400, 1);

// A pie chart: each slice filled in its own color and edged in white.
var Shares := [22, 18, 25, 15, 20];
var Inks   := [LightRed, Yellow, LightCyan, LightGreen, LightMagenta];
var At     := 0;

SetColor (V, White);

for var I := 0; I < 5; I := I + 1 do
begin
    SetFillStyle (V, SolidFill, Inks[I]);
    PieSlice (V, 200, 200, At * 36 div 10, (At + Shares[I]) * 36 div 10, 150);

    At := At + Shares[I];
end

Show (V);

// The first slice starts at three o'clock and sweeps upward.
System.WriteLn (V.Buf.GetInt ((199 * 340 + 259) * 4) <> 0);
System.WriteLn (GetColor (V) = White);

CloseGraph ();
```

```console
true
true
```

---

## PutPixel

*procedure, alias of `ViewPort.PutPixel`* — unit `graph`

**Function**

Paints one pixel of a viewport.

**Declaration**

```algol24
procedure PutPixel (V : ViewPort, X : Integer, Y : Integer, Color : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `PutPixel (V, …)` is
`V.PutPixel (…)`.

Coordinates are the viewport's own, counting from 1 — `1, 1` is *its*
top-left corner, not the screen's.

⚠️ **A pixel outside the viewport is discarded silently**, never raising. The
edge is the clip, which is what lets a drawing be written without bounds
checks and a viewport be moved without its contents escaping.

The color is written directly and does not use the pen, so
[`SetColor`](#setcolor) does not affect it. Drawing does not appear until
something presents; [`Show`](#show) is that.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`GetPixel`](#getpixel), [`Line`](#line), [`Show`](#show),
[`ViewPort`](#viewport)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'pixels', False);

var V := ViewPort (50, 50, 40, 30, 1);

PutPixel (V, 1, 1, White);
PutPixel (V, 40, 30, Yellow);
PutPixel (V, 41, 1, Red);          // past the edge: clipped, silently
Show (V);

System.WriteLn (GetPixel (V, 1, 1) = White);
System.WriteLn (GetPixel (V, 40, 30) = Yellow);
System.WriteLn (GetPixel (V, 41, 1) = Transparent);

CloseGraph ();
```

```console
true
true
true
```

---

## Random

*function* — unit `random`

**Function**

Returns the next value in the sequence, at least zero and below `Bound`.

**Declaration**

```algol24
function Random (Bound : Integer) : Integer;
```

**Remarks**

The short name for [`RandomInteger`](#randominteger), and a pass-through to it —
the same draw, not a parallel one.

It names the Integer form rather than the Double one because that is what is
reached for: picking an element, rolling a die and choosing a case all want a
bounded Integer. [`RandomReal`](#randomreal) has no short spelling for the same
reason.

⚠️ **It is a pass-through rather than a second overload of one name.** Turbo
Pascal spells both shapes `Random`, which is not available here: overload
selection happens at run time and warns at *every* call site, so a single
`Random` answering both shapes would make the unit's commonest call its
noisiest.

**See also**

[`RandomInteger`](#randominteger), [`RandomReal`](#randomreal), [`SetSeed`](#setseed)

**Example**

```algol24
uses random;

SetSeed (2024);

for var Roll := 0; Roll < 5; Roll := Roll + 1 do
    WriteLn ('die: ' + Str (Random (6) + 1));
```

```console
die: 5
die: 6
die: 5
die: 1
die: 4
```

---

## RandomInteger

*function* — unit `random`

**Function**

Returns the next value in the sequence, at least zero and below `Bound`.

**Declaration**

```algol24
function RandomInteger (Bound : Integer) : Integer;
```

**Remarks**

The upper bound is **excluded**. `RandomInteger (6)` answers one of 0, 1, 2, 3,
4 or 5, so a die is `RandomInteger (6) + 1`. Excluding it is what makes the
routine compose with a length: `L[RandomInteger (L.Length)]` is always an
element of `L`.

A bound of zero or less raises `RandomInteger needs a bound above zero.`, having
no value it could answer with.

The distribution is as even as 48 bits allow. Beyond a bound of `2^48` the
values thin out — there are only `2^48` sequences to draw from however large the
bound — and beyond `2^53` the multiplication itself loses precision. Neither
limit is reached by any ordinary use, and neither is checked.

**See also**

[`Random`](#random), [`RandomReal`](#randomreal), [`SetSeed`](#setseed)

**Example**

```algol24
uses random;

SetSeed (7);

var Colors := ['red', 'green', 'blue'];

for var I := 0; I < 4; I := I + 1 do
    WriteLn (Colors[RandomInteger (Colors.Length)]);
```

```console
red
blue
red
red
```

---

## Randomize

*procedure* — unit `random`

**Function**

Seeds the generator unpredictably, from the clock and the process.

**Declaration**

```algol24
procedure Randomize ();
```

**Remarks**

Call it once, at the start of a program that wants a different sequence on every
run.

⚠️ **A program that never calls it gets the same sequence every time.** That is
deliberate — a program is reproducible by default and becomes unpredictable only
where it asks to be — but it does surprise anyone expecting a fresh sequence for
free.

The seed is nanosecond time mixed with the process id, not the whole second
`time` would answer. Seconds would give two programs started in the same second
identical sequences, and the process id separates two that start within the same
nanosecond.

[`SetSeed`](#setseed) undoes it: seeding explicitly after `Randomize` restores a
repeatable sequence, which is how a program can be unpredictable in use and
deterministic under test.

**See also**

[`Random`](#random), [`SetSeed`](#setseed)

**Example**

```algol24
uses random;

Randomize ();

// Nothing here is repeatable, which is the point of it -- so all that can be
// shown is that the generator works.
var V := RandomReal ();

WriteLn (V >= 0.0 and V < 1.0);
```

```console
true
```

---

## RandomReal

*function* — unit `random`

**Function**

Returns the next value in the sequence, at least zero and below one.

**Declaration**

```algol24
function RandomReal () : Double;
```

**Remarks**

Zero may be answered and one never is.

The generator has 48 bits, so the result carries 48 bits of entropy rather than
the 53 a Double could hold. That is `drand48` itself, declared directly — its C
signature is exactly the shape wanted, so nothing wraps it.

Scale it for a range: `Low + RandomReal () * (High - Low)` lands in `[Low,
High)`. For a whole number use [`RandomInteger`](#randominteger) rather than
truncating this, which is the same thing said once instead of twice.

**See also**

[`Random`](#random), [`RandomInteger`](#randominteger), [`SetSeed`](#setseed)

**Example**

```algol24
uses random;

SetSeed (1);

WriteLn (RandomReal ());
WriteLn (RandomReal ());
```

```console
0.041630344771878214
0.45449244472862915
```

---

## ReadKey

*function* — unit `graph`

**Function**

Returns the next key, waiting for one if none is ready.

**Declaration**

```algol24
function ReadKey () : Char;
```

**Remarks**

One Char per key, always:

| the key | the answer |
| --- | --- |
| a typed character | itself, layout and shift applied — Unicode included, `é` and `你` are single answers |
| an arrow, function or navigation key | its [constant](#keys) — `KeyUp`, `KeyF1`, … |
| Enter, Escape, Tab, Backspace | `#13` `#27` `#9` `#8` |
| a Ctrl letter | its low character — Ctrl-A is `#1` |
| the window's close button | `KeyClose` |

Nothing echoes, as Turbo Pascal had it. What is **not** kept is TP's
extended-key protocol — `#0` and then a scan code on the next call — which
was the BIOS showing through, two reads per arrow.

While waiting, the blink phase keeps beating: the wait presents once a
frame, so `ReadKey` is also the idle a finished screen wants, and the demos
end with exactly that.

Raises `Graph is not open.` without a window.

**See also**

[`KeyPressed`](#keypressed), [`Keys`](#keys)

**Example**

The idiom is a loop, and it needs a keyboard:

    while ReadKey () <> #27 do
        Write ('.');

`examples/keys.a24` is that loop grown up — every key echoed by name until
Esc. What can run unattended is the empty-queue side:

```algol24
uses graph;

InitGraph (640, 480, 'keys', False);

System.WriteLn (KeyPressed ());

CloseGraph ();
System.WriteLn ('no key was waiting');
```

```console
false
no key was waiting
```

---

## Rectangle

*procedure, alias of `ViewPort.Rectangle`* — unit `graph`

**Function**

Draws a rectangle's outline on a viewport.

**Declaration**

```algol24
procedure Rectangle (V : ViewPort, X1 : Integer, Y1 : Integer, X2 : Integer, Y2 : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `Rectangle (V, …)` is
`V.Rectangle (…)`.

Four [`Line`](#line) calls in the current style and thickness.

⚠️ **The pen is put back where it was**, not left at the figure's corner: a
figure is not a journey, and [`Arc`](#arc), [`Circle`](#circle), [`Bar`](#bar)
and [`Bar3D`](#bar3d) keep the same rule. Only `Line`, [`LineTo`](#lineto) and
[`LineRel`](#linerel) move the pen, because moving it is what they are for.

The outline only; there is no filled form yet.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`Bar`](#bar), [`Bar3D`](#bar3d), [`Line`](#line), [`SetLineStyle`](#setlinestyle)

**Example**

See [`Line`](#line), which frames its chart with one.

---

## Round

*function* — unit `math`

**Function**

Returns its argument rounded to the nearest Integer.

**Declaration**

```algol24
function Round (X : Double) : Integer;
```

**Remarks**

⚠️ **A tie rounds to the EVEN neighbor, not away from zero.** `Round (2.5)` is
`2` and `Round (3.5)` is `4`; `Round (0.5)` is `0` and `Round (-2.5)` is `-2`.
This is not the schoolbook rule, which would answer `3` for `2.5`, and the
difference shows on exactly the values a test is most likely to try.

Two reasons for it. **Consistency downward:** ties-to-even is what IEEE 754
already does for every arithmetic result in the language, so a `Round` breaking
ties the other way would be the one operation disagreeing with the arithmetic
beneath it. **Bias:** rounding half away from zero sends every tie away from the
center and so pushes a column of figures upward on average, where ties-to-even
sends half each way and does not drift.

Away from a tie there is no subtlety — `Round (2.4)` is `2` and `Round (2.6)`
is `3`.

The result is an Integer proper, so it may be added to one or passed to
[`Odd`](#odd). An Integer argument is accepted and answers itself.

Every finite Double is answered exactly, on the same two paths as
[`Trunc`](#trunc): above `2^63` there is no nearest integer to search for, the
value being one already. Only `NaN` and the infinities are refused, raising
`Round of NaN.` and `Round of an infinity.`.

The primitive is libm's `llrint`, which rounds by the current floating-point
rounding direction. Nothing in the library changes that direction, and its
default is to-nearest-with-ties-to-even — the rule above.

**See also**

[`Frac`](#frac), [`Int`](#int), [`Trunc`](#trunc)

**Example**

```algol24
uses math;

WriteLn (Round (2.4));
WriteLn (Round (2.6));

for var V in [0.5, 1.5, 2.5, 3.5] do
    WriteLn (Str (V) + ' rounds to ' + Str (Round (V)));
```

```console
2
3
0.5 rounds to 0
1.5 rounds to 2
2.5 rounds to 2
3.5 rounds to 4
```

---

## ScreenHeight

*function* — unit `graph`

**Function**

Returns the desktop's height in pixels.

**Declaration**

```algol24
function ScreenHeight () : Integer;
```

**Remarks**

Everything said of [`ScreenWidth`](#screenwidth) holds here, for rows. Note
that on macOS a fullscreen window's height can be slightly less than this,
because the menu bar stays — [`GetMaxY`](#getmaxy) is the truth about the
window, this is the truth about the desktop.

**See also**

[`GetMaxY`](#getmaxy), [`InitGraph`](#initgraph), [`ScreenWidth`](#screenwidth)

**Example**

See [`ScreenWidth`](#screenwidth), which shows both.

---

## ScreenWidth

*function* — unit `graph`

**Function**

Returns the desktop's width in pixels.

**Declaration**

```algol24
function ScreenWidth () : Integer;
```

**Remarks**

Usable **before** [`InitGraph`](#initgraph), which is its purpose: it answers
how much room the desktop has, so a program can choose a window size to fit.
It is unchanged by windows opening and closing.

This and [`GetMaxX`](#getmaxx) answer different questions — how big a window
could be, versus how big this one is — and keeping the names apart is what
lets the `GetMaxX` drawing idiom stay safe in a window smaller than the
screen.

**See also**

[`GetMaxX`](#getmaxx), [`InitGraph`](#initgraph), [`ScreenHeight`](#screenheight)

**Example**

```algol24
uses graph;

// The desktop's size is the machine's, so only its soundness can be shown.
System.WriteLn (ScreenWidth () > 0);
System.WriteLn (ScreenHeight () > 0);
System.WriteLn (ScreenWidth () >= 640);
```

```console
true
true
true
```

---

## Sector

*procedure, alias of `ViewPort.Sector`* — unit `graph`

**Function**

Fills and outlines an elliptical sector on a viewport.

**Declaration**

```algol24
procedure Sector (V : ViewPort, X : Integer, Y : Integer, StartAngle : Integer,
                  EndAngle : Integer, XRadius : Integer, YRadius : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `Sector (V, …)` is `V.Sector (…)`.

[`PieSlice`](#pieslice) with two radii, exactly as [`Ellipse`](#ellipse) is
[`Circle`](#circle) with two — filled with the current fill pattern and color,
then outlined with the pen: the elliptical arc, and the two radii closing it
to the center.

With equal radii it *is* `PieSlice`: there is a test asserting the two paint
the same pixels. They share one primitive underneath, so they cannot drift
into filling different shapes.

Angles are the **parametric** ones [`Ellipse`](#ellipse) uses, so a sector of a
wide ellipse spans the same fraction of the figure that the same angles span of
a circle — which is what makes a pie chart drawn in perspective come out with
its shares intact. A sweep of 360 is the whole ellipse and draws no radii, the
two coinciding.

The fill and the outline are separate pens, and the pen is not moved.

Raises `Sector needs radii of zero or more.` and `CloseGraph has closed this
surface.`

**See also**

[`Ellipse`](#ellipse), [`FillEllipse`](#fillellipse),
[`GetArcCoords`](#getarccoords), [`PieSlice`](#pieslice),
[`SetFillStyle`](#setfillstyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'sector', False);

var V := ViewPort (40, 60, 520, 320, 1);

// A pie chart drawn in perspective: the same shares as a round one, laid on
// an ellipse, which is what a sector is for.
var Shares := [30, 25, 20, 15, 10];
var Inks   := [LightRed, Yellow, LightCyan, LightGreen, LightMagenta];
var At     := 0;

SetColor (V, White);

for var I := 0; I < 5; I := I + 1 do
begin
    SetFillStyle (V, SolidFill, Inks[I]);
    Sector (V, 250, 160, At * 36 div 10, (At + Shares[I]) * 36 div 10, 200, 90);

    At := At + Shares[I];
end

Show (V);

// Wide across, shallow down: the rim is 200 out but only 90 up, so a point
// 100 above the center is outside it and one 80 above is not.
System.WriteLn (V.Buf.GetInt ((159 * 480 + 429) * 4) <> 0);
System.WriteLn (V.Buf.GetInt ((79 * 480 + 249) * 4) <> 0);
System.WriteLn (V.Buf.GetInt ((59 * 480 + 249) * 4) <> 0);

CloseGraph ();
```

```console
true
true
false
```

---

## SetBkColor

*procedures, aliases of `ViewPort.SetBkColor` and `GetBkColor`* — unit `graph`

**Function**

Sets the color [`Clear`](#clear) returns a viewport to.

**Declaration**

```algol24
procedure SetBkColor (V : ViewPort, Color : Integer);
function  GetBkColor (V : ViewPort) : Integer;
```

**Remarks**

[Aliases](#aliases) of the methods: `SetBkColor (V, …)` is
`V.SetBkColor (…)`.

⚠️ **[`Transparent`](#colors) by default**, which is what a fresh viewport
already is and what lets whatever lies beneath show through when it is
cleared. Give it a color and [`Clear`](#clear) paints that instead. This is a
viewport's answer to a window's [`TextBackground`](#textbackground), which has
always governed what `ClrScr` fills.

Turbo Pascal's `SetBkColor` set the whole screen's background — there was one
screen and no stack to see through. It is per surface here for the same reason
everything else is.

⚠️ **Nothing else consults it.** In particular `EmptyFill` still paints
nothing at all rather than painting this: leaving pixels alone is what *empty*
has to mean on a stack of surfaces, and painting them would hide what is
under them. See [`FillStyles`](#fillstyles).

It is a third pen, independent of [`SetColor`](#setcolor)'s and
[`SetFillStyle`](#setfillstyle)'s; setting one never disturbs another.

Raises `SetBkColor wants a color or Transparent.` below `Transparent`, and
`CloseGraph has closed this surface.` once the window has gone.

**See also**

[`Clear`](#clear), [`ClearViewPort`](#clearviewport), [`Colors`](#colors),
[`SetFillStyle`](#setfillstyle), [`TextBackground`](#textbackground)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'bk', False);

var Under := ViewPort (50, 50, 200, 100, 1);
var Over  := ViewPort (50, 50, 200, 100, 2);

SetFillStyle (Under, SolidFill, Red);
Bar (Under, 1, 1, 200, 100);

// Transparent by default, so clearing the upper viewport lets the lower one
// show through it again.
SetFillStyle (Over, SolidFill, Yellow);
Bar (Over, 1, 1, 200, 100);
ClearViewPort (Over);

System.WriteLn ('transparent: ', GetPixel (Over, 20, 20) = Transparent);

// Given a background, clearing paints that instead.
SetBkColor (Over, Blue);
Bar (Over, 1, 1, 200, 100);
ClearViewPort (Over);

System.WriteLn ('blue:        ', GetPixel (Over, 20, 20) = Blue);
System.WriteLn ('reported:    ', GetBkColor (Over) = Blue);

CloseGraph ();
```

```console
transparent: true
blue:        true
reported:    true
```

---

## SetBlinkRate

*procedure* — unit `graph`

**Function**

Sets how long each blink phase lasts, in milliseconds.

**Declaration**

```algol24
procedure SetBlinkRate (Ms : Integer);
```

**Remarks**

Blinking ink is visible for `Ms` milliseconds and hidden for the same, so a
full blink takes twice this. Two cadences with history behind them:

| | |
| --- | --- |
| `267` | the adapter's own — sixteen frames of sixty each way; the default |
| `533` | the slower, one-second blink of the console caret that came after |

The rate is a program-level setting: [`TextMode`](#textmode) and
[`NormVideo`](#normvideo) reset the colors and the blink flag, never this. A
fresh `InitGraph` restores 267.

Turbo Pascal offered no such method, because the rate lived in a video
register — the hardware's limitation, not a design worth inheriting.

Raises `SetBlinkRate needs a positive time.` for an `Ms` of zero or less.

**See also**

[`Blink`](#blink), [`TextColor`](#textcolor)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'cadence', False);

SetBlinkRate (533);

TextColor (Yellow + Blink);
Write ('the unhurried blink');

CloseGraph ();
System.WriteLn ('set');
```

```console
set
```

---

## SetColor

*procedure, alias of `ViewPort.SetColor`* — unit `graph`

**Function**

Sets the color a viewport's pen draws in.

**Declaration**

```algol24
procedure SetColor (V : ViewPort, Color : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `SetColor (V, …)` is
`V.SetColor (…)`.

Turbo Pascal's `SetColor`, and it governs **everything the surface draws** —
the [`Line`](#line) family and the [`OutText`](#outtext) pair alike. One pen
for lines and letters both. A fresh viewport draws in `White`.

[`PutPixel`](#putpixel) is the exception: it names its own color and ignores
the pen.

Raises `SetColor wants an RGB color.` for a negative, there being no
transparent ink, and `CloseGraph has closed this surface.` once the window has
gone.

**See also**

[`Colors`](#colors), [`GetColor`](#getcolor), [`Line`](#line), [`SetLineStyle`](#setlinestyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'color', False);

var V := ViewPort (10, 10, 100, 60, 1);

SetColor (V, LightCyan);
Line (V, 1, 1, 99, 59);
OutTextXY (V, 4, 4, 'the same pen');
Show (V);

System.WriteLn (GetColor (V) = LightCyan);

CloseGraph ();
```

```console
true
```

---

## SetFillStyle

*procedures, aliases of `ViewPort.SetFillStyle` and `SetFillPattern`* — unit `graph`

**Function**

Sets the pattern and color figures are filled with.

**Declaration**

```algol24
procedure SetFillStyle (V : ViewPort, Style : Integer, Color : Integer);
procedure SetFillPattern (V : ViewPort, Rows : List, Color : Integer);

function GetFillStyle (V : ViewPort) : Integer;
function GetFillColor (V : ViewPort) : Integer;
```

**Remarks**

[Aliases](#aliases) of the methods: `SetFillStyle (V, …)` is
`V.SetFillStyle (…)`.

⚠️ **The fill is a separate pen from the one [`SetColor`](#setcolor) sets.** A
figure may be outlined in one color and filled in another, which is how Turbo
Pascal's `Bar3D` and `PieSlice` were drawn. Setting one never disturbs the
other.

`Style` is one of the [fill patterns](#fillstyles) — an eight-by-eight bit
pattern tiled across whatever is being filled. `SetFillPattern` supplies eight
rows of the caller's own instead, each the eight bits of one row with the high
bit leftmost, and sets the style to `UserFill`:

```algol24
SetFillPattern (V, [255, 129, 129, 129, 129, 129, 129, 255], Yellow);   // a box
```

⚠️ **The pattern is tiled from the surface's origin, not the figure's.** Two
figures side by side therefore share one weave rather than each starting its
own, which is what makes a patterned fill look woven rather than tiled.

A fresh viewport fills `SolidFill` in `White`, as it draws.

Nothing consults this state yet — the figures that take a fill are still to
come. It is settled first because they will all want it, and because the
choice between one pen and two is theirs to inherit rather than to argue.

Raises `SetFillStyle wants a known pattern.`, `SetFillStyle wants UserFill
from SetFillPattern.` for `UserFill` itself, `SetFillStyle wants an RGB
color.`, `SetFillPattern wants eight rows.`, and `SetFillPattern wants an RGB
color.`

**See also**

[`Colors`](#colors), [`FillStyles`](#fillstyles),
[`GetFillPattern`](#getfillpattern), [`GetFillSettings`](#getfillsettings),
[`SetColor`](#setcolor), [`SetLineStyle`](#setlinestyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'fill', False);

var V := ViewPort (10, 10, 100, 100, 1);

SetFillStyle (V, HatchFill, LightGreen);

System.WriteLn (GetFillStyle (V) = HatchFill);
System.WriteLn (GetFillColor (V) = LightGreen);

// The fill pen and the drawing pen are separate.
SetColor (V, Red);
System.WriteLn (GetColor (V) = Red);
System.WriteLn (GetFillColor (V) = LightGreen);

// A pattern of one's own: a hollow box, eight by eight.
SetFillPattern (V, [255, 129, 129, 129, 129, 129, 129, 255], Yellow);
System.WriteLn (GetFillStyle (V) = UserFill);

CloseGraph ();
```

```console
true
true
true
true
true
```

---

## SetLineStyle

*procedure, alias of `ViewPort.SetLineStyle`* — unit `graph`

**Function**

Sets the pattern and thickness a viewport draws lines with.

**Declaration**

```algol24
procedure SetLineStyle (V : ViewPort, Style : Integer, Pattern : Integer, Thickness : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `SetLineStyle (V, …)` is
`V.SetLineStyle (…)`.

`Style` is one of the [line styles](#linestyles). The style's sixteen bits are
walked along the line, one bit to the pixel and repeating, so a `DottedLn`
hundred-pixel line paints fifty. `Pattern` is consulted **only** for
`UserBitLn`; the other styles carry their own bits.

⚠️ **Thickness spreads across the axis the line runs least along**, which is
what keeps a thick diagonal from breaking into a staircase of blobs.
`NormWidth` is 1 and `ThickWidth` is 3, and any positive count is allowed —
Turbo Pascal permitted only those two.

Raises `SetLineStyle wants a known style.`, `SetLineStyle needs a thickness of
1 or more.`, and `CloseGraph has closed this surface.`

**See also**

[`Line`](#line), [`LineStyles`](#linestyles), [`SetColor`](#setcolor)

**Example**

See [`Line`](#line), which draws dashed and solid from one viewport.

---

## SetSeed

*procedure* — unit `random`

**Function**

Seeds the generator so that the sequence following is repeatable.

**Declaration**

```algol24
procedure SetSeed (Seed : Integer);
```

**Remarks**

The same seed always yields the same sequence. This is what makes a program
using `random` testable, and every test in the unit begins with it.

⚠️ **Every bit of `Seed` contributes, which took work.** `srand48` beneath keeps
only the low 32 bits of its argument — measured, not assumed: raw `srand48`
gives seeds 42, 42 + 2³² and 42 + 2³³ the same stream. An Algol-24 Integer is
unbounded, so passing one straight through would silently collide distinct
seeds. The C wrapper folds the high half onto the low half first.

A negative seed is accepted and behaves like any other.

**See also**

[`Random`](#random), [`Randomize`](#randomize), [`RandomReal`](#randomreal)

**Example**

```algol24
uses random;

SetSeed (99);
WriteLn (RandomInteger (1000));
WriteLn (RandomInteger (1000));

SetSeed (99);
WriteLn (RandomInteger (1000));
```

```console
380
504
380
```

---

## SetTextJustify

*procedure, alias of `ViewPort.SetTextJustify`* — unit `graph`

**Function**

Sets where free text sits relative to the point it is placed at.

**Declaration**

```algol24
procedure SetTextJustify (V : ViewPort, Horiz : Integer, Vert : Integer);
```

**Remarks**

An [alias](#aliases) of the method: `SetTextJustify (V, …)` is
`V.SetTextJustify (…)`.

`Horiz` is [`LeftText`, `CenterText` or `RightText`](#textjustify) and says
which end of the text meets the point; `Vert` is `TopText`, `CenterText` or
`BottomText` and says which edge of the line does. A fresh viewport is
`LeftText, TopText` — Turbo Pascal's default, and the top-left corner
[`OutTextXY`](#outtextxy) has always placed.

This is what [`TextWidth`](#textwidth) and `TextHeight` are for, applied by the
unit instead of by the caller. Centering a label becomes one call and one
placement rather than a measurement and a subtraction:

```algol24
SetTextJustify (V, CenterText, CenterText);
OutTextXY (V, 150, 50, 'centered');
```

⚠️ **Justification is measured along the text's own run and across it**, not
along the screen's axes. Under [`SetTextStyle`](#settextstyle) a turned string
justifies about its own start, middle and end — so `RightText` on text running
downward puts the *end* of the run at the point. Upright, which is the
ordinary case, the two readings are the same.

It governs [`OutText`](#outtext) as well, which is Turbo Pascal's rule.
Chaining `OutText` calls only reads sensibly under `LeftText`, the advance
being the width of what was drawn either way.

Raises `SetTextJustify wants a known justification.`, and `CloseGraph has
closed this surface.` once the window has gone.

**See also**

[`OutText`](#outtext), [`OutTextXY`](#outtextxy),
[`SetTextStyle`](#settextstyle), [`TextJustify`](#textjustify),
[`TextWidth`](#textwidth)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'justify', False);

var V := ViewPort (50, 50, 300, 100, 1);

SetColor (V, White);

// Centered about the middle of the viewport both ways, in one call. This is
// what TextWidth and TextHeight are for, applied by the unit rather than by
// the caller.
SetTextJustify (V, CenterText, CenterText);
OutTextXY (V, 150, 50, 'centered');
Show (V);

var Left := 9999;
var Right := 0;

for var Y := 1; Y <= 100; Y := Y + 1 do
    for var X := 1; X <= 300; X := X + 1 do
        if GetPixel (V, X, Y) <> Transparent then
        begin
            if X < Left then Left := X;
            if X > Right then Right := X;
        end

System.WriteLn ('the label is ', TextWidth (V, 'centered'), ' wide');
System.WriteLn ('its ink runs ', Left, ' to ', Right);
System.WriteLn ('straddling   ', (Left + Right) div 2);

CloseGraph ();
```

```console
the label is 128 wide
its ink runs 88 to 211
straddling   149
```

---

## SetTextStyle

*method of [`ViewPort`](#viewport)* — unit `graph`

**Function**

Sets the direction and size free text on a viewport is drawn in.

**Declaration**

```algol24
procedure SetTextStyle (V : ViewPort, Direction : Integer, CharSize : Integer);
```

**Remarks**

`Direction` is a quarter turn **counterclockwise** in degrees — `0`, `90`,
`180` or `270`, and anything else raises. A turn of 90° sends the glyph's top
edge to the left, so the text reads bottom to top: a chart's Y-axis label,
which is what the turn was wanted for.

`CharSize` magnifies the glyphs by a whole number, `1` being the font's own
size. Magnification replicates pixels, so a doubled glyph covers four times
the area and the antialiasing doubles with it — crisp rather than blurred,
and unmistakably scaled rather than redrawn.

⚠️ **The turned text is anchored at the same corner.** A 90° turn puts a 16
× 32 glyph into a 32 × 16 box whose top-left is the `OutTextXY` position, so
the text grows *right and down* from that point as it always does; only the
letters lie on their side. The advance follows the turn — right, up, left or
down — so successive [`OutText`](#viewport) calls read in the direction set.

⚠️ **Celled text has neither setting, and cannot.** A rotated glyph does not
fit a half-width cell, and a magnified one does not fit at all — which is why
these live on the viewport and not beside [`TextColor`](#textcolor). The
style is the viewport's own; the grid never inherits it.

Turbo Pascal's `SetTextStyle` took a font as its first argument, selecting
among `.CHR` stroked-font files. Here the glyph file installed by
[`InstallUserFont`](#installuserfont) *is* the font, so selecting one is
installing one and the parameter has nothing left to choose.

Raises `SetTextStyle wants 0, 90, 180 or 270 degrees.` and `SetTextStyle
needs a CharSize of 1 or more.`

**See also**

[`InstallUserFont`](#installuserfont), [`OutTextXY`](#outtextxy), [`ViewPort`](#viewport)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'style', False);

var Chart := ViewPort (1, 1, 400, 300, 1);

// A Y-axis label, reading bottom to top, at double size.
Chart.SetTextStyle (90, 2);
Chart.OutTextXY (20, 280, 'millivolts');

Chart.SetTextStyle (0, 1);
Chart.OutTextXY (60, 290, 'seconds');
Chart.Show ();

System.WriteLn (Chart.Dir);
System.WriteLn (Chart.Size);

CloseGraph ();
```

```console
0
1
```

---

## Show

*procedure, alias of `ViewPort.Show`* — unit `graph`

**Function**

Pushes a viewport's pixels to the screen.

**Declaration**

```algol24
procedure Show (V : ViewPort);
```

**Remarks**

An [alias](#aliases) of the method: `Show (V)` is `V.Show ()`.

Drawing writes into the viewport's own buffer and is not visible until
something presents the stack; this is that. So is any text verb, and so is
[`ReadKey`](#readkey) while it waits — a program that ends on `ReadKey` need
not call `Show` at all.

Raises `CloseGraph has closed this surface.` once the window it belongs to
has gone.

**See also**

[`Line`](#line), [`PutPixel`](#putpixel), [`ReadKey`](#readkey)

**Example**

See [`PutPixel`](#putpixel) and [`Line`](#line), which both end with one.

---

## Sin

*function* — unit `math`

**Function**

Returns the sine of an angle given in radians.

**Declaration**

```algol24
function Sin (X : Double) : Double;
```

**Remarks**

`X` is in **radians**. Multiply a degree measure by `Pi / 180`.

`Sin (Pi / 2)` answers exactly `1.0`, which is the fortunate case; see
[`Cos`](#cos) for the one that is not.

A foreign declaration onto libm's `sin`.

**See also**

[`ArcTan`](#arctan), [`Cos`](#cos), [`Pi`](#pi)

**Example**

```algol24
uses math;

WriteLn (Sin (0.0));
WriteLn (Sin (Pi / 2));
```

```console
0.0
1.0
```

---

## Sqr

*function* — unit `math`

**Function**

Returns the square of its argument.

**Declaration**

```algol24
function Sqr (X);
```

**Remarks**

The result has the same type as `X`, so an Integer squares to an Integer and a
Double to a Double. Integers being unbounded, the square of a value near the
machine width grows rather than wrapping.

Untyped, with the same consequence for a declared variable as `Abs`.

Raises `Operands must be numbers.` for an argument the multiplication cannot
take.

Note that `Sqr` squares and [`Sqrt`](#sqrt) takes the root — the names are close
and the operations are opposite.

**See also**

[`Abs`](#abs), [`Sqrt`](#sqrt), [`Trunc`](#trunc)

**Example**

```algol24
uses math;

WriteLn (Sqr (12));
WriteLn (Sqr (2.5));
```

```console
144
6.25
```

---

## Sqrt

*function* — unit `math`

**Function**

Returns the non-negative square root of its argument.

**Declaration**

```algol24
function Sqrt (X : Double) : Double;
```

**Remarks**

A negative argument answers `NaN` rather than raising, which is libm's behavior
and not this unit's choice. `NaN` is not equal to itself, which is how to detect
it.

A foreign declaration onto libm's `sqrt`.

**See also**

[`Exp`](#exp), [`Ln`](#ln), [`Sqr`](#sqr)

**Example**

The hypotenuse of a 3–4–5 triangle, which shows `Sqr` and `Sqrt` together.

```algol24
uses math;

WriteLn (Sqrt (2.0));

var A := 3.0;
var B := 4.0;

WriteLn (Sqrt (Sqr (A) + Sqr (B)));
```

```console
1.4142135623730951
5.0
```

---

## TextBackground

*procedure* — unit `graph`

**Function**

Sets the background cells are painted with.

**Declaration**

```algol24
procedure TextBackground (Color : Integer);              // the screen
procedure TextBackground (W : Window, Color : Integer);  // that window
```

**Remarks**

`TextBackground (W, Color)` is an [alias](#aliases) of `W.TextBackground`,
setting that window's background alone.

`Transparent` — the default — leaves everything beneath the glyph visible,
which is the merged world's natural state. Any RGB color makes the cell
opaque, which is what a menu bar or a readable HUD over a busy scene wants.

The setting applies to cells painted from now on; nothing already on the
grid changes.

Raises `TextBackground wants a color or Transparent.` for a negative that is
not `Transparent`.

**See also**

[`Colors`](#colors), [`ClrScr`](#clrscr), [`TextColor`](#textcolor)

**Example**

```algol24
uses graph;

InitGraph (320, 200, 'bg', False);

TextBackground (Blue);
TextColor (Yellow);
Write ('classic');

System.WriteLn ('painted');

CloseGraph ();
```

```console
painted
```

---

## TextColor

*procedure* — unit `graph`

**Function**

Sets the ink celled text draws with.

**Declaration**

```algol24
procedure TextColor (Color : Integer);              // the screen
procedure TextColor (W : Window, Color : Integer);  // that window
```

**Remarks**

`TextColor (W, Color)` is an [alias](#aliases) of `W.TextColor`, setting
that window's ink alone.

Any RGB Integer; the sixteen [color constants](#colors) are the classic
vocabulary, and adding [`Blink`](#blink) makes the ink blink. The default is
`LightGray`, which is what a text mode woke up in.

Ink is the one thing a cell always has, so `Transparent` raises
`TextColor cannot be transparent.`

**See also**

[`Colors`](#colors), [`Write`](#write), [`TextBackground`](#textbackground)

**Example**

See [`TextBackground`](#textbackground), which sets both.

---

## TextCols

*function* — unit `graph`

**Function**

Returns the grid's width in cells.

**Declaration**

```algol24
function TextCols () : Integer;
```

**Remarks**

80, until [`TextMode`](#textmode) says otherwise. The grid is **logical**,
not a division of the window: an 80 × 25 screen is 80 × 25 in every window,
scaled to fit at present time. A wide (CJK) glyph occupies two of its cells.

Raises `Graph is not open.` without a window.

**See also**

[`GetMaxX`](#getmaxx), [`TextMode`](#textmode), [`TextRows`](#textrows)

**Example**

```algol24
uses graph;

InitGraph (1280, 800, 'classic', False);

System.WriteLn (TextCols ());
System.WriteLn (TextRows ());

CloseGraph ();
```

```console
80
25
```

---

## TextJustify

*constants* — unit `graph`

**Function**

The justifications [`SetTextJustify`](#settextjustify) takes.

**Declaration**

```algol24
const LeftText   := 0;      const BottomText := 0;
const CenterText := 1;      const TopText    := 2;
const RightText  := 2;
```

**Remarks**

Turbo Pascal's five names and its five values, `CenterText` serving both axes.

⚠️ **`BottomText` and `LeftText` are both zero**, as they were there, so
passing one where the other belongs is not caught — `SetTextJustify` sees the
number, not which name was written. It is worth reading the second argument
twice.

The horizontal three say which end of the text meets the point; the vertical
three say which edge of the line does. `LeftText, TopText` is what a fresh
viewport holds.

**See also**

[`SetTextJustify`](#settextjustify), [`SetTextStyle`](#settextstyle)

**Example**

See [`SetTextJustify`](#settextjustify).

---

## TextMode

*procedure* — unit `graph`

**Function**

Chooses the grid: columns by rows of cells, whatever the window's size.

**Declaration**

```algol24
procedure TextMode (Cols : Integer, Rows : Integer);
```

**Remarks**

The screen clears, the cursor homes, and the colors return to their defaults
— which is what Turbo Pascal's `TextMode` did on a mode change, and the
behavior a program switching modes wants.

Turbo Pascal's argument was a video-card mode — `CO80`, `CO40`, `Font8x8` —
because the grid was a register setting. Here the grid is logical, so the
argument is simply the grid: the two classics are `TextMode (80, 25)` and
`TextMode (40, 25)`, and nothing stops `TextMode (132, 50)`.

⚠️ **The window does not change; the grid is scaled to it.** A side effect
worth having: an 80 × 25 grid shown in a 4:3 window — 640 × 480, say —
displays its cells at 1:2.4, which is exactly how real text mode looked on a
4:3 tube, whose pixels were not square.

Raises `Graph is not open.` without a window, and `TextMode needs a positive
grid.` for a Cols or Rows of zero or less.

**See also**

[`ClrScr`](#clrscr), [`TextCols`](#textcols), [`TextRows`](#textrows)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'modes', False);

System.WriteLn (TextCols ());

TextMode (40, 25);

System.WriteLn (TextCols ());
System.WriteLn (TextRows ());

CloseGraph ();
```

```console
80
40
25
```

---

## TextRows

*function* — unit `graph`

**Function**

Returns the grid's height in cells.

**Declaration**

```algol24
function TextRows () : Integer;
```

**Remarks**

25, until [`TextMode`](#textmode) says otherwise, on the same reasoning as
[`TextCols`](#textcols), which shows both.

Raises `Graph is not open.` without a window.

**See also**

[`TextCols`](#textcols), [`TextMode`](#textmode)

**Example**

See [`TextCols`](#textcols).

---

## TextWidth

*functions* — unit `graph`

**Function**

How much room a string would take up, in pixels.

**Declaration**

```algol24
function TextWidth (Text : String) : Integer;                  // the background
function TextWidth (V : ViewPort, Text : String) : Integer;    // that viewport

function TextHeight (Text : String) : Integer;                 // the background
function TextHeight (V : ViewPort, Text : String) : Integer;   // that viewport
```

**Remarks**

The viewport forms are [aliases](#aliases) of `V.TextWidth` and
`V.TextHeight`.

⚠️ **This has to be asked for; it cannot be worked out.** Glyphs are not one
width — a CJK ideograph is twice a Latin letter, an emoji likewise, and a code
the font lacks is half a cell — so `Length (Text) * CellWidth ()` is wrong for
anything but plain ASCII, and the per-glyph widths are private to the unit.

```algol24
TextWidth (V, 'A')      // 16
TextWidth (V, '你')     // 32, on the same font
```

What it measures is the box the text would occupy **on the surface**, in the
turn and magnification [`SetTextStyle`](#settextstyle) is holding. Width and
height are the surface's axes rather than the string's, so a quarter turn
swaps them:

| turn | `TextWidth` | `TextHeight` |
| --- | --- | --- |
| 0, 180 | the glyphs end to end | one line of the font |
| 90, 270 | one line of the font | the glyphs end to end |

Both scale with `CharSize`. Half a turn runs the text the other way but
occupies the same box as none.

`TextWidth` is exactly what [`OutText`](#outtext) advances the position by,
computed once and shared, so laying text out by hand lands where drawing it
would. Turned a quarter, the advance is `TextHeight` instead — it is the one
along the direction of travel.

The **screen-wide forms measure the background**, which has no
`SetTextStyle` of its own and so never turns or magnifies.

An empty string occupies nothing, both ways. `TextHeight` takes a string it
barely consults — the font is one height throughout — but takes one as Turbo
Pascal's did, so that an empty string can answer nothing and so a font of
varying height would not change the signature.

Raises `CloseGraph has closed this surface.` on a viewport whose window has
gone, and `Graph is not open.` for the screen-wide forms without a window.

**See also**

[`CellWidth`](#cellwidth), [`OutText`](#outtext), [`OutTextXY`](#outtextxy),
[`SetTextStyle`](#settextstyle)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'measure', False);

var V := ViewPort (40, 40, 300, 120, 1);

// Length times a cell width is wrong the moment a string leaves ASCII: a CJK
// ideograph is twice a Latin letter, and the font knows which is which.
System.WriteLn ('A is      ', TextWidth (V, 'A'), ' wide');
System.WriteLn ('你 is     ', TextWidth (V, '你'), ' wide');
System.WriteLn ('a line is ', TextHeight (V, 'A'), ' tall');

// Centering needs the real width, so it needs to be asked for.
var Caption := 'Hello 你好';
var Left    := (300 - TextWidth (V, Caption)) div 2 + 1;

SetColor (V, LightCyan);
OutTextXY (V, Left, 50, Caption);
Show (V);

System.WriteLn ('centered at ', Left, ' of 300');

// Turned a quarter, the same string occupies the other way about.
SetTextStyle (V, 90, 1);
System.WriteLn ('turned:   ', TextWidth (V, Caption), ' by ',
                TextHeight (V, Caption));

CloseGraph ();
```

```console
A is      16 wide
你 is     32 wide
a line is 32 tall
centered at 71 of 300
turned:   32 by 160
```

---

## Trunc

*function* — unit `math`

**Function**

Returns the integer part of its argument, discarding any fractional part.

**Declaration**

```algol24
function Trunc (X : Double) : Integer;
```

**Remarks**

Truncation is **toward zero**, so `Trunc (-3.7)` is `-3` rather than `-4`, which
differs from flooring for a negative argument. Truncating toward zero is what
makes `Trunc` and [`Int`](#int) one operation in two return types, and what
leaves [`Frac`](#frac) carrying the sign of its argument.

The result is an Integer proper, not a Double carrying a whole number, so it may
be used wherever an Integer is wanted — added to one, passed to
[`Odd`](#odd), rendered by `Str` without a `.0`.

An Integer argument is accepted as readily as a Double, and answers itself.

⚠️ **Every finite Double is answered exactly, however large.** An Algol-24
Integer is unbounded, so there is no width for a result to exceed. `Trunc
(1.0E300)` is the exact 301-digit integer — not a refusal, and not an
approximation.

That takes two paths, and they meet without a seam. Below `2^63` a C conversion
does the truncating. At or above it a Double has no fractional bits left and is
already a whole number, so there is nothing to truncate: the value is rebuilt
from its mantissa and exponent using unbounded Integer arithmetic.

Only two arguments are refused, neither having an integer to answer with:

| Argument | Result |
| --- | --- |
| `NaN` | raises `Trunc of NaN.` |
| either infinity | raises `Trunc of an infinity.` |

Unlike the other foreign-backed routines here, `Trunc` does not call libm for
its main primitive. libm's `trunc` returns a `double`, and a foreign declaration
must name a C function whose real signature is the one wanted, so `alg_trunc` is
written in `mathffi.c` and built by `./build.sh` — along with the two calls that
take a Double apart into mantissa and exponent.

**See also**

[`Frac`](#frac), [`Int`](#int), [`Round`](#round)

**Example**

```algol24
uses math;

WriteLn (Trunc (3.7));
WriteLn (Trunc (-3.7));
WriteLn (Trunc (2.999999));

WriteLn (Trunc (7.5) + Trunc (2.5));

// Far beyond what a C conversion could answer, and still exact.
WriteLn (Trunc (1.0E30));

try
    WriteLn (Trunc (1.0 / 0.0));
except
    on e : String do WriteLn (e);
end
```

```console
3
-3
2
9
1000000000000000019884624838656
Trunc of an infinity.
```

---

## ViewPort

*class* — unit `graph`

**Function**

A positioned, sized pixel surface with a place in the stack.

**Declaration**

```algol24
constructor ViewPort (X : Integer, Y : Integer, W : Integer, H : Integer,
                      Order : Integer);

procedure MoveTo (PX : Integer, PY : Integer);
procedure PutPixel (PX : Integer, PY : Integer, Color : Integer);
function  GetPixel (PX : Integer, PY : Integer) : Integer;
procedure FloodFill (X : Integer, Y : Integer, Border : Integer);
function  GetArcCoords () : ArcCoords;
function  GetFillPattern () : List;
function  GetFillSettings () : FillSettings;
function  GetLineSettings () : LineSettings;
procedure SetBkColor (Color : Integer);
function  GetBkColor () : Integer;
procedure SetTextJustify (Horiz : Integer, Vert : Integer);
function  TextWidth (Text : String) : Integer;
function  TextHeight (Text : String) : Integer;
procedure Clear ();
procedure OutText (Text : String);
procedure OutTextXY (PX : Integer, PY : Integer, Text : String);
procedure SetTextStyle (Direction : Integer, CharSize : Integer);
procedure Show ();

// the pen -- see Line
procedure SetColor (Color : Integer);
function  GetColor () : Integer;
procedure SetLineStyle (Style : Integer, Pattern : Integer, Thickness : Integer);
procedure PenTo (PX : Integer, PY : Integer);
procedure PenRel (DX : Integer, DY : Integer);
function  GetX () : Integer;
function  GetY () : Integer;
procedure Line (X1 : Integer, Y1 : Integer, X2 : Integer, Y2 : Integer);
procedure LineTo (PX : Integer, PY : Integer);
procedure LineRel (DX : Integer, DY : Integer);
procedure Rectangle (X1 : Integer, Y1 : Integer, X2 : Integer, Y2 : Integer);

// readable, and the first four settable
X, Y : Integer          the top-left pixel on screen
Order : Integer         under the root grid when negative, over it when positive
Alpha : Integer         0 invisible .. 255 opaque
W, H : Integer          the size, fixed at creation
Dir, Size : Integer     the text turn and magnification -- see SetTextStyle
HJust, VJust : Integer  the justification -- see SetTextJustify
```

**Remarks**

Turbo Pascal's `SetViewPort` named a clipped drawing region; this is that
grown into an object. `X, Y` place it in the **logical space the grid lives in** — the same space
a [`Window`](#window)'s cells are measured in, so the two scale together and
a chart placed inside a window stays inside it at every window size; see
[`CellWidth`](#cellwidth). `W, H` size it, and `Order` stacks it — negative under the root grid, positive over it, and
0 is refused because that is the root grid's own.

⚠️ **Coordinates inside a viewport are local**, counting from 1: `1, 1` is
*its* top-left corner, not the screen's. That is what makes the drawing
portable — [`MoveTo`](#viewport) the viewport and everything on it moves,
nothing redraws.

⚠️ **Local is also the clip.** A pixel outside the viewport is discarded
silently rather than raising, so a drawing cannot scribble on its neighbors
and there is no clipping rectangle to maintain.

The pen — [`Line`](#line) and its family — draws in these coordinates too.
`MoveTo` moves the **viewport**; [`PenTo`](#line) moves the **pen**.

Unpainted pixels are transparent, so a fresh viewport shows everything
beneath it; `Alpha` ghosts the whole surface at once. Both `Alpha` and
`Order` may be assigned after creation — reordering is bring-to-front.

`OutTextXY` draws **free** text: a graphic that happens to be letters, with
no cursor, no wrap, and no text verb that will ever erase it — see
[`Write`](#write) for the celled kind. `OutText` draws at the viewport's own
current position and advances it.
[`SetTextStyle`](#settextstyle) turns that text by a quarter and magnifies
it, neither of which celled text can have.

Drawing does not appear until something presents; `Show` is that, and
[`ReadKey`](#readkey) does it too while waiting.

`CloseGraph` invalidates every surface: a method called afterward raises
`CloseGraph has closed this surface.`

Raises `Graph is not open.` without a window, `Order 0 is the screen's.`,
and `ViewPort needs a positive size.`

**See also**

Its methods each have a free-function [alias](#aliases) with an entry of its
own: [`Arc`](#arc), [`Bar`](#bar), [`Bar3D`](#bar3d), [`Circle`](#circle),
[`DrawPoly`](#drawpoly),
[`Ellipse`](#ellipse), [`FillEllipse`](#fillellipse),
[`FillPoly`](#fillpoly), [`Line`](#line),
[`LineTo`](#lineto), [`PieSlice`](#pieslice), [`Sector`](#sector), [`LineRel`](#linerel), [`SetFillStyle`](#setfillstyle),
[`Rectangle`](#rectangle), [`PenTo`](#pento), [`PenRel`](#penrel),
[`GetX`](#getx), [`GetY`](#gety), [`SetColor`](#setcolor),
[`GetColor`](#getcolor), [`SetLineStyle`](#setlinestyle),
[`PutPixel`](#putpixel), [`GetPixel`](#getpixel), [`FloodFill`](#floodfill),
[`GetArcCoords`](#getarccoords),
[`OutText`](#outtext), [`OutTextXY`](#outtextxy),
[`SetTextStyle`](#settextstyle), [`Show`](#show), [`MoveTo`](#moveto),
[`ClearViewPort`](#clearviewport), [`TextWidth`](#textwidth),
[`SetBkColor`](#setbkcolor), [`SetTextJustify`](#settextjustify),
[`GetFillPattern`](#getfillpattern), [`GetFillSettings`](#getfillsettings),
`GetLineSettings`.

[`Clear`](#clear) is the method `ClearViewPort (V)` reaches.

[`CellWidth`](#cellwidth), [`ReadKey`](#readkey), [`Window`](#window)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'viewport', False);

var Chart := ViewPort (100, 60, 200, 120, 2);

Chart.PutPixel (1, 1, Yellow);
Chart.PutPixel (200, 120, Yellow);
Chart.PutPixel (201, 1, Red);
Chart.OutTextXY (8, 8, 'a label');
Chart.Show ();

System.WriteLn (Chart.W);
System.WriteLn (Chart.H);
System.WriteLn (Chart.Order);

Chart.Order := -1;
System.WriteLn (Chart.Order);

CloseGraph ();
```

```console
200
120
2
-1
```

---

## WhereX

*function* — unit `graph`

**Function**

Returns the cursor's column.

**Declaration**

```algol24
function WhereX () : Integer;              // the screen
function WhereX (W : Window) : Integer;    // that window
```

**Remarks**

One-based, like [`GotoXY`](#gotoxy), which shows it read back: 1 at the left
edge, `TextCols ()` at the right. `WhereX (W)` is an [alias](#aliases) of
`W.WhereX`, answering that window's cursor instead.

Raises `Graph is not open.` without a window.

**See also**

[`GotoXY`](#gotoxy), [`WhereY`](#wherey)

**Example**

See [`GotoXY`](#gotoxy) and [`Write`](#write).

---

## WhereY

*function* — unit `graph`

**Function**

Returns the cursor's row.

**Declaration**

```algol24
function WhereY () : Integer;              // the screen
function WhereY (W : Window) : Integer;    // that window
```

**Remarks**

One-based. Everything said of [`WhereX`](#wherex) holds here.

**See also**

[`GotoXY`](#gotoxy), [`WhereX`](#wherex)

**Example**

See [`GotoXY`](#gotoxy) and [`WriteLn`](#write).

---

## Window

*class* — unit `graph`

**Function**

A rectangle of text cells with a place in the stack.

**Declaration**

```algol24
constructor Window (X1 : Integer, Y1 : Integer, X2 : Integer, Y2 : Integer,
                    Order : Integer);

procedure MoveTo (Col : Integer, Row : Integer);
procedure GotoXY (Col : Integer, Row : Integer);
function  WhereX () : Integer;
function  WhereY () : Integer;
procedure TextColor (Color : Integer);
procedure TextBackground (Color : Integer);
procedure HighVideo ();
procedure LowVideo ();
procedure NormVideo ();
procedure Write (Text : String);
procedure WriteLn (Text : String);
procedure ClrEol ();
procedure Clear ();
procedure DelLine ();
procedure InsLine ();

// readable, and Order and Alpha settable
X1, Y1, X2, Y2 : Integer    the corner cells, inclusive
Cols, Rows : Integer        the size in cells
Order : Integer             under the root grid when negative, over it when positive
Alpha : Integer             0 invisible .. 255 opaque
```

**Remarks**

Turbo Pascal's `Window (X1, Y1, X2, Y2)` grown into an object. The corners
are cells of the root grid's space, **inclusive and one-based**, so
`Window (1, 1, 80, 25)` is the whole screen and `Window (10, 5, 49, 19)` is
40 × 15 cells.

⚠️ **A window is a terminal of its own.** It carries the whole text
vocabulary as methods, each acting on its own cells: its own cursor
(`1, 1` is *its* corner), its own ink and background, its own blink ink, and
its own scrolling — `WriteLn` at the window's bottom row scrolls **that
window's** cells and nothing else's. `ClrScr` clears it alone.

That is the design's one rule, and where it pays: `B.ClrEol` erases B's
prose and cannot touch a chart on another surface, however they overlap on
screen.

`Order` stacks it — negative under the root grid, positive over it, 0
refused as the root grid's own — and ties stack by creation, earlier
beneath later. Because text is a stackable surface, a window may sit **over**
a graphic that is itself over other text, which no single text plane can
express.

`MoveTo` drags the window, keeping its size; any position is allowed, off the
grid included, and what falls outside is clipped at present time.

⚠️ **The wrap is deferred**, as text mode always had it: a line that exactly
fills the last column leaves the cursor resting *on* that column with the
wrap owed, and it happens only when another character arrives. Without this
a window's own bottom border would scroll the window as it was drawn.
[`GotoXY`](#window) and `ClrScr` cancel a wrap that is owed.

An `InstallUserFont` of a different cell size rebuilds every live window at
the same columns and rows — the cells come back empty.

`CloseGraph` invalidates every surface: a method called afterward raises
`CloseGraph has closed this surface.`

Raises `Graph is not open.` without a window, `Order 0 is the screen's.`,
and `Window wants 1 <= X1 <= X2 and 1 <= Y1 <= Y2.`

**See also**

Its methods each have a free-function [alias](#aliases) with an entry of its
own: [`Write`](#write), `WriteLn`, [`GotoXY`](#gotoxy), [`WhereX`](#wherex),
[`WhereY`](#wherey), [`TextColor`](#textcolor),
[`TextBackground`](#textbackground), [`HighVideo`](#highvideo),
[`LowVideo`](#lowvideo), [`NormVideo`](#normvideo), [`ClrEol`](#clreol),
[`ClrScr`](#clrscr), [`MoveTo`](#moveto), [`DelLine`](#delline),
`InsLine`.

[`Clear`](#clear) is the method `ClrScr (W)` reaches.

[`CellWidth`](#cellwidth), [`ViewPort`](#viewport)

**Example**

`examples/surfaces.a24` drags one window over another's chart; this shows
that a window's cursor and the screen's are different things.

```algol24
uses graph;

InitGraph (640, 480, 'windows', False);

var W := Window (10, 5, 49, 19, 1);

W.TextBackground (Blue);
W.Clear ();
W.Write ('its own cursor and colors');

System.WriteLn (W.Cols);
System.WriteLn (W.Rows);
System.WriteLn (W.WhereX ());
System.WriteLn (WhereX ());

W.MoveTo (2, 2);
System.WriteLn (W.X2);

CloseGraph ();
```

```console
40
15
26
1
41
```

---

## Write

*procedures* — unit `graph`

**Function**

Writes to the screen's text grid, advancing its cursor.

**Declaration**

```algol24
procedure Write (Values : List of Any);     // the screen, or a leading Window
procedure WriteLn (Values : List of Any);
```

**Remarks**

The screen is the root [`Window`](#window), and these are its `Write` and
`WriteLn` — celled text that wraps at the last column, scrolls the grid *and
only the grid* at the bottom row, and is what [`ClrEol`](#clreol) and
[`ClrScr`](#clrscr) erase. A `#10` in a value starts a new row, a wide (CJK)
glyph takes two cells, and `WriteLn ()` is a bare newline.

Variadic, and each value rendered as `Str` renders it, which is the built-in's
own contract [RT-015] kept: `Write ('a', 42, ' ', 3.5)` writes `a42 3.5`.

⚠️ **These shadow the built-ins for any program that says `uses graph`**, and
that is the intent — in a graphics program `WriteLn` should reach the window
rather than standard output. The console remains reachable under its own
unit's name:

```algol24
WriteLn ('this lands in the window');
System.WriteLn ('this lands on the console');
```

`System.Write` and `System.WriteLn` are the built-ins exactly as they were,
variadic and untouched, and every example in this reference uses them to
report a value while a window is open. Nothing is lost by the shadowing; it
only has to be named.

Free, pixel-placed text is [`OutTextXY`](#outtextxy)'s job, and the two never
meet. A [`Window`](#window) has its own `Write` and `WriteLn` writing into its
own cells.

Raises `Graph is not open.` without a window.

**See also**

[`ClrScr`](#clrscr), [`GotoXY`](#gotoxy), [`TextColor`](#textcolor), [`Window`](#window)

**Example**

```algol24
uses graph;

InitGraph (640, 480, 'write', False);

Write ('Hi ', 42, ' 你好');
WriteLn ();

System.WriteLn (WhereX ());
System.WriteLn (WhereY ());

CloseGraph ();
```

```console
1
2
```
