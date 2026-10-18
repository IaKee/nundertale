"""F01 Ruins tileset (160x160, 20x20 tiles) from the AI guide.

The guide's tiles are separated by magenta gaps, carry their own border lines and do not
follow the slot layout of docs/art_prompts/ruins/F01_tiles_ruins.md section 3. So:
- stone floors and the arch are reduced from the guide, then cleaned and seam-fixed
  (cracks kept, the 1-px ring flattened to the base color so tiles join without a line);
- carpet, bricks, stairs, doors, pillar and the small decals are drawn here in the guide's
  palette and motifs, periodic by construction so they tile seamlessly.
"""
from __future__ import annotations

import numpy as np

import reduce_lib as rl

HEX = ["#111111", "#3B1F5C", "#4A2A70", "#5E3A87", "#7A55A8", "#9C7CC4", "#C9A0DC", "#F4E3FF", "#7CFF3A", "#1E3FBF"]
BLACK, DARK, DARK2, MID, MID2, LIGHT, BANNER, HI, NEON, CRT = range(10)
P = np.array([rl.hexrgb(h) for h in HEX], dtype=np.int32)
T = 20
DARKER = {BLACK: BLACK, DARK: BLACK, DARK2: DARK, MID: DARK2, MID2: MID, LIGHT: MID2, BANNER: LIGHT,
          HI: BANNER, NEON: NEON, CRT: CRT}


def tile(fill=MID):
    return np.full((T, T), fill, dtype=np.int32)


def darken(t, rows=None):
    out = t.copy()
    sel = slice(None) if rows is None else rows
    sub = out[sel]
    for k, v in DARKER.items():
        sub[t[sel] == k] = v
    return out


def stamp(t, art: list[str], x, y, keys: dict):
    for gy, line in enumerate(art):
        for gx, v in enumerate(line):
            if v in keys:
                t[(y + gy) % T, (x + gx) % T] = keys[v]


# ------------------------------------------------------------------ guide reductions
class Guide:
    def __init__(self, path):
        rgb, self.fg = rl.load(path)
        self.idx = rl.mode_filter(rl.snap(rgb, P), 3)
        try:
            # v1: tiles separated by magenta gaps
            rows = rl.bands(self.fg.sum(axis=1), 8)
            cols = rl.bands(self.fg[rows[0][0]:rows[0][1]].sum(axis=0), 8)
        except ValueError:
            # v2+: the full-tile rows touch edge to edge, so use the even 8x8 lattice
            H, W = self.fg.shape
            rows = [(round(H * k / 8), round(H * (k + 1) / 8)) for k in range(8)]
            cols = [(round(W * k / 8), round(W * (k + 1) / 8)) for k in range(8)]
        self.rows, self.cols = rows, cols

    def box(self, r, c):
        (y0, y1), (x0, x1) = self.rows[r], self.cols[c]
        return x0, y0, x1, y1

    def dark_fraction(self, r, c, inset=0.06, classes=(BLACK, DARK)):
        from PIL import Image
        x0, y0, x1, y1 = self.box(r, c)
        dx, dy = int((x1 - x0) * inset), int((y1 - y0) * inset)
        m = np.isin(self.idx[y0 + dy:y1 - dy, x0 + dx:x1 - dx], classes).astype(np.float32)
        return np.asarray(Image.fromarray(m).resize((T, T), Image.BOX))

    def reduce(self, r, c, w=T, h=T, inset=0.0, region=None):
        x0, y0, x1, y1 = region or self.box(r, c)
        dx, dy = (x1 - x0) * inset, (y1 - y0) * inset
        x0, x1, y0, y1 = x0 + dx, x1 - dx, y0 + dy, y1 - dy
        sc = max((x1 - x0) / w, (y1 - y0) / h)
        m = np.zeros_like(self.fg)
        m[int(y0):int(y1), int(x0):int(x1)] = self.fg[int(y0):int(y1), int(x0):int(x1)]
        return rl.reduce_region(self.idx, m, x0, y0, sc, 0, 0, w, h, len(P), alpha_thr=0.3)


