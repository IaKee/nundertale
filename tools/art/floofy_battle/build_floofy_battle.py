#!/usr/bin/env python3
"""Extend Floofy's battle sheet for ART-183 #2.

The original idle, talking, and sinister rows are copied pixel-for-pixel.
Rows added by this generator are soaked, attack_petals, attack_vines, and
attack_pounce. Run from the repository root with Python 3.13 and Pillow 11.
"""

import json
from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[3]
PNG = ROOT / "assets/sprites/battle/floofy.png"
JSON = ROOT / "assets/sprites/battle/floofy.json"
CELL_W, CELL_H = 50, 60
BASE_ROWS = 3
WHITE = (255, 255, 255, 255)
BLACK = (0, 0, 0, 255)
CLEAR = (0, 0, 0, 0)


def cell(sheet: Image.Image, column: int, row: int) -> Image.Image:
    return sheet.crop((column * CELL_W, row * CELL_H, (column + 1) * CELL_W, (row + 1) * CELL_H))


def transparent_cell() -> Image.Image:
    return Image.new("RGBA", (CELL_W, CELL_H), CLEAR)


def sprite_only(frame: Image.Image) -> Image.Image:
    bbox = frame.getchannel("A").getbbox()
    if bbox is None:
        raise ValueError("source frame is empty")
    return frame.crop(bbox)


