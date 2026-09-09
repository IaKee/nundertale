"""Rebuild the GRUB headset soul and the two-frame R11 mirror reflection."""

from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[3]
OUTLINE = "#141018"
HAIR = "#302532"
SKIN = "#d9c5c0"
SHADOW = "#a98e92"
BLANKET = "#d47b20"
BLANKET_DARK = "#8f451b"
SHIRT = "#1257d6"
HEADSET = "#4b3d51"
MIRROR = "#c7a4e8"
MIRROR_DARK = "#76518f"


def rebuild_souls() -> None:
    path = ROOT / "assets/ui/souls.png"
    old = Image.open(path).convert("RGBA")
    # Preserve the six established 12x12 frames and make reruns idempotent.
    out = Image.new("RGBA", (12, 84), (0, 0, 0, 0))
    out.paste(old.crop((0, 0, 12, 72)), (0, 0))
    draw = ImageDraw.Draw(out)
    y = 72

    # The canonical red heart, with a tiny over-ear headset and one-pixel mic.
    red = "#ff0000"
    draw.polygon([(2, y + 4), (2, y + 3), (3, y + 2), (5, y + 2),
                  (6, y + 3), (7, y + 2), (9, y + 2), (10, y + 3),
                  (10, y + 5), (6, y + 9)], fill=red)
    draw.line([(2, y + 4), (2, y + 2), (3, y + 1), (8, y + 1),
               (10, y + 3), (10, y + 5)], fill=HEADSET)
    draw.rectangle((1, y + 4, 2, y + 6), fill=HEADSET)
    draw.rectangle((9, y + 4, 10, y + 6), fill=HEADSET)
    draw.line([(10, y + 6), (9, y + 7), (8, y + 7)], fill=HEADSET)
    out.save(path)


def draw_human(draw: ImageDraw.ImageDraw, y: int, cat_ears: bool) -> None:
    # A deliberately chunky 20x40 reflection of the human Grub: hooded blanket,
    # headset, pale tired face, blue shirt, shorts and flip-flops.
    draw.polygon([(4, y + 8), (6, y + 4), (14, y + 4), (16, y + 8),
                  (17, y + 26), (15, y + 34), (5, y + 34), (3, y + 26)],
                 fill=BLANKET_DARK)
    draw.polygon([(5, y + 8), (7, y + 5), (13, y + 5), (15, y + 8),
                  (15, y + 29), (13, y + 32), (7, y + 32), (5, y + 29)],
                 fill=BLANKET)
    if cat_ears:
        draw.polygon([(5, y + 7), (6, y + 1), (10, y + 5)], fill=OUTLINE)
        draw.polygon([(10, y + 5), (14, y + 1), (15, y + 7)], fill=OUTLINE)
        draw.polygon([(6, y + 5), (7, y + 3), (9, y + 6)], fill=MIRROR)
        draw.polygon([(11, y + 6), (13, y + 3), (14, y + 5)], fill=MIRROR)
    draw.rectangle((5, y + 8, 15, y + 17), fill=OUTLINE)
    draw.rectangle((6, y + 8, 14, y + 16), fill=SKIN)
    draw.polygon([(6, y + 8), (8, y + 6), (14, y + 8), (13, y + 11),
                  (10, y + 9), (8, y + 11)], fill=HAIR)
    draw.point((8, y + 13), fill=SHADOW)
    draw.point((12, y + 13), fill=SHADOW)
    draw.line([(7, y + 8), (6, y + 7), (5, y + 9), (5, y + 13)], fill=HEADSET)
    draw.line([(13, y + 8), (14, y + 7), (15, y + 9), (15, y + 13)], fill=HEADSET)
    draw.rectangle((4, y + 11, 5, y + 14), fill=HEADSET)
    draw.rectangle((15, y + 11, 16, y + 14), fill=HEADSET)
    draw.rectangle((7, y + 18, 13, y + 25), fill=SHIRT)
    draw.rectangle((7, y + 25, 9, y + 29), fill=OUTLINE)
    draw.rectangle((11, y + 25, 13, y + 29), fill=OUTLINE)
    draw.rectangle((7, y + 29, 9, y + 34), fill=SKIN)
    draw.rectangle((11, y + 29, 13, y + 34), fill=SKIN)
    draw.line([(6, y + 35), (9, y + 35)], fill=OUTLINE)
    draw.line([(11, y + 35), (14, y + 35)], fill=OUTLINE)
    draw.point((5, y + 18), fill=MIRROR)
    draw.point((14, y + 21), fill=MIRROR)


def rebuild_mirror() -> None:
    path = ROOT / "assets/props/ruins_mirror.png"
    out = Image.new("RGBA", (20, 80), (0, 0, 0, 0))
    draw = ImageDraw.Draw(out)
    draw_human(draw, 0, False)
    draw_human(draw, 40, True)
    # Sparse violet glints keep the reflection material consistent with R11.
    for y in (0, 40):
        draw.line([(3, y + 9), (3, y + 17)], fill=MIRROR_DARK)
        draw.point((16, y + 18), fill=MIRROR)
        draw.point((4, y + 31), fill=MIRROR)
    out.save(path)


if __name__ == "__main__":
    rebuild_souls()
    rebuild_mirror()
