#!/usr/bin/env python3
"""Builds the FUR-stage overlay sheet for Grub the human NEET (FUR-007, ART-101, LORE v3.3 sections 8.4 and 10.1).

The overlays sit on top of assets/sprites/ow/grub.png (ART-180) and are read from it, so every anchor follows
the walk bob of the base frames. One 32x32 cell per base cell, the same row/column layout as grub.png
(down 4, up 4, left 2, right 2, lying 1, get_up 3 = 6 rows), repeated once per stage:

  stage block   sheet rows   FUR      look (LORE 8.4)
  ears          0-5          20-39    fluffy ears poke up through the blanket hood, next to the headset
  paws          6-11         40-59    paw hands and paw feet (the flip-flops are gone)
  tail          12-17        60-79    a shiny tail that wags as he walks (walk frames sway, stand frames rest)
  snout         18-23        80-99    a mint-green snout and fur patches on the face

Stages are cumulative: the game draws grub.png, then every block whose min_fur <= FUR (ears, paws, tail, snout).
Overlay cells hold only the pixels the stage adds or recolors, so a single draw call per block is enough and a
stage can be hidden without touching the base. The Mirror (100) is a choice screen, not a sprite (LORE 8.5).

Usage (repo root, needs Pillow + numpy):
  C:/Python313/python.exe tools/art/grub_fur/build_grub_fur.py            writes assets/sprites/ow/grub_fur.{png,json}
  C:/Python313/python.exe tools/art/grub_fur/build_grub_fur.py --preview out.png
                                                                          also writes a 2x preview (every stage
                                                                          composited on the base, plus the raw overlays)
"""
import argparse
import importlib.util
import json
import os

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
OW = os.path.join(ROOT, "assets", "sprites", "ow")
BASE_PNG = os.path.join(OW, "grub.png")
OUT_PNG = os.path.join(OW, "grub_fur.png")
OUT_JSON = os.path.join(OW, "grub_fur.json")

CW = CH = 32
ROWS_PER_STAGE = 6
STAGES = [("ears", 20), ("paws", 40), ("tail", 60), ("snout", 80)]

# Same frame counts as grub.json (down 4, up 4, left 2, right 2, lying 1, get_up 3).
FRAMES = [(0, c) for c in range(4)] + [(1, c) for c in range(4)] + [(2, c) for c in range(2)] + \
         [(3, c) for c in range(2)] + [(4, 0)] + [(5, c) for c in range(3)]

OUTLINE = (0x14, 0x10, 0x18, 255)
PAL = {
    "O": OUTLINE,
    "M": (0x7d, 0xdc, 0xa0, 255),  # mint fur
    "S": (0x4a, 0xa9, 0x78, 255),  # mint shade
    "H": (0xcf, 0xf7, 0xdc, 255),  # fluff highlight (the "shiny" of the tail)
    "P": (0xf2, 0x9a, 0xc0, 255),  # pink ear inside / pads
    "Q": (0xc4, 0x5a, 0x90, 255),  # pink shade
    "N": (0x2b, 0x2b, 0x2b, 255),  # nose (same dark grey as the headset cups)
}
SKIN = {(233, 217, 207, 255), (196, 169, 158, 255)}  # skin light / shade of grub.png
SOLE = {(43, 43, 43, 255), (255, 255, 255, 255)}      # flip-flop sole and strap highlight
HEADSET = {(140, 111, 134, 255), (154, 154, 170, 255), (43, 43, 43, 255)}  # band, band side, cups: never covered


def load_build_grub():
    spec = importlib.util.spec_from_file_location("build_grub", os.path.join(HERE, "..", "grub", "build_grub.py"))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


# --------------------------------------------------------------------------------------------- templates
# '.' transparent, letters index PAL. Coordinates in the comments are for grub.png cell (0,0) unless noted.

