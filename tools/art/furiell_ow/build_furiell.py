#!/usr/bin/env python3
"""Build Furiell's extended overworld sheet (ART-183 #12).

The original five rows keep their animation indices. A lavender-water spray
bottle is added to every existing pose, then six rows are appended:

  5 spritz_left     (3)  draw, squeeze, mist
  6 spritz_right    (3)  draw, squeeze, mist
  7 hold_walk_down  (4)  left paw held out for Grub
  8 hold_walk_up    (4)  left paw held out for Grub
  9 hold_walk_left  (2)  leading Grub
 10 hold_walk_right (2)  Grub trailing behind

Run from the repository root with Python 3.13 + Pillow. The preview is a 2x
nearest-neighbour contact sheet and includes Furiell beside Grub for scale.
"""
import json
import os

from PIL import Image, ImageDraw


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
PNG = os.path.join(ROOT, "assets", "sprites", "ow", "furiell.png")
JSN = os.path.join(ROOT, "assets", "sprites", "ow", "furiell.json")
PREVIEW = os.path.join(os.path.dirname(__file__), "furiell_preview_2x.png")
GRUB = os.path.join(ROOT, "assets", "sprites", "ow", "grub.png")

CW, CH = 32, 44
BASE_ROWS = 5
COLS = 4

TRANSPARENT = (0, 0, 0, 0)
OUTLINE = (20, 16, 24, 255)
FUR = (244, 240, 234, 255)
FUR_SHADE = (237, 228, 247, 255)
APRON = (138, 79, 208, 255)
WATER = (104, 197, 224, 255)
WATER_LIGHT = (185, 235, 244, 255)
LAVENDER = (190, 139, 224, 255)


def get_cell(sheet, row, col):
    return sheet.crop((col * CW, row * CH, (col + 1) * CW, (row + 1) * CH))


def put_cell(sheet, row, col, cell):
    sheet.paste(cell, (col * CW, row * CH), cell)


def bottle(cell, direction, held=False, squeeze=False):
    """Add a readable 5x9 bottle using only opaque palette colours."""
    d = ImageDraw.Draw(cell)
    if held:
        if direction == "left":
            bx, y, sign = 2, 20, -1
        else:
            bx, y, sign = 25, 20, 1
        # Paw and forearm connecting the bottle to the body.
        d.line((15, 24, 9 if sign < 0 else 22, 22), fill=OUTLINE, width=4)
        d.line((15, 24, 9 if sign < 0 else 22, 22), fill=FUR, width=2)
        # Neck, trigger and nozzle point away from Furiell.
        d.rectangle((bx + 1, y, bx + 3, y + 1), fill=OUTLINE)
        d.point((bx + (0 if sign < 0 else 4), y), fill=OUTLINE)
        if squeeze:
            d.point((bx + (3 if sign < 0 else 1), y + 2), fill=LAVENDER)
    else:
        # The bottle hangs on the right side of the apron; on back/side views
        # it remains visible at the hip silhouette.
        positions = {
            "down": (22, 27),
            "up": (22, 27),
            "left": (18, 28),
            "right": (9, 28),
        }
        bx, y = positions[direction]
        d.line((bx + 2, y - 2, bx + 2, y), fill=OUTLINE, width=1)
        d.rectangle((bx + 1, y - 1, bx + 3, y), fill=OUTLINE)

    d.rectangle((bx, y + 1, bx + 4, y + 7), fill=OUTLINE)
    d.rectangle((bx + 1, y + 2, bx + 3, y + 6), fill=WATER)
    d.rectangle((bx + 1, y + 2, bx + 2, y + 3), fill=WATER_LIGHT)
    d.point((bx + 2, y + 5), fill=LAVENDER)
    return cell


def add_hanging_bottles(base):
    rows = ((0, 4, "down"), (1, 4, "up"), (2, 2, "left"),
            (3, 2, "right"), (4, 2, "down"))
    for row, count, direction in rows:
        for col in range(count):
            cell = get_cell(base, row, col)
            put_cell(base, row, col, bottle(cell, direction))


def mist(cell, direction, phase):
    d = ImageDraw.Draw(cell)
    if phase == 0:
        return cell
    if direction == "left":
        pts = [(0, 19), (1, 17), (0, 22), (4, 16), (3, 23)]
    else:
        pts = [(31, 19), (30, 17), (31, 22), (27, 16), (28, 23)]
    for i, (x, y) in enumerate(pts[: 3 + phase]):
        d.point((x, y), fill=WATER_LIGHT if i % 2 else WATER)
    return cell


def spritz_frames(base, direction):
    source_row = 2 if direction == "left" else 3
    source = get_cell(base, source_row, 0)
    frames = []
    for phase in range(3):
        cell = source.copy()
        # Cover the hanging bottle before redrawing it in Furiell's paw. The
        # tiny patch is inside the purple apron/hip mass in these side poses.
        d = ImageDraw.Draw(cell)
        if direction == "left":
            d.rectangle((18, 26, 23, 36), fill=APRON)
            d.line((18, 26, 18, 35), fill=OUTLINE)
        else:
            d.rectangle((8, 26, 13, 36), fill=APRON)
            d.line((13, 26, 13, 35), fill=OUTLINE)
        bottle(cell, direction, held=True, squeeze=phase > 0)
        mist(cell, direction, phase)
        frames.append(cell)
    return frames


