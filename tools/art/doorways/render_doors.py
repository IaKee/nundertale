"""Renders the doorway area of every Ruins map with a doorway at 2x (tile layers only), side by side.

python tools/art/doorways/render_doors.py <old_dir> <out.png>
<old_dir> holds the 'before' ruins.png and the old map jsons (e.g. from `git show HEAD:...`)."""
import glob
import json
import os
import sys

from PIL import Image, ImageDraw

T = 20
S = 2  # 20px tile -> 40px in game -> x2 of that for the review sheet


def render(map_path, sheet_path, box):
    m = json.load(open(map_path))
    sheet = Image.open(sheet_path).convert("RGBA")
    w, h = m["width"], m["height"]
    img = Image.new("RGBA", (w * T, h * T), (0, 0, 0, 255))
    for layer in m["layers"]:
        if layer.get("type") != "tilelayer":
            continue
        for i, g in enumerate(layer["data"]):
            if not g:
                continue
            k = g - 1
            tile = sheet.crop(((k % 8) * T, (k // 8) * T, (k % 8) * T + T, (k // 8) * T + T))
            img.alpha_composite(tile, ((i % w) * T, (i // w) * T))
    x0, y0, x1, y1 = box
    return img.crop((x0 * T, y0 * T, x1 * T, y1 * T))


def door_cells(map_path):
    m = json.load(open(map_path))
    w = m["width"]
    cells = []
    for layer in m["layers"]:
        if layer.get("type") == "tilelayer":
            cells += [(i % w, i // w) for i, g in enumerate(layer["data"]) if g in (49, 51, 53)]
    return cells


def main():
    old_dir, out = sys.argv[1], sys.argv[2]
    rows = []
    for new_path in sorted(glob.glob("data/maps/*.json")):
        name = os.path.basename(new_path)
        cells = door_cells(new_path) or door_cells(os.path.join(old_dir, name))
        if not cells:
            continue
        cx, cy = cells[0]
        box = (max(0, cx - 4), max(0, cy - 1), cx + 6, cy + 4)
        before = render(os.path.join(old_dir, name), os.path.join(old_dir, "ruins.png"), box)
        after = render(new_path, "assets/tiles/ruins.png", box)
        row = Image.new("RGBA", (before.width * 2 * S + 8, before.height * S + 12), (40, 40, 40, 255))
        row.paste(before.resize((before.width * S, before.height * S), Image.NEAREST), (0, 12))
        row.paste(after.resize((after.width * S, after.height * S), Image.NEAREST), (before.width * S + 8, 12))
        ImageDraw.Draw(row).text((2, 0), name[:-5] + "   before | after", fill=(255, 255, 255, 255))
        rows.append(row)
    cols = 2
    cw, ch = rows[0].width, rows[0].height
    sheet = Image.new("RGBA", (cols * cw, ((len(rows) + 1) // cols) * ch), (0, 0, 0, 255))
    for i, r in enumerate(rows):
        sheet.paste(r, ((i % cols) * cw, (i // cols) * ch))
    sheet.save(out)
    print(len(rows), "rooms ->", out)


if __name__ == "__main__":
    main()
