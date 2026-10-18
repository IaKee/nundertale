"""Reduce the core (star) Ruins guide sheets in assets/_gen/ruins/ to native 1x assets.

Usage (repo root):  python tools/art_prompts/reduce_ruins.py [--out DIR] [--preview DIR] [SHEET ...]

Without --out, files go to their final assets/ paths, skipping any file that already
exists (--force overwrites generated files, --force-tracked committed ones). Specs (cell, grid, palette, JSON) come from the
prompt sheets in docs/art_prompts/ruins/.
"""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(__file__))
import reduce_generic  # noqa: E402
import reduce_lib as rl  # noqa: E402

GEN = "assets/_gen/ruins"


def pal(*hexes):
    return np.array([rl.hexrgb(h) for h in hexes], dtype=np.int32)


class Ctx:
    def __init__(self, out_dir, preview_dir, force, force_tracked=False):
        self.out_dir, self.preview_dir, self.force, self.force_tracked = out_dir, preview_dir, force, force_tracked
        self.written = []

    def path(self, final):
        return os.path.join(self.out_dir, final) if self.out_dir else final

    def blocked(self, p):
        """Existing files are kept unless --force; committed files also need --force-tracked."""
        if self.out_dir or not os.path.exists(p):
            return False
        if self.force_tracked:
            return False
        tracked = subprocess.run(["git", "ls-files", "--error-unmatch", p], capture_output=True).returncode == 0
        return tracked or not self.force

    def save_img(self, img: Image.Image, final: str, **kw):
        p = self.path(final)
        if self.blocked(p):
            # never clobber an existing asset: park the candidate in the git-ignored _gen tree
            p = os.path.join(GEN, "out", final)
            print(f"  {final} exists; candidate goes to {p} (--force replaces generated files, --force-tracked committed ones)")
        os.makedirs(os.path.dirname(p) or ".", exist_ok=True)
        img.save(p, **kw)
        self.written.append(p)
        print(f"  wrote {p} {img.size} {img.mode}")

    def save_json(self, data: dict, final: str):
        p = self.path(final)
        if self.blocked(p):
            print(f"  SKIP {p} (exists; use --force, or --force-tracked for committed files)")
            return
        with open(p, "w", encoding="utf-8", newline="\n") as f:
            f.write(format_sheet_json(data))
        self.written.append(p)
        print(f"  wrote {p}")

    def preview(self, guide, img, name, zoom=4):
        if self.preview_dir:
            os.makedirs(self.preview_dir, exist_ok=True)
            rl.preview(guide, img, os.path.join(self.preview_dir, f"{name}.png"), zoom)


def format_sheet_json(d: dict) -> str:
    """Same layout as the prompt sheets: one anim per line."""
    lines = ["{", f'  "image": "{d["image"]}",', f'  "cell": [{d["cell"][0]}, {d["cell"][1]}],',
             f'  "origin": [{d["origin"][0]}, {d["origin"][1]}],', '  "anims": {']
    items = list(d["anims"].items())
    for i, (name, a) in enumerate(items):
        body = ", ".join(f'"{k}": {json.dumps(v)}' for k, v in a.items())
        lines.append(f'    "{name}": {{ {body} }}' + ("," if i < len(items) - 1 else ""))
    lines += ["  }", "}", ""]
    return "\n".join(lines)


