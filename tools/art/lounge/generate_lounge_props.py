#!/usr/bin/env python3
"""
Art generator for Floofy's Headless Lounge (R2, LORE §12 / §19.0, ART-183 #18).

Writes (native 1x, drawn for the 2x 640x480 canvas, OW outline #141018):
- assets/props/ruins_lounge_flowerwall.png  (40x40 wall of flowers)
- assets/props/ruins_lounge_lights.png      (80x14 string of fairy lights)
- assets/props/ruins_lounge_beanbag.png     (40x32 heart-shaped beanbag)
- assets/props/ruins_lounge_rope.png        (64x28 velvet rope between two posts)
- assets/props/ruins_lounge_neon.png        (60x30 GOOD VIBES ONLY neon sign)
- assets/props/ruins_lounge_vanity.png      (52x50 vanity, 40 bulbs, one dead)
- assets/props/ruins_lounge_throne.png      (40x46 plush throne)
- assets/props/ruins_lounge_headrack.png    (80x52 rack of empty fursuit heads)
- assets/props/ruins_lounge_flowerbed.png   (3 frames of 56x28, "shaking")
- assets/props/ruins_lounge_flowerbed.json  (sheet metadata)

Usage:
  C:/Python313/python.exe tools/art/lounge/generate_lounge_props.py [--preview DIR]
"""

from __future__ import annotations

import json
import math
import os
import random
import sys

from PIL import Image, ImageDraw

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
PROPS = os.path.join(REPO_ROOT, "assets", "props")


def rgb(h: int) -> tuple[int, int, int, int]:
    return ((h >> 16) & 255, (h >> 8) & 255, h & 255, 255)


OUTLINE = rgb(0x141018)
CLEAR = (0, 0, 0, 0)

# Lounge palette: plum shadows, pinks, lilacs, warm bulb whites.
PLUM_DEEP = rgb(0x2A1636)
PLUM = rgb(0x3F2352)
PLUM_LIGHT = rgb(0x5A3573)
LILAC_DARK = rgb(0x8B64B8)
LILAC = rgb(0xB48CE0)
LILAC_LIGHT = rgb(0xD8BCF4)
PINK_DARK = rgb(0xB8508A)
PINK_MID = rgb(0xE07AAE)
PINK = rgb(0xFF9FCB)
PINK_LIGHT = rgb(0xFFC8E2)
WHITE = rgb(0xFFF0F5)
NEON = rgb(0xFF4FB0)
NEON_HOT = rgb(0xFFB0E0)
NEON_GLOW = rgb(0x8A2A78)
GOLD_DARK = rgb(0xA5731A)
GOLD = rgb(0xE8B93A)
GOLD_LIGHT = rgb(0xFFE88A)
LEAF_DARK = rgb(0x24502F)
LEAF = rgb(0x3F8A4D)
LEAF_LIGHT = rgb(0x6CBF6F)
WOOD_DARK = rgb(0x3A2430)
WOOD = rgb(0x5C3A48)
WOOD_LIGHT = rgb(0x7E5262)
SOIL_DARK = rgb(0x2E1C26)
SOIL = rgb(0x4A2E3A)
BLUE = rgb(0x7FC8F8)
YELLOW = rgb(0xFFE14A)
GREY_DEAD = rgb(0x4A4650)
BULB = rgb(0xFFF2B8)
BULB_HOT = rgb(0xFFFFFF)
BULB_GLOW = rgb(0xE0A860)


def new(w: int, h: int) -> tuple[Image.Image, ImageDraw.ImageDraw]:
    img = Image.new("RGBA", (w, h), CLEAR)
    return img, ImageDraw.Draw(img)


def outline(img: Image.Image, keep_inside: bool = True) -> Image.Image:
    """Add a 1px OW outline on transparent pixels 4-adjacent to opaque ones (inside the canvas)."""
    w, h = img.size
    px = img.load()
    todo = []
    for y in range(h):
        for x in range(w):
            if px[x, y][3] == 0:
                for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < w and 0 <= ny < h and px[nx, ny][3] and px[nx, ny] != OUTLINE:
                        todo.append((x, y))
                        break
    for x, y in todo:
        px[x, y] = OUTLINE
    return img


def put(d: ImageDraw.ImageDraw, x: int, y: int, c) -> None:
    d.point((x, y), fill=c)


