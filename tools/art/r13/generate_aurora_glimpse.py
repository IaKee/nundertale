"""Generate the 40x40 R13 doorway glimpse at native pixel-art resolution."""

from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[3]
OUTPUT = ROOT / "assets" / "props" / "ruins_aurora_glimpse.png"


def build_glimpse() -> Image.Image:
    image = Image.new("RGBA", (40, 40), (11, 13, 30, 255))
    draw = ImageDraw.Draw(image)

    # The iron doorway and its cold blue inner rim.
    draw.rectangle((0, 0, 39, 39), fill=(20, 16, 24, 255))
    draw.rectangle((2, 1, 37, 39), fill=(69, 80, 101, 255))
    draw.rectangle((3, 2, 36, 39), fill=(10, 16, 37, 255))

    # Three thin RGB ribbons make the industrial LED aurora readable at 1x.
    draw.line([(3, 7), (7, 5), (11, 7), (15, 4), (20, 6), (25, 3), (31, 6), (36, 4)],
              fill=(255, 79, 216, 255), width=2)
    draw.line([(3, 9), (8, 7), (13, 9), (18, 6), (23, 8), (28, 5), (33, 8), (36, 7)],
              fill=(79, 216, 255, 255), width=1)
    draw.line([(3, 11), (8, 9), (14, 11), (19, 8), (25, 10), (30, 7), (36, 10)],
              fill=(124, 255, 58, 255), width=1)

    # Frozen forest: pale distant trees behind a dark foreground tree line.
    for x, top in ((5, 17), (10, 15), (15, 18), (29, 16), (34, 18)):
        draw.polygon([(x, top), (x - 3, 27), (x + 3, 27)], fill=(88, 119, 151, 255))
        draw.rectangle((x, 23, x, 29), fill=(42, 59, 82, 255))
    for x, top in ((3, 21), (8, 19), (13, 22), (32, 20), (36, 22)):
        draw.polygon([(x, top), (x - 3, 31), (x + 3, 31)], fill=(20, 39, 58, 255))

    # Tiny sentry station, warm window, sign, and Senpai Sans's blue-hooded silhouette.
    draw.rectangle((19, 21, 29, 29), fill=(58, 37, 38, 255))
    draw.polygon([(18, 21), (24, 18), (30, 21)], fill=(30, 24, 34, 255))
    draw.rectangle((21, 23, 25, 26), fill=(255, 207, 103, 255))
    draw.point((22, 24), fill=(82, 105, 131, 255))
    draw.line((17, 22, 17, 29), fill=(104, 75, 62, 255), width=1)
    draw.line((14, 22, 19, 22), fill=(104, 75, 62, 255), width=1)
    draw.ellipse((27, 23, 29, 25), fill=(215, 227, 239, 255))
    draw.rectangle((27, 25, 29, 28), fill=(39, 92, 156, 255))
    draw.point((29, 28), fill=(255, 123, 184, 255))

    # Snow banks and a short path toward the station.
    draw.polygon([(3, 29), (11, 27), (18, 30), (27, 28), (36, 30), (36, 39), (3, 39)],
                 fill=(224, 236, 250, 255))
    draw.polygon([(21, 39), (24, 29), (28, 29), (32, 39)], fill=(158, 184, 214, 255))
    draw.line((3, 32, 18, 31), fill=(173, 202, 229, 255), width=1)
    draw.line((30, 34, 36, 33), fill=(173, 202, 229, 255), width=1)

    return image


if __name__ == "__main__":
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    build_glimpse().save(OUTPUT, optimize=True)
    print(f"wrote {OUTPUT.relative_to(ROOT)}")
