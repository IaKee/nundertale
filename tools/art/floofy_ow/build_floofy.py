#!/usr/bin/env python3
"""Extends assets/sprites/ow/floofy.png (ART-183 #1).

Rows 0-4 (idle, talk, sinister, sink, pounce) are kept pixel-for-pixel; the script only reads
the top 5 rows of the existing sheet, so it is safe to re-run. New rows:

  5 knocked_out  (2)  flattened on the ground, swirly eyes, bent crown (Grub lies on top of him in R1)
  6 vanity_back  (2)  seen from behind, sitting at the vanity in R2
  7 turn         (3)  front -> edge-on -> back
  8 soaked       (2)  fur flat and dark, crown dripping (Furiell's spray bottle)
  9 dive         (3)  dives head first into a flowerbed
 10 sinister_grin(2)  sinister frames with the too-wide smile

Usage: python tools/art/floofy_ow/build_floofy.py   (from the repo root; needs Pillow)
"""
import json
import os
import random
import sys

from PIL import Image

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
PNG = os.path.join(ROOT, "assets", "sprites", "ow", "floofy.png")
JSN = os.path.join(ROOT, "assets", "sprites", "ow", "floofy.json")
CW, CH = 40, 36
BASE_ROWS = 5
NEW_ROWS = 6

PAL = {
    ".": (0, 0, 0, 0),
    "#": (20, 16, 24, 255),
    "p": (255, 179, 217, 255),
    "w": (255, 240, 245, 255),
    "g": (44, 122, 56, 255),
    "G": (63, 163, 77, 255),
    "m": (224, 122, 174, 255),
    "M": (184, 80, 138, 255),
    "c": (244, 240, 234, 255),
    "y": (255, 225, 74, 255),
    "r": (138, 21, 48, 255),
    "R": (208, 38, 58, 255),
    "b": (42, 26, 26, 255),
    "v": (42, 22, 64, 255),
    "B": (90, 170, 235, 255),  # water
    "L": (170, 215, 250, 255),  # water highlight
    "k": (255, 255, 255, 255),
}


def cell(sheet, col, row):
    return sheet.crop((col * CW, row * CH, (col + 1) * CW, (row + 1) * CH))


def bbox_crop(im):
    return im.crop(im.getbbox())


def paste_bottom(dst, spr, cx=20, bottom=35, dx=0, dy=0):
    x = cx - spr.width // 2 + dx
    y = bottom - spr.height + 1 + dy
    dst.alpha_composite(spr, (x, y)) if x >= 0 and y >= 0 else dst.paste(spr, (x, y), spr)


def stamp(dst, rows, x0, y0, skip="."):
    for j, line in enumerate(rows):
        for i, ch in enumerate(line):
            if ch != skip and ch != " ":
                x, y = x0 + i, y0 + j
                if 0 <= x < dst.width and 0 <= y < dst.height:
                    dst.putpixel((x, y), PAL[ch])


def recolor(im, mapping):
    out = im.copy()
    px = out.load()
    for y in range(out.height):
        for x in range(out.width):
            if px[x, y] in mapping:
                px[x, y] = mapping[px[x, y]]
    return out


# ---------------------------------------------------------------- knocked out
SWIRL_L = [
    ".###.",
    "#...#",
    "#.#.#",
    "#.##.",
    ".#...",
]
SWIRL_R = [
    ".###.",
    "#...#",
    "#.#.#",
    ".##.#",
    "...#.",
]


