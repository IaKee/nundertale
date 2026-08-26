"""Generate the first two 200x110 lore intro cards.

The art is drawn at native resolution with a fixed six-colour sepia palette so
LoreScene can scale it 2x with nearest-neighbour filtering.  Run from the repo
root with Python 3.13 (Pillow required).
"""

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageDraw


WIDTH, HEIGHT = 200, 110
PALETTE = {
    "sky": (217, 160, 74, 255),
    "sun": (240, 200, 120, 255),
    "far": (184, 125, 53, 255),
    "mid": (140, 90, 36, 255),
    "shade": (90, 52, 22, 255),
    "ink": (43, 22, 8, 255),
}


def pixel_line(draw: ImageDraw.ImageDraw, points: list[tuple[int, int]], fill: str, width: int = 1) -> None:
    draw.line(points, fill=PALETTE[fill], width=width, joint="curve")


def draw_backdrop(draw: ImageDraw.ImageDraw, *, battlefield: bool = False) -> None:
    draw.rectangle((0, 0, WIDTH - 1, HEIGHT - 1), fill=PALETTE["sky"])
    if battlefield:
        draw.polygon([(0, 46), (17, 31), (31, 40), (47, 28), (67, 44), (85, 34),
                      (104, 47), (124, 33), (146, 43), (164, 29), (182, 42), (199, 35),
                      (199, 70), (0, 70)], fill=PALETTE["far"])
        draw.polygon([(0, 65), (16, 54), (29, 60), (45, 49), (61, 65), (79, 55),
                      (97, 67), (117, 54), (135, 64), (155, 50), (177, 63), (199, 52),
                      (199, 110), (0, 110)], fill=PALETTE["mid"])
        for x, y in ((11, 78), (28, 91), (84, 78), (105, 92), (188, 81)):
            draw.polygon([(x - 7, y + 4), (x, y - 2), (x + 8, y + 4)], fill=PALETTE["shade"])
    else:
        draw.ellipse((70, 11, 130, 71), fill=PALETTE["sun"])
        # A monitor in the sun is the shared "internet" both races rule.
        draw.rectangle((84, 27, 116, 49), fill=PALETTE["mid"])
        draw.rectangle((88, 30, 112, 45), fill=PALETTE["sun"])
        draw.rectangle((98, 49, 102, 55), fill=PALETTE["mid"])
        draw.rectangle((91, 54, 109, 57), fill=PALETTE["mid"])
        draw.polygon([(0, 47), (14, 38), (25, 44), (39, 33), (57, 47), (72, 38),
                      (90, 51), (109, 44), (128, 51), (145, 34), (161, 43), (177, 31),
                      (199, 43), (199, 73), (0, 73)], fill=PALETTE["far"])
        draw.polygon([(0, 65), (18, 55), (35, 65), (53, 53), (72, 66), (94, 56),
                      (115, 67), (139, 53), (159, 65), (179, 52), (199, 61),
                      (199, 110), (0, 110)], fill=PALETTE["mid"])
    draw.rectangle((0, 91, 199, 109), fill=PALETTE["shade"])
    draw.polygon([(0, 101), (25, 95), (48, 102), (70, 96), (93, 103), (120, 97),
                  (143, 103), (169, 96), (199, 102), (199, 109), (0, 109)], fill=PALETTE["ink"])


