"""Redraws the doorway tiles 49-52 of assets/tiles/ruins.png (BUG-41).

Run from the repo root:  python tools/art/doorways/make_doorways.py [--preview out.png]

Tile indices (gid = 1 + row * 8 + col, 20x20 px each, row 6):
    49 closed door, top    -> sits on the wall CAP row (same slab as tiles 25-27)
    50 closed door, bottom -> sits on the wall FRONT row, sill on the floor line
    51 open doorway, top   -> lintel on the wall CAP row
    52 open doorway, bottom-> dark opening, threshold on the floor line

The open doorway tiles are drawn full width (no jambs of their own): the wall end tiles
next to the gap (27 / 28) already carry the jamb edge, so a 2-wide opening is just 51+51 over 52+52.
Both bases are copied from the neighbouring wall tiles (26 cap, 29 brick) so the arch is cut
into the wall instead of standing in front of it. The script only touches tiles 49-52.
"""
import sys

from PIL import Image

SHEET = "assets/tiles/ruins.png"
T = 20

DARK = (59, 31, 92, 255)
BRICK = (74, 42, 112, 255)
MID = (94, 58, 135, 255)
SLAB = (122, 85, 168, 255)
LIGHT = (156, 124, 196, 255)
VOID = (17, 17, 17, 255)
HANDLE = (244, 227, 255, 255)
# the opening fades from the lintel shadow into black, then a faint floor glints at the sill
SHADE = [(38, 20, 60, 255), (28, 15, 44, 255)]
FLOOR_GLOW = [(21, 13, 31, 255), (25, 15, 38, 255), (30, 18, 46, 255), (36, 21, 55, 255),
              (43, 25, 65, 255), (52, 30, 78, 255)]


def tile_box(gid):
    i = gid - 1
    return ((i % 8) * T, (i // 8) * T, (i % 8) * T + T, (i // 8) * T + T)


def rect(img, x0, y0, x1, y1, color):
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            img.putpixel((x, y), color)


def closed_top(cap):
    t = cap.copy()
    rect(t, 2, 3, 17, 19, DARK)  # frame outline (top on the cap row)
    rect(t, 3, 4, 16, 19, LIGHT)  # lit stone frame
    rect(t, 4, 5, 15, 19, DARK)  # inner shadow line
    rect(t, 5, 6, 14, 19, MID)  # leaf
    rect(t, 7, 8, 12, 19, BRICK)  # upper panel
    rect(t, 7, 8, 12, 8, DARK)
    return t


def closed_bottom(front):
    t = front.copy()
    rect(t, 2, 0, 17, 19, DARK)
    rect(t, 3, 0, 16, 19, LIGHT)
    rect(t, 4, 0, 15, 19, DARK)
    rect(t, 5, 0, 14, 19, MID)  # leaf continues
    rect(t, 7, 0, 12, 12, BRICK)  # lower panel
    rect(t, 7, 12, 12, 12, DARK)
    t.putpixel((13, 4), HANDLE)  # handle
    t.putpixel((13, 5), SLAB)
    rect(t, 2, 18, 17, 19, DARK)  # sill flush with the floor line
    rect(t, 3, 18, 16, 18, SLAB)
    return t


def open_top(cap):
    t = cap.copy()  # y 0-5 stay cap slab: the lintel is the wall cap itself
    rect(t, 0, 5, T - 1, 5, LIGHT)  # lintel lip, like the cap's own lip
    rect(t, 0, 6, T - 1, 6, DARK)  # underside edge
    for i, c in enumerate(SHADE):
        rect(t, 0, 7 + i, T - 1, 7 + i, c)
    rect(t, 0, 9, T - 1, T - 1, VOID)
    return t


def open_bottom(front):
    t = front.copy()
    rect(t, 0, 0, T - 1, 8, VOID)
    for i, c in enumerate(FLOOR_GLOW):  # the far floor receding into the dark
        rect(t, 0, 9 + i, T - 1, 9 + i, c)
    rect(t, 0, 15, T - 1, 15, DARK)  # threshold edge
    rect(t, 0, 16, T - 1, 16, LIGHT)
    rect(t, 0, 17, T - 1, T - 1, SLAB)  # sill, same colour as the floor it sits on
    return t


def main():
    sheet = Image.open(SHEET).convert("RGBA")
    assert sheet.size == (160, 160), sheet.size
    cap = sheet.crop(tile_box(26))
    front = sheet.crop(tile_box(29))
    for gid, tile in (
        (49, closed_top(cap)),
        (50, closed_bottom(front)),
        (51, open_top(cap)),
        (52, open_bottom(front)),
    ):
        sheet.paste(tile, tile_box(gid)[:2])
    sheet.save(SHEET)
    if "--preview" in sys.argv:
        out = sys.argv[sys.argv.index("--preview") + 1]
        strip = Image.new("RGBA", (T * 6, T * 2), (255, 0, 255, 255))
        # closed door | 2-wide opening between wall ends
        for col, (top, bot) in enumerate(((49, 50), (51, 52), (51, 52))):
            strip.paste(sheet.crop(tile_box(top)), ((col + 1) * T, 0))
            strip.paste(sheet.crop(tile_box(bot)), ((col + 1) * T, T))
        strip.resize((T * 6 * 8, T * 2 * 8), Image.NEAREST).save(out)


if __name__ == "__main__":
    main()