def knocked_out(idle):
    out = []
    for f, tail_dy in ((0, 0), (1, -1)):
        spr = bbox_crop(idle[f]).rotate(90, expand=True, resample=Image.NEAREST)  # head to the left
        spr = spr.resize((spr.width, int(spr.height * 0.72)), Image.NEAREST)  # flattened
        im = Image.new("RGBA", (CW, CH), (0, 0, 0, 0))
        paste_bottom(im, spr, cx=20, bottom=34)
        # grid reference for the head: left part of the rotated sprite
        hx = 20 - spr.width // 2
        hy = 34 - spr.height + 1
        # swirly eyes (two spirals on the face side)
        stamp(im, SWIRL_L, hx + 13, hy + 2)
        stamp(im, SWIRL_R, hx + 13, hy + 8)
        # little stars/zzz over the head
        if f == 0:
            stamp(im, ["y.y", ".y.", "y.y"], hx + 2, hy - 4)
        else:
            stamp(im, ["y.y", ".y.", "y.y"], hx + 6, hy - 5)
        # bent crown: broken vine flopped off the head with a drooping flower
        stamp(im, ["gGg", "#gGg#", "..#gy#", "...#wy"], hx + 4, hy - 1 + tail_dy * 0)
        # ground shadow
        for x in range(hx + 1, hx + spr.width - 1):
            if im.getpixel((x, 35))[3] == 0:
                im.putpixel((x, 35), PAL["#"])
        out.append(im)
    return out


# ------------------------------------------------------------- back / vanity
BACK_HALF = [
    "...##.......",
    "..#pp#......",
    "..#pmp#.....",
    "..#pmmp#....",
    "..#ppmmp#...",
    ".#pppppppp#.",
    ".#ppppppppp#",
    "#ppppppppppp",
    "#GgGywGgGywG",
    "#gGgGyGgGgGg",
    "#pppppppppmM",
    "#pppppppppmM",
    ".#pppppppppM",
    ".##pppppppwp",
    "..#pppppppwp",
    ".#pppppppppM",
    "#pppppppppmM",
    "#pppppppppmM",
    "#pppppppppmM",
    "#ppppppppppM",
    ".#pppppppppM",
    ".#pppppppppM",
    ".#pppppppppM",
    ".#ppppppppMM",
    ".#ppppppppMM",
    ".#ppppppppMM",
    "..#ppppppppM",
    "..#ppppppppM",
    "..#pMMMMMMMM",
    "..#MMMMMMMMM",
    "..#MMMMM#MMM",
    "..#MMMM##MMM",
    "..##########",
]
TAIL_BACK = [
    "...####...",
    "..#wwww#..",
    ".#yywwyy#.",
    ".#ywwwwy#.",
    "#wwwwwwww#",
    "#wcwwwwcw#",
    "#wwwwwwww#",
    "#cwwwwwwc#",
    "#wwwwwwww#",
    ".#wwwwww#.",
    ".#cwwwwc#.",
    "..#wwww#..",
    "...####...",
]