def reduce_sprite_sheet(guide, cols_per_row, cell, origin, palette, *, scale, weights=None,
                        layout=None, alpha_thr=0.5, post=None, anchor_frac=0.15, denoise=0):
    """Generic character sheet: every guide cell is anchored at its feet onto `origin`."""
    rgb, fg = rl.load(guide)
    idx = rl.mode_filter(rl.snap(rgb, palette), denoise)
    boxes = rl.slice_cells(fg, cols_per_row)
    cw, ch = cell
    layout = layout or {(r, c): (r, c) for r, row in enumerate(boxes) for c in range(len(row))}
    nrows = 1 + max(t[0] for t in layout.values())
    ncols = 1 + max(t[1] for t in layout.values())
    sheet = np.full((nrows * ch, ncols * cw), -1, dtype=np.int32)
    cells = {}
    for (gr, gc), (tr, tc) in layout.items():
        if (gr, gc) not in cells:
            box = boxes[gr][gc]
            ax, ay = rl.foot_anchor(fg, box, anchor_frac)
            # clip the vote to this guide cell so neighbours never bleed in
            m = np.zeros_like(fg)
            x0, y0, x1, y1 = box
            m[y0:y1, x0:x1] = fg[y0:y1, x0:x1]
            c = rl.reduce_region(idx, m, ax, ay, scale, origin[0], origin[1] + 1, cw, ch,
                                 len(palette), weights, alpha_thr)
            if post:
                c = post(c, gr, gc)
            cells[(gr, gc)] = c
        sheet[tr * ch:(tr + 1) * ch, tc * cw:(tc + 1) * cw] = cells[(gr, gc)]
    return Image.fromarray(rl.to_rgba(sheet, palette)), boxes


# ---------------------------------------------------------------- A04 Barky overworld
def a04(ctx):
    guide = reduce_generic.source("A04_ow_barky")
    P = pal("#141418", "#9A9AAA", "#6E6E7E", "#C98AA8", "#232329", "#3C3C46", "#C8C8D2", "#7CFF3A", "#D0263A")
    OUT, FUR, SHD, EAR, JKT, JHI, STUD, NEON, EYE = range(9)
    w = np.ones(len(P))
    w[[EAR, NEON, EYE]] = 2.0  # small accents survive the vote
    w[STUD] = 1.3
    w[OUT] = 1.2

    def post(c, r, col):
        c = rl.drop_specks(c, 2)
        c = rl.despeckle(c, protect=(EYE, NEON, STUD), passes=2)
        return rl.outline(c, OUT)

    # guide rows: 4, 4, 2, 2, 3; columns 2 of rows 0-1 are idle copies of column 0
    layout = {(0, 0): (0, 0), (0, 1): (0, 1), (0, 2): (0, 2), (0, 3): (0, 3),
              (1, 0): (1, 0), (1, 1): (1, 1), (1, 2): (1, 2), (1, 3): (1, 3),
              (2, 0): (2, 0), (2, 1): (2, 1), (3, 0): (3, 0), (3, 1): (3, 1),
              (4, 0): (4, 0), (4, 1): (4, 1), (4, 2): (4, 2)}
    rgb, fg = rl.load(guide)
    boxes = rl.slice_cells(fg, [4, 4, 2, 2, 3])
    stand_h = np.median([b[3] - b[1] for b in boxes[0] + boxes[1]])
    scale = stand_h / 33.0
    widest = max(b[2] - b[0] for row in boxes for b in row)
    scale = max(scale, widest / 27.0)
    print(f"  A04 scale {scale:.2f} src px / art px")
    img, _ = reduce_sprite_sheet(guide, [4, 4, 2, 2, 3], (28, 36), (14, 35), P, scale=scale,
                                 weights=w, layout=layout, post=post, denoise=5)
    # spec: column 2 of the walk rows is a copy of column 0
    a = np.array(img)
    for r in (0, 1):
        a[r * 36:(r + 1) * 36, 56:84] = a[r * 36:(r + 1) * 36, 0:28]
    img = Image.fromarray(a)
    ctx.save_img(img, "assets/sprites/ow/barky.png")
    ctx.save_json({"image": "barky.png", "cell": [28, 36], "origin": [14, 35], "anims": {
        "stand_down": {"row": 0, "frames": 1}, "walk_down": {"row": 0, "frames": 4, "fps": 6},
        "stand_up": {"row": 1, "frames": 1}, "walk_up": {"row": 1, "frames": 4, "fps": 6},
        "stand_left": {"row": 2, "frames": 1}, "walk_left": {"row": 2, "frames": 2, "fps": 4},
        "stand_right": {"row": 3, "frames": 1}, "walk_right": {"row": 3, "frames": 2, "fps": 4},
        "drop": {"row": 4, "frames": 3, "fps": 6, "loop": False}}}, "assets/sprites/ow/barky.json")
    ctx.preview(guide, img, "A04_ow_barky", 5)


