"""Generate the Fursuit Dummy battle sheet at native 1x resolution."""

from pathlib import Path

from PIL import Image, ImageDraw


CELL_W = 48
CELL_H = 64
OUT = Path(__file__).parents[3] / "assets/sprites/battle/fursuit_dummy.png"
INK = (0, 0, 0, 255)
FUR = (255, 255, 255, 255)
CLEAR = (0, 0, 0, 0)


def polygon(draw: ImageDraw.ImageDraw, points: list[tuple[int, int]]) -> None:
    draw.polygon(points, fill=FUR, outline=INK, width=2)


def frame(sway: int = 0, hurt: bool = False) -> Image.Image:
    image = Image.new("RGBA", (CELL_W, CELL_H), CLEAR)
    draw = ImageDraw.Draw(image)

    if hurt:
        # The rack bends while the empty costume bunches around it.
        draw.line([(32, 2), (29, 13)], fill=FUR, width=3)
        draw.arc((29, 0, 37, 7), 170, 340, fill=FUR, width=2)
        draw.line([(29, 13), (24, 53)], fill=FUR, width=5)
        draw.line([(29, 13), (24, 53)], fill=INK, width=2)
        polygon(draw, [(13, 23), (18, 17), (29, 19), (38, 28), (34, 35), (28, 31),
                       (31, 49), (25, 57), (16, 54), (18, 35), (11, 32)])
        polygon(draw, [(18, 11), (23, 7), (32, 9), (37, 16), (34, 24), (25, 25),
                       (19, 20)])
        polygon(draw, [(20, 11), (14, 3), (22, 7), (25, 12)])
        polygon(draw, [(31, 10), (39, 4), (35, 13), (33, 16)])
        draw.ellipse((22, 14, 25, 18), fill=INK)
        draw.ellipse((31, 14, 34, 18), fill=INK)
        draw.line([(26, 20), (29, 21), (31, 19)], fill=INK, width=1)
        draw.line([(9, 12), (14, 14)], fill=FUR, width=2)
        draw.line([(10, 8), (15, 11)], fill=FUR, width=2)
        draw.line([(38, 35), (43, 37)], fill=FUR, width=2)
        return image

    x = sway
    # Rack feet and pole remain almost still; the loose suit sways around them.
    draw.line([(24, 2), (24, 15)], fill=FUR, width=3)
    draw.arc((23, 0, 31, 7), 170, 340, fill=FUR, width=2)
    draw.line([(24, 15), (24, 56)], fill=FUR, width=5)
    draw.line([(24, 15), (24, 56)], fill=INK, width=2)
    draw.line([(17, 57), (31, 57)], fill=FUR, width=3)
    draw.line([(19, 61), (24, 57), (29, 61)], fill=FUR, width=2)
    draw.line([(19, 19), (30, 19)], fill=INK, width=3)

    polygon(draw, [(15 + x, 22), (20 + x, 18), (29 + x, 19), (34 + x, 23),
                   (39 + x, 37), (34 + x, 42), (31 + x, 35), (33 + x, 54),
                   (28 + x, 59), (24 + x, 55), (20 + x, 59), (15 + x, 54),
                   (17 + x, 35), (13 + x, 42), (8 + x, 37)])
    # Empty head perched on the top peg: dark sockets sell that nobody is inside.
    polygon(draw, [(17 + x, 10), (21 + x, 6), (28 + x, 6), (33 + x, 11),
                   (32 + x, 21), (27 + x, 25), (20 + x, 22), (16 + x, 17)])
    polygon(draw, [(19 + x, 9), (14 + x, 1), (21 + x, 5), (23 + x, 10)])
    polygon(draw, [(28 + x, 7), (34 + x, 1), (33 + x, 11), (30 + x, 12)])
    draw.ellipse((20 + x, 12, 23 + x, 17), fill=INK)
    draw.ellipse((27 + x, 12, 30 + x, 17), fill=INK)
    draw.polygon([(24 + x, 18), (26 + x, 18), (25 + x, 20)], fill=INK)
    draw.line([(21 + x, 21), (25 + x, 22), (29 + x, 20)], fill=INK)
    # Zipper, seams, and dangling mitten paws reinforce "costume on a rack".
    draw.line([(25 + x, 28), (25 + x, 49)], fill=INK)
    for y in range(30, 49, 4):
        draw.point((26 + x, y), fill=INK)
    draw.arc((7 + x, 34, 14 + x, 43), 10, 170, fill=INK)
    draw.arc((35 + x, 34, 42 + x, 43), 10, 170, fill=INK)
    return image


def main() -> None:
    sheet = Image.new("RGBA", (CELL_W * 2, CELL_H * 2), CLEAR)
    sheet.paste(frame(-1), (0, 0), frame(-1))
    sheet.paste(frame(1), (CELL_W, 0), frame(1))
    sheet.paste(frame(hurt=True), (0, CELL_H), frame(hurt=True))
    OUT.parent.mkdir(parents=True, exist_ok=True)
    sheet.save(OUT, optimize=True)


if __name__ == "__main__":
    main()
