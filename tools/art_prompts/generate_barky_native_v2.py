"""Draw Barky's overworld sheet directly on the native 28x36 pixel grid.

This intentionally does not downscale generated art.  Every coordinate below is an
in-game art pixel, following docs/art_prompts/ruins/A04_ow_barky.md.
"""
from pathlib import Path

from PIL import Image, ImageColor, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "_gen" / "ruins"
NATIVE = OUT / "A04_ow_barky-native-v2.png"
COMPARE = OUT / "A04_ow_barky-before-after-v2.png"
CURRENT = ROOT / "assets" / "sprites" / "ow" / "barky.png"

O = "#141418"  # outline
F = "#9A9AAA"  # fur
S = "#6E6E7E"  # fur shadow
P = "#C98AA8"  # inner ear
J = "#232329"  # jacket/gloves
H = "#3C3C46"  # jacket highlight
M = "#C8C8D2"  # studs/chains
G = "#7CFF3A"  # fringe streak
R = "#D0263A"  # eyeliner

CELL = (28, 36)
SHEET = (112, 180)


def px(draw, xy, color):
    draw.point(xy, fill=color)


def rect(draw, xy, fill, outline=O):
    draw.rectangle(xy, fill=fill, outline=outline)


def poly(draw, points, fill, outline=O):
    draw.polygon(points, fill=fill)
    if outline is not None:
        draw.line(points + [points[0]], fill=outline, width=1)


def line(draw, points, fill, width=1):
    draw.line(points, fill=fill, width=width)


def ears(draw, back=False, flattened=False):
    if flattened:
        poly(draw, [(6, 9), (2, 6), (10, 7), (12, 11)], F)
        poly(draw, [(17, 10), (19, 7), (26, 6), (22, 11)], F)
        return
    poly(draw, [(6, 10), (7, 2), (12, 7), (12, 11)], F)
    poly(draw, [(16, 10), (20, 2), (22, 10)], F)
    poly(draw, [(8, 7), (8, 4), (10, 8)], P, outline=None)
    poly(draw, [(19, 8), (20, 4), (21, 8)], P, outline=None)
    if back:
        line(draw, [(8, 8), (10, 6)], S)
        line(draw, [(19, 7), (21, 9)], S)


def tail(draw, side="right", raised=False):
    if side == "right":
        pts = [(20, 21), (24, 18 if raised else 21), (25, 24), (23, 29), (20, 27)]
        poly(draw, pts, S)
        px(draw, (21, 22), M)
        line(draw, [(21, 23), (23, 25)], M)
    else:
        pts = [(8, 21), (4, 18 if raised else 21), (3, 24), (5, 29), (8, 27)]
        poly(draw, pts, S)
        px(draw, (6, 22), M)
        line(draw, [(6, 23), (4, 25)], M)


def legs(draw, step=0, crouch=False):
    if crouch:
        rect(draw, (7, 29, 13, 33), S)
        rect(draw, (15, 29, 22, 33), S)
        rect(draw, (5, 33, 13, 35), J)
        rect(draw, (15, 33, 24, 35), J)
        return
    ly = 27 + (1 if step == 1 else 0)
    ry = 27 + (1 if step == -1 else 0)
    rect(draw, (8, ly, 12, 33), S)
    rect(draw, (16, ry, 20, 33), S)
    rect(draw, (7, 33, 12, 35), J)
    rect(draw, (16, 33, 21, 35), J)


def jacket(draw, y0=18, y1=28, crossed=True):
    poly(draw, [(7, y0 + 2), (9, y0), (19, y0), (21, y0 + 2), (20, y1), (8, y1)], J)
    line(draw, [(9, y0 + 1), (9, y1 - 2)], H)
    line(draw, [(19, y0 + 1), (19, y1 - 2)], H)
    for x in (9, 12, 16, 19):
        px(draw, (x, y0 + 1), M)
    if crossed:
        line(draw, [(8, y0 + 4), (18, y0 + 8)], M, 2)
        line(draw, [(20, y0 + 4), (11, y0 + 8)], F, 2)
        px(draw, (9, y0 + 4), J)
        px(draw, (19, y0 + 4), J)


