"""Redraw intro cards 8-10 with the human Grub (LORE v3.3 section 9, ART-182/183).

* card08.png      200x110  Grub (blanket hood, headset, Dew bag, phone) hurrying along the Rift's edge.
* card09.png      200x110  a pale foot in a flip-flop slipping on a Tide Pod, a tail next to it.
* card10_tall.png 200x320  the Rift seen from below; the bottom (where the scroll starts) is Grub
                           landing on a pile of plushies with a flower-crowned fox under him.

The v3.1 art (goblin Grub) is kept as the backdrop in ``src/*_v31.png`` and repainted
at native resolution with the same six-colour sepia palette LoreScene scales 2x with
nearest-neighbour filtering.  Run from the repo root with Python 3.13 (Pillow required):

    C:/Python313/python.exe tools/art/cards810/generate_cards810.py [--preview out.png]
"""

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageDraw

HERE = Path(__file__).resolve().parent
OUT_DIR = Path("assets/img/lore")

PALETTE = {
    "sky": (217, 160, 74, 255),
    "sun": (240, 200, 120, 255),
    "far": (184, 125, 53, 255),
    "mid": (140, 90, 36, 255),
    "shade": (90, 52, 22, 255),
    "ink": (43, 22, 8, 255),
}
C = PALETTE


def load(name: str) -> Image.Image:
    return Image.open(HERE / "src" / name).convert("RGBA")


def poly(d: ImageDraw.ImageDraw, pts: list[tuple[float, float]], fill: str) -> None:
    d.polygon(pts, fill=C[fill])


def rect(d: ImageDraw.ImageDraw, x0: int, y0: int, x1: int, y1: int, fill: str) -> None:
    d.rectangle((x0, y0, x1, y1), fill=C[fill])


def ell(d: ImageDraw.ImageDraw, x0: int, y0: int, x1: int, y1: int, fill: str) -> None:
    d.ellipse((x0, y0, x1, y1), fill=C[fill])


def line(d: ImageDraw.ImageDraw, pts: list[tuple[int, int]], fill: str, width: int = 1) -> None:
    d.line(pts, fill=C[fill], width=width, joint="curve")


def dot(d: ImageDraw.ImageDraw, x: int, y: int, fill: str) -> None:
    d.point((x, y), fill=C[fill])


# --------------------------------------------------------------------------- Grub


def grub_head(d: ImageDraw.ImageDraw, x: int, y: int, *, side: bool, flip: bool = False) -> None:
    """Pale face under a blanket hood with the headset over it.

    (x, y) is the top-left of a 26x24 box.  ``side`` draws a 3/4 profile facing right
    (mirrored when ``flip``), otherwise a face-up view (used on card 10).
    """
    # Hood: the black silhouette element, two headset cups bump the outline.
    ell(d, x + 2, y, x + 23, y + 21, "ink")
    # Cups and band over the hood.
    rect(d, x - 1, y + 7, x + 3, y + 15, "ink")
    rect(d, x + 22, y + 7, x + 26, y + 15, "ink")
    rect(d, x, y + 9, x + 1, y + 12, "mid")
    rect(d, x + 24, y + 9, x + 25, y + 12, "mid")
    line(d, [(x + 3, y + 7), (x + 5, y + 1), (x + 20, y + 1), (x + 22, y + 7)], "shade", 1)
    # Face opening.
    if side:
        rect(d, x + 9, y + 6, x + 22, y + 17, "sun")
        rect(d, x + 20, y + 10, x + 24, y + 14, "sun")        # nose
        rect(d, x + 8, y + 5, x + 17, y + 8, "shade")          # messy fringe
        dot(d, x + 8, y + 9, "shade")
        rect(d, x + 14, y + 9, x + 17, y + 9, "ink")           # heavy eyelid
        rect(d, x + 14, y + 11, x + 17, y + 11, "far")         # eye bag
        rect(d, x + 17, y + 15, x + 21, y + 15, "mid")         # flat mouth
        if flip:
            pass
    else:
        rect(d, x + 6, y + 5, x + 19, y + 17, "sun")
        rect(d, x + 5, y + 3, x + 20, y + 7, "shade")          # fringe
        for hx in (6, 10, 14, 18):
            dot(d, x + hx, y + 8, "shade")
        rect(d, x + 7, y + 9, x + 10, y + 9, "ink")            # eyelids
        rect(d, x + 15, y + 9, x + 18, y + 9, "ink")
        rect(d, x + 7, y + 11, x + 10, y + 11, "far")          # eye bags
        rect(d, x + 15, y + 11, x + 18, y + 11, "far")
        rect(d, x + 11, y + 12, x + 14, y + 13, "far")         # nose
        rect(d, x + 10, y + 15, x + 15, y + 15, "mid")         # mouth
    # Messy hair sticking out of the hood.
    for px, py in ((x + 6, y - 1), (x + 10, y - 2), (x + 15, y - 1), (x + 19, y)):
        dot(d, px, py, "shade")
        dot(d, px, py + 1, "shade")


