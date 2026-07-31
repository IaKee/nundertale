#!/usr/bin/env python3
"""Generate the R1a Registration and R1b Panel Hall prop sheets.

All artwork is authored at native 1x for the game's nearest-neighbour 2x
overworld presentation. The only edge colour is the overworld outline
#141018. Pass ``--preview DIR`` to write 1x and 2x contact sheets.
"""

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[3]
PROPS = ROOT / "assets" / "props"
RGBA = tuple[int, int, int, int]


def colour(value: int) -> RGBA:
    return (value >> 16 & 255, value >> 8 & 255, value & 255, 255)


CLEAR: RGBA = (0, 0, 0, 0)
OUTLINE = colour(0x141018)
INK = colour(0x2A1636)
PURPLE_DARK = colour(0x3F2352)
PURPLE = colour(0x5A3573)
PURPLE_LIGHT = colour(0x8B64B8)
LILAC = colour(0xB48CE0)
CREAM_DARK = colour(0xBFAE91)
CREAM = colour(0xE8DCC4)
PAPER = colour(0xFFF4DE)
WHITE = colour(0xFFF8F2)
PINK_DARK = colour(0xB8508A)
PINK = colour(0xE87BAD)
PINK_WARM = colour(0xFF9FCB)
PINK_LIGHT = colour(0xFFC8E2)
GOLD_DARK = colour(0xA5731A)
GOLD = colour(0xE8B93A)
GOLD_LIGHT = colour(0xFFE88A)
GREEN = colour(0x6CBF6F)
BLUE = colour(0x65A9D8)
GREY_DARK = colour(0x4A4650)
GREY = colour(0x77727E)
GREY_LIGHT = colour(0xAAA5B0)
RED = colour(0xD85462)

FONT = {
    "A": ("010", "101", "111", "101", "101"), "B": ("110", "101", "110", "101", "110"),
    "C": ("011", "100", "100", "100", "011"), "D": ("110", "101", "101", "101", "110"),
    "E": ("111", "100", "110", "100", "111"), "F": ("111", "100", "110", "100", "100"),
    "G": ("011", "100", "101", "101", "011"), "H": ("101", "101", "111", "101", "101"),
    "I": ("111", "010", "010", "010", "111"), "L": ("100", "100", "100", "100", "111"),
    "M": ("10001", "11011", "10101", "10101", "10101"), "N": ("101", "111", "111", "111", "101"),
    "O": ("010", "101", "101", "101", "010"), "P": ("110", "101", "110", "100", "100"),
    "R": ("110", "101", "110", "101", "101"), "S": ("011", "100", "010", "001", "110"),
    "T": ("111", "010", "010", "010", "010"), "U": ("101", "101", "101", "101", "111"),
    "V": ("101", "101", "010", "010", "010"), "Y": ("101", "101", "010", "010", "010"),
    "1": ("010", "110", "010", "010", "111"), "2": ("110", "001", "010", "100", "111"),
    "4": ("101", "101", "111", "001", "001"), "0": ("111", "101", "101", "101", "111"),
    "'": ("1", "1", "0", "0", "0"), "-": ("000", "000", "111", "000", "000"),
}


def canvas(w: int, h: int) -> tuple[Image.Image, ImageDraw.ImageDraw]:
    im = Image.new("RGBA", (w, h), CLEAR)
    return im, ImageDraw.Draw(im)


def add_outline(im: Image.Image) -> Image.Image:
    """Place a one-pixel 8-neighbour outline behind every opaque silhouette."""
    source = im.copy()
    src = source.load()
    dst = im.load()
    for y in range(im.height):
        for x in range(im.width):
            if src[x, y][3]:
                continue
            if any(0 <= x + dx < im.width and 0 <= y + dy < im.height and src[x + dx, y + dy][3]
                   for dx, dy in ((-1, -1), (0, -1), (1, -1), (-1, 0), (1, 0),
                                  (-1, 1), (0, 1), (1, 1))):
                dst[x, y] = OUTLINE
    return im


def text_width(text: str, scale: int = 1) -> int:
    return sum((len(FONT.get(ch, ("111",) * 5)[0]) + 1) * scale for ch in text) - scale