def draw_human_gamer(draw: ImageDraw.ImageDraw, x: int, y: int, *, armored: bool = False) -> None:
    """Draw an unmistakably human gamer: round head, hair, nose and headset."""
    ink, shade, mid, sun = (PALETTE[name] for name in ("ink", "shade", "mid", "sun"))
    # Legs and ordinary shoes.
    draw.rectangle((x + 5, y + 38, x + 10, y + 57), fill=ink)
    draw.rectangle((x + 19, y + 38, x + 24, y + 57), fill=ink)
    draw.rectangle((x + 1, y + 55, x + 11, y + 59), fill=ink)
    draw.rectangle((x + 19, y + 55, x + 30, y + 59), fill=ink)
    if armored:
        draw.polygon([(x + 2, y + 18), (x + 28, y + 18), (x + 32, y + 42),
                      (x + 25, y + 48), (x + 5, y + 48), (x - 2, y + 42)], fill=ink)
        draw.polygon([(x + 5, y + 22), (x + 25, y + 22), (x + 27, y + 39),
                      (x + 21, y + 44), (x + 9, y + 44), (x + 3, y + 39)], fill=mid)
        draw.rectangle((x + 9, y + 27, x + 21, y + 30), fill=shade)
        draw.rectangle((x + 13, y + 25, x + 17, y + 35), fill=shade)
        # Pauldrons and raised sword.
        draw.rectangle((x - 7, y + 20, x + 3, y + 29), fill=ink)
        draw.rectangle((x + 27, y + 20, x + 37, y + 29), fill=ink)
        draw.rectangle((x + 34, y - 5, x + 36, y + 31), fill=ink)
        draw.polygon([(x + 35, y - 16), (x + 31, y - 5), (x + 39, y - 5)], fill=ink)
        draw.rectangle((x + 29, y + 4, x + 41, y + 7), fill=ink)
    else:
        draw.polygon([(x + 2, y + 20), (x + 27, y + 20), (x + 30, y + 45),
                      (x, y + 45)], fill=ink)
        draw.polygon([(x + 5, y + 23), (x + 24, y + 23), (x + 25, y + 41),
                      (x + 4, y + 41)], fill=mid)
        # Controller emblem and controller in both hands.
        draw.rectangle((x + 12, y + 28, x + 16, y + 29), fill=shade)
        draw.rectangle((x + 13, y + 27, x + 15, y + 30), fill=shade)
        draw.rectangle((x + 29, y + 31, x + 39, y + 37), fill=ink)
        draw.rectangle((x + 31, y + 32, x + 33, y + 35), fill=mid)
        draw.point((x + 36, y + 33), fill=mid)
        pixel_line(draw, [(x + 23, y + 29), (x + 33, y + 34)], "ink", 3)
    # Human head and ear, with an actual face visible through the open helmet.
    draw.ellipse((x + 6, y + 1, x + 24, y + 21), fill=ink)
    draw.rectangle((x + 7, y + 6, x + 23, y + 17), fill=sun)
    draw.rectangle((x + 6, y + 5, x + 21, y + 9), fill=shade)
    draw.rectangle((x + 8, y + 3, x + 18, y + 7), fill=shade)
    draw.rectangle((x + 22, y + 10, x + 26, y + 14), fill=sun)  # human nose
    draw.point((x + 19, y + 11), fill=ink)
    draw.rectangle((x + 16, y + 16, x + 22, y + 17), fill=shade)
    # Headset band and round ear cups; never ears/eyes of a frog.
    pixel_line(draw, [(x + 7, y + 7), (x + 9, y), (x + 20, y), (x + 23, y + 7)], "ink", 2)
    draw.rectangle((x + 4, y + 8, x + 8, y + 16), fill=ink)
    draw.rectangle((x + 22, y + 8, x + 26, y + 16), fill=ink)
    if armored:
        draw.rectangle((x + 3, y - 2, x + 27, y + 3), fill=ink)
        draw.rectangle((x + 1, y + 2, x + 5, y + 15), fill=ink)
        draw.rectangle((x + 27, y + 2, x + 30, y + 15), fill=ink)