# ---------------------------------------------------------------- B01 Barky battle
def b01(ctx):
    guide = reduce_generic.source("B01_battle_barky")
    P = pal("#FFFFFF", "#000000", "#FF77C8")
    WHITE, BLACK, BLUSH = range(3)
    w = np.array([1.35, 1.0, 3.0])

    def post(c, r, col):
        if r != 2:
            c[c == BLUSH] = WHITE
        c = rl.prune_color(c, BLUSH, 3, keep_largest=True)  # magenta fringe that snapped to pink
        c = rl.drop_specks(c, 3)
        c = rl.fill_holes(c, BLACK)
        return c

    rgb, fg = rl.load(guide)
    boxes = rl.slice_cells(fg, [2, 1, 1, 1])
    tall = max(b[3] - b[1] for row in boxes for b in row)
    wide = max(b[2] - b[0] for row in boxes for b in row)
    scale = max(tall / 79.0, wide / 59.0)
    print(f"  B01 scale {scale:.2f}")
    img, _ = reduce_sprite_sheet(guide, [2, 1, 1, 1], (60, 80), (30, 79), P, scale=scale, weights=w, post=post)
    ctx.save_img(img, "assets/sprites/battle/barky.png")
    ctx.save_json({"image": "barky.png", "cell": [60, 80], "origin": [30, 80], "anims": {
        "idle": {"row": 0, "frames": 2, "fps": 2}, "hurt": {"row": 1, "frames": 1},
        "blush": {"row": 2, "frames": 1}, "spared": {"row": 3, "frames": 1}}}, "assets/sprites/battle/barky.json")
    ctx.preview(guide, img, "B01_battle_barky", 4)


# ---------------------------------------------------------------- E01 battle buttons
# 7-px-tall chunky caps, drawn by hand (the AI letters are unreliable; sheet section 9)
GLYPHS = {
    "F": ["#####", "##...", "##...", "####.", "##...", "##...", "##..."],
    "I": ["####", ".##.", ".##.", ".##.", ".##.", ".##.", "####"],
    "G": [".###.", "##..#", "##...", "##.##", "##..#", "##..#", ".###."],
    "H": ["##.##", "##.##", "##.##", "#####", "##.##", "##.##", "##.##"],
    "T": ["######", "..##..", "..##..", "..##..", "..##..", "..##..", "..##.."],
    "A": [".###.", "##.##", "##.##", "#####", "##.##", "##.##", "##.##"],
    "C": [".####", "##...", "##...", "##...", "##...", "##...", ".####"],
    "E": ["#####", "##...", "##...", "####.", "##...", "##...", "#####"],
    "M": ["##...##", "###.###", "#######", "##.#.##", "##...##", "##...##", "##...##"],
    "R": ["####.", "##.##", "##.##", "####.", "##.##", "##.##", "##.##"],
    "Y": ["##..##", "##..##", ".####.", "..##..", "..##..", "..##..", "..##.."],
}


# icons the reduction blurs at 12x12 are drawn by hand from the guide's design
ICONS = {
    "FIGHT": [".....##.....", "...#.##.#...", "..#..##..#..", ".#........#.", "#..........#", "####.##.####",
              "####.##.####", "#..........#", ".#........#.", "..#..##..#..", "...#.##.#...", ".....##....."],
    "MERCY": ["#...........", "#.#########.", "#.##.#.#.##.", "#.#########.", "#.###...##..", "#.##.....#..",
              "#.###...##..", "#.#########.", "#.##########", "#...........", "#...........", "#..........."],
}


def draw_text(cell, text, x, y, color):
    for ch in text:
        g = GLYPHS[ch]
        for gy, line in enumerate(g):
            for gx, v in enumerate(line):
                if v == "#":
                    cell[y + gy, x + gx] = color
        x += len(g[0]) + 1


