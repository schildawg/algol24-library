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
| [`ArcTan`](#arctan) | function | `math` |
| [`AssertNear`](#assertnear) | procedure | `testing` |
| [`Black` … `White`](#colors) | constants | `graph` |
| [`CloseGraph`](#closegraph) | procedure | `graph` |
| [`ClrEol`](#clreol) | procedure | `graph` |
| [`ClrScr`](#clrscr) | procedure | `graph` |
| [`Cos`](#cos) | function | `math` |
| [`Exp`](#exp) | function | `math` |
| [`Frac`](#frac) | function | `math` |
| [`GetMaxX`](#getmaxx) | function | `graph` |
| [`GetMaxY`](#getmaxy) | function | `graph` |
| [`GotoXY`](#gotoxy) | procedure | `graph` |
| [`HighVideo`](#highvideo) | procedure | `graph` |
| [`Infinity`](#infinity) | constant | `math` |
| [`InitGraph`](#initgraph) | procedure | `graph` |
| [`InstallUserFont`](#installuserfont) | procedure | `graph` |
| [`Int`](#int) | function | `math` |
| [`IsInfinite`](#isinfinite) | function | `math` |
| [`IsNaN`](#isnan) | function | `math` |
| [`Ln`](#ln) | function | `math` |
| [`LowVideo`](#lowvideo) | procedure | `graph` |
| [`Max`](#max) | function | `math` |
| [`Min`](#min) | function | `math` |
| [`NaN`](#nan) | constant | `math` |
| [`NormVideo`](#normvideo) | procedure | `graph` |
| [`Odd`](#odd) | function | `math` |
| [`OutText`](#outtext) | procedure | `graph` |
| [`OutTextXY`](#outtextxy) | procedure | `graph` |
| [`Pi`](#pi) | constant | `math` |
| [`Print`](#print) | procedure | `graph` |
| [`PrintLn`](#println) | procedure | `graph` |
| [`Random`](#random) | function | `random` |
| [`RandomInteger`](#randominteger) | function | `random` |
| [`Randomize`](#randomize) | procedure | `random` |
| [`RandomReal`](#randomreal) | function | `random` |
| [`Round`](#round) | function | `math` |
| [`ScreenHeight`](#screenheight) | function | `graph` |
| [`ScreenWidth`](#screenwidth) | function | `graph` |
| [`SetSeed`](#setseed) | procedure | `random` |
| [`TextBackground`](#textbackground) | procedure | `graph` |
| [`TextColor`](#textcolor) | procedure | `graph` |
| [`TextMode`](#textmode) | procedure | `graph` |
| [`TextCols`](#textcols) | function | `graph` |
| [`TextRows`](#textrows) | function | `graph` |
| [`Transparent`](#colors) | constant | `graph` |
| [`Sin`](#sin) | function | `math` |
| [`Sqr`](#sqr) | function | `math` |
| [`Sqrt`](#sqrt) | function | `math` |
| [`Trunc`](#trunc) | function | `math` |
| [`WhereX`](#wherex) | function | `graph` |
| [`WhereY`](#wherey) | function | `graph` |

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
answers 2¹²⁷, where C's `abs(INT_MIN)` is undefined behaviour.

Negative zero is normalised: `Abs (-0.0)` is `0.0`, not `-0.0`. The comparison
is `<=` rather than `<` for exactly this reason — `-0.0 < 0` is false, so `<`
would hand `-0.0` straight back.

Being untyped, the result cannot initialise a variable of a written type:
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

WriteLn ('closed without complaint');
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
procedure ClrEol ();
```

**Remarks**

The cleared cells take the current [`TextBackground`](#textbackground), and
the cursor does not move.

⚠️ **Only the grid is touched.** A graphic beneath the cleared cells — a
chart a window was sitting on — shows through, intact. Erasing a graphic is a
graphics act; this is a text verb, and text verbs act on text. That is the
design's one rule, and this routine is where it is most visible.

Raises `Graph is not open.` without a window.

**See also**

[`ClrScr`](#clrscr), [`Print`](#print), [`TextBackground`](#textbackground)

**Example**

```algol24
uses graph;

InitGraph (320, 200, 'eol', False);

Print ('doomed text');
GotoXY (0, 0);
ClrEol ();

WriteLn (WhereX ());

CloseGraph ();
```

```console
0
```

---

## ClrScr

*procedure* — unit `graph`

**Function**

Clears the whole grid and homes the cursor.

**Declaration**

```algol24
procedure ClrScr ();
```

**Remarks**

Every cell takes the current [`TextBackground`](#textbackground) —
`TextBackground (Blue); ClrScr;` is how a classic screen paints its field in
two lines. With the background `Transparent` the grid becomes wholly
invisible and everything beneath shows.

Graphics are not touched, on the same rule as [`ClrEol`](#clreol).

Raises `Graph is not open.` without a window.

**See also**

[`ClrEol`](#clreol), [`TextBackground`](#textbackground)

**Example**

```algol24
uses graph;

InitGraph (320, 200, 'scr', False);

GotoXY (5, 3);
TextBackground (Blue);
ClrScr ();

WriteLn (WhereX ());
WriteLn (WhereY ());

CloseGraph ();
```

```console
0
0
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

WriteLn (White);
WriteLn (Blue);
WriteLn (Transparent);
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

## GetMaxX

*function* — unit `graph`

**Function**

Returns the highest X a program can draw to — the window's width minus one.

**Declaration**

```algol24
function GetMaxX () : Integer;
```

**Remarks**

The minus one is the point of the routine: `for var X := 0; X <= GetMaxX ();`
visits every column exactly, which is the idiom drawing code is written in.

⚠️ **The answer describes the window that actually opened**, not the request.
Under a fullscreen [`InitGraph`](#initgraph) it is the desktop's width minus
one, whatever size was asked for.

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

for var X := 0; X <= GetMaxX (); X := X + 1 do
    Columns := Columns + 1;

WriteLn (Columns);

CloseGraph ();
```

```console
100
```

---

## GetMaxY

*function* — unit `graph`

**Function**

Returns the highest Y a program can draw to — the window's height minus one.

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

## GotoXY

*procedure* — unit `graph`

**Function**

Moves the cursor to a column and row of the grid.

**Declaration**

```algol24
procedure GotoXY (Col : Integer, Row : Integer);
```

**Remarks**

⚠️ **Zero-based**: the top-left cell is `0, 0`, matching the language's
strings and the window's pixels. Turbo Pascal counted from 1, and that habit
is the one to unlearn here.

A position off the grid raises `GotoXY is off the grid.` rather than being
clamped or ignored — the cursor is always somewhere real, and a wrong
computation is caught where it happened.

Raises `Graph is not open.` without a window.

**See also**

[`Print`](#print), [`WhereX`](#wherex), [`WhereY`](#wherey)

**Example**

```algol24
uses graph;

InitGraph (320, 200, 'goto', False);

GotoXY (10, 5);

WriteLn (WhereX ());
WriteLn (WhereY ());

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
procedure HighVideo ();
```

**Remarks**

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
Print ('now yellow');

NormVideo ();
PrintLn (' and back to normal');

WriteLn (WhereY ());

CloseGraph ();
```

```console
1
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

The window is `Width` by `Height` pixels, centred on the desktop, with `Title`
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

WriteLn (GetMaxX ());
WriteLn (GetMaxY ());

CloseGraph ();
```

```console
639
479
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
WriteLn ('two fonts, one window');
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
procedure LowVideo ();
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

Like `Abs`, `Max` is untyped, so its result cannot initialise a variable of a
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
procedure NormVideo ();
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
procedure OutText (Text : String);
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
lands. Ink is white, until the unit grows a way to choose a color.

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
WriteLn ('drawn');
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
procedure OutTextXY (X : Integer, Y : Integer, Text : String);
```

**Remarks**

The current position is neither consulted nor moved, which is Turbo Pascal's
rule and the useful one: placed text is placed, and a following
[`OutText`](#outtext) carries on from wherever it was.

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

## Print

*procedure* — unit `graph`

**Function**

Writes text at the cursor, in the current colors, advancing it.

**Declaration**

```algol24
procedure Print (Text : String);
```

**Remarks**

This is **celled** text: it lands on the grid, wraps at the last column,
scrolls the grid — and only the grid — at the bottom row, and is what
[`ClrEol`](#clreol) and [`ClrScr`](#clrscr) erase. Free, pixel-placed text is
[`OutTextXY`](#outtextxy)'s job, and the two never meet.

A `#10` in the text starts a new row. A wide (CJK) glyph takes two cells,
an emoji likewise, and a codepoint the font lacks takes one blank cell.

The name is a placeholder with a settled future: when the compiler learns to
overload built-ins, `Write` and `WriteLn` absorb `Print` and
[`PrintLn`](#println).

Raises `Graph is not open.` without a window.

**See also**

[`GotoXY`](#gotoxy), [`PrintLn`](#println), [`TextColor`](#textcolor)

**Example**

Three narrow glyphs, then two wide ones at two cells each.

```algol24
uses graph;

InitGraph (320, 200, 'print', False);

Print ('Hi 你好');

WriteLn (WhereX ());

CloseGraph ();
```

```console
7
```

---

## PrintLn

*procedure* — unit `graph`

**Function**

Writes text and moves the cursor to the start of the next row.

**Declaration**

```algol24
procedure PrintLn (Text : String);
```

**Remarks**

`PrintLn ('')` is a bare newline. At the bottom row the grid scrolls, taking
the current background into the vacated band; graphics stay put.

Everything said of [`Print`](#print) holds here, the placeholder name
included.

Raises `Graph is not open.` without a window.

**See also**

[`Print`](#print)

**Example**

```algol24
uses graph;

InitGraph (320, 200, 'lines', False);

PrintLn ('one');
PrintLn ('two');

WriteLn (WhereY ());

CloseGraph ();
```

```console
2
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

var Colours := ['red', 'green', 'blue'];

for var I := 0; I < 4; I := I + 1 do
    WriteLn (Colours[RandomInteger (Colours.Length)]);
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

## Round

*function* — unit `math`

**Function**

Returns its argument rounded to the nearest Integer.

**Declaration**

```algol24
function Round (X : Double) : Integer;
```

**Remarks**

⚠️ **A tie rounds to the EVEN neighbour, not away from zero.** `Round (2.5)` is
`2` and `Round (3.5)` is `4`; `Round (0.5)` is `0` and `Round (-2.5)` is `-2`.
This is not the schoolbook rule, which would answer `3` for `2.5`, and the
difference shows on exactly the values a test is most likely to try.

Two reasons for it. **Consistency downward:** ties-to-even is what IEEE 754
already does for every arithmetic result in the language, so a `Round` breaking
ties the other way would be the one operation disagreeing with the arithmetic
beneath it. **Bias:** rounding half away from zero sends every tie away from the
centre and so pushes a column of figures upward on average, where ties-to-even
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
WriteLn (ScreenWidth () > 0);
WriteLn (ScreenHeight () > 0);
WriteLn (ScreenWidth () >= 640);
```

```console
true
true
true
```

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

A negative argument answers `NaN` rather than raising, which is libm's behaviour
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
procedure TextBackground (Color : Integer);
```

**Remarks**

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
Print ('classic');

WriteLn ('painted');

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
procedure TextColor (Color : Integer);
```

**Remarks**

Any RGB Integer; the sixteen [color constants](#colors) are the classic
vocabulary. The default is `LightGray`, which is what a text mode woke up in.

Ink is the one thing a cell always has, so `Transparent` raises
`TextColor cannot be transparent.`

**See also**

[`Colors`](#colors), [`Print`](#print), [`TextBackground`](#textbackground)

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

WriteLn (TextCols ());
WriteLn (TextRows ());

CloseGraph ();
```

```console
80
25
```

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
behaviour a program switching modes wants.

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

WriteLn (TextCols ());

TextMode (40, 25);

WriteLn (TextCols ());
WriteLn (TextRows ());

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

## WhereX

*function* — unit `graph`

**Function**

Returns the cursor's column.

**Declaration**

```algol24
function WhereX () : Integer;
```

**Remarks**

Zero-based, like [`GotoXY`](#gotoxy), which shows it read back.

Raises `Graph is not open.` without a window.

**See also**

[`GotoXY`](#gotoxy), [`WhereY`](#wherey)

**Example**

See [`GotoXY`](#gotoxy) and [`Print`](#print).

---

## WhereY

*function* — unit `graph`

**Function**

Returns the cursor's row.

**Declaration**

```algol24
function WhereY () : Integer;
```

**Remarks**

Zero-based. Everything said of [`WhereX`](#wherex) holds here.

**See also**

[`GotoXY`](#gotoxy), [`WhereX`](#wherex)

**Example**

See [`GotoXY`](#gotoxy) and [`PrintLn`](#println).