def holding_paw(cell, direction, phase):
    """Extend one paw to the left edge, the join point used by Grub's half."""
    d = ImageDraw.Draw(cell)
    bob = phase & 1
    if direction in ("down", "up"):
        shoulder = (9, 24 + bob)
        elbow = (6, 27 + bob)
        hand = (2, 27 + bob)
    elif direction == "left":
        shoulder = (10, 23 + bob)
        elbow = (6, 24 + bob)
        hand = (2, 25 + bob)
    else:
        shoulder = (11, 24 + bob)
        elbow = (7, 27 + bob)
        hand = (2, 28 + bob)
    d.line((shoulder, elbow, hand), fill=OUTLINE, width=5, joint="curve")
    d.line((shoulder, elbow, hand), fill=FUR, width=3, joint="curve")
    d.rectangle((1, hand[1] - 1, 3, hand[1] + 1), fill=OUTLINE)
    d.point((2, hand[1]), fill=FUR_SHADE)
    return cell


def holding_frames(base, row, count, direction):
    frames = []
    for col in range(count):
        cell = get_cell(base, row, col).copy()
        holding_paw(cell, direction, col)
        frames.append(cell)
    return frames


def validate(sheet, original_base):
    assert sheet.size == (COLS * CW, 11 * CH)
    # Bottle additions are intentionally the only changes above row 5: at
    # least 98% of the original pixels must remain byte-identical.
    before = list(original_base.getdata())
    after = list(sheet.crop((0, 0, COLS * CW, BASE_ROWS * CH)).getdata())
    same = sum(a == b for a, b in zip(before, after))
    assert same / len(before) > 0.98, "base poses changed beyond bottle overlay"
    colours = {p for p in sheet.getdata() if p[3]}
    assert OUTLINE in colours and (0, 0, 0, 255) not in colours
    assert all(p[3] == 255 for p in colours), "semi-transparent pixel"
    return len(colours), same / len(before)


def write_json():
    with open(JSN, encoding="utf-8") as fh:
        meta = json.load(fh)
    meta["anims"].update({
        "spritz_left": {"row": 5, "frames": 3, "fps": 8, "loop": False},
        "spritz_right": {"row": 6, "frames": 3, "fps": 8, "loop": False},
        "hold_walk_down": {"row": 7, "frames": 4, "fps": 6},
        "hold_walk_up": {"row": 8, "frames": 4, "fps": 6},
        "hold_walk_left": {"row": 9, "frames": 2, "fps": 4},
        "hold_walk_right": {"row": 10, "frames": 2, "fps": 4},
    })
    with open(JSN, "w", encoding="utf-8", newline="\n") as fh:
        json.dump(meta, fh, indent=2)
        fh.write("\n")


def make_preview(sheet):
    bg = (67, 42, 86, 255)
    title_h = 8
    canvas = Image.new("RGBA", (COLS * 38 + 8, title_h + 11 * 48 + 52), bg)
    for row in range(11):
        for col in range(COLS):
            frame = get_cell(sheet, row, col)
            canvas.alpha_composite(frame, (4 + col * 38, title_h + row * 48))

    # Scale check: current standing Grub next to Furiell's new holding pose.
    grub = Image.open(GRUB).convert("RGBA").crop((0, 0, 32, 48))
    fur = get_cell(sheet, 7, 0)
    baseline = title_h + 11 * 48 + 46
    canvas.alpha_composite(grub, (45, baseline - grub.height))
    canvas.alpha_composite(fur, (74, baseline - fur.height))
    canvas = canvas.resize((canvas.width * 2, canvas.height * 2), Image.Resampling.NEAREST)
    canvas.save(PREVIEW)


def main():
    src = Image.open(PNG).convert("RGBA")
    original_base = src.crop((0, 0, COLS * CW, BASE_ROWS * CH))
    base = original_base.copy()
    add_hanging_bottles(base)

    rows = [
        spritz_frames(base, "left"),
        spritz_frames(base, "right"),
        holding_frames(base, 0, 4, "down"),
        holding_frames(base, 1, 4, "up"),
        holding_frames(base, 2, 2, "left"),
        holding_frames(base, 3, 2, "right"),
    ]
    sheet = Image.new("RGBA", (COLS * CW, 11 * CH), TRANSPARENT)
    sheet.paste(base, (0, 0))
    for row, frames in enumerate(rows, start=BASE_ROWS):
        for col, frame in enumerate(frames):
            put_cell(sheet, row, col, frame)

    colours, unchanged = validate(sheet, original_base)
    sheet.save(PNG)
    write_json()
    make_preview(sheet)
    print(f"wrote {PNG}: {sheet.width}x{sheet.height}, {colours} colours")
    print(f"base sheet pixels retained: {unchanged:.2%}; preview: {PREVIEW}")


if __name__ == "__main__":
    main()