def pixel_text(d: ImageDraw.ImageDraw, xy: tuple[int, int], text: str, fill: RGBA,
               scale: int = 1, centered: int | None = None) -> None:
    text = text.upper()
    x, y = xy
    if centered is not None:
        x = centered - text_width(text, scale) // 2
    for ch in text:
        glyph = FONT.get(ch)
        if glyph is None:
            x += 4 * scale
            continue
        for gy, row in enumerate(glyph):
            for gx, bit in enumerate(row):
                if bit == "1":
                    d.rectangle((x + gx * scale, y + gy * scale,
                                 x + (gx + 1) * scale - 1, y + (gy + 1) * scale - 1), fill=fill)
        x += (len(glyph[0]) + 1) * scale


def make_desk() -> Image.Image:
    im, d = canvas(64, 42)
    d.polygon(((3, 9), (55, 9), (61, 15), (9, 15)), fill=PURPLE_LIGHT)
    d.polygon(((9, 15), (61, 15), (61, 37), (9, 37)), fill=PURPLE)
    d.polygon(((3, 9), (9, 15), (9, 37), (3, 31)), fill=PURPLE_DARK)
    d.line((10, 17, 60, 17), fill=LILAC)
    d.rectangle((14, 20, 55, 34), fill=PURPLE_DARK)
    d.rectangle((16, 21, 53, 32), fill=PURPLE)
    d.line((34, 21, 34, 32), fill=PURPLE_DARK)
    d.rectangle((21, 25, 28, 26), fill=GOLD_DARK)
    d.rectangle((40, 25, 47, 26), fill=GOLD_DARK)
    d.rectangle((17, 2, 48, 11), fill=CREAM)
    d.rectangle((19, 4, 46, 9), fill=PAPER)
    pixel_text(d, (0, 4), "REG", PURPLE_DARK, centered=33)
    d.rectangle((8, 7, 15, 11), fill=PAPER)
    d.line((9, 8, 14, 8), fill=GREY)
    d.line((10, 10, 15, 7), fill=BLUE)
    d.point((52, 12), fill=CREAM_DARK)
    d.line((53, 18, 53, 28), fill=PINK)
    d.rectangle((51, 27, 55, 31), fill=PINK_DARK)
    return add_outline(im)


def draw_badge(d: ImageDraw.ImageDraw, x: int, y: int, colour_: RGBA, tilt: int = 0) -> None:
    d.line((x + 2, y, x + 2 + tilt, y + 2), fill=CREAM_DARK)
    d.rectangle((x, y + 2, x + 5, y + 6), fill=colour_)
    d.line((x + 1, y + 3, x + 4, y + 3), fill=WHITE)
    d.point((x + 1, y + 5), fill=INK)


def make_badges() -> Image.Image:
    im, d = canvas(42, 30)
    d.polygon(((3, 8), (36, 8), (40, 12), (7, 12)), fill=CREAM)
    d.polygon(((7, 12), (40, 12), (37, 27), (6, 27)), fill=CREAM_DARK)
    d.polygon(((3, 8), (7, 12), (6, 27), (2, 23)), fill=GOLD_DARK)
    d.rectangle((10, 16, 34, 23), fill=PURPLE_DARK)
    for args in ((7, 3, PINK, 0), (15, 2, BLUE, 1), (23, 4, GREEN, -1),
                 (30, 2, GOLD, 0), (11, 9, LILAC, 1), (21, 10, RED, 0), (29, 9, PINK_WARM, -1)):
        draw_badge(d, *args)
    d.rectangle((14, 17, 31, 22), fill=PAPER)
    pixel_text(d, (0, 17), "2014", PURPLE_DARK, centered=23)
    return add_outline(im)


def make_rules() -> Image.Image:
    im, d = canvas(48, 44)
    d.rectangle((3, 2, 44, 38), fill=PURPLE_DARK)
    d.rectangle((6, 5, 41, 35), fill=CREAM)
    d.rectangle((8, 7, 39, 13), fill=PAPER)
    pixel_text(d, (0, 8), "RULES", RED, centered=24)
    for i, width in enumerate((23, 27, 18)):
        yy = 17 + i * 6
        pixel_text(d, (9, yy), str(i + 1), PURPLE_DARK)
        d.line((15, yy + 1, 15 + width, yy + 1), fill=PURPLE)
        d.line((15, yy + 3, 12 + width, yy + 3), fill=GREY)
    d.point((35, 23), fill=RED); d.point((37, 23), fill=RED); d.point((39, 23), fill=RED)
    d.rectangle((7, 39, 10, 41), fill=PURPLE_DARK)
    d.rectangle((37, 39, 40, 41), fill=PURPLE_DARK)
    return add_outline(im)