def flip_flop(d: ImageDraw.ImageDraw, x: int, y: int, *, face_down: bool = False) -> None:
    """A 10x4 flip-flop sole, strap in sun."""
    rect(d, x, y, x + 9, y + 3, "ink")
    rect(d, x + 1, y + 1, x + 8, y + 2, "far")
    rect(d, x + 4, y, x + 5, y + 1, "sun")


def dew_bottle(d: ImageDraw.ImageDraw, x: int, y: int, *, horizontal: bool = False) -> None:
    """A 2-liter bottle: ink body, light label with a 'DEW' dash pattern, ink cap."""
    if horizontal:
        rect(d, x, y, x + 19, y + 7, "ink")
        rect(d, x + 4, y + 1, x + 15, y + 6, "sun")
        rect(d, x + 6, y + 3, x + 8, y + 4, "ink")
        rect(d, x + 10, y + 3, x + 13, y + 4, "ink")
        rect(d, x + 20, y + 2, x + 23, y + 5, "ink")
        rect(d, x + 1, y + 1, x + 3, y + 6, "far")
    else:
        rect(d, x, y + 4, x + 7, y + 19, "ink")
        rect(d, x + 1, y + 7, x + 6, y + 16, "sun")
        rect(d, x + 2, y + 10, x + 5, y + 10, "ink")
        rect(d, x + 2, y + 13, x + 4, y + 13, "ink")
        rect(d, x + 2, y, x + 5, y + 4, "ink")
        rect(d, x + 1, y + 17, x + 6, y + 18, "far")


# --------------------------------------------------------------------------- card 8