def clean_stone(dark_frac, thr=0.24):
    """Guide stone -> base MID with DARK cracks and a MID2 bevel; edge ring flattened (seam fix).

    `dark_frac` is the share of crack-colored guide pixels under each 1x pixel: thin AI crack
    lines never win a majority vote, so they are thresholded instead."""
    crack = dark_frac > thr
    out = tile(MID)
    out[crack] = DARK
    out = rl.prune_color(out, DARK, 3)
    out[out != DARK] = MID
    # bevel: light pixel right below / right of a crack
    c = out == DARK
    bev = (np.roll(c, 1, 0) | np.roll(c, 1, 1)) & ~c
    out[bev] = MID2
    out[0, :] = out[-1, :] = out[:, 0] = out[:, -1] = MID
    return out


# ------------------------------------------------------------------ carpet
def carpet_center():
    t = tile(DARK)
    # 90s convention carpet: neon zigzag squiggles + lilac dots on a 20x20 torus
    squig = ["#.#", ".#."]
    for x, y in ((1, 2), (11, 5), (6, 11), (15, 15), (3, 16)):
        stamp(t, squig, x, y, {"#": NEON})
    stamp(t, ["#.", ".#", "#."], 16, 1, {"#": NEON})
    stamp(t, [".#", "#.", ".#"], 8, 17, {"#": NEON})
    for x, y in ((7, 3), (17, 8), (2, 9), (12, 12), (9, 7), (18, 19), (5, 6)):
        t[y, x] = MID2
    return t


def carpet_edge(top=False, bottom=False, left=False, right=False):
    t = carpet_center()
    zig = [1, 0]  # zigzag line: 2-px period, one pixel up, one down

    def band(get, set_, n):
        for i in range(T):
            for d in range(5):
                set_(i, d, DARK)
            set_(i, 0, DARK2)        # outer lip
            set_(i, 1, NEON)         # straight neon line
            set_(i, 3 + zig[i % 2], NEON)  # zigzag

    if top:
        band(None, lambda i, d, v: t.__setitem__((d, i), v), T)
    if bottom:
        band(None, lambda i, d, v: t.__setitem__((T - 1 - d, i), v), T)
    if left:
        band(None, lambda i, d, v: t.__setitem__((i, d), v), T)
    if right:
        band(None, lambda i, d, v: t.__setitem__((i, T - 1 - d), v), T)
    # corners: clear the crossing so the two borders meet cleanly
    for cy, cx, sy, sx in ((top, left, 0, 0), (top, right, 0, T - 5), (bottom, left, T - 5, 0), (bottom, right, T - 5, T - 5)):
        if cy and cx:
            t[sy:sy + 5, sx:sx + 5] = DARK
    if top and left:
        t[0, 0:5] = DARK2; t[0:5, 0] = DARK2; t[1, 1:5] = NEON; t[1:5, 1] = NEON
    if top and right:
        t[0, T - 5:] = DARK2; t[0:5, T - 1] = DARK2; t[1, T - 5:T - 1] = NEON; t[1:5, T - 2] = NEON
    if bottom and left:
        t[T - 1, 0:5] = DARK2; t[T - 5:, 0] = DARK2; t[T - 2, 1:5] = NEON; t[T - 5:T - 1, 1] = NEON
    if bottom and right:
        t[T - 1, T - 5:] = DARK2; t[T - 5:, T - 1] = DARK2; t[T - 2, T - 5:T - 1] = NEON; t[T - 5:T - 1, T - 2] = NEON
    return t


# ------------------------------------------------------------------ walls
def bricks():
    t = tile(DARK)
    for course in range(4):
        y = course * 5
        off = 0 if course % 2 == 0 else 5
        for bx in range(2):
            x0 = (off + bx * 10) % T
            for x in range(9):
                xx = (x0 + 1 + x) % T
                t[y + 1:y + 5, xx] = DARK2
                t[y + 1, xx] = MID
    return t


def wall_front(left=False, right=False):
    t = bricks()
    if left:
        t[:, 0] = BLACK; t[:, 1] = MID2; t[:, 2] = MID
    if right:
        t[:, T - 1] = BLACK; t[:, T - 2] = DARK2; t[:, T - 3] = DARK
    return t