EAR_FRONT = [  # left ear, drawn at (3,0); the right ear is its mirror
    "..OO...",
    ".OHMO..",
    "OMPPMO.",
    "OMPPMMO",
    ".OMPMMO",
    "..OOMMO",
]
EAR_BACK = [  # seen from behind: no pink inside, a shade stripe instead
    "..OO...",
    ".OHMO..",
    "OMSSMO.",
    "OMSSMMO",
    ".OMSMMO",
    "..OOMMO",
]
EAR_SIDE_NEAR = [  # profile, ear next to the headset cup
    ".OO..",
    "OHMO.",
    "OMPMO",
    "OMPPO",
    ".OMMO",
]
EAR_SIDE_FAR = [  # the far ear, a bit smaller, behind the cup
    ".OO.",
    "OHMO",
    "OMPO",
    ".OMO",
]

PAW_HAND = [  # the hand that holds the blanket shut at the chest (front view)
    ".OOO.",
    "OMHMO",
    "OMMSO",
    ".OSO.",
]

# Tail paths (control points, then thickened by tail_pixels). Front and side tails are drawn "behind" the body
# (only where the base is transparent); the back tail hangs over the blanket.
TAIL_FRONT_PATH = {  # peeks out right of the hem; the wag moves the tip up and down
    "mid": [(25, 25), (28, 23), (30, 20), (30, 17)],
    "up": [(25, 25), (28, 22), (30, 18), (29, 14)],
    "down": [(25, 25), (28, 25), (30, 24), (30, 21)],
}
TAIL_SIDE_PATH = {  # left-facing Grub, tail to the right; the right-facing sheet mirrors x
    "mid": [(23, 28), (27, 29), (30, 28), (30, 25)],
    "up": [(23, 26), (27, 25), (30, 22), (30, 19)],
}
TAIL_BACK_PATH = {  # hangs from the hem down the middle and sways
    "mid": [(16, 19), (16, 23), (16, 26)],
    "left": [(16, 19), (15, 23), (13, 26)],
    "right": [(16, 19), (17, 23), (19, 26)],
}

SNOUT_FRONT = [  # x0 = mouth x - 2, y0 = 12; the two outline pixels of the mouth stay as drawn in grub.png
    "..MM..",
    ".MNNM.",
    ".MOOM.",
    "..MM..",
]
CHEEK_PATCHES = [(-3, 14, "M"), (-4, 14, "S"), (-3, 15, "S"), (4, 14, "M"), (5, 14, "S"), (4, 15, "S"),
                 (-1, 18, "M"), (0, 18, "S"), (0, 19, "M")]  # relative to the mouth x; skin only
SNOUT_SIDE_L = [  # left-facing: a muzzle that sticks out of the face
    "..OOO.",
    ".OMMMM",
    "ONMMMS",
    ".OMMS.",
    "..OO..",
]


def tpl_pixels(tpl, mirror=False):
    out = []
    w = max(len(row) for row in tpl)
    for dy, row in enumerate(tpl):
        for dx, ch in enumerate(row):
            if ch == ".":
                continue
            out.append(((w - 1 - dx) if mirror else dx, dy, PAL[ch]))
    return out


def stamp(layer, base, tpl, x0, y0, mirror=False, protect=True):
    for dx, dy, col in tpl_pixels(tpl, mirror):
        x, y = x0 + dx, y0 + dy
        if not (0 <= x < CW and 0 <= y < CH):
            continue
        if protect and tuple(base[y, x]) in HEADSET and base[y, x, 3]:
            continue
        layer[y, x] = col


