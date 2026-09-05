#!/usr/bin/env python3
"""The library's font generator: hello-scripts' makefont.py, extended.

Two changes against the original:

1. Coverage grows to the whole CP437 repertoire (the 'full character set' of
   the era TurboVision lived in), Latin-1, Latin Extended-A and Greek --
   everything rendered from Arial Unicode as before.

2. The geometry-critical characters -- box drawing, block elements, shades,
   and the arrows/triangles the scrollbars need -- are SYNTHESIZED, not
   rendered from a font. A frame character must touch its cell's edges so
   that adjacent cells connect without seams, and a TTF glyph rendered at 80%
   size and centered never does. Terminals draw these themselves for the same
   reason.
"""
import sys, unicodedata
from PIL import Image, ImageDraw, ImageFont

FONT  = "/System/Library/Fonts/Supplemental/Arial Unicode.ttf"
EMOJI = "/System/Library/Fonts/Apple Color Emoji.ttc"

COLORED = "🙂🌍🎉👋⭐🐈"

CP437_PICTURES = "☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶§▬↨↑↓→←∟↔▲▼⌂"
CP437_HIGH     = bytes(range(0x80, 0x100)).decode("cp437")

TEXT = (
    "".join(chr(c) for c in range(0x20, 0x7F)) +
    CP437_PICTURES + CP437_HIGH +
    "".join(chr(c) for c in range(0xA1, 0x100)) +            # Latin-1
    "".join(chr(c) for c in range(0x100, 0x180)) +           # Latin Extended-A
    "".join(chr(c) for c in range(0x391, 0x3CA) if c != 0x3A2) +  # Greek
    "".join(chr(c) for c in range(0x2500, 0x2570)) +         # box drawing used below
    "▀▄█▌▐░▒▓■▲▼◄►↑↓→←↔↕∟" +
    "Hello in five languages"
    "Merhaba Turkce Türkçe"
    "Geia sou Ellinika"
    "Γεια σου Ελληνικά"
    "你好 中文"
    "안녕하세요 한국어"
    "Hola Espanol Español ¡"
    "0123456789 ()%,.:-"
)

# ---------------------------------------------------------------- synthesis --