def wall_top(left=False, right=False, bottom_lip=True):
    t = tile(MID2)
    for x, y in ((3, 4), (12, 9), (7, 14), (16, 3), (1, 11)):
        t[y, x] = LIGHT
    if bottom_lip:
        t[T - 2, :] = LIGHT
        t[T - 1, :] = DARK
    if left:
        t[:, 0] = BLACK; t[:, 1] = LIGHT
    if right:
        t[:, T - 1] = BLACK; t[:, T - 2] = MID
    return t


def inner_corner(side):
    """Cap turning down into a side wall: cap fills the outer half, lip along the inner edges."""
    t = wall_top(bottom_lip=False)
    if side == "L":
        t[:, T - 2] = LIGHT; t[:, T - 1] = DARK
        t[T - 2, :T - 1] = LIGHT; t[T - 1, :] = DARK
    else:
        t[:, 1] = LIGHT; t[:, 0] = DARK
        t[T - 2, 1:] = LIGHT; t[T - 1, :] = DARK
    return t


def outer_corner(side):
    """Convex corner: only a quarter of cap, the rest is wall front."""
    t = bricks()
    q = wall_top(bottom_lip=False)[:12, :12]
    if side == "L":
        t[:12, 8:] = q
        t[:12, 8] = BLACK; t[:12, 9] = LIGHT; t[11, 8:] = DARK; t[10, 9:] = LIGHT
        t[12:, 8] = BLACK
    else:
        t[:12, :12] = q
        t[:12, 11] = BLACK; t[:12, 10] = MID; t[11, :12] = DARK; t[10, :11] = LIGHT
        t[12:, 11] = BLACK
    return t


# ------------------------------------------------------------------ stairs, doors, pillar
def stairs(part, side):
    t = tile(MID2)
    for y in range(T):
        k = y % 5
        t[y, :] = LIGHT if k == 0 else MID2 if k < 3 else DARK2
    if part == "top":
        t[0:2, :] = DARK
    if part == "base":
        t[15:, :] = MID
        t[15, :] = DARK
    if side == "L":
        t[:, 0] = BLACK; t[:, 1:3] = MID; t[:, 3] = DARK
    if side == "R":
        t[:, T - 1] = BLACK; t[:, T - 3:T - 1] = DARK2; t[:, T - 4] = DARK
    return t


def door(open_):
    top, bot = wall_front(), wall_front()
    full = np.vstack([top, bot])
    x0, x1, y0 = 3, 17, 5
    full[y0:, x0:x1] = BLACK if open_ else MID
    full[y0, x0:x1] = DARK
    full[y0:, x0] = DARK
    full[y0:, x1 - 1] = DARK
    if open_:
        full[y0 + 1:, x1 - 4:x1 - 1] = MID2   # door leaf swung inward
        full[y0 + 1:, x1 - 4] = DARK
    else:
        full[y0 + 3:y0 + 15, x0 + 3:x1 - 3] = MID2
        full[y0 + 18:2 * T - 3, x0 + 3:x1 - 3] = MID2
        full[y0 + 3:y0 + 15, x0 + 3] = DARK2
        full[y0 + 18:2 * T - 3, x0 + 3] = DARK2
        full[y0 + 19, x1 - 4] = HI           # knob
    full[2 * T - 1, x0:x1] = DARK
    return full[:T], full[T:]


def pillar(part):
    t = np.full((T, T), -1, dtype=np.int32)
    t[:, 4] = BLACK; t[:, 15] = BLACK
    t[:, 5:7] = LIGHT; t[:, 7:11] = MID2; t[:, 11:14] = MID; t[:, 14] = DARK2
    t[:, 9] = MID   # flute
    if part == "top":
        t[:8] = -1
        t[8:14, 2:18] = MID2
        t[8, 2:18] = BLACK; t[13, 2:18] = BLACK; t[8:14, 2] = BLACK; t[8:14, 17] = BLACK
        t[9, 3:17] = LIGHT
    if part == "base":
        t[12:T, 2:18] = MID
        t[12, 2:18] = BLACK; t[T - 1, 2:18] = BLACK; t[12:, 2] = BLACK; t[12:, 17] = BLACK
        t[13, 3:17] = MID2
    return t