def tail_pixels(points, brush=2, tip=3):
    """A thick fluffy tail along a polyline: mint body, outline all round, shade underneath, shiny tip."""
    fill = set()

    def block(cx, cy, size):
        for ox in range(size):
            for oy in range(size):
                fill.add((cx - size // 2 + ox, cy - size // 2 + oy))

    for (x0, y0), (x1, y1) in zip(points, points[1:]):
        n = max(abs(x1 - x0), abs(y1 - y0), 1)
        for i in range(n + 1):
            block(int(round(x0 + (x1 - x0) * i / n)), int(round(y0 + (y1 - y0) * i / n)), brush)
    tx, ty = points[-1]
    block(tx, ty, tip)
    ring = {(x + a, y + b) for x, y in fill for a, b in ((1, 0), (-1, 0), (0, 1), (0, -1))} - fill
    px = {p: "O" for p in ring}
    for x, y in fill:
        col = "M"
        if (x, y + 1) in ring or (x + 1, y) in ring:
            col = "S"
        if abs(x - tx) <= tip // 2 and abs(y - ty) <= tip // 2 and x <= tx and y <= ty:
            col = "H"
        px[(x, y)] = col
    return px


def paint(layer, base, px, behind=False, mirror_x=None):
    for (x, y), ch in px.items():
        if mirror_x is not None:
            x = mirror_x - x
        if not (0 <= x < CW and 0 <= y < CH):
            continue
        if behind and base[y, x, 3]:
            continue
        layer[y, x] = PAL[ch]


def recolor(layer, base, box, shade_map, only=None):
    x0, y0, x1, y1 = box
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            c = tuple(base[y, x])
            if c in shade_map and (only is None or c in only):
                layer[y, x] = shade_map[c]


FUR_MAP = {
    (233, 217, 207, 255): PAL["M"],
    (196, 169, 158, 255): PAL["S"],
    (255, 255, 255, 255): PAL["H"],
    (43, 43, 43, 255): PAL["S"],
}


# ----------------------------------------------------------------------------------- per-frame anchors
# Read off grub.png (cell column -> value). Ears follow the hood corner, snout the mouth, paws the hem.

FRONT_EAR_DX = {0: 0, 1: -1, 2: -1, 3: -1}
BACK_EAR = {0: (-1, 3), 1: (-1, 3), 2: (0, 3), 3: (-1, 3)}  # dx, dy (head top of the back pose is 3 rows lower)
# ears in profile: (near ear x, far ear x) per frame; y = 0
LEFT_EARS = {0: (10, 19), 1: (9, 18)}
RIGHT_EARS = {0: (18, 9), 1: (18, 9)}
# profile hand blobs (box where skin is recolored) and muzzle anchors
LEFT_HAND_BOX = {0: (11, 18, 15, 23), 1: (11, 18, 15, 23)}
RIGHT_HAND_BOX = {0: (17, 17, 21, 23), 1: (17, 17, 21, 23)}
LEFT_MUZZLE_X = {0: 4, 1: 3}
RIGHT_MUZZLE_X = {0: 21, 1: 20}

TAIL_FRONT_SEQ = {0: "mid", 1: "up", 2: "mid", 3: "down"}
TAIL_BACK_SEQ = {0: "mid", 1: "left", 2: "mid", 3: "right"}


def find_mouth_x(base):
    """Left x of the two-pixel mouth on row 14 of a front frame (outline pair with skin on both sides)."""
    for x in range(12, 19):
        if (tuple(base[14, x]) == OUTLINE and tuple(base[14, x + 1]) == OUTLINE
                and tuple(base[14, x - 1]) in SKIN and tuple(base[14, x + 2]) in SKIN):
            return x
    raise RuntimeError("mouth not found")


# ------------------------------------------------------------------------------------------ the stages
def new_layer():
    return np.zeros((CH, CW, 4), dtype=np.uint8)


def ears(row, col, base):
    layer = new_layer()
    if row == 0:
        dx = FRONT_EAR_DX[col]
        stamp(layer, base, EAR_FRONT, 3 + dx, 0)
        stamp(layer, base, EAR_FRONT, 23 + dx, 0, mirror=True)
    elif row == 1:
        dx, dy = BACK_EAR[col]
        stamp(layer, base, EAR_BACK, 3 + dx, dy)
        stamp(layer, base, EAR_BACK, 23 + dx, dy, mirror=True)
    elif row == 2:
        near, far = LEFT_EARS[col]
        stamp(layer, base, EAR_SIDE_FAR, far, 0)
        stamp(layer, base, EAR_SIDE_NEAR, near, 0)
    elif row == 3:
        near, far = RIGHT_EARS[col]
        stamp(layer, base, EAR_SIDE_FAR, far, 0, mirror=True)
        stamp(layer, base, EAR_SIDE_NEAR, near, 0, mirror=True)
    return layer


def paws(row, col, base):
    layer = new_layer()
    feet = {k: v for k, v in FUR_MAP.items()}
    recolor(layer, base, (0, 28, CW - 1, CH - 1), feet, only=SKIN | SOLE)
    if row == 0:
        stamp(layer, base, PAW_HAND, 16 + FRONT_EAR_DX[col], 18)
    elif row == 2:
        recolor(layer, base, LEFT_HAND_BOX[col], FUR_MAP, only=SKIN)
    elif row == 3:
        recolor(layer, base, RIGHT_HAND_BOX[col], FUR_MAP, only=SKIN)
    return layer


def tail(row, col, base):
    layer = new_layer()
    if row == 0:
        paint(layer, base, tail_pixels(TAIL_FRONT_PATH[TAIL_FRONT_SEQ[col]]), behind=True)
    elif row == 1:
        paint(layer, base, tail_pixels(TAIL_BACK_PATH[TAIL_BACK_SEQ[col]], brush=3, tip=4))
    elif row in (2, 3):
        px = tail_pixels(TAIL_SIDE_PATH["mid" if col == 0 else "up"])
        paint(layer, base, px, behind=True, mirror_x=None if row == 2 else CW - 1)
    return layer


def snout(row, col, base):
    layer = new_layer()
    if row == 0:
        mx = find_mouth_x(base)
        stamp(layer, base, SNOUT_FRONT, mx - 2, 12)
        for dx, y, ch in CHEEK_PATCHES:
            if tuple(base[y, mx + dx]) in SKIN:
                layer[y, mx + dx] = PAL[ch]
    elif row == 2:
        stamp(layer, base, SNOUT_SIDE_L, LEFT_MUZZLE_X[col], 12)
    elif row == 3:
        stamp(layer, base, SNOUT_SIDE_L, RIGHT_MUZZLE_X[col], 12, mirror=True)
    return layer


STAGE_FN = {"ears": ears, "paws": paws, "tail": tail, "snout": snout}


def lying_cell(stack):
    """build_grub.lying_cell for an N-channel stack (base RGBA first): the front pose turned 90 degrees."""
    rot = np.rot90(stack, 1)
    out = np.zeros((CH, CW, stack.shape[2]), dtype=np.uint8)
    ys = np.where(rot[:, :, 3].any(axis=1))[0]
    xs = np.where(rot[:, :, 3].any(axis=0))[0]
    rot = rot[ys[0]:ys[-1] + 1, xs[0]:xs[-1] + 1]
    h, w = rot.shape[:2]
    x0 = (CW - w) // 2
    out[CH - h:, x0:x0 + w] = rot
    return out


def base_cell(sheet, row, col):
    return sheet[row * CH:(row + 1) * CH, col * CW:(col + 1) * CW]


def build_stage(name, sheet, bg):
    """Returns the (6*32, 4*32, 4) block of one stage."""
    fn = STAGE_FN[name]
    block = np.zeros((ROWS_PER_STAGE * CH, 4 * CW, 4), dtype=np.uint8)
    cells = {}
    for row, col in FRAMES:
        if row < 4:
            cells[(row, col)] = fn(row, col, base_cell(sheet, row, col))
    # lying and get_up are derived from the stand_down overlay with the same transforms that built the base
    stand = np.concatenate([base_cell(sheet, 0, 0), cells[(0, 0)]], axis=2)
    lying = lying_cell(stand)
    assert (lying[:, :, :4] == base_cell(sheet, 4, 0)).all(), "lying base does not match grub.png"
    cells[(4, 0)] = lying[:, :, 4:] if name != "tail" else new_layer()  # the tail is under him on the plush pile
    for i, f in enumerate(bg.get_up_cells(stand)):
        assert (f[:, :, :4] == base_cell(sheet, 5, i)).all(), "get_up base does not match grub.png"
        cells[(5, i)] = f[:, :, 4:]
    for (row, col), cell in cells.items():
        block[row * CH:(row + 1) * CH, col * CW:(col + 1) * CW] = cell
    return block


def build_sheet():
    sheet = np.array(Image.open(BASE_PNG).convert("RGBA"))
    bg = load_build_grub()
    return sheet, np.concatenate([build_stage(n, sheet, bg) for n, _ in STAGES], axis=0)


def check_palette(over):
    cols = {tuple(p) for p in over.reshape(-1, 4) if p[3]}
    assert all(c[3] == 255 for c in cols), "semi-transparent pixel"
    assert (0, 0, 0, 255) not in cols, "pure black is not allowed"
    return len(cols)


def anims_for(name, offset):
    base = json.load(open(os.path.join(OW, "grub.json")))["anims"]
    out = {}
    for k, v in base.items():
        a = dict(v)
        a["row"] = v["row"] + offset
        out["%s_%s" % (name, k)] = a
    return out


def write_json():
    anims = {}
    stages = []
    for i, (name, min_fur) in enumerate(STAGES):
        anims.update(anims_for(name, i * ROWS_PER_STAGE))
        stages.append({"name": name, "min_fur": min_fur, "row_offset": i * ROWS_PER_STAGE})
    doc = {
        "image": "grub_fur.png",
        "cell": [CW, CH],
        "origin": [CW // 2, CH - 1],
        "base": "grub.png",
        "stages": stages,
        "anims": anims,
    }
    with open(OUT_JSON, "w", newline="\n") as f:
        json.dump(doc, f, indent=2)
        f.write("\n")


def composite(sheet, over, stage_count, row, col):
    cell = Image.fromarray(base_cell(sheet, row, col).copy())
    for s in range(stage_count):
        o = over[(s * ROWS_PER_STAGE + row) * CH:(s * ROWS_PER_STAGE + row + 1) * CH, col * CW:(col + 1) * CW]
        cell.alpha_composite(Image.fromarray(o))
    return cell


def write_preview(path, sheet, over):
    """2x preview on the Ruins purple: the base and each cumulative stage (rows), then the raw overlay blocks."""
    bg = (0x5a, 0x3c, 0x78, 255)
    pitch = 34
    n = len(FRAMES)
    labels = 1 + len(STAGES)
    h_comp = labels * pitch + 4
    h_raw = len(STAGES) * pitch + 4
    canvas = Image.new("RGBA", (n * pitch + 8, h_comp + h_raw + 8), bg)
    for s in range(labels):
        for i, (row, col) in enumerate(FRAMES):
            canvas.alpha_composite(composite(sheet, over, s, row, col), (4 + i * pitch, 4 + s * pitch))
    for s in range(len(STAGES)):
        for i, (row, col) in enumerate(FRAMES):
            o = over[(s * ROWS_PER_STAGE + row) * CH:(s * ROWS_PER_STAGE + row + 1) * CH, col * CW:(col + 1) * CW]
            cell = Image.new("RGBA", (CW, CH), (0x2a, 0x1c, 0x3a, 255))
            cell.alpha_composite(Image.fromarray(o))
            canvas.alpha_composite(cell, (4 + i * pitch, h_comp + 8 + s * pitch))
    canvas.resize((canvas.width * 2, canvas.height * 2), Image.NEAREST).save(path)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--preview")
    ap.add_argument("--out", default=OUT_PNG)
    args = ap.parse_args()
    sheet, over = build_sheet()
    n = check_palette(over)
    Image.fromarray(over).save(args.out)
    if args.out == OUT_PNG:
        write_json()
    print("%s: %dx%d, %d colours" % (args.out, over.shape[1], over.shape[0], n))
    if args.preview:
        write_preview(args.preview, sheet, over)


if __name__ == "__main__":
    main()