def head_down(draw, embarrassed=False):
    ears(draw, flattened=embarrassed)
    poly(draw, [(7, 9), (10, 7), (19, 7), (22, 11), (21, 17), (18, 20), (10, 19), (6, 16)], F)
    line(draw, [(7, 16), (10, 19), (18, 20)], S)
    poly(draw, [(7, 9), (12, 7), (18, 8), (15, 11), (13, 14), (9, 12)], J)
    line(draw, [(12, 8), (10, 14)], G)
    line(draw, [(16, 13), (19, 13)], R)
    px(draw, (18, 13), O)
    rect(draw, (12, 16, 16, 18), S)
    px(draw, (13, 16), O)
    px(draw, (15, 16), O)


def head_up(draw):
    ears(draw, back=True)
    poly(draw, [(7, 9), (10, 7), (19, 7), (22, 11), (21, 18), (17, 20), (10, 19), (6, 16)], F)
    line(draw, [(7, 13), (10, 18), (18, 19), (21, 14)], S)
    poly(draw, [(7, 9), (12, 7), (19, 8), (17, 11), (9, 13)], J)
    line(draw, [(12, 8), (9, 12)], G)


def down_frame(step=0):
    im = Image.new("RGBA", CELL, (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    tail(d, "right", raised=step == 1)
    legs(d, step)
    jacket(d)
    head_down(d)
    return im


def up_frame(step=0):
    im = Image.new("RGBA", CELL, (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    tail(d, "right", raised=step == -1)
    legs(d, step)
    jacket(d, crossed=False)
    line(d, [(14, 20), (14, 27)], M)
    px(d, (13, 21), M)
    px(d, (15, 23), M)
    head_up(d)
    return im


def side_frame(right=True, step=0):
    im = Image.new("RGBA", CELL, (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    flip = -1 if not right else 1
    tail(d, "left" if right else "right", raised=step != 0)
    legs(d, step if right else -step)
    jacket(d, crossed=False)
    # One dominant ear and a smaller rear ear make the direction readable at 28 px.
    pts = [(10, 10), (12, 2), (16, 9), (18, 10)] if right else [(18, 10), (16, 2), (12, 9), (10, 10)]
    poly(d, pts, F)
    if right:
        poly(d, [(7, 11), (11, 7), (18, 8), (23, 13), (20, 18), (11, 19), (7, 16)], F)
        poly(d, [(8, 10), (14, 7), (17, 9), (13, 14), (9, 14)], J)
        line(d, [(13, 8), (11, 13)], G)
        line(d, [(18, 13), (21, 13)], R)
        px(d, (22, 15), O)
    else:
        poly(d, [(21, 11), (17, 7), (10, 8), (5, 13), (8, 18), (17, 19), (21, 16)], F)
        poly(d, [(20, 10), (14, 7), (11, 9), (15, 14), (19, 14)], J)
        line(d, [(15, 8), (17, 13)], G)
        line(d, [(10, 13), (7, 13)], R)
        px(d, (5, 15), O)
    # Forward arm changes by one pixel in the walking frame.
    x0 = 20 if right else 8
    line(d, [(x0, 21), (x0 + flip * (2 + step), 27)], F, 2)
    return im


def falling_frame():
    im = Image.new("RGBA", CELL, (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    legs(d, -1)
    jacket(d, 18, 28, crossed=False)
    head_down(d, embarrassed=True)
    line(d, [(8, 22), (4, 15), (5, 10)], F, 2)
    line(d, [(20, 22), (24, 15), (23, 10)], F, 2)
    rect(d, (3, 9, 6, 12), J)
    rect(d, (22, 9, 25, 12), J)
    return im


def landing_frame():
    im = Image.new("RGBA", CELL, (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    tail(d, "right", raised=True)
    legs(d, crouch=True)
    poly(d, [(7, 23), (10, 19), (19, 20), (22, 26), (19, 31), (9, 30)], J)
    # Bent head and swept ears.
    poly(d, [(6, 20), (3, 15), (11, 17), (13, 21)], F)
    poly(d, [(17, 20), (20, 14), (24, 19), (21, 23)], F)
    poly(d, [(8, 19), (12, 16), (20, 18), (21, 23), (15, 26), (8, 24)], F)
    poly(d, [(8, 19), (14, 16), (18, 19), (13, 23)], J)
    line(d, [(13, 17), (11, 21)], G)
    line(d, [(8, 25), (4, 32)], F, 2)
    rect(d, (2, 32, 6, 35), J)
    return im


def dramatic_frame():
    im = down_frame(0)
    d = ImageDraw.Draw(im)
    # Replace crossed arms with asymmetric dramatic gesture.
    poly(d, [(7, 20), (5, 18), (2, 13), (4, 12), (10, 21)], F)
    rect(d, (1, 11, 4, 14), J)
    poly(d, [(20, 20), (23, 22), (25, 18), (23, 17), (18, 23)], F)
    rect(d, (22, 16, 25, 19), J)
    return im


def build_sheet():
    sheet = Image.new("RGBA", SHEET, (0, 0, 0, 0))
    frames = {
        (0, 0): down_frame(0), (1, 0): down_frame(-1),
        (2, 0): down_frame(0), (3, 0): down_frame(1),
        (0, 1): up_frame(0), (1, 1): up_frame(-1),
        (2, 1): up_frame(0), (3, 1): up_frame(1),
        (0, 2): side_frame(False, 0), (1, 2): side_frame(False, 1),
        (0, 3): side_frame(True, 0), (1, 3): side_frame(True, 1),
        (0, 4): falling_frame(), (1, 4): landing_frame(), (2, 4): dramatic_frame(),
    }
    for (col, row), frame in frames.items():
        sheet.alpha_composite(frame, (col * CELL[0], row * CELL[1]))
    return sheet


def checker(size, block=8):
    bg = Image.new("RGB", size, (62, 62, 70))
    d = ImageDraw.Draw(bg)
    for y in range(0, size[1], block):
        for x in range(0, size[0], block):
            if (x // block + y // block) % 2:
                d.rectangle((x, y, x + block - 1, y + block - 1), fill=(82, 82, 92))
    return bg


def make_comparison(before, after):
    zoom = 4
    b = before.resize((before.width * zoom, before.height * zoom), Image.Resampling.NEAREST)
    a = after.resize((after.width * zoom, after.height * zoom), Image.Resampling.NEAREST)
    panel_w, panel_h = b.width, b.height
    pad, top = 24, 48
    out = Image.new("RGB", (panel_w * 2 + pad * 3, panel_h + top + pad), (28, 28, 34))
    for i, art in enumerate((b, a)):
        bg = checker(art.size, 16)
        bg.paste(art, (0, 0), art)
        out.paste(bg, (pad + i * (panel_w + pad), top))
    d = ImageDraw.Draw(out)
    font = ImageFont.load_default()
    d.text((pad, 18), "ANTES - sprite atual", fill="white", font=font)
    d.text((pad * 2 + panel_w, 18), "DEPOIS - nativo 112x180", fill="white", font=font)
    return out


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    sheet = build_sheet()
    assert sheet.size == SHEET
    allowed = {tuple(ImageColor.getrgb(c)) for c in (O, F, S, P, J, H, M, G, R)}
    used = {p[:3] for p in sheet.getdata() if p[3]}
    assert used <= allowed, used - allowed
    assert sheet.crop((0, 0, 28, 36)).tobytes() == sheet.crop((56, 0, 84, 36)).tobytes()
    assert sheet.crop((0, 36, 28, 72)).tobytes() == sheet.crop((56, 36, 84, 72)).tobytes()
    for col, row in ((2, 2), (3, 2), (2, 3), (3, 3), (3, 4)):
        assert sheet.crop((col * 28, row * 36, (col + 1) * 28, (row + 1) * 36)).getbbox() is None
    sheet.save(NATIVE)
    before = Image.open(CURRENT).convert("RGBA")
    make_comparison(before, sheet).save(COMPARE)
    print(NATIVE)
    print(COMPARE)
    print(f"validated: {sheet.width}x{sheet.height}, {len(used)} palette colors, transparent empty cells")


if __name__ == "__main__":
    main()