def make_escalator() -> Image.Image:
    im, d = canvas(84, 54)
    d.polygon(((4, 43), (21, 43), (65, 8), (79, 8), (34, 47), (4, 47)), fill=GREY_DARK)
    d.polygon(((11, 39), (27, 39), (65, 9), (72, 9), (31, 43), (11, 43)), fill=GREY)
    for i in range(8):
        x = 17 + i * 6; y = 38 - i * 4
        d.line((x, y, x + 13, y), fill=GREY_LIGHT)
        d.line((x + 12, y, x + 12, y + 3), fill=INK)
    d.line((4, 37, 61, 2), fill=OUTLINE, width=3)
    d.line((5, 36, 60, 3), fill=PURPLE_LIGHT)
    d.line((23, 25, 28, 22), fill=GREY_DARK, width=2)
    d.line((68, 6, 80, 6), fill=OUTLINE, width=3)
    d.line((68, 5, 80, 5), fill=PURPLE_LIGHT)
    d.rectangle((44, 22, 78, 37), fill=BLUE)
    d.rectangle((46, 24, 76, 35), fill=PAPER)
    pixel_text(d, (0, 25), "TEMPOR", RED, centered=61)
    pixel_text(d, (0, 31), "ARILY", PURPLE_DARK, centered=61)
    d.point((47, 24), fill=WHITE); d.point((75, 24), fill=WHITE)
    d.line((49, 38, 46, 47), fill=GREY_DARK)
    d.line((73, 38, 76, 47), fill=GREY_DARK)
    return add_outline(im)


def make_lostfound() -> Image.Image:
    im, d = canvas(46, 36)
    d.polygon(((3, 9), (39, 9), (43, 13), (8, 13)), fill=BLUE)
    d.polygon(((8, 13), (43, 13), (39, 33), (7, 33)), fill=PURPLE)
    d.polygon(((3, 9), (8, 13), (7, 33), (2, 28)), fill=PURPLE_DARK)
    d.rectangle((11, 17, 36, 25), fill=PAPER)
    pixel_text(d, (0, 19), "LOST", PURPLE_DARK, centered=24)
    d.ellipse((8, 3, 18, 13), fill=PINK_WARM)
    for x, y in ((7, 4), (10, 1), (14, 1), (17, 4)):
        d.ellipse((x, y, x + 3, y + 4), fill=PINK_LIGHT)
    d.line((25, 5, 34, 3, 30, 10, 37, 7), fill=GREY_LIGHT, width=2)
    d.rectangle((36, 5, 40, 9), fill=GREY_DARK)
    d.rectangle((25, 8, 35, 13), fill=CREAM)
    d.line((27, 10, 33, 10), fill=RED)
    return add_outline(im)


PANEL_TITLES = (("SUIT", "MAINT", RED), ("FURSONA", "PARENTS", GREEN), ("GAMERS", "REAL", GOLD))


def panel_door(title: tuple[str, str, RGBA], index: int) -> Image.Image:
    im, d = canvas(44, 58)
    d.rectangle((4, 2, 39, 56), fill=PURPLE_DARK)
    d.rectangle((7, 5, 36, 55), fill=PURPLE)
    d.rectangle((9, 7, 34, 28), fill=PURPLE_LIGHT)
    d.rectangle((9, 31, 34, 52), fill=PURPLE_LIGHT)
    d.line((10, 8, 33, 8), fill=LILAC)
    d.line((10, 32, 33, 32), fill=LILAC)
    d.rectangle((28, 28, 32, 31), fill=GOLD)
    d.point((29, 29), fill=GOLD_LIGHT)
    d.rectangle((1, 11, 27, 28), fill=CREAM_DARK)
    d.rectangle((3, 13, 25, 26), fill=PAPER)
    pixel_text(d, (0, 14), title[0], PURPLE_DARK, centered=14)
    pixel_text(d, (0, 20), title[1], PURPLE_DARK, centered=14)
    status = ("CANCEL", "FULL", "4 PM")[index]
    d.rectangle((2, 27, 28, 34), fill=title[2])
    pixel_text(d, (0, 28), status, WHITE, centered=15)
    d.line((2, 57, 41, 57), fill=OUTLINE)
    return add_outline(im)


def make_panel_doors() -> Image.Image:
    frames = [panel_door(title, i) for i, title in enumerate(PANEL_TITLES)]
    sheet = Image.new("RGBA", (44 * 3, 58), CLEAR)
    for i, frame in enumerate(frames):
        sheet.alpha_composite(frame, (i * 44, 0))
    return sheet