def neon_sign():
    t = np.full((T, T), -1, dtype=np.int32)
    t[3:17, 2:18] = BLACK
    t[3, 2:18] = DARK; t[16, 2:18] = DARK; t[3:17, 2] = DARK; t[3:17, 17] = DARK
    paw = ["..#.#..", ".#...#.", "...#...", "..###..", ".#####.", ".##.##."]
    stamp(t, paw, 6, 6, {"#": NEON})
    return t


def light_beam(part):
    """Flat translucent beam: returns (index tile, alpha tile)."""
    t = np.full((T, T), -1, dtype=np.int32)
    a = np.zeros((T, T), dtype=np.uint8)
    for y in range(T):
        yy = y if part == "top" else y + T
        half = 3 + yy * 5 // (2 * T)       # cone widens downward over two tiles
        x0, x1 = 10 - half, 10 + half
        t[y, x0:x1] = HI
        a[y, x0:x1] = 70
        a[y, x0 + 2:x1 - 2] = 110
    return t, a


def ledge(base):
    t = base.copy()
    t[14, :] = LIGHT
    t[15:, :] = DARK
    t[T - 1, :] = BLACK
    return t


# ------------------------------------------------------------------ decals on stone
def decal_wrapper(base):
    t = base.copy()
    art = ["..####..", ".#BBBB##", "#BNNBBB#", "#BBBNNB#", ".##BBBB#", "...####."]
    stamp(t, art, 6, 7, {"#": DARK, "B": BANNER, "N": NEON})
    return t


def decal_crumbs(base):
    t = base.copy()
    for x, y, c in ((4, 5, BANNER), (5, 5, HI), (12, 4, BANNER), (9, 9, BANNER), (10, 9, BANNER), (15, 12, HI),
                    (6, 14, BANNER), (13, 15, BANNER), (14, 15, HI), (3, 10, BANNER)):
        t[y, x] = c
        if c == BANNER and t[y + 1, x] != BANNER:
            t[y + 1, x] = DARK
    return t


def decal_glitter(base):
    t = base.copy()
    for x, y, c in ((4, 4, HI), (13, 3, NEON), (8, 9, HI), (16, 11, BANNER), (3, 14, NEON), (11, 16, HI), (15, 17, BANNER)):
        t[y, x] = c
    stamp(t, [".#.", "#H#", ".#."], 7, 8, {"#": BANNER, "H": HI})
    return t


def decal_tape(base):
    t = base.copy()
    t[8:12, :] = BANNER
    t[8, :] = HI
    t[11, :] = LIGHT
    t[8:12, 6] = LIGHT   # torn seam
    return t


def decal_mat(base):
    t = base.copy()
    t[4:16, 2:18] = MID2
    t[4, 2:18] = DARK; t[15, 2:18] = DARK; t[4:16, 2] = DARK; t[4:16, 17] = DARK
    for x in range(4, 16, 2):
        t[7, x] = NEON
        t[12, x + 1] = NEON
    stamp(t, ["#.#.#", ".###.", "..#.."], 8, 9, {"#": HI})  # little paw
    return t


def decal_paw(base):
    t = base.copy()
    stamp(t, [".#..#.", "......", "#....#", "..##..", ".####.", ".####.", "..##.."], 7, 6, {"#": DARK2})
    return t


def worn(c):
    t = c.copy()
    for x, y in ((4, 4), (5, 4), (4, 5), (12, 9), (13, 9), (13, 10), (8, 15), (9, 15)):
        t[y, x] = DARK2
    t[t == NEON] = np.where(np.arange(T)[None, :].repeat(T, 0)[t == NEON] % 3 == 0, DARK2, NEON)
    return t


def stain(c):
    t = c.copy()
    stamp(t, ["..####..", ".######.", "########", "#######.", ".#####..", "..##...."], 5, 7, {"#": DARK2})
    return t