def make_card08() -> Image.Image:
    img = load("card08_v31.png")
    d = ImageDraw.Draw(img)
    # Cover the v3.1 goblin: clone the plain rock from the far left (mirrored) over it,
    # then lay the ledge surface under his feet.
    src = img.copy()
    for yy in range(52, 93):
        for xx in range(38, 80):
            img.putpixel((xx, yy), src.getpixel((37 - (xx - 38) % 38, yy)))
    d = ImageDraw.Draw(img)
    poly(d, [(36, 91), (60, 88), (86, 90), (92, 94), (92, 100), (36, 100)], "far")

    x, y = 56, 46          # top-left of the head box; feet land at y = 93
    # Legs mid-stride (pale shins), flip-flops flapping.
    rect(d, x + 7, y + 40, x + 11, y + 46, "ink")
    rect(d, x + 8, y + 41, x + 10, y + 46, "sun")
    flip_flop(d, x + 4, y + 46)
    rect(d, x + 17, y + 38, x + 21, y + 44, "ink")
    rect(d, x + 18, y + 39, x + 20, y + 44, "sun")
    flip_flop(d, x + 18, y + 44)
    # Shorts under the cape.
    rect(d, x + 6, y + 34, x + 22, y + 41, "ink")
    rect(d, x + 7, y + 35, x + 21, y + 40, "shade")
    # Blanket cape: held shut at the chest, billowing behind him as he hurries.
    poly(d, [(x + 6, y + 19), (x + 22, y + 19), (x + 27, y + 38), (x - 17, y + 44), (x - 9, y + 30)], "ink")
    poly(d, [(x + 8, y + 21), (x + 21, y + 21), (x + 24, y + 35), (x - 12, y + 40), (x - 5, y + 29)], "mid")
    line(d, [(x - 8, y + 36), (x + 4, y + 33), (x + 14, y + 35)], "shade", 1)
    line(d, [(x - 4, y + 30), (x + 6, y + 28)], "far", 1)
    # Nacho-stained T-shirt at the open front.
    rect(d, x + 17, y + 24, x + 22, y + 32, "far")
    dot(d, x + 19, y + 27, "sun")
    dot(d, x + 21, y + 30, "sun")
    grub_head(d, x, y, side=True)

    # Front arm raised: the phone is lit, SQUAD QUEUES IN 30:00.
    line(d, [(x + 20, y + 25), (x + 27, y + 20)], "ink", 3)
    rect(d, x + 26, y + 13, x + 33, y + 24, "ink")
    rect(d, x + 27, y + 14, x + 32, y + 23, "sun")
    rect(d, x + 28, y + 16, x + 31, y + 16, "ink")
    rect(d, x + 28, y + 19, x + 31, y + 19, "mid")
    rect(d, x + 28, y + 21, x + 30, y + 21, "ink")
    # Back arm swings the Mountain Dew bag.
    line(d, [(x + 8, y + 26), (x - 1, y + 31)], "ink", 3)
    line(d, [(x - 2, y + 32), (x - 6, y + 35)], "ink", 1)
    poly(d, [(x - 19, y + 36), (x - 3, y + 36), (x - 1, y + 56), (x - 20, y + 56)], "ink")
    poly(d, [(x - 17, y + 38), (x - 5, y + 38), (x - 3, y + 54), (x - 18, y + 54)], "sun")
    rect(d, x - 15, y + 31, x - 12, y + 38, "ink")
    rect(d, x - 9, y + 29, x - 6, y + 38, "ink")
    rect(d, x - 14, y + 43, x - 6, y + 43, "ink")
    rect(d, x - 14, y + 46, x - 11, y + 46, "ink")
    rect(d, x - 9, y + 46, x - 6, y + 46, "ink")
    line(d, [(x - 18, y + 51), (x - 4, y + 51)], "far", 1)
    # Speed lines: the clock is running.
    line(d, [(x - 38, y + 22), (x - 28, y + 22)], "shade", 1)
    line(d, [(x - 42, y + 29), (x - 30, y + 29)], "shade", 1)
    return img


# --------------------------------------------------------------------------- card 9


def make_card09() -> Image.Image:
    img = load("card09_v31.png")
    d = ImageDraw.Draw(img)
    # The v3.1 pod read as a Dorito chip: repaint it as a Tide Pod, a fat two-tone
    # pillow with a swirl, squashed under the toe.
    x0, y0 = 76, 67
    poly(d, [(68, 66), (122, 66), (126, 74), (124, 92), (70, 92), (66, 80)], "mid")
    ell(d, x0 - 2, y0 + 14, x0 + 50, y0 + 20, "shade")          # hard shadow
    # Pillow body: three fat compartments like a detergent capsule, ink keyline.
    d.rounded_rectangle((x0, y0, x0 + 46, y0 + 17), radius=8, fill=C["ink"])
    d.rounded_rectangle((x0 + 1, y0 + 1, x0 + 45, y0 + 16), radius=7, fill=C["sun"])
    # Left lobe in far, right lobe in mid, wavy swirl divides them from the sun middle.
    poly(d, [(x0 + 1, y0 + 4), (x0 + 11, y0 + 2), (x0 + 17, y0 + 8), (x0 + 13, y0 + 12),
             (x0 + 17, y0 + 16), (x0 + 3, y0 + 16), (x0 + 1, y0 + 12)], "far")
    poly(d, [(x0 + 45, y0 + 4), (x0 + 36, y0 + 2), (x0 + 29, y0 + 8), (x0 + 33, y0 + 12),
             (x0 + 29, y0 + 16), (x0 + 43, y0 + 16), (x0 + 45, y0 + 12)], "mid")
    line(d, [(x0 + 17, y0 + 8), (x0 + 13, y0 + 12), (x0 + 17, y0 + 16)], "ink", 1)
    line(d, [(x0 + 29, y0 + 8), (x0 + 33, y0 + 12), (x0 + 29, y0 + 16)], "ink", 1)
    # A little swirl in the middle lobe.
    line(d, [(x0 + 20, y0 + 8), (x0 + 23, y0 + 5), (x0 + 26, y0 + 8), (x0 + 23, y0 + 11)], "far", 1)
    # Plastic shine.
    rect(d, x0 + 4, y0 + 3, x0 + 7, y0 + 3, "sky")
    rect(d, x0 + 20, y0 + 2, x0 + 26, y0 + 2, "sky")
    # Squirt of goo from the pressed side.
    for ox, oy in ((x0 + 48, y0 + 5), (x0 + 52, y0 + 7), (x0 + 50, y0 + 12)):
        rect(d, ox, oy, ox + 2, oy + 1, "sun")
    return img