def back_view(tail_dx, bloom):
    im = Image.new("RGBA", (CW, CH), (0, 0, 0, 0))
    rows = []
    for i, line in enumerate(BACK_HALF):
        if i >= 20:  # taper toward the hips
            line = "." * min(2, (i - 18) // 2) + line[: 12 - min(2, (i - 18) // 2)]
        rows.append(line + line[::-1])
    stamp(im, rows, 8, 2)
    # arm bumps at the sides
    for y in range(17, 25):
        im.putpixel((7, y), PAL["#"])
        im.putpixel((32, y), PAL["#"])
    stamp(im, TAIL_BACK, 15 + tail_dx, 22)
    if bloom:  # crown flowers pulse
        stamp(im, ["y", ], 15, 10)
        stamp(im, ["y", ], 24, 10)
    return im


def vanity_back():
    return [back_view(0, False), back_view(1, True)]


# ----------------------------------------------------------------------- turn
def squash_x(im, k):
    spr = bbox_crop(im)
    w = max(3, int(round(spr.width * k)))
    spr = spr.resize((w, spr.height), Image.NEAREST)
    out = Image.new("RGBA", (CW, CH), (0, 0, 0, 0))
    paste_bottom(out, spr, cx=20, bottom=35)
    return out


def turn(idle, back):
    return [squash_x(idle[0], 0.7), squash_x(idle[0], 0.4), squash_x(back, 0.7)]


# --------------------------------------------------------------------- soaked
WET = {
    PAL["p"]: (190, 150, 200, 255),
    PAL["w"]: (210, 190, 225, 255),
    PAL["c"]: (200, 190, 215, 255),
    PAL["m"]: (160, 105, 165, 255),
    PAL["M"]: (130, 75, 130, 255),
    PAL["G"]: (50, 120, 110, 255),
    PAL["g"]: (35, 90, 90, 255),
    PAL["y"]: (190, 175, 90, 255),
}


def soaked(idle):
    out = []
    for f in range(2):
        base = recolor(idle[f], WET)
        spr = bbox_crop(base)
        # flat fur: squash the whole body a little and drop the ears
        spr = spr.resize((spr.width + 2, int(spr.height * 0.88)), Image.NEAREST)
        im = Image.new("RGBA", (CW, CH), (0, 0, 0, 0))
        paste_bottom(im, spr, cx=20, bottom=34)
        top = 34 - spr.height + 1
        # plastered strands over the forehead
        stamp(im, ["MMMMMMMM", ".M.MM.M."], 14, top + 5)
        # drooping crown: leaves hang down both sides, dripping
        stamp(im, ["g..", "G.."], 9, top + 8)
        stamp(im, ["..g", "..G"], 29, top + 8)
        # drips (animate downward)
        d = 4 * f
        stamp(im, ["B", "L"], 11, top + 11 + d)
        stamp(im, ["B", "B"], 20, top + 12 + d // 2)
        stamp(im, ["L", "B"], 28, top + 10 + (4 - d))
        # small puddle
        for x in range(9, 31):
            im.putpixel((x, 35), PAL["B"] if (x + f) % 3 else PAL["L"])
        out.append(im)
    return out


# ------------------------------------------------------------------ flowerbed
def flowerbed(w=32, h=14, seed=3):
    rnd = random.Random(seed)
    im = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    for y in range(h):
        # rounded mound
        t = (y - (h - 1) * 0.35) / ((h - 1) * 0.65)
        half = (w / 2) * (1 - max(0.0, -t) ** 2 * 0.0) if t >= 0 else (w / 2) * (1 - t * t) ** 0.5
        if y < 2:
            half *= 0.55 + 0.2 * y
        for x in range(w):
            if abs(x + 0.5 - w / 2) <= half:
                im.putpixel((x, y), PAL["G"] if rnd.random() > 0.35 else PAL["g"])
    # outline
    px = im.load()
    edge = []
    for y in range(h):
        for x in range(w):
            if px[x, y][3] and (
                x in (0, w - 1) or y == 0 or y == h - 1 or any(px[x + dx, y + dy][3] == 0 for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)))
            ):
                edge.append((x, y))
    for p in edge:
        px[p] = PAL["#"]
    # flowers: yellow / white / pink / red dots with a petal ring
    for _ in range(9):
        x, y = rnd.randrange(3, w - 3), rnd.randrange(2, h - 3)
        if px[x, y][3] == 0 or px[x, y] == PAL["#"]:
            continue
        petal = rnd.choice("wpRw")
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            if px[x + dx, y + dy][3] and px[x + dx, y + dy] != PAL["#"]:
                px[x + dx, y + dy] = PAL[petal]
        px[x, y] = PAL["y"]
    return im