def box_rects(cp, W, H, t):
    """Exclusive-coordinate rectangles for one box-drawing character, or None.

    Positions: XC/YC center a single line; X1,X2/Y1,Y2 are the two lines of a
    double. Corners are exact: an outer line meets its perpendicular outer
    line, an inner meets inner, so double frames keep their inner gap.

    The gap between a double's two lines is the same ABSOLUTE size in both
    directions -- one line-thickness -- as the VGA font had it. Spreading the
    pair proportionally to the cell was tried first and made every corner
    join a tall spread to a narrow one, which read as a vertically stretched
    frame.
    """
    XC, YC = (W - t) // 2, (H - t) // 2
    X1 = (W - 3 * t) // 2
    X2 = X1 + 2 * t
    Y1 = (H - 3 * t) // 2
    Y2 = Y1 + 2 * t
    R = {
        "─": [(0, W, YC, YC + t)],
        "│": [(XC, XC + t, 0, H)],
        "┌": [(XC, W, YC, YC + t), (XC, XC + t, YC, H)],
        "┐": [(0, XC + t, YC, YC + t), (XC, XC + t, YC, H)],
        "└": [(XC, W, YC, YC + t), (XC, XC + t, 0, YC + t)],
        "┘": [(0, XC + t, YC, YC + t), (XC, XC + t, 0, YC + t)],
        "├": [(XC, XC + t, 0, H), (XC, W, YC, YC + t)],
        "┤": [(XC, XC + t, 0, H), (0, XC + t, YC, YC + t)],
        "┬": [(0, W, YC, YC + t), (XC, XC + t, YC, H)],
        "┴": [(0, W, YC, YC + t), (XC, XC + t, 0, YC + t)],
        "┼": [(0, W, YC, YC + t), (XC, XC + t, 0, H)],
        "═": [(0, W, Y1, Y1 + t), (0, W, Y2, Y2 + t)],
        "║": [(X1, X1 + t, 0, H), (X2, X2 + t, 0, H)],
        "╔": [(X1, W, Y1, Y1 + t), (X2, W, Y2, Y2 + t),
              (X1, X1 + t, Y1, H), (X2, X2 + t, Y2, H)],
        "╗": [(0, X2 + t, Y1, Y1 + t), (0, X1 + t, Y2, Y2 + t),
              (X2, X2 + t, Y1, H), (X1, X1 + t, Y2, H)],
        "╚": [(X1, W, Y2, Y2 + t), (X2, W, Y1, Y1 + t),
              (X1, X1 + t, 0, Y2 + t), (X2, X2 + t, 0, Y1 + t)],
        "╝": [(0, X2 + t, Y2, Y2 + t), (0, X1 + t, Y1, Y1 + t),
              (X2, X2 + t, 0, Y2 + t), (X1, X1 + t, 0, Y1 + t)],
        "╠": [(X1, X1 + t, 0, H), (X2, X2 + t, 0, Y1 + t), (X2, X2 + t, Y2, H),
              (X2, W, Y1, Y1 + t), (X2, W, Y2, Y2 + t)],
        "╣": [(X2, X2 + t, 0, H), (X1, X1 + t, 0, Y1 + t), (X1, X1 + t, Y2, H),
              (0, X1 + t, Y1, Y1 + t), (0, X1 + t, Y2, Y2 + t)],
        "╦": [(0, W, Y1, Y1 + t), (0, X1 + t, Y2, Y2 + t), (X2, W, Y2, Y2 + t),
              (X1, X1 + t, Y2, H), (X2, X2 + t, Y2, H)],
        "╩": [(0, W, Y2, Y2 + t), (0, X1 + t, Y1, Y1 + t), (X2, W, Y1, Y1 + t),
              (X1, X1 + t, 0, Y1 + t), (X2, X2 + t, 0, Y1 + t)],
        "╬": [(0, X1 + t, Y1, Y1 + t), (X2, W, Y1, Y1 + t),
              (0, X1 + t, Y2, Y2 + t), (X2, W, Y2, Y2 + t),
              (X1, X1 + t, 0, Y1 + t), (X2, X2 + t, 0, Y1 + t),
              (X1, X1 + t, Y2, H), (X2, X2 + t, Y2, H)],
        "╒": [(XC, W, Y1, Y1 + t), (XC, W, Y2, Y2 + t), (XC, XC + t, Y1, H)],
        "╓": [(XC, W, YC, YC + t), (X1, X1 + t, YC, H), (X2, X2 + t, YC, H)],
        "╕": [(0, XC + t, Y1, Y1 + t), (0, XC + t, Y2, Y2 + t), (XC, XC + t, Y1, H)],
        "╖": [(0, XC + t, YC, YC + t), (X1, X1 + t, YC, H), (X2, X2 + t, YC, H)],
        "╘": [(XC, W, Y1, Y1 + t), (XC, W, Y2, Y2 + t), (XC, XC + t, 0, Y2 + t)],
        "╙": [(XC, W, YC, YC + t), (X1, X1 + t, 0, YC + t), (X2, X2 + t, 0, YC + t)],
        "╛": [(0, XC + t, Y1, Y1 + t), (0, XC + t, Y2, Y2 + t), (XC, XC + t, 0, Y2 + t)],
        "╜": [(0, XC + t, YC, YC + t), (X1, X1 + t, 0, YC + t), (X2, X2 + t, 0, YC + t)],
        "╞": [(XC, XC + t, 0, H), (XC, W, Y1, Y1 + t), (XC, W, Y2, Y2 + t)],
        "╟": [(X1, X1 + t, 0, H), (X2, X2 + t, 0, H), (X2, W, YC, YC + t)],
        "╡": [(XC, XC + t, 0, H), (0, XC + t, Y1, Y1 + t), (0, XC + t, Y2, Y2 + t)],
        "╢": [(X1, X1 + t, 0, H), (X2, X2 + t, 0, H), (0, X1 + t, YC, YC + t)],
        "╤": [(0, W, Y1, Y1 + t), (0, W, Y2, Y2 + t), (XC, XC + t, Y2, H)],
        "╥": [(0, W, YC, YC + t), (X1, X1 + t, YC, H), (X2, X2 + t, YC, H)],
        "╧": [(0, W, Y1, Y1 + t), (0, W, Y2, Y2 + t), (XC, XC + t, 0, Y1 + t)],
        "╨": [(0, W, YC, YC + t), (X1, X1 + t, 0, YC + t), (X2, X2 + t, 0, YC + t)],
        "╪": [(XC, XC + t, 0, H), (0, W, Y1, Y1 + t), (0, W, Y2, Y2 + t)],
        "╫": [(X1, X1 + t, 0, H), (X2, X2 + t, 0, H), (0, W, YC, YC + t)],
        "▀": [(0, W, 0, H // 2)],
        "▄": [(0, W, H // 2, H)],
        "█": [(0, W, 0, H)],
        "▌": [(0, W // 2, 0, H)],
        "▐": [(W // 2, W, 0, H)],
        "▬": [(W // 8, W - W // 8, H // 2, H // 2 + max(2 * t, 3))],
    }
    return R.get(chr(cp))


def synthesize(cp, W, H):
    """A geometry-critical glyph as an L image, or None to use the font."""
    t = max(1, W // 8)
    img = Image.new("L", (W, H), 0)
    px = img.load()
    ch = chr(cp)

    rects = box_rects(cp, W, H, t)
    if rects is not None:
        for (x0, x1, y0, y1) in rects:
            for y in range(y0, y1):
                for x in range(x0, x1):
                    px[x, y] = 255
        return img

    if ch in "░▒▓":
        # The dither is scaled with the cell -- a 1-pixel checker at twice
        # VGA's cell size reads as flat gray, not as the chunky shade the
        # era's scrollbars were made of.
        s2 = max(1, W // 8)
        keep = {"░": lambda x, y: x % 2 == 0 and y % 2 == 0,
                "▒": lambda x, y: (x + y) % 2 == 0,
                "▓": lambda x, y: not (x % 2 == 0 and y % 2 == 0)}[ch]
        for y in range(H):
            for x in range(W):
                if keep(x // s2, y // s2):
                    px[x, y] = 255
        return img

    if ch == "■":
        m = W // 8
        side = W - 2 * m
        y0 = (H - side) // 2
        for y in range(y0, y0 + side):
            for x in range(m, W - m):
                px[x, y] = 255
        return img

    if ch in "▲▼":
        h = H // 2
        y0 = H // 4
        for i in range(h):
            row = i if ch == "▲" else h - 1 - i
            half = max(1, (row + 1) * W // (2 * h))
            for x in range(W // 2 - half, W // 2 + half):
                if 0 <= x < W:
                    px[x, y0 + i] = 255
        return img

    if ch in "◄►":
        w = (3 * W) // 4
        x0 = W // 8
        for i in range(w):
            col = i if ch == "►" else w - 1 - i
            half = max(1, (w - col) * H // (2 * w) * 3 // 4)
            x = x0 + i if ch == "►" else x0 + (w - 1 - i)
            for y in range(H // 2 - half, H // 2 + half):
                px[(x0 + i), y] = 255 if ch == "►" else px[x0 + i, y]
        if ch == "◄":
            for i in range(w):
                half = max(1, (i + 1) * H // (2 * w) * 3 // 4)
                for y in range(H // 2 - half, H // 2 + half):
                    px[x0 + i, y] = 255
        return img

    def shaft_v(x, y0, y1):
        for y in range(y0, y1):
            for dx in range(t):
                px[x + dx, y] = 255

    def shaft_h(y, x0, x1):
        for x in range(x0, x1):
            for dy in range(t):
                px[x, y + dy] = 255

    def head_up(cy):
        for i in range(H // 8):
            half = (H // 8 - i) * W // (2 * (H // 8)) + 1
            for x in range(W // 2 - half, W // 2 + half):
                if 0 <= x < W:
                    px[x, cy + i] = 255

    def head_down(cy):
        for i in range(H // 8):
            half = (i + 1) * W // (2 * (H // 8))
            for x in range(W // 2 - half, W // 2 + half):
                if 0 <= x < W:
                    px[x, cy - i] = 255

    def head_left(cx):
        for i in range(W // 3):
            half = (W // 3 - i) * H // (4 * (W // 3)) + 1
            for y in range(H // 2 - half, H // 2 + half):
                px[cx + i, y] = 255

    def head_right(cx):
        for i in range(W // 3):
            half = (i + 1) * H // (4 * (W // 3))
            for y in range(H // 2 - half, H // 2 + half):
                px[cx - i, y] = 255

    XC = (W - t) // 2
    if ch == "↑":
        shaft_v(XC, H // 4, 3 * H // 4); head_up(H // 4); return img
    if ch == "↓":
        shaft_v(XC, H // 4, 3 * H // 4); head_down(3 * H // 4); return img
    if ch == "↕":
        shaft_v(XC, H // 5, 4 * H // 5); head_up(H // 5); head_down(4 * H // 5); return img
    if ch == "→":
        shaft_h((H - t) // 2, W // 8, 7 * W // 8); head_right(7 * W // 8); return img
    if ch == "←":
        shaft_h((H - t) // 2, W // 8, 7 * W // 8); head_left(W // 8); return img
    if ch == "↔":
        shaft_h((H - t) // 2, W // 8, 7 * W // 8)
        head_left(W // 8); head_right(7 * W // 8); return img
    if ch == "∟":
        shaft_v(W // 4, H // 4, 3 * H // 4)
        shaft_h(3 * H // 4 - t, W // 4, 7 * W // 8); return img

    return None

# --------------------------------------------------- the original machinery --

def is_wide(ch):
    return unicodedata.east_asian_width(ch) in ("W", "F")

def render(ch, font, cell_w, cell_h, baseline):
    img = Image.new("L", (cell_w, cell_h), 0)
    try:
        box = font.getbbox(ch)
    except Exception:
        box = (0, 0, 0, 0)
    w = box[2] - box[0]

    if w > cell_w:
        wide = Image.new("L", (w + 2, cell_h), 0)
        ImageDraw.Draw(wide).text((-box[0], baseline), ch, font=font, fill=255, anchor="ls")
        return wide.resize((cell_w, cell_h), Image.LANCZOS)

    ImageDraw.Draw(img).text((-box[0] + max(0, (cell_w - w) // 2), baseline),
                             ch, font=font, fill=255, anchor="ls")
    return img

def main():
    args = sys.argv[1:]
    gray = False
    cell_h = 16

    if args and args[0] == "--gray":
        gray, cell_h, args = True, int(args[1]), args[2:]

    size     = int(cell_h * 0.80)
    baseline = int(cell_h * 0.78)
    font     = ImageFont.truetype(FONT, size)

    seen, out = set(), []
    if gray:
        out.append("# gray8 %d" % cell_h)
        color = ImageFont.truetype(EMOJI, 160)

        for ch in COLORED:
            cp = ord(ch)
            seen.add(cp)

            img = Image.new("RGBA", (200, 200), (0, 0, 0, 0))
            ImageDraw.Draw(img).text((0, 0), ch, font=color, embedded_color=True)

            box = img.getbbox() or (0, 0, 160, 160)
            px  = img.crop(box).resize((cell_h, cell_h), Image.LANCZOS).load()

            body = "".join("%02X%02X%02X%02X" % px[x, y]
                           for y in range(cell_h) for x in range(cell_h))
            out.append("%04X+%s" % (cp, body))

    for ch in sorted(set(TEXT)):
        cp = ord(ch)
        if cp in seen or cp < 0x20:
            continue
        seen.add(cp)

        cell_w = cell_h if is_wide(ch) else cell_h // 2

        synth = synthesize(cp, cell_w, cell_h)
        px = (synth if synth is not None
              else render(ch, font, cell_w, cell_h, baseline)).load()

        if gray:
            body = "".join("%02X" % px[x, y]
                           for y in range(cell_h) for x in range(cell_w))
        else:
            rows = []
            for y in range(cell_h):
                bits = 0
                for x in range(cell_w):
                    if px[x, y] > 110:
                        bits |= 1 << (cell_w - 1 - x)
                rows.append(("%04X" if cell_w > 8 else "%02X") % bits)
            body = "".join(rows)

        out.append("%04X:%s" % (cp, body))

    with open(args[0], "w") as f:
        f.write("\n".join(out) + "\n")

    print("%d glyphs -> %s" % (len(out) - (1 if gray else 0), args[0]))

main()
