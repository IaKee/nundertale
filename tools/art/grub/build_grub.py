#!/usr/bin/env python3
"""Builds the overworld sheet of Grub the human NEET (ART-180, LORE v3.3 section 10.1).

Design source: the user's mockups (tools/art/grub/src/grub_{A_orange,B_mustard,C_red}.png, 160x160,
40x40 cells, rows = down / up / left / right, 4+4+2+2 frames). The script crops them, trims the front
and back poses by two rows so the whole sheet fits the 32x32 cell, keeps the 15-colour mockup palette
and adds the two new animations of pepe_wakeup:

  rows 0-3  stand_*/walk_*  down(4) up(4) left(2) right(2)   (same names/frame counts as pepe_frog)
  row 4     lying           1 frame, face up (Grub fell on the plush pile in R1, BUG-40: lying from frame 0)
  row 5     get_up          3 frames, sits up -> crouches -> standing (loop = false)

The sheet replaces both pepe_frog.png and pepe_wakeup.png (cell 32x32, origin 16,31, same anim names).

Usage (repo root, needs Pillow + numpy):
  C:/Python313/python.exe tools/art/grub/build_grub.py                 writes assets/sprites/ow/grub.{png,json}
  C:/Python313/python.exe tools/art/grub/build_grub.py --preview out.png   also writes a 2x preview next to
                                                                          Floofy and Furiell (+ purple tile bg)
  C:/Python313/python.exe tools/art/grub/build_grub.py --blanket B     A orange (default) / B mustard / C red
"""
import argparse
import json
import os

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
OUT_PNG = os.path.join(ROOT, "assets", "sprites", "ow", "grub.png")
OUT_JSON = os.path.join(ROOT, "assets", "sprites", "ow", "grub.json")

SRC_NAMES = {"A": "grub_A_orange.png", "B": "grub_B_mustard.png", "C": "grub_C_red.png"}
SRC_CELL = 40
CW, CH = 32, 32
X_CROP = 4  # source 40-wide cell -> columns 4..35 (the figure sits at x 7..33)
BOTTOM_ROW = 38  # last painted source row of every pose (flip-flop soles)
OUTLINE = (0x14, 0x10, 0x18, 255)

# (row in the mockup, frames, trim front/back rows). Trim = source rows (relative to the cell) that are
# dropped so the 34 px tall front/back poses become 32 px; chosen as the rows most similar to the one above.
WALK_ROWS = [4, 4, 2, 2]


def load_cell(sheet, row, col):
    box = (col * SRC_CELL, row * SRC_CELL, (col + 1) * SRC_CELL, (row + 1) * SRC_CELL)
    return np.array(sheet.crop(box).convert("RGBA"))


def pick_trim_rows(cells, count, lo, hi):
    """Pick `count` rows in [lo, hi) whose removal changes the frames the least (same rows for every frame)."""
    cost = []
    for y in range(lo, hi):
        c = 0
        for a in cells:
            c += int((a[y] != a[y - 1]).any(axis=1).sum())
        cost.append((c, y))
    cost.sort()
    chosen = []
    for _, y in cost:
        if all(abs(y - o) > 1 for o in chosen):
            chosen.append(y)
        if len(chosen) == count:
            break
    return sorted(chosen)


def drop_rows(a, rows):
    keep = [y for y in range(a.shape[0]) if y not in rows]
    return a[keep]


def to_cell(a):
    """Place a 40-wide source cell (rows already trimmed) into a 32x32 cell, feet on the last row."""
    out = np.zeros((CH, CW, 4), dtype=np.uint8)
    body = a[:, X_CROP:X_CROP + CW]
    ys = np.where(body[:, :, 3].any(axis=1))[0]
    body = body[ys[0]:ys[-1] + 1]
    assert body.shape[0] <= CH, body.shape
    out[CH - body.shape[0]:] = body
    return out