def paste_bottom(dst: Image.Image, sprite: Image.Image, x: int = 25, bottom: int = 59) -> None:
    dst.alpha_composite(sprite, (x - sprite.width // 2, bottom - sprite.height + 1))


def outlined_drop(draw: ImageDraw.ImageDraw, x: int, y: int) -> None:
    draw.polygon([(x, y), (x - 2, y + 4), (x, y + 6), (x + 2, y + 4)], fill=BLACK)
    draw.point((x, y + 3), fill=WHITE)


def soaked(source: Image.Image, frame_no: int) -> Image.Image:
    spr = sprite_only(source)
    spr = spr.resize((max(1, spr.width - 5), max(1, spr.height - 6)), Image.Resampling.NEAREST)
    out = transparent_cell()
    paste_bottom(out, spr, bottom=57)
    draw = ImageDraw.Draw(out)
    for x, length in ((16, 5), (21, 7), (27, 6), (32, 4)):
        draw.line((x, 7, x - 1, 7 + length), fill=BLACK, width=2)
        draw.point((x - 1, 8 + length), fill=WHITE)
    offset = 3 * frame_no
    outlined_drop(draw, 12, 20 + offset)
    outlined_drop(draw, 36, 24 - offset)
    outlined_drop(draw, 26, 33 + frame_no)
    draw.ellipse((9, 56, 41, 59), fill=BLACK)
    draw.line((13, 57, 37, 57), fill=WHITE, width=1)
    return out


def petal_cluster(draw: ImageDraw.ImageDraw, cx: int, cy: int, phase: int) -> None:
    wobble = -1 if phase == 0 else 1
    for dx, dy in ((0, -5), (4, -2), (3, 3), (-3, 3), (-4, -2)):
        x, y = cx + dx + wobble, cy + dy
        draw.ellipse((x - 2, y - 2, x + 2, y + 2), fill=BLACK)
        draw.rectangle((x - 1, y - 1, x + 1, y + 1), fill=WHITE)
    draw.rectangle((cx - 1 + wobble, cy - 1, cx + 1 + wobble, cy + 1), fill=BLACK)


def attack_petals(source: Image.Image, frame_no: int) -> Image.Image:
    spr = sprite_only(source).rotate(7 if frame_no == 0 else 3, expand=True, resample=Image.Resampling.NEAREST)
    spr = spr.resize((max(1, spr.width - 7), max(1, spr.height - 5)), Image.Resampling.NEAREST)
    out = transparent_cell()
    paste_bottom(out, spr, x=21 - frame_no)
    draw = ImageDraw.Draw(out)
    draw.line((29, 29, 38, 20 - frame_no), fill=BLACK, width=5)
    draw.line((29, 29, 38, 20 - frame_no), fill=WHITE, width=3)
    draw.ellipse((36, 16 - frame_no, 41, 22 - frame_no), fill=BLACK)
    draw.rectangle((38, 18 - frame_no, 40, 20 - frame_no), fill=WHITE)
    petal_cluster(draw, 43, 11 + frame_no, frame_no)
    petal_cluster(draw, 8, 24 - frame_no, 1 - frame_no)
    petal_cluster(draw, 43, 36 + frame_no, 1 - frame_no)
    return out


def attack_vines(source: Image.Image, frame_no: int) -> Image.Image:
    spr = sprite_only(source)
    spr = spr.resize((spr.width, max(1, spr.height * 2 // 3)), Image.Resampling.NEAREST)
    spr = spr.rotate(-8 + 5 * frame_no, expand=True, resample=Image.Resampling.NEAREST)
    out = transparent_cell()
    paste_bottom(out, spr, x=27, bottom=58)
    draw = ImageDraw.Draw(out)
    points = [(1, 50 - frame_no), (10, 45), (20, 47 + frame_no), (30, 42), (48, 45 - frame_no)]
    draw.line(points, fill=BLACK, width=5)
    draw.line(points, fill=WHITE, width=2)
    for x, y, side in ((9, 45, -1), (18, 47, 1), (29, 42, -1), (39, 44, 1)):
        draw.polygon([(x, y), (x + 3, y + 5 * side), (x + 1, y + side)], fill=BLACK)
        draw.line((x + 1, y, x + 2, y + 3 * side), fill=WHITE)
    return out


def attack_pounce(source: Image.Image, frame_no: int) -> Image.Image:
    spr = sprite_only(source).rotate(-24 if frame_no == 0 else -37, expand=True, resample=Image.Resampling.NEAREST)
    spr = spr.resize((max(1, spr.width - 8), max(1, spr.height - 8)), Image.Resampling.NEAREST)
    out = transparent_cell()
    x, y = 23 + 5 * frame_no, 8 - 3 * frame_no
    out.alpha_composite(spr, (x - spr.width // 2, y))
    draw = ImageDraw.Draw(out)
    for dy, length in ((16, 7), (25, 10), (35, 6)):
        draw.line((2, dy, 2 + length, dy - 2), fill=BLACK, width=3)
        draw.line((3, dy - 1, 2 + length, dy - 2), fill=WHITE, width=1)
    draw.ellipse((34, 47, 48, 58), outline=BLACK, width=3)
    draw.arc((36, 49, 46, 57), 180, 350, fill=WHITE, width=1)
    return out


def main() -> int:
    source = Image.open(PNG).convert("RGBA")
    if source.width < 2 * CELL_W or source.height < BASE_ROWS * CELL_H:
        raise ValueError(f"unexpected source sheet size: {source.size}")
    base = source.crop((0, 0, 2 * CELL_W, BASE_ROWS * CELL_H))
    idle = [cell(base, column, 0) for column in range(2)]
    sinister = [cell(base, column, 2) for column in range(2)]
    rows = [
        [soaked(idle[column], column) for column in range(2)],
        [attack_petals(sinister[column], column) for column in range(2)],
        [attack_vines(sinister[column], column) for column in range(2)],
        [attack_pounce(idle[column], column) for column in range(2)],
    ]
    sheet = Image.new("RGBA", (2 * CELL_W, (BASE_ROWS + len(rows)) * CELL_H), CLEAR)
    sheet.paste(base, (0, 0))
    for row_no, frames in enumerate(rows, start=BASE_ROWS):
        for column, frame in enumerate(frames):
            sheet.paste(frame, (column * CELL_W, row_no * CELL_H))
    sheet.save(PNG, optimize=True)

    metadata = json.loads(JSON.read_text(encoding="utf-8"))
    metadata["anims"].update({
        "soaked": {"row": 3, "frames": 2, "fps": 3},
        "attack_petals": {"row": 4, "frames": 2, "fps": 5, "loop": False},
        "attack_vines": {"row": 5, "frames": 2, "fps": 5, "loop": False},
        "attack_pounce": {"row": 6, "frames": 2, "fps": 7, "loop": False},
    })
    JSON.write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {PNG.relative_to(ROOT)} {sheet.size}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
