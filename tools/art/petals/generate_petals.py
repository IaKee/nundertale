#!/usr/bin/env python3
"""Build ART-183 petal bullet sprites and an optional inspection preview.

The buzzing sprites are two-frame horizontal sheets.  Each frame is 12x12;
the remaining bullets are single 16x16 sprites.  Source pixels are transparent
or white so the game can tint them without inheriting an outline.
"""

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[3]
OUTPUT = ROOT / "assets" / "sprites" / "bullets"
WHITE = (255, 255, 255, 255)
CLEAR = (0, 0, 0, 0)


def image(width: int, height: int) -> Image.Image:
    return Image.new("RGBA", (width, height), CLEAR)


def put_pixels(canvas: Image.Image, pixels: set[tuple[int, int]], ox: int = 0) -> None:
    for x, y in pixels:
        canvas.putpixel((x + ox, y), WHITE)


def build_petal() -> Image.Image:
    """A rose-first 12px silhouette, shifted one pixel for its buzz frame."""
    rose = {
        (4, 1), (5, 1), (7, 1),
        (3, 2), (4, 2), (6, 2), (7, 2), (8, 2),
        (3, 3), (4, 3), (5, 3), (6, 3), (7, 3), (8, 3),
        (4, 4), (5, 4), (6, 4), (7, 4),
        (5, 5), (6, 5),
        (5, 6),
        (3, 7), (5, 7), (7, 7),
        (4, 8), (5, 8), (6, 8),
        (5, 9),
        (4, 10), (5, 10), (6, 10),
    }
    sheet = image(24, 12)
    put_pixels(sheet, rose)
    # The bloom rocks right while the stem stays visually flower-like.
    put_pixels(sheet, {(x + 1 if y <= 5 else x, y) for x, y in rose}, 12)
    return sheet


def build_petal_bullet() -> Image.Image:
    """A plain rounded capsule; frame two moves up for a restrained buzz."""
    capsule = {
        (3, 4), (4, 3), (5, 3), (6, 3), (7, 3), (8, 4),
        (3, 5), (4, 4), (5, 4), (6, 4), (7, 4), (8, 5),
        (3, 6), (4, 5), (5, 5), (6, 5), (7, 5), (8, 6),
        (4, 6), (5, 6), (6, 6), (7, 6),
    }
    sheet = image(24, 12)
    put_pixels(sheet, capsule)
    put_pixels(sheet, {(x, y - 1) for x, y in capsule}, 12)
    return sheet


def build_rose_vine() -> Image.Image:
    """A curling vine whose alternating triangular thorns remain clear at 1x."""
    vine = {
        # S-shaped stem.
        (7, 15), (7, 14), (8, 13), (8, 12), (7, 11), (6, 10),
        (6, 9), (7, 8), (8, 7), (9, 6), (9, 5), (8, 4),
        (7, 3), (7, 2), (8, 1), (9, 1),
        # Large, alternating thorns.
        (6, 13), (5, 12), (9, 11), (10, 10),
        (5, 8), (4, 7), (10, 5), (11, 4),
        (6, 3), (5, 2), (9, 2), (10, 3),
    }
    canvas = image(16, 16)
    put_pixels(canvas, vine)
    return canvas


def build_thorn_petal() -> Image.Image:
    """A compact petal/leaf mass with unmistakable spikes on every side."""
    petal = {
        # Pointed, elongated petal body.
        (7, 1), (6, 2), (7, 2), (8, 2),
        (5, 3), (6, 3), (7, 3), (8, 3), (9, 3),
        (4, 4), (5, 4), (6, 4), (7, 4), (8, 4), (9, 4), (10, 4),
        (3, 5), (4, 5), (5, 5), (6, 5), (7, 5), (8, 5), (9, 5), (10, 5), (11, 5),
        (4, 6), (5, 6), (6, 6), (7, 6), (8, 6), (9, 6), (10, 6),
        (4, 7), (5, 7), (6, 7), (7, 7), (8, 7), (9, 7), (10, 7),
        (5, 8), (6, 8), (7, 8), (8, 8), (9, 8),
        (6, 9), (7, 9), (8, 9), (7, 10), (7, 11),
        # Four side thorns and sharp tips.
        (7, 0), (3, 3), (2, 2), (11, 3), (12, 2),
        (2, 6), (1, 6), (12, 6), (13, 6),
        (4, 9), (3, 10), (10, 9), (11, 10), (7, 13),
    }
    canvas = image(16, 16)
    put_pixels(canvas, petal)
    return canvas


def build_pollen() -> Image.Image:
    """A loose, asymmetric cluster whose satellite motes imply lingering dust."""
    cloud = {
        (2, 3), (5, 2), (9, 1), (13, 3),
        (4, 5), (5, 4), (6, 4), (7, 4), (8, 4), (10, 4),
        (3, 6), (4, 6), (5, 5), (6, 5), (7, 5), (8, 5), (9, 5), (10, 5), (11, 6),
        (4, 7), (5, 6), (6, 6), (7, 6), (8, 6), (9, 6), (10, 7), (12, 7),
        (5, 8), (6, 7), (7, 7), (8, 7), (9, 8), (3, 9),
        (6, 9), (7, 8), (8, 9), (10, 10),
        (1, 11), (5, 12), (8, 11), (13, 12), (11, 14),
    }
    canvas = image(16, 16)
    put_pixels(canvas, cloud)
    return canvas


def verify(name: str, sprite: Image.Image) -> None:
    frame_width = 12 if name in {"petal.png", "petal_bullet.png"} else sprite.width
    assert 8 <= frame_width <= 16 and 8 <= sprite.height <= 16
    assert all(pixel == CLEAR or pixel == WHITE for pixel in sprite.getdata())
    assert sprite.getbbox() is not None


def make_preview(sprites: dict[str, Image.Image], destination: Path) -> None:
    scale = 8
    panel_width = 224
    preview = Image.new("RGB", (panel_width * len(sprites), 184), (8, 8, 12))
    draw = ImageDraw.Draw(preview)
    for index, (name, sprite) in enumerate(sprites.items()):
        x = index * panel_width
        scaled = sprite.resize((sprite.width * scale, sprite.height * scale), Image.Resampling.NEAREST)
        preview.paste(scaled, (x + (panel_width - scaled.width) // 2, 28), scaled)
        draw.text((x + 8, 8), name, fill=(255, 255, 255))
        if sprite.width == 24:
            divider = x + panel_width // 2
            draw.line((divider, 24, divider, 132), fill=(70, 70, 76))
        draw.text((x + 8, 160), f"native {sprite.width}x{sprite.height}", fill=(180, 180, 188))
    destination.parent.mkdir(parents=True, exist_ok=True)
    preview.save(destination)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--preview", type=Path, help="optional black-background contact sheet")
    args = parser.parse_args()

    sprites = {
        "petal.png": build_petal(),
        "petal_bullet.png": build_petal_bullet(),
        "rose_vine.png": build_rose_vine(),
        "thorn_petal.png": build_thorn_petal(),
        "pollen.png": build_pollen(),
    }
    OUTPUT.mkdir(parents=True, exist_ok=True)
    for name, sprite in sprites.items():
        verify(name, sprite)
        sprite.save(OUTPUT / name, optimize=True)

    if args.preview:
        make_preview(sprites, args.preview)

    print("Generated and verified: " + ", ".join(sprites))


if __name__ == "__main__":
    main()