def build(guide_path):
    g = Guide(guide_path)
    stone = [clean_stone(g.dark_fraction(0, c)) for c in (0, 1, 4, 6)]
    A, B, C, CRACK = stone
    sheet = np.full((8 * T, 8 * T), -1, dtype=np.int32)
    alpha = np.full((8 * T, 8 * T), 255, dtype=np.uint8)

    def put(r, c, t, a=None):
        sheet[r * T:(r + 1) * T, c * T:(c + 1) * T] = t
        if a is not None:
            alpha[r * T:(r + 1) * T, c * T:(c + 1) * T] = a

    # row 0: stone A, B, C, cracked, glitter, wrapper, crumbs, void
    for c, t in enumerate([A, B, C, CRACK, decal_glitter(A), decal_wrapper(B), decal_crumbs(C), tile(BLACK)]):
        put(0, c, t)
    # row 1: carpet tl, top, tr, left, center, right, worn, stain
    cc = carpet_center()
    for c, t in enumerate([carpet_edge(top=True, left=True), carpet_edge(top=True), carpet_edge(top=True, right=True),
                           carpet_edge(left=True), cc, carpet_edge(right=True), worn(cc), stain(cc)]):
        put(1, c, t)
    # row 2: carpet bl, bottom, br, tape, welcome mat, paw print, free, free
    for c, t in enumerate([carpet_edge(bottom=True, left=True), carpet_edge(bottom=True),
                           carpet_edge(bottom=True, right=True), decal_tape(A), decal_mat(B), decal_paw(C)]):
        put(2, c, t)
    # row 3: wall top l/m/r, wall front l/m/r, cracked front, front with vent
    cracked = wall_front()
    stamp(cracked, ["..#...", "..#...", ".#....", ".##...", "...#..", "...##.", "....#.", "...#..", "...#..",
                    "....#.", "....##"], 8, 3, {"#": BLACK})
    vent = wall_front()
    vent[5:12, 4:16] = DARK
    for y in (6, 8, 10):
        vent[y, 5:15] = BLACK
    for y in (7, 9):
        vent[y, 5:15] = MID2
    for c, t in enumerate([wall_top(left=True), wall_top(), wall_top(right=True), wall_front(left=True), wall_front(),
                           wall_front(right=True), cracked, vent]):
        put(3, c, t)
    # row 4: inner corner l/r, outer corner l/r, banner hook, outlet + cable, wall-base shadow, dark wall
    hook = wall_front()
    stamp(hook, ["##", ".#", ".#", "##"], 9, 2, {"#": BANNER})
    outlet = wall_front()
    stamp(outlet, ["#####", "#HHH#", "#K.K#", "#HHH#", "#HHH#", "#####", "..#..", "..#..", "...#.", "...#.",
                   "....#", "....#"], 7, 3, {"#": DARK, "H": HI, "K": BLACK})
    for y in range(9, T):
        outlet[y, 9 + min(3, (y - 9) // 2)] = BLACK
    shadow = darken(A, slice(0, 4))
    for c, t in enumerate([inner_corner("L"), inner_corner("R"), outer_corner("L"), outer_corner("R"), hook, outlet,
                           shadow, darken(wall_front())]):
        put(4, c, t)
    # row 5: stairs top l/m/r, middle l/m/r, base l/r
    for c, (part, side) in enumerate([("top", "L"), ("top", "M"), ("top", "R"), ("mid", "L"), ("mid", "M"),
                                      ("mid", "R"), ("base", "L"), ("base", "R")]):
        put(5, c, stairs(part, side))
    # row 6: closed door top/bottom, open door top/bottom, arch 2x2 (from the guide)
    dt, db = door(False)
    ot, ob = door(True)
    arch = g.reduce(5, 4, 40, 40, inset=0.0)
    arch = rl.drop_specks(arch, 3)
    arch[arch < 0] = -1
    # the arch opening is a doorway: fill it with the void color
    for y in range(40):
        xs = np.where(arch[y] >= 0)[0]
        if len(xs):
            row = arch[y, xs[0]:xs[-1] + 1]
            row[row < 0] = BLACK
    for c, t in enumerate([dt, db, ot, ob, arch[:T, :T], arch[:T, T:], arch[T:, :T], arch[T:, T:]]):
        put(6, c, t)
    # row 7: pillar top/mid/base, neon sign, light beam top/base, ledge edge, free
    for c, t in enumerate([pillar("top"), pillar("mid"), pillar("base"), neon_sign()]):
        put(7, c, t)
    for c, part in ((4, "top"), (5, "base")):
        t, a = light_beam(part)
        put(7, c, t, a)
    put(7, 6, ledge(A))
    return sheet, alpha