def text_width(text):
    return sum(len(GLYPHS[c][0]) for c in text) + len(text) - 1


def e01(ctx):
    guide = reduce_generic.source("E01_ui_buttons")
    P = pal("#FF7F27", "#FFFF00", "#808080", "#000000")
    ORANGE, YELLOW, GRAY, BLACK = range(4)
    rgb, fg = rl.load(guide)
    idx = rl.snap(rgb, P)
    boxes = rl.slice_cells(fg, [1] * 9)
    rows = [("FIGHT", ORANGE, True), ("FIGHT", YELLOW, False), ("ACT", ORANGE, True), ("ACT", YELLOW, False),
            ("ITEM", ORANGE, True), ("ITEM", YELLOW, False), ("MERCY", ORANGE, True), ("MERCY", YELLOW, False),
            ("MERCY", GRAY, True)]
    W, H = 55, 21
    sheet = np.full((H * 9, W), BLACK, dtype=np.int32)
    for r, ((x0, y0, x1, y1), (word, col, icon)) in enumerate(zip([b[0] for b in boxes], rows)):
        c = np.full((H, W), BLACK, dtype=np.int32)
        c[0, :] = c[-1, :] = c[:, 0] = c[:, -1] = col
        if icon and word in ICONS:
            for gy, line in enumerate(ICONS[word]):
                for gx, v in enumerate(line):
                    if v == "#":
                        c[4 + gy, 3 + gx] = col
        elif icon:
            bw, bh = x1 - x0, y1 - y0
            ix0, ix1 = x0 + int(bw * 0.07), x0 + int(bw * 0.33)
            iy0, iy1 = y0 + int(bh * 0.17), y0 + int(bh * 0.83)
            m = idx[iy0:iy1, ix0:ix1] == col
            ys, xs = np.where(m)
            gx0, gx1, gy0, gy1 = ix0 + xs.min(), ix0 + xs.max() + 1, iy0 + ys.min(), iy0 + ys.max() + 1
            sc = max(gx1 - gx0, gy1 - gy0) / 12.0
            iw, ih = round((gx1 - gx0) / sc), round((gy1 - gy0) / sc)
            vote_fg = np.zeros_like(fg)
            vote_fg[gy0:gy1, gx0:gx1] = idx[gy0:gy1, gx0:gx1] == col
            ic = rl.reduce_region(idx, vote_fg, gx0, gy0, sc, 0, 0, iw, ih, len(P), alpha_thr=0.3)
            ox, oy = 3 + (12 - iw) // 2, 4 + (12 - ih) // 2
            sub = c[oy:oy + ih, ox:ox + iw]
            sub[ic >= 0] = col
        tw = text_width(word)
        draw_text(c, word, 17 + (37 - tw) // 2, 7, col)
        sheet[r * H:(r + 1) * H] = c
    img = Image.fromarray(rl.to_rgba(sheet, P))
    ctx.save_img(img, "assets/ui/buttons.png")
    names = ["fight", "fight_selected", "act", "act_selected", "item", "item_selected",
             "mercy", "mercy_selected", "mercy_locked"]
    ctx.save_json({"image": "buttons.png", "cell": [55, 21], "origin": [0, 0],
                   "anims": {n: {"row": i, "frames": 1} for i, n in enumerate(names)}}, "assets/ui/buttons.json")
    ctx.preview(guide, img, "E01_ui_buttons", 6)


# ---------------------------------------------------------------- E02 souls
# The guide hearts are 9x8 at ~15 px per art pixel; the spec wants one identical 8x8 heart
# centered in 12x12, so the shapes are set by hand and the colors and extras follow the guide.
HEART = [".##..##.", "########", "########", "########", "########", ".######.", "..####..", "...##..."]
PLUSH = ["............", ".pp......pp.", ".psPP..PPsp.", "..PPPPPPPP..", "..PPPsPPPP..", "..PPPPsPPP..",
         "..PPPsPPPP..", "...PPPsPP...", "....PPPP....", ".....PP.....", "............", "............"]
FROG = ["..GGG..GGG..", "..WKG..GKW..", "..GGGGGGGG..", "..GGGGGGGG..", "..GGGGGGGG..", "..GGGGGGGG..",
        "..GGGGGGGG..", "...GGGGGG...", "....GGGG....", ".....GG.....", "............", "............"]


def e02(ctx):
    guide = reduce_generic.source("E02_ui_souls")
    P = pal("#FF0000", "#0026FF", "#00C000", "#FFFF00", "#FFB3E6", "#FF77C8", "#6FC418", "#FFFFFF", "#000000")
    sheet = np.full((72, 12), -1, dtype=np.int32)
    for r in range(4):
        for y, line in enumerate(HEART):
            for x, v in enumerate(line):
                if v == "#":
                    sheet[r * 12 + 2 + y, 2 + x] = r
    keys = {"P": 4, "p": 4, "s": 5, "G": 6, "W": 7, "K": 8}
    for r, art in ((4, PLUSH), (5, FROG)):
        for y, line in enumerate(art):
            for x, v in enumerate(line):
                if v != ".":
                    sheet[r * 12 + y, x] = keys[v]
    img = Image.fromarray(rl.to_rgba(sheet, P))
    ctx.save_img(img, "assets/ui/souls.png")
    names = ["red", "blue", "green", "yellow", "plush", "frog"]
    ctx.save_json({"image": "souls.png", "cell": [12, 12], "origin": [6, 6],
                   "anims": {n: {"row": i, "frames": 1} for i, n in enumerate(names)}}, "assets/ui/souls.json")
    ctx.preview(guide, img, "E02_ui_souls", 12)


# ---------------------------------------------------------------- E10 save point
def star_frame(theta, fill, shine, edge, size=(20, 10), cx=10.0, cy=5.0, r_out=5.0, r_in=2.0, ss=8):
    """4-point star, supersampled to a hard 1x mask, outlined, with a bright core."""
    w, h = size
    pts = []
    for k in range(8):
        a = theta + k * np.pi / 4
        r = r_out if k % 2 == 0 else r_in
        pts.append((cx + r * np.sin(a), cy - r * np.cos(a)))
    from PIL import ImageDraw
    big = Image.new("L", (w * ss, h * ss), 0)
    ImageDraw.Draw(big).polygon([(x * ss, y * ss) for x, y in pts], fill=255)
    cover = (np.asarray(big) > 0).reshape(h, ss, w, ss).mean(axis=(1, 3))
    cell = np.where(cover >= 0.4, fill, -1).astype(np.int32)
    grown = rl.outline(cell, edge)  # rim pixels become the dark outline
    grown[int(cy) - 1:int(cy) + 1, int(cx) - 1:int(cx) + 1] = shine
    return grown


def e10(ctx):
    guide = reduce_generic.source("E10_props_save_point")
    P = pal("#FFD23F", "#FFF6B0", "#8A5A00", "#3FA34D", "#2C7A38", "#7CFF3A", "#E6E6E6")
    STAR, SHINE, BROWN, BOTTLE, BSHADE, LABEL, CAP = range(7)
    rgb, fg = rl.load(guide)
    idx = rl.mode_filter(rl.snap(rgb, P), 5)
    cols = rl.slice_cells(fg, [8])[0]
    W, H = 20, 28
    sheet = np.full((H, W * 8), -1, dtype=np.int32)
    # bottle: frame 0 of the guide, copied into all 8 frames (sheet section 9)
    x0, y0, x1, y1 = cols[0]
    sub = fg[y0:y1, x0:x1]
    gap = np.where(sub.sum(axis=1) < 2)[0]          # empty rows between star and bottle
    split = y0 + gap[len(gap) // 2]
    bottle_top = split + np.where(fg[split:y1, x0:x1].sum(axis=1) >= 2)[0][0]
    scale = (y1 - bottle_top) / 18.0
    print(f"  E10 scale {scale:.2f}")
    m = np.zeros_like(fg)
    m[bottle_top:y1, x0:x1] = fg[bottle_top:y1, x0:x1]
    ax, ay = rl.foot_anchor(fg, (x0, bottle_top, x1, y1), 0.5)
    w = np.ones(len(P))
    w[[BROWN]] = 1.4
    w[[CAP, LABEL]] = 1.5
    bottle = rl.reduce_region(idx, m, ax, ay, scale, 10, 28, W, H, len(P), w)
    bottle = rl.drop_specks(bottle, 2)
    # the AI stars are irregular at 10 px, so the spin is rasterised: a 4-point star turning
    # 11.25 degrees per frame (8 frames = its 90-degree symmetry, so the loop is seamless)
    for f in range(8):
        star = star_frame(f * np.pi / 16, STAR, SHINE, BROWN)
        cell = bottle.copy()
        cell[:10][star >= 0] = star[star >= 0]
        sheet[:, f * W:(f + 1) * W] = cell
    img = Image.fromarray(rl.to_rgba(sheet, P))
    ctx.save_img(img, "assets/props/save_point.png")
    ctx.save_json({"image": "save_point.png", "cell": [20, 28], "origin": [10, 27],
                   "anims": {"spin": {"row": 0, "frames": 8, "fps": 8}}}, "assets/props/save_point.json")
    ctx.preview(guide, img, "E10_props_save_point", 6)


# ---------------------------------------------------------------- F01 tileset
def f01(ctx):
    import reduce_f01
    guide = reduce_generic.source("F01_tiles_ruins")
    sheet, alpha = reduce_f01.build(guide)
    rgba = rl.to_rgba(sheet, reduce_f01.P)
    rgba[..., 3] = np.where(sheet >= 0, alpha, 0)
    img = Image.fromarray(rgba)
    ctx.save_img(img, "assets/tiles/ruins.png")
    ctx.preview(guide, img, "F01_tiles_ruins", 5)
    if ctx.preview_dir:
        # seam test: every floor-type tile repeated 3x3
        floors = [(0, c) for c in range(8)] + [(1, 4), (1, 6), (1, 7), (3, 1), (3, 4), (5, 4)]
        a = np.array(img)
        out = Image.new("RGBA", (len(floors) * 66, 66), (40, 40, 48, 255))
        for i, (r, c) in enumerate(floors):
            t = Image.fromarray(a[r * 20:(r + 1) * 20, c * 20:(c + 1) * 20])
            for yy in range(3):
                for xx in range(3):
                    out.paste(t, (i * 66 + 3 + xx * 20, 3 + yy * 20))
        out.resize((out.width * 3, out.height * 3), Image.NEAREST).save(os.path.join(ctx.preview_dir, "F01_seams.png"))


# ---------------------------------------------------------------- E08 title logo
def e08(ctx):
    guide = reduce_generic.source("E08_ui_logo")
    rgb = np.asarray(Image.open(guide).convert("RGB")).astype(np.int32)
    # palette is white only: the guide's black outline is not part of the asset, so key on white
    fg = (rgb.min(axis=2) > 170) & ~((rgb[..., 1] < 110) & (rgb[..., 0] > 170))
    ys, xs = np.where(fg)
    x0, x1, y0, y1 = xs.min(), xs.max() + 1, ys.min(), ys.max() + 1
    scale = max((x1 - x0) / 184.0, (y1 - y0) / 30.0)   # spec: ~16 px free on the left for the red n (E09)
    idx = np.zeros(fg.shape, dtype=np.int32)
    cell = rl.reduce_region(idx, fg, x0, (y0 + y1) / 2, scale, 16, 15, 200, 30, 1)
    cell = rl.drop_specks(cell, 3)
    cell[:, :16] = -1  # keep the n slot clear of stray specks
    img = Image.fromarray(rl.to_rgba(cell, pal("#FFFFFF")))
    ctx.save_img(img, "assets/ui/logo.png")
    ctx.preview(guide, img, "E08_ui_logo", 4)


# ---------------------------------------------------------------- E11 HUD pieces
# 60x10 is too small for the guide's five pieces to survive a reduction, so they are set by
# hand in the guide's order: HP bar, HP lost, FUR overlay (solid, dotted), "FUR" label.
FUR_LABEL = ["###.#.#.##.", "#...#.#.#.#", "##..#.#.##.", "#...#.#.#.#", "#....#..#.#"]


def e11(ctx):
    guide = reduce_generic.source("E11_ui_hud")
    P = pal("#FFFFFF", "#000000", "#FFFF00", "#FF0000", "#B04CFF")
    WHITE, BLACK, YEL, RED, FUR = range(5)
    c = np.full((10, 60), -1, dtype=np.int32)
    for k, fill in enumerate((YEL, RED, FUR, None)):
        x = k * 11
        c[1:9, x:x + 10] = WHITE
        c[2:8, x + 1:x + 9] = BLACK if fill is None else fill
        if fill is None:  # dotted overlay, as drawn over a partly filled bar
            for yy in range(2, 8):
                for xx in range(x + 1, x + 9):
                    if (xx + yy) % 2 == 0:
                        c[yy, xx] = FUR
    for gy, line in enumerate(FUR_LABEL):
        for gx, v in enumerate(line):
            if v == "#":
                c[2 + gy, 47 + gx] = WHITE
    img = Image.fromarray(rl.to_rgba(c, P))
    ctx.save_img(img, "assets/ui/hud.png")
    ctx.preview(guide, img, "E11_ui_hud", 8)


# ---------------------------------------------------------------- G01 lore4 card
def g01(ctx):
    guide = reduce_generic.source("G01_img_lore4")
    # sepia ramp only: the optional red is reserved for BEGONE!, and the guide's red is the chip bag
    P = pal("#2B1608", "#5A3416", "#8C5A24", "#B87D35", "#D9A04A", "#F0C878", "#FFE8B8")
    src = Image.open(guide).convert("RGB").resize((398, 214), Image.BOX)
    # map by luminance rank onto the 7-step ramp: nearest-RGB snapping flattens the guide's
    # mid tones into two colors, a stretched luminance keeps the light/dark reading
    a = np.asarray(src).astype(np.float64)
    lum = a @ np.array([0.299, 0.587, 0.114])
    lo, hi = np.percentile(lum, [3, 99])
    t = np.clip((lum - lo) / (hi - lo), 0, 1)
    idx = np.minimum((t * len(P)).astype(np.int32), len(P) - 1)
    idx = rl.despeckle(idx, passes=1)
    img = Image.fromarray(P[idx].astype(np.uint8))
    ctx.save_img(img, "assets/img/lore4.bmp")
    ctx.preview(guide, img.convert("RGBA"), "G01_img_lore4", 2)


SHEETS = {"A04": a04, "B01": b01, "E01": e01, "E02": e02, "E10": e10, "F01": f01, "G01": g01, "E08": e08, "E11": e11}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("sheets", nargs="*")
    ap.add_argument("--out", help="write here instead of the final assets/ paths")
    ap.add_argument("--preview", help="write side-by-side previews here")
    ap.add_argument("--force", action="store_true", help="overwrite existing untracked (generated) final files")
    ap.add_argument("--force-tracked", action="store_true", help="also overwrite files committed to git")
    a = ap.parse_args()
    ctx = Ctx(a.out, a.preview, a.force, a.force_tracked)
    generic = {s["id"]: s for s in reduce_generic.all_specs()}
    # A01 (Pepe) is a hand-finished sheet already in assets/; never regenerate it from here
    names = a.sheets or list(SHEETS) + [k for k in generic if k not in SHEETS and k != "A01"]
    for name in names:
        print(name)
        if name in SHEETS:
            SHEETS[name](ctx)
        else:
            reduce_generic.run(generic[name], ctx)


if __name__ == "__main__":
    main()