def bbox_rows(a):
    ys = np.where(a[:, :, 3].any(axis=1))[0]
    return ys[0], ys[-1]


def shift_rows(a, shears):
    """Shear: row y moves horizontally by shears[y] px (positive = right)."""
    out = np.zeros_like(a)
    for y in range(a.shape[0]):
        s = shears[y]
        if s >= 0:
            out[y, s:] = a[y, :a.shape[1] - s] if s else a[y]
        else:
            out[y, :s] = a[y, -s:]
    return out


def squash(a, rows):
    """Remove body rows (hood-to-hem, never the feet) and drop the sprite back to the floor."""
    top, bot = bbox_rows(a)
    feet_start = bot - 4
    body = [y for y in range(top + 16, feet_start)]
    step = len(body) / float(rows) if rows else 1
    drop = sorted({body[int(i * step)] for i in range(rows)})
    b = drop_rows(a, drop)
    out = np.zeros_like(a)
    out[CH - b.shape[0]:] = b[-CH:] if b.shape[0] > CH else b
    return out


def sheet_cells(src_path):
    sheet = Image.open(src_path).convert("RGBA")
    rows = []
    for r, n in enumerate(WALK_ROWS):
        cells = [load_cell(sheet, r, c) for c in range(n)]
        if r < 2:  # front and back poses are 34 px tall: trim two rows
            trim = pick_trim_rows(cells, 2, 9, 30) if r == 0 else pick_trim_rows(cells, 2, 12, 30)
            cells = [drop_rows(a, trim) for a in cells]
        rows.append([to_cell(a) for a in cells])
    return rows


def lying_cell(stand_down):
    """Face up, head to the left: the front pose turned 90 degrees (the camera looks down on him)."""
    rot = np.rot90(stand_down, 1)  # counter-clockwise: head left
    out = np.zeros((CH, CW, 4), dtype=np.uint8)
    ys = np.where(rot[:, :, 3].any(axis=1))[0]
    xs = np.where(rot[:, :, 3].any(axis=0))[0]
    rot = rot[ys[0]:ys[-1] + 1, xs[0]:xs[-1] + 1]
    h, w = rot.shape[:2]
    x0 = (CW - w) // 2
    out[CH - h:, x0:x0 + w] = rot
    return out


def get_up_cells(stand_down):
    """sit up (lean back, squashed) -> crouch (leaning forward) -> stand (head bowed a pixel)."""
    top, bot = bbox_rows(stand_down)
    h = bot - top + 1
    f0 = squash(stand_down, 6)
    sh = [0] * CH
    t0, b0 = bbox_rows(f0)
    for y in range(t0, b0 + 1):  # lean back: top of the figure moves right by up to 3 px
        sh[y] = int(round(3 * (b0 - y) / float(max(1, b0 - t0 - 8)))) if y < b0 - 8 else 0
        sh[y] = min(sh[y], 3)
    f0 = shift_rows(f0, sh)
    f1 = squash(stand_down, 3)
    sh = [0] * CH
    t1, b1 = bbox_rows(f1)
    for y in range(t1, b1 + 1):  # crouch: head pushed forward (left) by up to 1 px
        sh[y] = -1 if y < t1 + 10 else 0
    f1 = shift_rows(f1, sh)
    f2 = squash(stand_down, 1)  # one pixel shorter = the hunch before he straightens
    del h
    return [f0, f1, f2]


def build(blanket):
    rows = sheet_cells(os.path.join(HERE, "src", SRC_NAMES[blanket]))
    rows.append([lying_cell(rows[0][0])])
    rows.append(get_up_cells(rows[0][0]))
    sheet = np.zeros((len(rows) * CH, 4 * CW, 4), dtype=np.uint8)
    for r, frames in enumerate(rows):
        for c, f in enumerate(frames):
            sheet[r * CH:(r + 1) * CH, c * CW:(c + 1) * CW] = f
    return sheet