# --------------------------------------------------------------------------- card 10


def plush(d: ImageDraw.ImageDraw, x: int, y: int, w: int, h: int, body: str, belly: str, kind: str) -> None:
    """A plushie: ink keyline, soft body, ears by kind, two-dot face."""
    # Ears first, so the head covers their base.
    if kind == "bear":
        ell(d, x - 1, y - 3, x + 5, y + 3, "ink")
        ell(d, x + w - 5, y - 3, x + w + 1, y + 3, "ink")
    elif kind == "bunny":
        rect(d, x + 2, y - 8, x + 5, y + 2, "ink")
        rect(d, x + w - 5, y - 8, x + w - 2, y + 2, "ink")
        rect(d, x + 3, y - 6, x + 4, y, "sun")
        rect(d, x + w - 4, y - 6, x + w - 3, y, "sun")
    elif kind == "cat":
        poly(d, [(x, y + 3), (x + 1, y - 5), (x + 7, y)], "ink")
        poly(d, [(x + w, y + 3), (x + w - 1, y - 5), (x + w - 7, y)], "ink")
    elif kind == "shark":
        poly(d, [(x + w // 2 - 3, y + 1), (x + w // 2, y - 8), (x + w // 2 + 4, y + 1)], "ink")
    ell(d, x - 1, y - 1, x + w + 1, y + h + 1, "ink")
    ell(d, x, y, x + w, y + h, body)
    ell(d, x + 3, y + h // 2, x + w - 3, y + h - 2, belly)
    # Face.
    ey = y + h // 3
    dot(d, x + w // 3, ey, "ink")
    dot(d, x + w - w // 3, ey, "ink")
    rect(d, x + w // 2 - 1, ey + 2, x + w // 2 + 1, ey + 2, "ink")
    if kind == "shark":
        rect(d, x + w // 2 - 3, ey + 4, x + w // 2 + 3, ey + 4, "sun")


def flower_crown(d: ImageDraw.ImageDraw, cx: int, cy: int, rx: int, ry: int, *, broken: bool) -> None:
    """A ring of tiny flowers around a head: sun petals, ink eyes, shade leaves."""
    import math

    n = 7
    for i in range(n):
        a = math.pi + math.pi * i / (n - 1)          # upper arc, left to right
        fx = int(round(cx + rx * math.cos(a)))
        fy = int(round(cy + ry * math.sin(a)))
        if broken and i == n - 2:
            fx += 3
            fy += 5
        dot(d, fx - 1, fy, "shade")
        dot(d, fx + 1, fy + 1, "shade")
        rect(d, fx - 1, fy - 1, fx + 1, fy + 1, "sun")
        dot(d, fx, fy, "mid" if i % 2 else "ink")


def fox_head(d: ImageDraw.ImageDraw, x: int, y: int) -> None:
    """The fox fursuiter, lying under Grub, head tilted, eyes closed in a smile."""
    # Ears.
    poly(d, [(x + 1, y + 6), (x - 1, y - 8), (x + 10, y + 1)], "ink")
    poly(d, [(x + 3, y + 5), (x + 2, y - 4), (x + 8, y + 2)], "sun")
    poly(d, [(x + 25, y + 5), (x + 28, y - 8), (x + 18, y + 1)], "ink")
    poly(d, [(x + 24, y + 5), (x + 25, y - 4), (x + 20, y + 2)], "mid")
    # Head and white cheek ruff.
    ell(d, x - 1, y, x + 29, y + 22, "ink")
    ell(d, x + 1, y + 2, x + 27, y + 20, "far")
    poly(d, [(x + 2, y + 14), (x + 8, y + 11), (x + 14, y + 20), (x + 6, y + 21)], "sun")
    poly(d, [(x + 26, y + 14), (x + 20, y + 11), (x + 14, y + 20), (x + 22, y + 21)], "sun")
    # Muzzle and nose.
    poly(d, [(x + 8, y + 12), (x + 20, y + 12), (x + 17, y + 20), (x + 11, y + 20)], "sun")
    rect(d, x + 12, y + 12, x + 16, y + 14, "ink")
    # Closed, smiling eyes (^ ^), the "out cold, smiling anyway" read.
    for ex in (x + 6, x + 19):
        dot(d, ex, y + 9, "ink")
        dot(d, ex + 1, y + 8, "ink")
        dot(d, ex + 2, y + 9, "ink")
    line(d, [(x + 11, y + 17), (x + 14, y + 19), (x + 17, y + 17)], "ink", 1)
    flower_crown(d, x + 14, y + 6, 13, 7, broken=True)


def make_card10_tall() -> Image.Image:
    img = load("card10_tall_v31.png")
    d = ImageDraw.Draw(img)
    top = 228
    # Cave floor: walls above stay; below, a dark well lit by the beam from the hole.
    poly(d, [(0, top + 14), (16, top + 8), (36, top + 14), (58, top + 7), (84, top + 12),
             (104, top + 6), (128, top + 12), (150, top + 7), (174, top + 13), (199, top + 8),
             (199, 319), (0, 319)], "shade")
    poly(d, [(0, top + 44), (24, top + 30), (60, top + 22), (100, top + 18), (140, top + 22),
             (176, top + 30), (199, top + 44), (199, 319), (0, 319)], "mid")
    # Crushed cans that fell from Mt. Dew, on the floor either side.
    for cx, cy in ((4, top + 70), (184, top + 66)):
        rect(d, cx, cy, cx + 11, cy + 6, "ink")
        rect(d, cx + 1, cy + 1, cx + 10, cy + 5, "far")
        rect(d, cx + 3, cy + 3, cx + 8, cy + 3, "ink")

    # --- The pile: plushies, back to front, dense, lit from the top centre.
    mound = [(8, 319), (6, 296), (20, 276), (46, 262), (80, 254), (120, 254), (154, 262),
             (180, 276), (194, 296), (192, 319)]
    poly(d, mound, "ink")
    back = [
        (16, 270, 24, 20, "mid", "far", "bear"),
        (42, 258, 22, 19, "far", "sun", "bunny"),
        (68, 252, 22, 18, "sun", "sky", "shark"),
        (112, 252, 22, 18, "far", "sun", "cat"),
        (138, 258, 24, 20, "mid", "far", "bear"),
        (160, 270, 24, 20, "far", "sun", "bunny"),
    ]
    for p in back:
        plush(d, *p)
    mid = [
        (4, 290, 28, 22, "far", "sun", "cat"),
        (160, 288, 28, 22, "sun", "sky", "bear"),
        (28, 280, 20, 18, "sun", "sky", "bunny"),
        (146, 278, 20, 18, "mid", "far", "shark"),
    ]
    for p in mid:
        plush(d, *p)

    # --- The fox under Grub: tail out to the right, head and flower crown at lower left.
    poly(d, [(140, 296), (158, 288), (176, 292), (188, 302), (178, 312), (156, 310), (140, 308)], "ink")
    poly(d, [(143, 298), (158, 291), (172, 295), (183, 302), (174, 309), (156, 307), (143, 305)], "far")
    poly(d, [(172, 295), (183, 302), (174, 309), (167, 304)], "sun")
    # Plush cluster hiding the fox's body under Grub.
    plush(d, 58, 296, 40, 26, "mid", "far", "bear")
    plush(d, 104, 298, 36, 24, "far", "sun", "cat")
    ell(d, 28, 303, 46, 316, "ink")                               # paw
    ell(d, 30, 305, 44, 314, "far")
    fox_head(d, 20, 284)

    # --- Grub on top, face-up, arms flung out, blanket bunched under his shoulders.
    poly(d, [(70, 262), (134, 262), (150, 282), (132, 292), (72, 292), (54, 282)], "ink")
    poly(d, [(74, 266), (130, 266), (143, 281), (129, 289), (75, 289), (61, 281)], "mid")
    line(d, [(66, 276), (80, 281), (92, 279)], "shade", 1)
    line(d, [(138, 277), (126, 282)], "shade", 1)
    # Legs and shorts; flip-flops hang off the bottom of the frame.
    rect(d, 84, 300, 94, 314, "sun")
    rect(d, 106, 300, 116, 314, "sun")
    rect(d, 82, 292, 118, 303, "ink")
    rect(d, 84, 294, 116, 301, "shade")
    flip_flop(d, 80, 314)
    flip_flop(d, 106, 314)
    # Torso: blue T-shirt (the 'far' mid-tone) with nacho stains (sun dots).
    poly(d, [(78, 268), (122, 268), (118, 292), (82, 292)], "ink")
    poly(d, [(80, 270), (120, 270), (116, 290), (84, 290)], "far")
    # Hood collar around the neck.
    poly(d, [(82, 266), (118, 266), (114, 274), (86, 274)], "ink")
    line(d, [(86, 271), (114, 271)], "mid", 1)
    for sx, sy in ((92, 279), (96, 282), (104, 277), (108, 284), (100, 286)):
        rect(d, sx, sy, sx + 1, sy + 1, "sun")
    # Arms flung out, pale hands.
    line(d, [(80, 274), (68, 271), (58, 278)], "ink", 4)
    line(d, [(120, 274), (132, 271), (142, 278)], "ink", 4)
    ell(d, 53, 274, 61, 282, "sun")
    ell(d, 139, 274, 147, 282, "sun")
    # The Dew bag burst open, bottles tumbling across the plushies.
    poly(d, [(150, 262), (168, 258), (178, 270), (170, 282), (152, 280)], "ink")
    poly(d, [(153, 264), (166, 261), (174, 270), (168, 279), (154, 277)], "sun")
    rect(d, 159, 269, 163, 269, "ink")
    dew_bottle(d, 128, 240, horizontal=True)
    dew_bottle(d, 12, 246)
    # The Tide Pod that fell in with him, on the plush.
    d.rounded_rectangle((120, 304, 137, 311), radius=3, fill=C["ink"])
    d.rounded_rectangle((121, 305, 136, 310), radius=2, fill=C["sun"])
    poly(d, [(121, 308), (128, 305), (136, 308), (128, 310)], "far")
    # Head last so it sits on top of the blanket.
    grub_head(d, 87, 248, side=False)
    return img


# --------------------------------------------------------------------------- driver


def validate(img: Image.Image, size: tuple[int, int]) -> None:
    assert img.size == size, (img.size, size)
    assert img.mode == "RGBA"
    stray = set(img.getdata()) - set(PALETTE.values())
    assert not stray, stray


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--preview", type=Path, help="optional 3x side-by-side preview")
    ap.add_argument("--no-write", action="store_true")
    args = ap.parse_args()

    cards = {
        "card08.png": (make_card08(), (200, 110)),
        "card09.png": (make_card09(), (200, 110)),
        "card10_tall.png": (make_card10_tall(), (200, 320)),
    }
    for name, (img, size) in cards.items():
        validate(img, size)
        if not args.no_write:
            img.save(OUT_DIR / name, optimize=True)

    if args.preview:
        scale = 3
        w = sum(i.width for i, _ in cards.values()) * scale + 20
        h = max(i.height for i, _ in cards.values()) * scale
        sheet = Image.new("RGBA", (w, h), C["ink"])
        x = 0
        for img, _ in cards.values():
            sheet.paste(img.resize((img.width * scale, img.height * scale), Image.Resampling.NEAREST), (x, 0))
            x += img.width * scale + 10
        args.preview.parent.mkdir(parents=True, exist_ok=True)
        sheet.save(args.preview)


if __name__ == "__main__":
    main()
