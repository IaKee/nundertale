"""Prepare and decode an 8x macro-pixel ImageGen pass for A04 Barky.

The model works on a standard 1024x1536 canvas.  The actual sprite area is an
exact 8x enlargement of 112x180, centered at (64, 48).  Decoding assigns one
palette color to every aligned 8x8 block; it never resamples between pixels.
"""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[2]
GEN = ROOT / "assets" / "_gen" / "ruins"
CURRENT = ROOT / "assets" / "sprites" / "ow" / "barky.png"
GUIDE = GEN / "A04_ow_barky-macro8-guide.png"
RAW = GEN / "A04_ow_barky-macro8-generated.png"
NATIVE = GEN / "A04_ow_barky-macro8-native.png"
COMPARE = GEN / "A04_ow_barky-macro8-before-after.png"

CANVAS = (1024, 1536)
OFFSET = (64, 48)
SCALE = 8
NATIVE_SIZE = (112, 180)
MAGENTA = (255, 0, 255)
PALETTE = [
    (20, 20, 24), (154, 154, 170), (110, 110, 126),
    (201, 138, 168), (35, 35, 41), (60, 60, 70),
    (200, 200, 210), (124, 255, 58), (208, 38, 58),
]


def prepare() -> None:
    GEN.mkdir(parents=True, exist_ok=True)
    native = Image.open(CURRENT).convert("RGBA")
    assert native.size == NATIVE_SIZE
    # Exact nearest-neighbour macro blocks.  Magenta replaces transparency so
    # the model receives a fully opaque, easy-to-key guide.
    flat = Image.new("RGB", NATIVE_SIZE, MAGENTA)
    flat.paste(native.convert("RGB"), mask=native.getchannel("A"))
    macro = flat.resize((NATIVE_SIZE[0] * SCALE, NATIVE_SIZE[1] * SCALE), Image.Resampling.NEAREST)
    canvas = Image.new("RGB", CANVAS, MAGENTA)
    canvas.paste(macro, OFFSET)
    canvas.save(GUIDE)
    print(GUIDE)


def nearest_indices(rgb: np.ndarray) -> np.ndarray:
    colors = np.asarray(PALETTE + [MAGENTA], dtype=np.int32)
    flat = rgb.reshape(-1, 3).astype(np.int32)
    out = np.empty(len(flat), dtype=np.uint8)
    for start in range(0, len(flat), 200_000):
        part = flat[start:start + 200_000]
        distance = ((part[:, None, :] - colors[None, :, :]) ** 2).sum(axis=2)
        out[start:start + len(part)] = np.argmin(distance, axis=1)
    return out.reshape(rgb.shape[:2])


def decode(source: Path) -> Image.Image:
    image = Image.open(source).convert("RGB")
    if image.size != CANVAS:
        raise ValueError(f"ImageGen output is {image.size}, expected exact {CANVAS}")
    x, y = OFFSET
    crop = np.asarray(image.crop((x, y, x + 896, y + 1440)), dtype=np.uint8)
    indices = nearest_indices(crop)
    native = np.full((180, 112), len(PALETTE), dtype=np.uint8)
    for py in range(180):
        for px in range(112):
            block = indices[py * 8:(py + 1) * 8, px * 8:(px + 1) * 8]
            native[py, px] = np.bincount(block.ravel(), minlength=len(PALETTE) + 1).argmax()

    # Spec invariants: exact duplicated idle frames and guaranteed empty cells.
    native[0:36, 56:84] = native[0:36, 0:28]
    native[36:72, 56:84] = native[36:72, 0:28]
    for col, row in ((2, 2), (3, 2), (2, 3), (3, 3), (3, 4)):
        native[row * 36:(row + 1) * 36, col * 28:(col + 1) * 28] = len(PALETTE)

    rgba = np.zeros((180, 112, 4), dtype=np.uint8)
    opaque = native < len(PALETTE)
    colors = np.asarray(PALETTE, dtype=np.uint8)
    rgba[opaque, :3] = colors[native[opaque]]
    rgba[opaque, 3] = 255
    return Image.fromarray(rgba, "RGBA")


def checker(size: tuple[int, int], block: int = 16) -> Image.Image:
    bg = Image.new("RGB", size, (62, 62, 70))
    d = ImageDraw.Draw(bg)
    for y in range(0, size[1], block):
        for x in range(0, size[0], block):
            if (x // block + y // block) % 2:
                d.rectangle((x, y, x + block - 1, y + block - 1), fill=(82, 82, 92))
    return bg


def comparison(before: Image.Image, after: Image.Image) -> Image.Image:
    zoom, pad, top = 4, 24, 48
    arts = [im.resize((448, 720), Image.Resampling.NEAREST) for im in (before, after)]
    out = Image.new("RGB", (448 * 2 + pad * 3, 720 + top + pad), (28, 28, 34))
    for i, art in enumerate(arts):
        bg = checker(art.size)
        bg.paste(art, mask=art.getchannel("A"))
        out.paste(bg, (pad + i * (448 + pad), top))
    d = ImageDraw.Draw(out)
    font = ImageFont.load_default()
    d.text((pad, 18), "ANTES - sprite atual", fill="white", font=font)
    d.text((pad * 2 + 448, 18), "DEPOIS - ImageGen macro-pixel 8x", fill="white", font=font)
    return out


def finish(source: Path) -> None:
    GEN.mkdir(parents=True, exist_ok=True)
    generated = Image.open(source)
    generated.save(RAW)
    native = decode(source)
    native.save(NATIVE)
    before = Image.open(CURRENT).convert("RGBA")
    comparison(before, native).save(COMPARE)
    used = {pixel[:3] for pixel in native.getdata() if pixel[3]}
    assert native.size == NATIVE_SIZE
    assert used <= set(PALETTE)
    print(RAW)
    print(NATIVE)
    print(COMPARE)
    print(f"validated: {native.size[0]}x{native.size[1]}, {len(used)} colors")


def main() -> None:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="command", required=True)
    sub.add_parser("prepare")
    decode_parser = sub.add_parser("decode")
    decode_parser.add_argument("source", type=Path)
    args = parser.parse_args()
    if args.command == "prepare":
        prepare()
    else:
        finish(args.source)


if __name__ == "__main__":
    main()