def check_palette(sheet):
    cols = {tuple(p) for p in sheet.reshape(-1, 4) if p[3]}
    assert all(c[3] == 255 for c in cols), "semi-transparent pixel"
    assert OUTLINE in cols, "outline colour missing"
    assert (0, 0, 0, 255) not in cols, "pure black is not allowed"
    return len(cols)


ANIMS = {
    "stand_down": {"row": 0, "frames": 1},
    "walk_down": {"row": 0, "frames": 4, "fps": 6},
    "stand_up": {"row": 1, "frames": 1},
    "walk_up": {"row": 1, "frames": 4, "fps": 6},
    "stand_left": {"row": 2, "frames": 1},
    "walk_left": {"row": 2, "frames": 2, "fps": 4},
    "stand_right": {"row": 3, "frames": 1},
    "walk_right": {"row": 3, "frames": 2, "fps": 4},
    "lying": {"row": 4, "frames": 1},
    "get_up": {"row": 5, "frames": 3, "fps": 4, "loop": False},
}


def write_json():
    doc = {"image": "grub.png", "cell": [CW, CH], "origin": [CW // 2, CH - 1], "anims": ANIMS}
    with open(OUT_JSON, "w", newline="\n") as f:
        json.dump(doc, f, indent=2)
        f.write("\n")


def sheet_frame(path, cell_w, cell_h, row, col):
    im = Image.open(path).convert("RGBA")
    return im.crop((col * cell_w, row * cell_h, (col + 1) * cell_w, (row + 1) * cell_h))


def write_preview(path, sheet_img):
    """2x preview on the Ruins purple: Grub's sheet (walks, lying, get-up), then Grub next to Floofy,
    Furiell and the old frog sheet for scale."""
    ow = os.path.join(ROOT, "assets", "sprites", "ow")
    bg = (0x5a, 0x3c, 0x78, 255)
    cells = []
    for r, n in enumerate(WALK_ROWS):
        for c in range(n):
            cells.append(sheet_img.crop((c * CW, r * CH, (c + 1) * CW, (r + 1) * CH)))
    for r, n in ((4, 1), (5, 3)):
        for c in range(n):
            cells.append(sheet_img.crop((c * CW, r * CH, (c + 1) * CW, (r + 1) * CH)))
    pitch = 36
    per_row = 8
    rows = (len(cells) + per_row - 1) // per_row
    h1 = rows * pitch
    refs = [
        sheet_img.crop((0, 0, CW, CH)),
        sheet_frame(os.path.join(ow, "floofy.png"), 40, 36, 0, 0),
        sheet_frame(os.path.join(ow, "furiell.png"), 32, 44, 0, 0),
        sheet_frame(os.path.join(ow, "pepe_frog.png"), 24, 32, 0, 0),
    ]
    refs = [im.crop(im.getbbox()) for im in refs]
    ref_h = max(im.height for im in refs)
    canvas = Image.new("RGBA", (per_row * pitch + 8, h1 + ref_h + 20), bg)
    for i, cell in enumerate(cells):
        canvas.alpha_composite(cell, (4 + (i % per_row) * pitch, 4 + (i // per_row) * pitch))
    x = 8
    for im in refs:
        canvas.alpha_composite(im, (x, h1 + 12 + ref_h - im.height))
        x += im.width + 14
    canvas = canvas.resize((canvas.width * 2, canvas.height * 2), Image.NEAREST)
    canvas.save(path)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--blanket", default="A", choices=sorted(SRC_NAMES))
    ap.add_argument("--preview")
    ap.add_argument("--out", default=OUT_PNG)
    args = ap.parse_args()
    sheet = build(args.blanket)
    n = check_palette(sheet)
    img = Image.fromarray(sheet)
    img.save(args.out)
    if args.out == OUT_PNG:
        write_json()
    print("%s: %dx%d, %d colours" % (args.out, img.width, img.height, n))
    if args.preview:
        write_preview(args.preview, img)


if __name__ == "__main__":
    main()