def make_lounge_door() -> Image.Image:
    im, d = canvas(68, 66)
    d.rectangle((4, 3, 63, 64), fill=INK)
    d.rectangle((7, 6, 60, 62), fill=PURPLE_DARK)
    d.rectangle((10, 9, 57, 59), fill=PURPLE)
    d.rectangle((13, 12, 54, 31), fill=PURPLE_LIGHT)
    d.rectangle((13, 35, 54, 56), fill=PURPLE_LIGHT)
    d.line((14, 13, 53, 13), fill=LILAC)
    d.line((14, 36, 53, 36), fill=LILAC)
    d.rectangle((49, 31, 54, 35), fill=GOLD)
    d.point((50, 32), fill=GOLD_LIGHT)
    d.rectangle((8, 19, 59, 36), fill=GREY_DARK)
    d.rectangle((10, 21, 57, 34), fill=GREY_LIGHT)
    pixel_text(d, (0, 22), "HEADLESS", INK, centered=34)
    pixel_text(d, (0, 28), "LOUNGE", INK, centered=34)
    d.polygon(((5, 17), (61, 14), (62, 26), (7, 29)), fill=PINK_LIGHT)
    d.line((8, 18, 58, 16), fill=WHITE)
    pixel_text(d, (0, 19), "FLOOFY'S", PINK_DARK, centered=34)
    d.rectangle((5, 18, 8, 27), fill=CREAM)
    d.rectangle((59, 15, 62, 24), fill=CREAM)
    return add_outline(im)


def petal(d: ImageDraw.ImageDraw, cx: int, cy: int, flip: int = 0) -> None:
    # Real loose petals; deliberately unlike the compact white battle bullets.
    d.point((cx, cy), fill=PINK_LIGHT)
    d.point((cx - 1, cy - 1), fill=PINK_WARM)
    d.point((cx + 1, cy - 1 + flip), fill=PINK)
    d.point((cx - 1 + flip, cy + 1), fill=PINK)
    d.point((cx + 1, cy + 1), fill=PINK_DARK)
    d.point((cx, cy - 1), fill=PINK_LIGHT)


def make_petal_trail() -> Image.Image:
    sheet = Image.new("RGBA", (96, 16), CLEAR)
    patterns = (
        ((7, 9, 0),),
        ((5, 11, 0), (14, 6, 1)),
        ((4, 5, 1), (11, 11, 0), (19, 7, 0)),
        ((3, 11, 0), (9, 7, 1), (15, 10, 0), (20, 4, 1)),
    )
    for frame, points in enumerate(patterns):
        tile, d = canvas(24, 16)
        for cx, cy, flip in points:
            petal(d, cx, cy, flip)
        add_outline(tile)
        sheet.alpha_composite(tile, (frame * 24, 0))
    return sheet


def assets() -> dict[str, Image.Image]:
    return {
        "ruins_reg_desk.png": make_desk(), "ruins_reg_badges.png": make_badges(),
        "ruins_reg_rules.png": make_rules(), "ruins_reg_escalator.png": make_escalator(),
        "ruins_reg_lostfound.png": make_lostfound(), "ruins_panel_door.png": make_panel_doors(),
        "ruins_panel_lounge_door.png": make_lounge_door(), "petal_trail.png": make_petal_trail(),
    }


def preview(images: dict[str, Image.Image], out_dir: Path) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    board = Image.new("RGBA", (320, 190), colour(0x24182E))
    positions = {
        "ruins_reg_desk.png": (8, 12), "ruins_reg_badges.png": (82, 18),
        "ruins_reg_rules.png": (132, 10), "ruins_reg_escalator.png": (185, 6),
        "ruins_reg_lostfound.png": (270, 18), "ruins_panel_door.png": (8, 75),
        "ruins_panel_lounge_door.png": (150, 68), "petal_trail.png": (220, 145),
    }
    for name, im in images.items():
        board.alpha_composite(im, positions[name])
    board.save(out_dir / "r1ab_props_1x.png")
    board.resize((640, 380), Image.Resampling.NEAREST).save(out_dir / "r1ab_props_2x.png")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--preview", type=Path)
    args = parser.parse_args()
    PROPS.mkdir(parents=True, exist_ok=True)
    generated = assets()
    for name, im in generated.items():
        im.save(PROPS / name, optimize=True)
        print(f"wrote {name}: {im.width}x{im.height}")
    if args.preview:
        preview(generated, args.preview)
        print(f"wrote 1x/2x previews to {args.preview}")


if __name__ == "__main__":
    main()