def save(img: Image.Image, name: str) -> None:
    os.makedirs(PROPS, exist_ok=True)
    img.save(os.path.join(PROPS, name))


# --------------------------------------------------------------------------- flower wall


def flower(d, cx: int, cy: int, petal, centre) -> None:
    for dx, dy in ((0, -1), (1, 0), (0, 1), (-1, 0)):
        put(d, cx + dx, cy + dy, petal)
    put(d, cx, cy, centre)


def make_flowerwall() -> Image.Image:
    img, d = new(40, 40)
    d.rounded_rectangle((1, 1, 38, 38), 4, fill=LEAF)
    rnd = random.Random(1830)
    for _ in range(70):
        x, y = rnd.randint(2, 37), rnd.randint(2, 37)
        put(d, x, y, rnd.choice((LEAF_DARK, LEAF_LIGHT, LEAF_DARK)))
    d.rectangle((2, 33, 37, 37), fill=LEAF_DARK)
    pal = [(PINK, YELLOW), (LILAC, WHITE), (WHITE, GOLD), (PINK_MID, WHITE), (LILAC_LIGHT, PINK_DARK)]
    spots = []
    for gy in range(4, 38, 6):
        for gx in range(4, 38, 6):
            spots.append((gx + rnd.randint(-1, 1) + (3 if (gy // 6) % 2 else 0), gy + rnd.randint(-1, 1)))
    for x, y in spots:
        if 3 <= x <= 36 and 3 <= y <= 36:
            p, c = rnd.choice(pal)
            flower(d, x, y, p, c)
    # soft top highlight, shadow under the rim
    d.line((4, 2, 35, 2), fill=LEAF_LIGHT)
    d.line((3, 38, 36, 38), fill=LEAF_DARK)
    return outline(img)


# --------------------------------------------------------------------------- fairy lights


def make_lights() -> Image.Image:
    w, h = 80, 14
    img, d = new(w, h)
    cols = [(PINK, PINK_LIGHT), (LILAC, LILAC_LIGHT), (YELLOW, BULB), (BLUE, WHITE), (PINK_MID, PINK_LIGHT)]
    pts = []
    for x in range(1, w - 1):
        t = (x - 1) / (w - 3)
        y = 1 + 4 * math.sin(math.pi * t) * 1.0
        # three swags
        y = 1 + 3.2 * abs(math.sin(math.pi * t * 3))
        pts.append((x, int(round(y))))
    for x, y in pts:
        put(d, x, y, PLUM_LIGHT)
    for i, (x, y) in enumerate(pts):
        if i % 8 == 4:
            c, hi = cols[(i // 8) % len(cols)]
            d.rectangle((x - 1, y + 1, x + 1, y + 2), fill=c)  # glass
            put(d, x, y + 3, c)
            put(d, x, y + 1, hi)
            put(d, x - 1, y + 4 if False else y + 2, c)
            put(d, x, y + 1, hi)
    return outline(img)


# --------------------------------------------------------------------------- beanbag


def make_beanbag() -> Image.Image:
    w, h = 40, 32
    img, d = new(w, h)
    px = img.load()

    def inside(x: float, y: float) -> bool:
        # heart: two circles on top + triangle point down
        u = (x - 19.5) / 13.0
        v = (17.5 - y) / 13.0
        return (u * u + v * v - 1) ** 3 - u * u * v ** 3 <= 0

    for y in range(h - 1):
        for x in range(w):
            if inside(x + 0.5, y + 0.5 - 0.0):
                px[x, y] = PINK
    # shading: lower-right darker, upper-left lighter
    for y in range(h):
        for x in range(w):
            if px[x, y] == PINK:
                if x + y > 36:
                    px[x, y] = PINK_MID
                if x + y > 46:
                    px[x, y] = PINK_DARK
                if x + y < 20:
                    px[x, y] = PINK_LIGHT
    # centre dent / seam, stitching and a squish crease
    d.line((19, 4, 19, 9), fill=PINK_DARK)
    d.arc((9, 10, 29, 24), 20, 160, fill=PINK_DARK)
    for sx, sy in ((8, 8), (30, 8), (14, 16), (24, 16)):
        put(d, sx, sy, WHITE)
    # warm spot ("still warm")
    d.ellipse((17, 12, 22, 15), fill=PINK_LIGHT)
    return outline(img)


# --------------------------------------------------------------------------- velvet rope


def make_rope() -> Image.Image:
    w, h = 64, 28
    img, d = new(w, h)
    for px0 in (3, 56):
        d.rectangle((px0 + 1, 7, px0 + 3, 23), fill=GOLD)
        put(d, px0 + 1, 8, GOLD_LIGHT)
        d.line((px0 + 1, 8, px0 + 1, 22), fill=GOLD_LIGHT)
        d.line((px0 + 3, 8, px0 + 3, 22), fill=GOLD_DARK)
        d.rectangle((px0 - 1, 23, px0 + 5, 25), fill=GOLD)
        d.line((px0 - 1, 25, px0 + 5, 25), fill=GOLD_DARK)
        d.ellipse((px0, 2, px0 + 4, 7), fill=GOLD)
        put(d, px0 + 1, 3, GOLD_LIGHT)
    # sagging rope
    for x in range(7, 57):
        t = (x - 7) / 49
        y = 9 + int(round(7 * math.sin(math.pi * t)))
        d.line((x, y, x, y + 2), fill=PINK_DARK)
        put(d, x, y, NEON)
        put(d, x, y + 1, PINK_MID if x % 4 else PINK_DARK)
    return outline(img)


# --------------------------------------------------------------------------- neon sign

FONT = {
    "G": ("111", "100", "101", "101", "111"),
    "O": ("111", "101", "101", "101", "111"),
    "D": ("110", "101", "101", "101", "110"),
    "V": ("101", "101", "101", "101", "010"),
    "I": ("111", "010", "010", "010", "111"),
    "B": ("110", "101", "110", "101", "110"),
    "E": ("111", "100", "110", "100", "111"),
    "S": ("111", "100", "111", "001", "111"),
    "N": ("101", "111", "111", "111", "101"),
    "L": ("100", "100", "100", "100", "111"),
    "Y": ("101", "101", "010", "010", "010"),
}


def text_pixels(s: str, gap: int = 2, space: int = 4) -> list[tuple[int, int]]:
    out = []
    x = 0
    for ch in s:
        if ch == " ":
            x += space
            continue
        for ry, row in enumerate(FONT[ch]):
            for rx, bit in enumerate(row):
                if bit == "1":
                    out.append((x + rx, ry))
        x += 3 + gap
    return out


def text_width(s: str, gap: int = 2, space: int = 4) -> int:
    px = text_pixels(s, gap, space)
    return max(p[0] for p in px) + 1


def make_neon() -> Image.Image:
    w, h = 60, 30
    img, d = new(w, h)
    d.rounded_rectangle((0, 0, w - 1, h - 1), 3, fill=PLUM_DEEP)
    d.rounded_rectangle((2, 2, w - 3, h - 3), 2, outline=NEON_GLOW)
    # tube border
    d.rounded_rectangle((4, 4, w - 5, h - 5), 2, outline=LILAC_DARK)
    lines = [("GOOD VIBES", 8), ("ONLY", 17)]
    for s, y0 in lines:
        x0 = (w - text_width(s)) // 2
        pts = text_pixels(s)
        glow = set()
        for x, y in pts:
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    glow.add((x0 + x + dx, y0 + y + dy))
        for gx, gy in glow:
            put(d, gx, gy, NEON_GLOW)
        for x, y in pts:
            put(d, x0 + x, y0 + y, NEON)
        for x, y in pts:  # hot core on the top row of each stroke
            if (x + y) % 2 == 0:
                put(d, x0 + x, y0 + y, NEON_HOT)
    # tiny heart between the corners
    for hx, hy in ((8, 23), (51, 23)):
        put(d, hx, hy, NEON)
        put(d, hx + 2, hy, NEON)
        d.line((hx, hy + 1, hx + 2, hy + 1), fill=NEON)
        put(d, hx + 1, hy + 2, NEON)
    return outline(img)


# --------------------------------------------------------------------------- vanity

BULBS = 40
DEAD_BULB = 17


def bulb_positions() -> list[tuple[int, int]]:
    x0, y0, x1, y1 = 3, 3, 46, 35  # centreline of the ring
    per = 2 * ((x1 - x0) + (y1 - y0))
    out = []
    for i in range(BULBS):
        s = per * (i + 0.5) / BULBS
        if s < x1 - x0:
            p = (x0 + s, y0)
        elif s < (x1 - x0) + (y1 - y0):
            p = (x1, y0 + s - (x1 - x0))
        elif s < 2 * (x1 - x0) + (y1 - y0):
            p = (x1 - (s - (x1 - x0) - (y1 - y0)), y1)
        else:
            p = (x0, y1 - (s - 2 * (x1 - x0) - (y1 - y0)))
        out.append((int(round(p[0])), int(round(p[1]))))
    return out


def make_vanity() -> Image.Image:
    w, h = 50, 52
    img, d = new(w, h)
    d.rectangle((1, 1, 48, 38), fill=PLUM_LIGHT)  # frame
    d.rectangle((6, 6, 43, 33), fill=LILAC_LIGHT)  # glass
    d.rectangle((6, 6, 43, 7), fill=LILAC)
    for k in range(5):  # glint
        d.line((12 + k * 2, 31, 22 + k * 2, 9), fill=WHITE) if k in (0, 2) else None
    d.line((28, 31, 36, 12), fill=WHITE)
    # reflection of a lumpy shape so it reads as a mirror
    d.ellipse((30, 20, 40, 32), fill=LILAC)
    # bulbs (2x2), one dead
    pos = bulb_positions()
    assert len(set(pos)) == BULBS
    for i, (bx, by) in enumerate(pos):
        if i == DEAD_BULB:
            d.rectangle((bx, by, bx + 1, by + 1), fill=GREY_DEAD)
            put(d, bx, by, PLUM_DEEP)
        else:
            d.rectangle((bx, by, bx + 1, by + 1), fill=BULB)
            put(d, bx, by, BULB_HOT)
    # table
    d.rectangle((0, 39, 49, 42), fill=PINK_MID)
    d.line((0, 39, 49, 39), fill=PINK_LIGHT)
    d.line((0, 42, 49, 42), fill=PINK_DARK)
    d.rectangle((2, 43, 47, 51), fill=PINK)
    d.rectangle((2, 43, 47, 43), fill=PINK_DARK)
    d.line((24, 44, 24, 50), fill=PINK_DARK)
    d.rectangle((9, 46, 13, 47), fill=GOLD)
    d.rectangle((35, 46, 39, 47), fill=GOLD)
    d.line((2, 51, 47, 51), fill=PINK_DARK)
    # tabletop clutter: a brush and a pot of glitter
    d.rectangle((8, 36, 10, 38), fill=LILAC_DARK)
    d.rectangle((38, 37, 41, 38), fill=WHITE)
    put(d, 39, 36, GOLD)
    return outline(img)


# --------------------------------------------------------------------------- throne


def make_throne() -> Image.Image:
    w, h = 40, 46
    img, d = new(w, h)
    # tall tufted back with a scalloped top
    d.rounded_rectangle((6, 3, 33, 34), 6, fill=PINK_MID)
    d.rounded_rectangle((8, 5, 31, 32), 5, fill=PINK)
    for cx in (12, 19, 27):
        d.ellipse((cx - 3, 1, cx + 3, 7), fill=PINK_MID)
    for tx, ty in ((14, 12), (25, 12), (14, 20), (25, 20), (19, 16), (19, 24)):
        put(d, tx, ty, PINK_DARK)
        put(d, tx - 1, ty - 1, PINK_LIGHT)
    # gold trim
    d.line((7, 33, 32, 33), fill=GOLD)
    # arms and seat
    d.rounded_rectangle((1, 22, 9, 40), 3, fill=PINK_MID)
    d.rounded_rectangle((30, 22, 38, 40), 3, fill=PINK_MID)
    put(d, 3, 24, PINK_LIGHT)
    put(d, 33, 24, PINK_LIGHT)
    d.rounded_rectangle((7, 28, 32, 40), 4, fill=PINK)
    d.line((9, 29, 30, 29), fill=PINK_LIGHT)
    d.line((8, 39, 31, 39), fill=PINK_DARK)
    # little feet
    for fx in (3, 31):
        d.rectangle((fx, 41, fx + 5, 44), fill=GOLD)
        d.line((fx, 44, fx + 5, 44), fill=GOLD_DARK)
    # court of tiny plushies on the seat
    d.ellipse((10, 30, 15, 36), fill=BLUE)
    put(d, 11, 31, WHITE)
    d.ellipse((24, 31, 29, 36), fill=YELLOW)
    put(d, 25, 32, WHITE)
    # crown on the back (Floofy's flower crown, hung up)
    for fx, c in ((15, YELLOW), (19, PINK_LIGHT), (23, LILAC_LIGHT)):
        put(d, fx, 3, c)
        put(d, fx, 4, LEAF)
    return outline(img)


# --------------------------------------------------------------------------- head rack

HEAD_FURS = [
    (rgb(0xE8934A), rgb(0xFFD8A8)),  # fox
    (rgb(0xB8BCC8), rgb(0xF0F0F6)),  # wolf
    (rgb(0x7FA8E8), rgb(0xD8E8FF)),  # blue
    (rgb(0xA874D8), rgb(0xE6D0FA)),  # purple
    (rgb(0x8A5A3C), rgb(0xD8B08A)),  # brown
    (rgb(0xF2F2F2), rgb(0xFFFFFF)),  # white
    (rgb(0xE86A8C), rgb(0xFFC0D0)),  # pink
    (rgb(0x6CBF6F), rgb(0xD0F0C8)),  # green (the murals were right about one)
]


def draw_head(d, x: int, y: int, kind: int) -> None:
    """12x13 empty fursuit head, front view, eyes and pupils on the viewer (the door)."""
    fur, light = HEAD_FURS[kind % len(HEAD_FURS)]
    ears = kind % 3
    # ears
    if ears == 0:  # pointed
        d.polygon([(x + 0, y + 4), (x + 1, y), (x + 4, y + 2)], fill=fur)
        d.polygon([(x + 11, y + 4), (x + 10, y), (x + 7, y + 2)], fill=fur)
        put(d, x + 1, y + 2, light)
        put(d, x + 10, y + 2, light)
    elif ears == 1:  # round
        d.ellipse((x, y, x + 3, y + 3), fill=fur)
        d.ellipse((x + 8, y, x + 11, y + 3), fill=fur)
    else:  # floppy
        d.rectangle((x - 0, y + 2, x + 2, y + 8), fill=fur)
        d.rectangle((x + 9, y + 2, x + 11, y + 8), fill=fur)
    # head
    d.ellipse((x + 1, y + 2, x + 10, y + 11), fill=fur)
    # muzzle
    d.ellipse((x + 3, y + 7, x + 8, y + 12), fill=light)
    put(d, x + 5, y + 8, PLUM_DEEP)
    put(d, x + 6, y + 8, PLUM_DEEP)
    # empty eye sockets: dark holes, one white speck each, looking straight out
    for ex in (x + 3, x + 7):
        d.rectangle((ex, y + 4, ex + 1, y + 6), fill=PLUM_DEEP)
        put(d, ex + (0 if ex < x + 5 else 1), y + 5, WHITE)


def make_headrack() -> Image.Image:
    w, h = 80, 52
    img, d = new(w, h)
    d.rectangle((0, 0, w - 1, h - 1), fill=WOOD_DARK)
    d.rectangle((1, 1, w - 2, h - 2), fill=PLUM)
    n = 0
    for row in range(3):
        sy = 3 + row * 16
        for col in range(6):
            draw_head(d, 2 + col * 13, sy, (n * 3 + row) % len(HEAD_FURS))
            n += 1
        d.rectangle((0, sy + 13, w - 1, sy + 15), fill=WOOD)
        d.line((0, sy + 13, w - 1, sy + 13), fill=WOOD_LIGHT)
        d.line((0, sy + 15, w - 1, sy + 15), fill=WOOD_DARK)
    d.rectangle((0, 0, 1, h - 1), fill=WOOD)
    d.rectangle((w - 2, 0, w - 1, h - 1), fill=WOOD)
    d.line((0, 0, w - 1, 0), fill=WOOD_LIGHT)
    return outline(img)


# --------------------------------------------------------------------------- flowerbed

BED_W, BED_H, BED_FRAMES = 56, 28, 3


def make_flowerbed_frame(f: int) -> Image.Image:
    img, d = new(BED_W, BED_H)
    rnd = random.Random(77)
    # the whole bed shakes: x offset + squash per frame
    ox = (0, -1, 1)[f]
    oy = (0, 0, -1)[f]
    d.rounded_rectangle((2, 16, 53, 27), 4, fill=WOOD)
    d.line((4, 16, 51, 16), fill=WOOD_LIGHT)
    d.line((4, 27, 51, 27), fill=WOOD_DARK)
    d.rectangle((3, 17, 52, 20), fill=SOIL)
    # foliage mound
    mound = Image.new("RGBA", (BED_W, BED_H), CLEAR)
    md = ImageDraw.Draw(mound)
    md.ellipse((4 + ox, 5 + oy, 51 + ox, 24 + oy), fill=LEAF)
    for _ in range(90):
        x, y = rnd.randint(6, 49), rnd.randint(8, 22)
        if md.im.getpixel((x + ox, y + oy))[3] if False else True:
            pass
    img.alpha_composite(mound)
    d = ImageDraw.Draw(img)
    rnd = random.Random(99)
    pal = [(PINK, YELLOW), (LILAC, WHITE), (WHITE, GOLD), (PINK_MID, WHITE), (LILAC_LIGHT, PINK_DARK)]
    for i in range(18):
        x = rnd.randint(8, 47)
        y = rnd.randint(9, 20)
        # keep inside the ellipse
        if ((x - 27.5) / 24) ** 2 + ((y - 14.5) / 10) ** 2 > 0.85:
            continue
        sway = ox if i % 2 == 0 else -ox
        p, c = pal[i % len(pal)]
        flower(d, x + sway, y + oy, p, c)
    for i in range(10):
        x = rnd.randint(8, 47)
        y = rnd.randint(10, 22)
        put(d, x + ox, y + oy, LEAF_DARK if i % 2 else LEAF_LIGHT)
    d.arc((4 + ox, 5 + oy, 51 + ox, 24 + oy), 200, 340, fill=LEAF_LIGHT)
    # frames 1-2: loose petals pop up
    if f >= 1:
        for px_, py_, c in ((10, 4, PINK), (44, 3, LILAC_LIGHT), (27, 1, WHITE), (50, 8, PINK_LIGHT)):
            put(d, px_ + f * 2 - 2, py_ - (f - 1) * 2, c)
    if f == 2:
        for px_, py_, c in ((16, 2, WHITE), (36, 4, PINK)):
            put(d, px_, py_, c)
    return outline(img)


def make_flowerbed() -> tuple[Image.Image, dict]:
    sheet = Image.new("RGBA", (BED_W * BED_FRAMES, BED_H), CLEAR)
    for f in range(BED_FRAMES):
        sheet.paste(make_flowerbed_frame(f), (f * BED_W, 0))
    meta = {
        "image": "ruins_lounge_flowerbed.png",
        "cell": [BED_W, BED_H],
        "origin": [BED_W // 2, BED_H - 1],
        "anims": {
            "still": {"row": 0, "frames": 1, "fps": 1, "loop": False},
            "shaking": {"row": 0, "frames": BED_FRAMES, "fps": 12, "loop": True},
        },
    }
    return sheet, meta


# --------------------------------------------------------------------------- main


def main() -> None:
    props = {
        "flowerwall": make_flowerwall(),
        "lights": make_lights(),
        "beanbag": make_beanbag(),
        "rope": make_rope(),
        "neon": make_neon(),
        "vanity": make_vanity(),
        "throne": make_throne(),
        "headrack": make_headrack(),
    }
    for k, im in props.items():
        save(im, f"ruins_lounge_{k}.png")
    sheet, meta = make_flowerbed()
    save(sheet, "ruins_lounge_flowerbed.png")
    with open(os.path.join(PROPS, "ruins_lounge_flowerbed.json"), "w", encoding="utf-8") as fh:
        json.dump(meta, fh, indent=2)
        fh.write("\n")
    props["flowerbed"] = sheet
    if "--preview" in sys.argv:
        out = sys.argv[sys.argv.index("--preview") + 1]
        os.makedirs(out, exist_ok=True)
        bg = (0x2A, 0x1A, 0x34, 255)
        for scale in (1, 2):
            W = sum(im.width for im in props.values()) + 8 * (len(props) + 1)
            H = max(im.height for im in props.values()) + 16
            sheet_img = Image.new("RGBA", (W, H), bg)
            x = 8
            for im in props.values():
                sheet_img.alpha_composite(im, (x, 8))
                x += im.width + 8
            if scale > 1:
                sheet_img = sheet_img.resize((W * scale, H * scale), Image.NEAREST)
            sheet_img.save(os.path.join(out, f"lounge_preview_{scale}x.png"))


if __name__ == "__main__":
    main()