def dive(idle):
    bed = flowerbed()
    spr0 = bbox_crop(idle[0])
    out = []
    # f0: airborne, tilted 55 deg clockwise (head toward the bed), small bed at the bottom right
    a = spr0.rotate(-55, expand=True, resample=Image.NEAREST)
    a = a.resize((int(a.width * 0.8), int(a.height * 0.8)), Image.NEAREST)
    im = Image.new("RGBA", (CW, CH), (0, 0, 0, 0))
    im.alpha_composite(bed, (CW - bed.width - 1, CH - bed.height))
    im.alpha_composite(a, (1, 0))
    out.append(im)
    # f1: nearly vertical, head already in the bush
    b = spr0.rotate(-140, expand=True, resample=Image.NEAREST)
    b = b.resize((int(b.width * 0.8), int(b.height * 0.8)), Image.NEAREST)
    im = Image.new("RGBA", (CW, CH), (0, 0, 0, 0))
    im.alpha_composite(b, (CW // 2 - b.width // 2 - 2, CH - bed.height - b.height + 8))
    im.alpha_composite(bed, (4, CH - bed.height))
    out.append(im)
    # f2: only legs and tail stick out, petals fly
    c = spr0.rotate(180, expand=True, resample=Image.NEAREST)
    c = c.resize((int(c.width * 0.8), int(c.height * 0.8)), Image.NEAREST)
    im = Image.new("RGBA", (CW, CH), (0, 0, 0, 0))
    im.alpha_composite(c, (CW // 2 - c.width // 2, CH - bed.height - c.height + 19))
    im.alpha_composite(bed, (4, CH - bed.height))
    stamp(im, ["w", ".", ".", "p"], 5, 3)
    stamp(im, ["p"], 30, 6)
    stamp(im, ["w"], 8, 12)
    stamp(im, ["y"], 33, 12)
    out.append(im)
    return out


# ------------------------------------------------------------ sinister grin
def sinister_grin(sheet):
    out = []
    # (x_left, x_right) of the grin per frame, row 11-13
    for f, (xl, xr) in ((0, (10, 27)), (1, (7, 24))):
        im = cell(sheet, f, 2).copy()
        px = im.load()
        # wipe the muzzle band, then draw one wide smile that nearly crosses the whole head
        for y in range(11, 14):
            for x in range(xl + 1, xr):
                if px[x, y][3]:
                    px[x, y] = PAL["p"]
        stamp(im, ["#" + "." * (xr - xl - 1) + "#"], xl, 10)
        for x in range(xl + 1, xr):
            px[x, 13] = PAL["#"]
            px[x, 11] = PAL["w"] if (x - xl) % 3 else PAL["#"]
            px[x, 12] = PAL["w"] if (x - xl) % 3 else PAL["#"]
        px[xl, 11] = PAL["#"]
        px[xr, 11] = PAL["#"]
        px[xl, 12] = PAL["#"]
        px[xr, 12] = PAL["#"]
        px[xl - 1, 10] = PAL["#"]
        px[xr + 1, 10] = PAL["#"]
        out.append(im)
    return out


# ----------------------------------------------------------------------- main
def main():
    src = Image.open(PNG).convert("RGBA")
    base = src.crop((0, 0, 3 * CW, BASE_ROWS * CH))
    idle = [cell(base, 0, 0), cell(base, 1, 0)]
    rows = [
        knocked_out(idle),
        vanity_back(),
        None,  # turn needs the back view
        soaked(idle),
        dive(idle),
        sinister_grin(base),
    ]
    back = rows[1][0]
    rows[2] = turn(idle, back)
    sheet = Image.new("RGBA", (3 * CW, (BASE_ROWS + NEW_ROWS) * CH), (0, 0, 0, 0))
    sheet.paste(base, (0, 0))
    for r, frames in enumerate(rows):
        for c, fr in enumerate(frames):
            sheet.paste(fr, (c * CW, (BASE_ROWS + r) * CH))
    sheet.save(PNG)

    with open(JSN, encoding="utf-8") as fh:
        meta = json.load(fh)
    new = {
        "knocked_out": {"row": 5, "frames": 2, "fps": 1},
        "vanity_back": {"row": 6, "frames": 2, "fps": 2},
        "turn": {"row": 7, "frames": 3, "fps": 8, "loop": False},
        "soaked": {"row": 8, "frames": 2, "fps": 3},
        "dive": {"row": 9, "frames": 3, "fps": 8, "loop": False},
        "sinister_grin": {"row": 10, "frames": 2, "fps": 3},
    }
    meta["anims"].update(new)
    with open(JSN, "w", encoding="utf-8", newline="\n") as fh:
        json.dump(meta, fh, indent=2)
        fh.write("\n")
    print("wrote", PNG, sheet.size)
    return 0


if __name__ == "__main__":
    sys.exit(main())