def draw_furry(draw: ImageDraw.ImageDraw, x: int, y: int, *, charging: bool = False) -> None:
    ink, shade, mid, sun = (PALETTE[name] for name in ("ink", "shade", "mid", "sun"))
    # Digitigrade legs and big costume paws.
    draw.polygon([(x + 9, y + 35), (x + 17, y + 36), (x + 14, y + 53),
                  (x + 5, y + 58), (x, y + 55), (x + 8, y + 48)], fill=ink)
    draw.polygon([(x + 24, y + 35), (x + 31, y + 37), (x + 34, y + 52),
                  (x + 42, y + 56), (x + 38, y + 60), (x + 26, y + 55)], fill=ink)
    # Torso and lighter belly.
    draw.ellipse((x + 5, y + 15, x + 34, y + 45), fill=ink)
    draw.ellipse((x + 12, y + 23, x + 29, y + 43), fill=mid)
    # Wolf head, pointed ears and protruding muzzle.
    draw.polygon([(x + 5, y + 7), (x + 7, y - 8), (x + 15, y - 1),
                  (x + 25, y - 5), (x + 31, y + 7)], fill=ink)
    draw.polygon([(x + 8, y + 5), (x + 10, y - 3), (x + 15, y + 2),
                  (x + 24, y), (x + 28, y + 9), (x + 17, y + 17)], fill=mid)
    draw.polygon([(x + 20, y + 8), (x + 38, y + 10), (x + 42, y + 16),
                  (x + 35, y + 21), (x + 19, y + 17)], fill=sun)
    draw.rectangle((x + 38, y + 12, x + 43, y + 16), fill=ink)
    draw.point((x + 23, y + 7), fill=ink)
    # Tail leaves no doubt this is the furry half of the pairing.
    draw.polygon([(x + 6, y + 29), (x - 6, y + 23), (x - 14, y + 29),
                  (x - 8, y + 37), (x + 5, y + 36)], fill=ink)
    draw.polygon([(x + 2, y + 30), (x - 6, y + 27), (x - 9, y + 30),
                  (x - 5, y + 33), (x + 4, y + 34)], fill=mid)
    if charging:
        # Raised arm and a large cartoon bone, matching the canonical card.
        pixel_line(draw, [(x + 27, y + 26), (x + 40, y + 14), (x + 48, y + 2)], "ink", 5)
        pixel_line(draw, [(x + 45, y + 4), (x + 62, y - 10)], "ink", 4)
        draw.ellipse((x + 56, y - 15, x + 65, y - 7), fill=ink)
        draw.ellipse((x + 59, y - 9, x + 67, y - 1), fill=ink)
        pixel_line(draw, [(x + 47, y + 2), (x + 61, y - 11)], "sun", 2)
    else:
        pixel_line(draw, [(x + 9, y + 23), (x - 1, y + 34)], "ink", 4)


def make_card01() -> Image.Image:
    image = Image.new("RGBA", (WIDTH, HEIGHT), PALETTE["sky"])
    draw = ImageDraw.Draw(image)
    draw_backdrop(draw)
    # Equal footing and a clear gap: two races, side by side, not fighting yet.
    draw_human_gamer(draw, 48, 39)
    draw_furry(draw, 119, 39)
    # Their controller cable and paw-print mark meet beneath the monitor.
    pixel_line(draw, [(83, 75), (93, 80), (107, 80), (117, 75)], "ink", 2)
    draw.rectangle((96, 77, 104, 83), fill=PALETTE["ink"])
    draw.point((99, 79), fill=PALETTE["sun"])
    draw.point((102, 81), fill=PALETTE["sun"])
    return image


def make_card02() -> Image.Image:
    image = Image.new("RGBA", (WIDTH, HEIGHT), PALETTE["sky"])
    draw = ImageDraw.Draw(image)
    draw_backdrop(draw, battlefield=True)
    draw_furry(draw, 24, 39, charging=True)
    draw_human_gamer(draw, 132, 38, armored=True)
    # Motion streaks emphasize the confrontation without muddying silhouettes.
    pixel_line(draw, [(78, 58), (101, 58)], "shade", 2)
    pixel_line(draw, [(84, 66), (111, 66)], "shade", 2)
    pixel_line(draw, [(92, 74), (119, 74)], "shade", 2)
    return image


def validate(image: Image.Image) -> None:
    assert image.size == (WIDTH, HEIGHT)
    assert image.mode == "RGBA"
    assert set(image.getdata()) <= set(PALETTE.values())


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--preview", type=Path, help="optional 2x side-by-side preview")
    args = parser.parse_args()

    output_dir = Path("assets/img/lore")
    cards = (make_card01(), make_card02())
    for number, image in enumerate(cards, 1):
        validate(image)
        image.save(output_dir / f"card{number:02d}.png", optimize=True)

    if args.preview:
        preview = Image.new("RGBA", (WIDTH * 2, HEIGHT * 2 * len(cards)), PALETTE["ink"])
        for index, image in enumerate(cards):
            enlarged = image.resize((WIDTH * 2, HEIGHT * 2), Image.Resampling.NEAREST)
            preview.paste(enlarged, (0, index * HEIGHT * 2))
        args.preview.parent.mkdir(parents=True, exist_ok=True)
        preview.save(args.preview)


if __name__ == "__main__":
    main()
