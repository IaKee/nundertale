#!/usr/bin/env python3
"""
Art generator for the Landing (R1 outside the Con):
- assets/tiles/landing.png (20x20 tileset: can-compacted rock, nacho stratum, trash floor, light beam)
- assets/props/ruins_con_doors.png (Con facade with sunken double doors, claw marks, REGISTRATION -> sign)
- assets/props/ruins_plush_pile.png (40x40 plush pile: frame 0 knocked-out Floofy, frame 1 fox-shaped dent)
- assets/props/ruins_plush_pile.json (prop sprite sheet metadata)

Usage:
  python tools/art/landing/generate_landing_art.py [--preview]
"""

from __future__ import annotations

import json
import os
import sys
import numpy as np
from PIL import Image

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

# Canonical outline color across nUndertale overworld (never pure black)
OUTLINE = (0x14, 0x10, 0x18, 255)
TRANSPARENT = (0, 0, 0, 0)

# Cave Rock Palette (outside the Con: slate/earthy rock with purple-tinted shadows, no purple brick)
ROCK_DARKEST = (0x20, 0x18, 0x26, 255)
ROCK_DARK = (0x2D, 0x24, 0x35, 255)
ROCK_MID = (0x44, 0x38, 0x4E, 255)
ROCK_LIGHT = (0x5E, 0x51, 0x6B, 255)
ROCK_BRIGHT = (0x7B, 0x6E, 0x8A, 255)
ROCK_HIGHLIGHT = (0x9D, 0x90, 0xAC, 255)

# Compacted Cans Palette (Citrus Blast neon green, aluminum silver, grape purple, red cola)
CAN_GREEN_DARK = (0x17, 0x47, 0x0C, 255)
CAN_GREEN_MID = (0x33, 0x99, 0x1A, 255)
CAN_GREEN_BRIGHT = (0x7C, 0xFF, 0x3A, 255)  # Canon neon green accent (LORE §25.2)

CAN_SILVER_DARK = (0x47, 0x4C, 0x55, 255)
CAN_SILVER_MID = (0x8B, 0x93, 0x9E, 255)
CAN_SILVER_BRIGHT = (0xD2, 0xD6, 0xDC, 255)

CAN_PURPLE_DARK = (0x4A, 0x1B, 0x73, 255)
CAN_PURPLE_MID = (0x7E, 0x22, 0xCE, 255)
CAN_PURPLE_BRIGHT = (0xC0, 0x84, 0xFC, 255)

CAN_RED_DARK = (0x7F, 0x1D, 0x1D, 255)
CAN_RED_MID = (0xDC, 0x26, 0x26, 255)
CAN_RED_BRIGHT = (0xF8, 0x71, 0x71, 255)

# Nacho Stratum Palette (geological cheese dust layers: 40% cans, 30% nacho dust, 30% do not ask)
NACHO_DEEP = (0x66, 0x30, 0x00, 255)
NACHO_DARK = (0x99, 0x4D, 0x00, 255)
NACHO_MID = (0xD9, 0x77, 0x06, 255)
NACHO_BRIGHT = (0xF5, 0x9E, 0x0B, 255)
NACHO_LIGHT = (0xFB, 0xBF, 0x24, 255)
NACHO_HIGHLIGHT = (0xFE, 0xE3, 0x88, 255)

# Floofy Palette (matching assets/sprites/ow/floofy.png and LORE §10.10)
FLOOFY_BODY = (0xFF, 0xB3, 0xD9, 255)
FLOOFY_BODY_SHADE = (0xE0, 0x7A, 0xAE, 255)
FLOOFY_BODY_DARK = (0xB8, 0x50, 0x8A, 255)
FLOOFY_WHITE = (0xFF, 0xF0, 0xF5, 255)
FLOOFY_WHITE_SHADE = (0xF4, 0xF0, 0xEA, 255)
FLOOFY_CROWN_GREEN = (0x3F, 0xA3, 0x4D, 255)
FLOOFY_CROWN_DARK = (0x2C, 0x7A, 0x38, 255)
FLOOFY_CROWN_YELLOW = (0xFF, 0xE1, 0x4A, 255)
FLOOFY_CROWN_RED = (0xD0, 0x26, 0x3A, 255)
FLOOFY_CROWN_RED_DARK = (0x8A, 0x15, 0x30, 255)

# Plush Pile Toy Colors
PLUSH_BLUE = (0x38, 0xBD, 0xF8, 255)
PLUSH_BLUE_DARK = (0x02, 0x84, 0xC7, 255)
PLUSH_GREEN = (0x4A, 0xDE, 0x80, 255)
PLUSH_GREEN_DARK = (0x16, 0xA3, 0x4A, 255)
PLUSH_ORANGE = (0xFB, 0x92, 0x3C, 255)
PLUSH_ORANGE_DARK = (0xEA, 0x58, 0x0C, 255)
PLUSH_PURPLE = (0xC0, 0x84, 0xFC, 255)
PLUSH_PURPLE_DARK = (0x7C, 0x3A, 0xED, 255)
PLUSH_YELLOW = (0xFD, 0xE0, 0x47, 255)
PLUSH_YELLOW_DARK = (0xCA, 0x8A, 0x04, 255)
PLUSH_RED = (0xF4, 0x3F, 0x5E, 255)
PLUSH_RED_DARK = (0xBE, 0x12, 0x3C, 255)


def set_px(arr: np.ndarray, x: int, y: int, color: tuple[int, int, int, int]):
    if 0 <= x < arr.shape[1] and 0 <= y < arr.shape[0]:
        arr[y, x] = color


def fill_rect(arr: np.ndarray, x0: int, y0: int, x1: int, y1: int, color: tuple[int, int, int, int]):
    for y in range(max(0, y0), min(arr.shape[0], y1)):
        for x in range(max(0, x0), min(arr.shape[1], x1)):
            arr[y, x] = color


def blend_px(arr: np.ndarray, x: int, y: int, color: tuple[int, int, int, int]):
    if 0 <= x < arr.shape[1] and 0 <= y < arr.shape[0]:
        src_a = color[3] / 255.0
        dst_a = arr[y, x, 3] / 255.0
        out_a = src_a + dst_a * (1.0 - src_a)
        if out_a > 0.0:
            out_r = (color[0] * src_a + arr[y, x, 0] * dst_a * (1.0 - src_a)) / out_a
            out_g = (color[1] * src_a + arr[y, x, 1] * dst_a * (1.0 - src_a)) / out_a
            out_b = (color[2] * src_a + arr[y, x, 2] * dst_a * (1.0 - src_a)) / out_a
            arr[y, x] = [int(round(out_r)), int(round(out_g)), int(round(out_b)), int(round(out_a * 255))]


# =============================================================================
# 1. PLUSH PILE (assets/props/ruins_plush_pile.png, ruins_plush_pile.json)
# 40x40 image: row 0 (40x20) = with knocked-out Floofy, row 1 (40x20) = with fox-shaped dent
# =============================================================================

def draw_plush_mound_base(arr: np.ndarray, y_offset: int):
    """Draws the underlying mound of 40+ convention plushies."""
    # Bottom shadow on cave dirt
    for x in range(2, 38):
        set_px(arr, x, y_offset + 19, (0x10, 0x0C, 0x14, 180))
    for x in range(4, 36):
        set_px(arr, x, y_offset + 18, OUTLINE)

    # Base plushie blobs:
    # 1. Left base: Cyan shark / wolf plush (x: 2..11, y: 12..18)
    for y in range(13, 18):
        for x in range(3, 11):
            set_px(arr, x, y_offset + y, PLUSH_BLUE_DARK)
    for y in range(14, 17):
        for x in range(4, 9):
            set_px(arr, x, y_offset + y, PLUSH_BLUE)
    set_px(arr, 3, y_offset + 14, OUTLINE)
    set_px(arr, 10, y_offset + 14, OUTLINE)
    set_px(arr, 2, y_offset + 15, OUTLINE)
    set_px(arr, 11, y_offset + 15, OUTLINE)
    set_px(arr, 5, y_offset + 15, OUTLINE)  # button eye
    set_px(arr, 6, y_offset + 15, (0xFF, 0xFF, 0xFF, 255))  # white stitch

    # 2. Right base: Purple teddy bear plush (x: 28..37, y: 12..18)
    for y in range(13, 18):
        for x in range(29, 37):
            set_px(arr, x, y_offset + y, PLUSH_PURPLE_DARK)
    for y in range(14, 17):
        for x in range(30, 36):
            set_px(arr, x, y_offset + y, PLUSH_PURPLE)
    set_px(arr, 28, y_offset + 15, OUTLINE)
    set_px(arr, 37, y_offset + 15, OUTLINE)
    set_px(arr, 33, y_offset + 14, OUTLINE)  # button eye
    set_px(arr, 34, y_offset + 14, (0xFF, 0xFF, 0xFF, 255))
    # Round bear ear
    set_px(arr, 36, y_offset + 12, OUTLINE)
    set_px(arr, 35, y_offset + 12, PLUSH_PURPLE)

    # 3. Far left top plush: Orange fox/tiger plush (x: 5..14, y: 7..13)
    for y in range(8, 13):
        for x in range(6, 14):
            set_px(arr, x, y_offset + y, PLUSH_ORANGE_DARK)
    for y in range(9, 12):
        for x in range(7, 13):
            set_px(arr, x, y_offset + y, PLUSH_ORANGE)
    set_px(arr, 5, y_offset + 9, OUTLINE)
    set_px(arr, 6, y_offset + 8, OUTLINE)
    set_px(arr, 7, y_offset + 7, OUTLINE)  # pointy ear
    set_px(arr, 8, y_offset + 8, PLUSH_ORANGE)
    set_px(arr, 9, y_offset + 10, OUTLINE)  # eye
    set_px(arr, 10, y_offset + 10, (0xFF, 0xFF, 0xFF, 255))

    # 4. Far right top plush: Green dragon/dino plush (x: 25..34, y: 7..13)
    for y in range(8, 13):
        for x in range(26, 34):
            set_px(arr, x, y_offset + y, PLUSH_GREEN_DARK)
    for y in range(9, 12):
        for x in range(27, 33):
            set_px(arr, x, y_offset + y, PLUSH_GREEN)
    set_px(arr, 33, y_offset + 8, OUTLINE)
    set_px(arr, 34, y_offset + 9, OUTLINE)
    set_px(arr, 30, y_offset + 9, OUTLINE)  # eye
    set_px(arr, 31, y_offset + 9, (0xFF, 0xFF, 0xFF, 255))
    set_px(arr, 26, y_offset + 7, (0xFD, 0xE0, 0x47, 255))  # yellow felt horn
    set_px(arr, 25, y_offset + 7, OUTLINE)

    # 5. Top rim plushies: Yellow chick (x: 10..17, y: 4..9), Red panda (x: 22..29, y: 4..9)
    # Yellow chick
    for y in range(5, 9):
        for x in range(11, 17):
            set_px(arr, x, y_offset + y, PLUSH_YELLOW_DARK)
    for y in range(5, 8):
        for x in range(12, 16):
            set_px(arr, x, y_offset + y, PLUSH_YELLOW)
    set_px(arr, 10, y_offset + 6, OUTLINE)
    set_px(arr, 13, y_offset + 5, OUTLINE)  # eye
    set_px(arr, 14, y_offset + 6, (0xFB, 0x92, 0x3C, 255))  # orange beak
    set_px(arr, 11, y_offset + 4, OUTLINE)

    # Red panda
    for y in range(5, 9):
        for x in range(23, 29):
            set_px(arr, x, y_offset + y, PLUSH_RED_DARK)
    for y in range(5, 8):
        for x in range(24, 28):
            set_px(arr, x, y_offset + y, PLUSH_RED)
    set_px(arr, 29, y_offset + 6, OUTLINE)
    set_px(arr, 26, y_offset + 5, OUTLINE)  # eye
    set_px(arr, 27, y_offset + 5, (0xFF, 0xFF, 0xFF, 255))

    # Bottom center plushies holding up the front edge (x: 12..27, y: 14..18)
    for x in range(12, 28):
        set_px(arr, x, y_offset + 17, OUTLINE)
    for y in range(14, 17):
        for x in range(13, 20):
            set_px(arr, x, y_offset + y, PLUSH_PURPLE_DARK)
        for x in range(20, 27):
            set_px(arr, x, y_offset + y, PLUSH_BLUE_DARK)
    set_px(arr, 15, y_offset + 15, PLUSH_PURPLE)
    set_px(arr, 16, y_offset + 15, PLUSH_PURPLE)
    set_px(arr, 22, y_offset + 15, PLUSH_BLUE)
    set_px(arr, 23, y_offset + 15, PLUSH_BLUE)


def draw_plush_pile_frame0_floofy(arr: np.ndarray):
    """Row 0: Plush pile with knocked-out Floofy under it.
    Matches Floofy's canon look (LORE §10.10, assets/sprites/ow/floofy.png):
    flattened, swirly dizzy eyes (@ @), bent flower crown, smiling asleep.
    """
    y_off = 0
    draw_plush_mound_base(arr, y_off)

    # Floofy is squished right in the center (x: 14..27, y: 7..15)
    # Head outline
    head_outline = [
        (16, 8), (17, 7), (18, 7), (19, 7), (20, 7), (21, 7), (22, 7), (23, 8),
        (15, 9), (24, 9),
        (14, 10), (25, 10),
        (14, 11), (25, 11),
        (14, 12), (25, 12),
        (15, 13), (24, 13),
        (16, 14), (17, 14), (18, 14), (19, 14), (20, 14), (21, 14), (22, 14), (23, 14)
    ]
    for x, y in head_outline:
        set_px(arr, x, y_off + y, OUTLINE)

    # Floofy face fill (pastel pink fur)
    for y in range(8, 14):
        for x in range(15, 25):
            if (x, y) not in head_outline:
                set_px(arr, x, y_off + y, FLOOFY_BODY)

    # Floofy cheeks / shading
    for x in range(15, 25):
        set_px(arr, x, y_off + 13, FLOOFY_BODY_SHADE)
    set_px(arr, 15, y_off + 12, FLOOFY_BODY_SHADE)
    set_px(arr, 24, y_off + 12, FLOOFY_BODY_SHADE)

    # Snout (cream white)
    snout_pixels = [
        (18, 11), (19, 11), (20, 11), (21, 11),
        (18, 12), (19, 12), (20, 12), (21, 12),
        (19, 13), (20, 13)
    ]
    for x, y in snout_pixels:
        set_px(arr, x, y_off + y, FLOOFY_WHITE)

    # Black nose
    set_px(arr, 19, y_off + 11, OUTLINE)
    set_px(arr, 20, y_off + 11, OUTLINE)

    # Sleeping sweet smile / giggle ("He's out cold. He's smiling anyway.")
    set_px(arr, 18, y_off + 12, OUTLINE)
    set_px(arr, 21, y_off + 12, OUTLINE)
    set_px(arr, 19, y_off + 13, (0xE0, 0x7A, 0xAE, 255))  # cute pink mouth
    set_px(arr, 20, y_off + 13, (0xE0, 0x7A, 0xAE, 255))

    # SWIRLY DIZZY EYES: (@ @)
    # Left eye spiral (center at 16, 10):
    #  # #
    #  . #
    #  ###
    set_px(arr, 16, y_off + 9, OUTLINE)
    set_px(arr, 17, y_off + 9, OUTLINE)
    set_px(arr, 17, y_off + 10, OUTLINE)
    set_px(arr, 16, y_off + 11, OUTLINE)
    set_px(arr, 17, y_off + 11, OUTLINE)
    set_px(arr, 18, y_off + 10, OUTLINE)
    set_px(arr, 16, y_off + 10, FLOOFY_WHITE)  # inner white spiral gap

    # Right eye spiral (center at 22, 10):
    set_px(arr, 21, y_off + 9, OUTLINE)
    set_px(arr, 22, y_off + 9, OUTLINE)
    set_px(arr, 23, y_off + 10, OUTLINE)
    set_px(arr, 21, y_off + 11, OUTLINE)
    set_px(arr, 22, y_off + 11, OUTLINE)
    set_px(arr, 21, y_off + 10, OUTLINE)
    set_px(arr, 22, y_off + 10, FLOOFY_WHITE)

    # Flattened Fox Ears (splayed sideways into the plushies)
    # Left ear:
    set_px(arr, 14, y_off + 8, OUTLINE)
    set_px(arr, 13, y_off + 7, OUTLINE)
    set_px(arr, 12, y_off + 7, OUTLINE)
    set_px(arr, 11, y_off + 8, OUTLINE)
    set_px(arr, 12, y_off + 8, FLOOFY_BODY)
    set_px(arr, 13, y_off + 8, FLOOFY_WHITE)  # white ear tuft
    # Right ear:
    set_px(arr, 25, y_off + 8, OUTLINE)
    set_px(arr, 26, y_off + 7, OUTLINE)
    set_px(arr, 27, y_off + 7, OUTLINE)
    set_px(arr, 28, y_off + 8, OUTLINE)
    set_px(arr, 27, y_off + 8, FLOOFY_BODY)
    set_px(arr, 26, y_off + 8, FLOOFY_WHITE)

    # BENT FLOWER CROWN:
    # Sits askew/crushed across his brow! Yellow daisy + Red rose + Green leaves
    # Green vine stem askew
    for x in range(16, 24):
        set_px(arr, x, y_off + 7, FLOOFY_CROWN_GREEN)
    set_px(arr, 15, y_off + 7, FLOOFY_CROWN_DARK)
    set_px(arr, 24, y_off + 7, FLOOFY_CROWN_DARK)

    # Yellow flower (crushed/skewed on left brow x: 16..18, y: 5..6)
    set_px(arr, 16, y_off + 6, FLOOFY_CROWN_YELLOW)
    set_px(arr, 17, y_off + 6, FLOOFY_CROWN_YELLOW)
    set_px(arr, 17, y_off + 5, FLOOFY_CROWN_YELLOW)
    set_px(arr, 16, y_off + 5, OUTLINE)
    set_px(arr, 18, y_off + 6, (0xCA, 0x8A, 0x04, 255))  # yellow shading

    # Red rose / friendship petal flower (bent on right brow x: 21..24, y: 5..7)
    set_px(arr, 22, y_off + 6, FLOOFY_CROWN_RED)
    set_px(arr, 23, y_off + 6, FLOOFY_CROWN_RED)
    set_px(arr, 22, y_off + 5, FLOOFY_CROWN_RED_DARK)
    set_px(arr, 23, y_off + 5, FLOOFY_CROWN_RED)
    set_px(arr, 24, y_off + 6, FLOOFY_CROWN_RED_DARK)
    set_px(arr, 21, y_off + 6, FLOOFY_CROWN_DARK)

    # Paw poking out of plushies on bottom-left, holding buzzing flower petal ("phone"):
    set_px(arr, 12, y_off + 15, FLOOFY_BODY)
    set_px(arr, 13, y_off + 15, FLOOFY_WHITE)
    set_px(arr, 11, y_off + 16, FLOOFY_WHITE)
    set_px(arr, 12, y_off + 16, FLOOFY_WHITE)
    set_px(arr, 10, y_off + 16, FLOOFY_CROWN_RED)  # red petal tip in paw
    set_px(arr, 9, y_off + 15, (0x7C, 0xFF, 0x3A, 255))  # faint buzz particle


def draw_plush_pile_frame1_dent(arr: np.ndarray):
    """Row 1: Plush pile with fox-shaped dent.
    Floofy is gone. In the center is an unmistakable deep fox-shaped crater/hollow
    with dark indentation shading, compressed plush edges, stray pink fur wisps,
    and a fallen flower petal from his broken crown.
    """
    y_off = 20
    draw_plush_mound_base(arr, y_off)

    # The FOX-SHAPED DENT in the plushies (x: 14..27, y: 7..15)
    # The crater rim has pressed-in crumpled plush edges with deep shadows:
    # Outer dark shadow rim
    rim_pixels = [
        (16, 7), (17, 7), (18, 6), (19, 6), (20, 6), (21, 6), (22, 7), (23, 7),
        (15, 8), (24, 8),
        (14, 9), (25, 9),
        (13, 10), (26, 10),
        (13, 11), (26, 11),
        (14, 12), (25, 12),
        (15, 13), (24, 13),
        (16, 14), (17, 14), (22, 14), (23, 14),
        (18, 15), (19, 15), (20, 15), (21, 15)
    ]
    for x, y in rim_pixels:
        set_px(arr, x, y_off + y, OUTLINE)

    # Indentation cavity fill: deep plush shadow
    for y in range(8, 14):
        for x in range(15, 25):
            set_px(arr, x, y_off + y, (0x22, 0x18, 0x2E, 255))  # deep purple-black crater shadow

    # Fox ear indentations (two distinct triangular notches at top left and top right of dent)
    # Left ear dent notch
    set_px(arr, 13, y_off + 7, OUTLINE)
    set_px(arr, 12, y_off + 7, OUTLINE)
    set_px(arr, 13, y_off + 8, (0x2D, 0x1F, 0x3D, 255))
    set_px(arr, 14, y_off + 8, (0x22, 0x18, 0x2E, 255))
    # Right ear dent notch
    set_px(arr, 26, y_off + 7, OUTLINE)
    set_px(arr, 27, y_off + 7, OUTLINE)
    set_px(arr, 26, y_off + 8, (0x2D, 0x1F, 0x3D, 255))
    set_px(arr, 25, y_off + 8, (0x22, 0x18, 0x2E, 255))

    # Inner floor of the crater (fox silhouette contours)
    # The impression shows where his head and snout pressed down:
    for y in range(9, 13):
        for x in range(16, 24):
            set_px(arr, x, y_off + y, (0x2D, 0x1F, 0x3D, 255))
    for x in range(18, 22):
        set_px(arr, x, y_off + 12, (0x3B, 0x28, 0x4E, 255))  # snout depression
        set_px(arr, x, y_off + 13, (0x22, 0x18, 0x2E, 255))

    # Stray wisps of pink fur left behind inside the dent:
    set_px(arr, 16, y_off + 10, FLOOFY_BODY)
    set_px(arr, 17, y_off + 11, FLOOFY_BODY_SHADE)
    set_px(arr, 23, y_off + 10, FLOOFY_BODY)
    set_px(arr, 22, y_off + 11, FLOOFY_BODY_SHADE)
    set_px(arr, 19, y_off + 9, FLOOFY_WHITE)

    # A single fallen yellow flower petal from his broken crown:
    set_px(arr, 17, y_off + 8, FLOOFY_CROWN_YELLOW)
    set_px(arr, 18, y_off + 8, (0xCA, 0x8A, 0x04, 255))
    set_px(arr, 18, y_off + 9, FLOOFY_CROWN_GREEN)  # tiny bent green leaf bit


def build_plush_pile() -> Image.Image:
    arr = np.zeros((40, 40, 4), dtype=np.uint8)
    draw_plush_pile_frame0_floofy(arr)
    draw_plush_pile_frame1_dent(arr)
    return Image.fromarray(arr)


def get_plush_pile_json() -> dict:
    return {
        "image": "ruins_plush_pile.png",
        "cell": [40, 20],
        "origin": [20, 19],
        "anims": {
            "floofy": {"row": 0, "frames": 1, "fps": 1.0, "loop": false_bool()},
            "pile": {"row": 0, "frames": 1, "fps": 1.0, "loop": false_bool()},
            "knocked_out": {"row": 0, "frames": 1, "fps": 1.0, "loop": false_bool()},
            "dent": {"row": 1, "frames": 1, "fps": 1.0, "loop": false_bool()},
            "empty": {"row": 1, "frames": 1, "fps": 1.0, "loop": false_bool()},
            "bed": {"row": 1, "frames": 1, "fps": 1.0, "loop": false_bool()}
        }
    }


def false_bool():
    return False


# =============================================================================
# 2. CON FRONT DOORS (assets/props/ruins_con_doors.png)
# 60x60 native 1x facade prop:
# Brutalist convention portal, header "FURNATION CON '14", plaque "REGISTRATION ->",
# sunken reveal, heavy double push-bar doors with claw marks on the inside.
# =============================================================================

def build_con_doors() -> Image.Image:
    arr = np.zeros((60, 60, 4), dtype=np.uint8)

    # 1. Concrete portal frame:
    # Outer bounds: x = 2..57, y = 2..59
    # Cornice / Wall Cap (y = 2..8):
    for y in range(3, 8):
        for x in range(2, 58):
            set_px(arr, x, y, (0x68, 0x56, 0x78, 255))
    for x in range(2, 58):
        set_px(arr, x, 2, (0x8B, 0x78, 0x9D, 255))  # highlight bevel
        set_px(arr, x, 8, OUTLINE)  # shadow line under cornice
    set_px(arr, 1, 2, OUTLINE)
    set_px(arr, 58, 2, OUTLINE)

    # Concrete lintel block (y = 9..20)
    for y in range(9, 21):
        for x in range(4, 56):
            set_px(arr, x, y, (0x4D, 0x3D, 0x59, 255))
    for y in range(9, 21):
        set_px(arr, 3, y, OUTLINE)
        set_px(arr, 56, y, OUTLINE)

    # Weathered concrete texture & cracks on lintel
    set_px(arr, 12, 11, (0x36, 0x2A, 0x3F, 255))
    set_px(arr, 13, 12, (0x36, 0x2A, 0x3F, 255))
    set_px(arr, 47, 10, (0x36, 0x2A, 0x3F, 255))
    set_px(arr, 48, 11, (0x36, 0x2A, 0x3F, 255))
    set_px(arr, 48, 12, (0x36, 0x2A, 0x3F, 255))

    # Embossed Header: "FURNATION CON '14"
    # We carve it in 1x pixel lettering (dark carve with bottom highlight)
    # Lintel text y = 13..17
    # Letters: F U R N A T I O N   C O N   ' 1 4
    header_carve = [
        # F
        (7, 13), (7, 14), (7, 15), (7, 16), (7, 17), (8, 13), (9, 13), (8, 15),
        # U
        (11, 13), (11, 14), (11, 15), (11, 16), (12, 17), (13, 16), (13, 15), (13, 14), (13, 13),
        # R
        (15, 13), (15, 14), (15, 15), (15, 16), (15, 17), (16, 13), (17, 14), (16, 15), (17, 16), (17, 17),
        # N
        (19, 13), (19, 14), (19, 15), (19, 16), (19, 17), (20, 14), (20, 15), (21, 13), (21, 14), (21, 15), (21, 16), (21, 17),
        # C O N
        (24, 14), (24, 15), (24, 16), (25, 13), (26, 13), (25, 17), (26, 17),
        (28, 14), (28, 15), (28, 16), (29, 13), (30, 13), (29, 17), (30, 17), (31, 14), (31, 15), (31, 16),
        (33, 13), (33, 14), (33, 15), (33, 16), (33, 17), (34, 14), (35, 13), (35, 14), (35, 15), (35, 16), (35, 17),
        # ' 1 4
        (38, 12), (38, 13),
        (40, 14), (41, 13), (41, 14), (41, 15), (41, 16), (41, 17), (40, 17), (42, 17),
        (44, 13), (44, 14), (44, 15), (45, 15), (46, 13), (46, 14), (46, 15), (46, 16), (46, 17)
    ]
    for x, y in header_carve:
        set_px(arr, x, y, (0x23, 0x18, 0x2B, 255))
        set_px(arr, x, y + 1, (0x68, 0x56, 0x78, 255))  # embossed lip highlight

    # Sign Board: "REGISTRATION ->" (y = 20..27, centered x: 10..50)
    for y in range(21, 27):
        for x in range(11, 49):
            set_px(arr, x, y, (0x1F, 0x14, 0x2B, 255))  # dark acrylic sign plate
    for x in range(10, 50):
        set_px(arr, x, 20, OUTLINE)
        set_px(arr, x, 27, OUTLINE)
    for y in range(20, 28):
        set_px(arr, 10, y, OUTLINE)
        set_px(arr, 49, y, OUTLINE)

    # Directional sign text: "REGISTRATION ->" in neon green (#7CFF3A)
    # Compact pixel typography:
    # R
    for y in range(22, 26): set_px(arr, 13, y, CAN_GREEN_BRIGHT)
    set_px(arr, 14, 22, CAN_GREEN_BRIGHT); set_px(arr, 15, 23, CAN_GREEN_BRIGHT)
    set_px(arr, 14, 24, CAN_GREEN_BRIGHT); set_px(arr, 15, 25, CAN_GREEN_BRIGHT)
    # E
    for y in range(22, 26): set_px(arr, 17, y, CAN_GREEN_BRIGHT)
    set_px(arr, 18, 22, CAN_GREEN_BRIGHT); set_px(arr, 18, 24, CAN_GREEN_BRIGHT); set_px(arr, 18, 25, CAN_GREEN_BRIGHT)
    # G
    for y in range(23, 25): set_px(arr, 20, y, CAN_GREEN_BRIGHT)
    set_px(arr, 21, 22, CAN_GREEN_BRIGHT); set_px(arr, 22, 22, CAN_GREEN_BRIGHT)
    set_px(arr, 21, 25, CAN_GREEN_BRIGHT); set_px(arr, 22, 25, CAN_GREEN_BRIGHT); set_px(arr, 22, 24, CAN_GREEN_BRIGHT)
    # I
    for y in range(22, 26): set_px(arr, 24, y, CAN_GREEN_BRIGHT)
    # S
    set_px(arr, 26, 22, CAN_GREEN_BRIGHT); set_px(arr, 27, 22, CAN_GREEN_BRIGHT)
    set_px(arr, 26, 23, CAN_GREEN_BRIGHT); set_px(arr, 27, 24, CAN_GREEN_BRIGHT)
    set_px(arr, 26, 25, CAN_GREEN_BRIGHT); set_px(arr, 27, 25, CAN_GREEN_BRIGHT)
    # T
    set_px(arr, 29, 22, CAN_GREEN_BRIGHT); set_px(arr, 30, 22, CAN_GREEN_BRIGHT); set_px(arr, 31, 22, CAN_GREEN_BRIGHT)
    for y in range(23, 26): set_px(arr, 30, y, CAN_GREEN_BRIGHT)
    # R
    for y in range(22, 26): set_px(arr, 33, y, CAN_GREEN_BRIGHT)
    set_px(arr, 34, 22, CAN_GREEN_BRIGHT); set_px(arr, 34, 24, CAN_GREEN_BRIGHT); set_px(arr, 34, 25, CAN_GREEN_BRIGHT)
    # -> ARROW
    for x in range(38, 45): set_px(arr, x, 24, CAN_GREEN_BRIGHT)
    set_px(arr, 43, 23, CAN_GREEN_BRIGHT); set_px(arr, 42, 22, CAN_GREEN_BRIGHT)
    set_px(arr, 43, 25, CAN_GREEN_BRIGHT); set_px(arr, 42, 26, CAN_GREEN_BRIGHT)

    # 2. Sunken Reveal / Portal Recess (y = 28..58):
    # Left jamb: x = 6..12 angled in
    for y in range(28, 59):
        set_px(arr, 6, y, OUTLINE)
        set_px(arr, 7, y, (0x36, 0x2A, 0x3F, 255))
        set_px(arr, 8, y, (0x23, 0x18, 0x2B, 255))
        set_px(arr, 9, y, (0x17, 0x10, 0x20, 255))  # deep recess shadow
        set_px(arr, 10, y, OUTLINE)
    # Right jamb: x = 49..55 angled in
    for y in range(28, 59):
        set_px(arr, 49, y, OUTLINE)
        set_px(arr, 50, y, (0x17, 0x10, 0x20, 255))
        set_px(arr, 51, y, (0x23, 0x18, 0x2B, 255))
        set_px(arr, 52, y, (0x36, 0x2A, 0x3F, 255))
        set_px(arr, 53, y, OUTLINE)

    # Top recess shadow across top of doors
    for y in range(28, 31):
        for x in range(11, 49):
            set_px(arr, x, y, (0x10, 0x0A, 0x17, 255))

    # 3. Double Doors (x = 11..48, y = 31..58):
    # Left Door: x = 11..28
    # Right Door: x = 30..48
    # Center seam: x = 29
    for y in range(31, 58):
        set_px(arr, 29, y, OUTLINE)
        # Deep purple ambient glow leaking through seam
        if 35 <= y <= 50:
            set_px(arr, 29, y, (0x5E, 0x3A, 0x87, 255))

    # Left door body (heavy dark convention metal & reinforced glass)
    for y in range(31, 58):
        for x in range(11, 29):
            set_px(arr, x, y, (0x20, 0x17, 0x2A, 255))
    # Right door body
    for y in range(31, 58):
        for x in range(30, 49):
            set_px(arr, x, y, (0x20, 0x17, 0x2A, 255))

    # Glass panels on doors (x: 14..26 and 33..45, y: 33..50)
    for y in range(33, 51):
        for x in range(14, 27):
            set_px(arr, x, y, (0x16, 0x10, 0x20, 255))
        for x in range(33, 46):
            set_px(arr, x, y, (0x16, 0x10, 0x20, 255))
        # Subtle glass reflection diagonal sheen
        set_px(arr, 15 + (y - 33) // 2, y, (0x32, 0x24, 0x40, 255))
        set_px(arr, 34 + (y - 33) // 2, y, (0x32, 0x24, 0x40, 255))

    # Push Bars (horizontal industrial crash bars at waist height y = 47..48)
    for x in range(14, 27):
        set_px(arr, x, 47, (0xD2, 0xD6, 0xDC, 255))
        set_px(arr, x, 48, (0x64, 0x6A, 0x75, 255))
    for x in range(33, 46):
        set_px(arr, x, 47, (0xD2, 0xD6, 0xDC, 255))
        set_px(arr, x, 48, (0x64, 0x6A, 0x75, 255))
    # Push bar mounting brackets
    set_px(arr, 14, 46, OUTLINE); set_px(arr, 26, 46, OUTLINE)
    set_px(arr, 33, 46, OUTLINE); set_px(arr, 45, 46, OUTLINE)

    # 4. CLAW MARKS ("There are claw marks on the doors. On the inside.")
    # Desperate, frantic scratches scored diagonally across the glass and metal panels:
    # Left door claw rake (3 parallel diagonal gouges, bright scratch highlight with deep gouge shadow)
    claws_left = [
        # Claw 1
        [(15, 36), (16, 37), (17, 39), (18, 41), (19, 43)],
        # Claw 2
        [(17, 35), (18, 36), (19, 38), (20, 40), (21, 42)],
        # Claw 3
        [(19, 34), (20, 35), (21, 37), (22, 39), (23, 41)],
        # Low scrape
        [(22, 44), (23, 45), (24, 47), (25, 49)]
    ]
    for claw in claws_left:
        for x, y in claw:
            set_px(arr, x, y, (0xF4, 0xE3, 0xFF, 255))      # bright claw highlight
            set_px(arr, x + 1, y, (0x10, 0x06, 0x16, 255))  # deep gouge shadow

    # Right door crossed claw rake
    claws_right = [
        # Down-right rake
        [(34, 35), (35, 36), (36, 38), (37, 40), (38, 42)],
        [(36, 34), (37, 35), (38, 37), (39, 39), (40, 41)],
        [(38, 33), (39, 34), (40, 36), (41, 38), (42, 40)],
        # Crossed slash
        [(43, 35), (42, 36), (41, 38), (40, 40), (39, 42)],
        [(41, 34), (40, 35), (39, 37), (38, 39), (37, 41)]
    ]
    for claw in claws_right:
        for x, y in claw:
            set_px(arr, x, y, (0xF4, 0xE3, 0xFF, 255))
            set_px(arr, x + 1, y, (0x10, 0x06, 0x16, 255))

    # 5. Base Threshold Sill (y = 57..59)
    for x in range(6, 54):
        set_px(arr, x, 57, OUTLINE)
        set_px(arr, x, 58, (0x4D, 0x3D, 0x59, 255))
        set_px(arr, x, 59, (0x2D, 0x24, 0x35, 255))

    return Image.fromarray(arr)


# =============================================================================
# 3. LANDING TILESET (assets/tiles/landing.png)
# 160x160 native 1x tileset (8 columns x 8 rows of 20x20 tiles = 64 tiles):
# - can-compacted rock wall + wall cap
# - the layered nacho stratum in geological bands
# - trash floor (crushed cans, wrappers, crumbs)
# - cave rock floor (base floor, natural terrain)
# - light-beam overlay (semi-transparent golden light rays and pool)
# =============================================================================

def draw_tile(arr: np.ndarray, col: int, row: int, draw_fn):
    tile_arr = np.zeros((20, 20, 4), dtype=np.uint8)
    draw_fn(tile_arr)
    x0 = col * 20
    y0 = row * 20
    arr[y0:y0 + 20, x0:x0 + 20] = tile_arr


# --- Row 0: Base Cave Rock Floor Tiles (natural ground outside the Con) ---

def tile_floor_plain_1(t: np.ndarray):
    """Natural cave rock floor 1: slate dirt with subtle pebble texture."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    # Texture speckles
    for (x, y) in [(2, 3), (7, 8), (14, 2), (18, 12), (5, 16), (11, 14)]:
        set_px(t, x, y, ROCK_LIGHT)
    for (x, y) in [(3, 4), (8, 9), (15, 3), (19, 13), (6, 17), (12, 15)]:
        set_px(t, x, y, ROCK_DARK)
    for (x, y) in [(4, 11), (13, 7), (16, 17)]:
        set_px(t, x, y, ROCK_BRIGHT)


def tile_floor_plain_2(t: np.ndarray):
    """Natural cave rock floor 2: cracked earthy slate."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    # Small natural fissures
    cracks = [(4, 5), (5, 6), (6, 6), (7, 7), (8, 8), (12, 14), (13, 15), (14, 15), (15, 16)]
    for x, y in cracks:
        set_px(t, x, y, ROCK_DARKEST)
        set_px(t, x + 1, y, ROCK_LIGHT)
    set_px(t, 17, 4, ROCK_BRIGHT); set_px(t, 18, 5, ROCK_DARK)


def tile_floor_plain_3(t: np.ndarray):
    """Natural cave rock floor 3: gravelly dirt texture."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    for x in range(0, 20, 4):
        for y in range(0, 20, 4):
            if (x + y) % 8 == 0:
                set_px(t, x + 1, y + 1, ROCK_LIGHT)
                set_px(t, x + 2, y + 2, ROCK_DARK)


def tile_floor_plain_4(t: np.ndarray):
    """Natural cave rock floor 4: rough rocky slab."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    for x in range(2, 18):
        set_px(t, x, 6, ROCK_LIGHT)
        set_px(t, x, 7, ROCK_DARK)
    for y in range(7, 16):
        set_px(t, 9, y, ROCK_LIGHT)
        set_px(t, 10, y, ROCK_DARK)


def tile_floor_nacho_crumbs(t: np.ndarray):
    """Cave floor with fallen Doritos / nacho cheese crumbs."""
    tile_floor_plain_1(t)
    crumbs = [
        (4, 7, NACHO_BRIGHT), (5, 7, NACHO_MID), (5, 8, NACHO_DARK),
        (12, 11, NACHO_LIGHT), (13, 11, NACHO_BRIGHT),
        (8, 14, NACHO_MID), (9, 15, NACHO_DARK),
        (16, 4, NACHO_BRIGHT), (17, 5, NACHO_DARK)
    ]
    for x, y, c in crumbs:
        set_px(t, x, y, c)


def tile_floor_bottle_cap(t: np.ndarray):
    """Cave floor with crushed metal bottle cap."""
    tile_floor_plain_2(t)
    # Bottle cap at x: 8..11, y: 9..12
    set_px(t, 9, 9, CAN_GREEN_BRIGHT)
    set_px(t, 10, 9, CAN_GREEN_BRIGHT)
    set_px(t, 8, 10, CAN_GREEN_BRIGHT); set_px(t, 9, 10, CAN_GREEN_MID); set_px(t, 10, 10, CAN_GREEN_MID); set_px(t, 11, 10, CAN_GREEN_DARK)
    set_px(t, 8, 11, CAN_GREEN_DARK); set_px(t, 9, 11, CAN_GREEN_DARK); set_px(t, 10, 11, CAN_GREEN_DARK); set_px(t, 11, 11, OUTLINE)
    set_px(t, 9, 12, OUTLINE); set_px(t, 10, 12, OUTLINE)


def tile_floor_pull_tab(t: np.ndarray):
    """Cave floor with shiny silver aluminum pull tab ring."""
    tile_floor_plain_1(t)
    # Pull tab ring at x: 13..16, y: 6..9
    set_px(t, 14, 6, CAN_SILVER_BRIGHT); set_px(t, 15, 6, CAN_SILVER_BRIGHT)
    set_px(t, 13, 7, CAN_SILVER_BRIGHT); set_px(t, 16, 7, CAN_SILVER_DARK)
    set_px(t, 13, 8, CAN_SILVER_DARK); set_px(t, 16, 8, OUTLINE)
    set_px(t, 14, 9, CAN_SILVER_DARK); set_px(t, 15, 9, OUTLINE)
    set_px(t, 14, 7, ROCK_DARK); set_px(t, 15, 7, ROCK_DARK)  # ring hole


def tile_void_black(t: np.ndarray):
    """Empty void outside cave boundary (#141018)."""
    fill_rect(t, 0, 0, 20, 20, OUTLINE)


# --- Row 1: Trash Floor Tiles (crushed cans, wrappers, convention waste) ---

def tile_trash_crushed_citrus_can(t: np.ndarray):
    """Trash floor: crushed neon green Citrus Blast can flattened into rock."""
    tile_floor_plain_1(t)
    # Flattened can cylinder x: 5..15, y: 7..13
    for y in range(8, 13):
        for x in range(6, 15):
            set_px(t, x, y, CAN_GREEN_MID)
    # Top rim & highlight
    for x in range(7, 14):
        set_px(t, x, 8, CAN_GREEN_BRIGHT)
        set_px(t, x, 7, CAN_SILVER_BRIGHT)  # silver top rim
    # Bottom rim & crumpled folds
    for x in range(7, 14):
        set_px(t, x, 12, CAN_GREEN_DARK)
        set_px(t, x, 13, CAN_SILVER_DARK)
    # Crumpled dent lines
    set_px(t, 9, 9, CAN_GREEN_DARK); set_px(t, 10, 10, CAN_GREEN_DARK); set_px(t, 11, 10, CAN_GREEN_BRIGHT)
    set_px(t, 6, 8, OUTLINE); set_px(t, 5, 9, OUTLINE); set_px(t, 5, 10, OUTLINE); set_px(t, 5, 11, OUTLINE); set_px(t, 6, 12, OUTLINE)
    set_px(t, 14, 8, OUTLINE); set_px(t, 15, 9, OUTLINE); set_px(t, 15, 10, OUTLINE); set_px(t, 15, 11, OUTLINE); set_px(t, 14, 12, OUTLINE)


def tile_trash_dorito_wrapper(t: np.ndarray):
    """Trash floor: crumpled shiny Doritos snack bag."""
    tile_floor_plain_2(t)
    # Crumpled foil triangle bag x: 4..16, y: 6..14
    wrapper_pts = [
        (4, 12), (5, 11), (6, 9), (7, 8), (8, 7), (9, 7), (10, 8),
        (11, 9), (12, 10), (13, 11), (14, 12), (15, 13), (16, 14)
    ]
    for x, y in wrapper_pts:
        set_px(t, x, y, OUTLINE)
    for y in range(9, 14):
        for x in range(6, 15):
            set_px(t, x, y, (0xEA, 0x58, 0x0C, 255))  # red-orange Dorito bag
    # Shiny foil crinkles
    set_px(t, 8, 9, (0xFE, 0xE3, 0x88, 255))
    set_px(t, 9, 9, (0xFF, 0xFF, 0xFF, 255))
    set_px(t, 10, 10, (0xC2, 0x41, 0x0C, 255))
    set_px(t, 12, 11, (0xFE, 0xE3, 0x88, 255))
    # Fallen cheese chip beside bag
    set_px(t, 17, 8, NACHO_LIGHT); set_px(t, 18, 9, NACHO_BRIGHT); set_px(t, 17, 9, NACHO_MID)


def tile_trash_crushed_silver_can(t: np.ndarray):
    """Trash floor: crushed silver aluminum can + plastic shreds."""
    tile_floor_plain_3(t)
    # Crushed silver can x: 4..13, y: 9..15
    for y in range(10, 15):
        for x in range(5, 13):
            set_px(t, x, y, CAN_SILVER_MID)
    for x in range(6, 12):
        set_px(t, x, 10, CAN_SILVER_BRIGHT)
        set_px(t, x, 14, CAN_SILVER_DARK)
    set_px(t, 4, 11, OUTLINE); set_px(t, 4, 12, OUTLINE); set_px(t, 4, 13, OUTLINE)
    set_px(t, 13, 11, OUTLINE); set_px(t, 13, 12, OUTLINE); set_px(t, 13, 13, OUTLINE)
    # Plastic blue bottle ring nearby
    set_px(t, 15, 6, (0x38, 0xBD, 0xF8, 255)); set_px(t, 16, 6, (0x38, 0xBD, 0xF8, 255))
    set_px(t, 15, 7, (0x02, 0x84, 0xC7, 255)); set_px(t, 16, 7, (0x02, 0x84, 0xC7, 255))


def tile_trash_debris_mix(t: np.ndarray):
    """Trash floor: dense mix of crushed cans, bottle caps, and nacho crumbs."""
    tile_floor_plain_1(t)
    # Crushed purple can fragment x: 2..8, y: 4..9
    for y in range(5, 9):
        for x in range(3, 8):
            set_px(t, x, y, CAN_PURPLE_MID)
    set_px(t, 4, 5, CAN_PURPLE_BRIGHT); set_px(t, 6, 8, CAN_PURPLE_DARK)
    # Crushed green can fragment x: 11..17, y: 11..16
    for y in range(12, 16):
        for x in range(12, 17):
            set_px(t, x, y, CAN_GREEN_MID)
    set_px(t, 13, 12, CAN_GREEN_BRIGHT); set_px(t, 15, 15, CAN_GREEN_DARK)
    # Nacho crumbs scattered between them
    set_px(t, 9, 8, NACHO_BRIGHT); set_px(t, 10, 9, NACHO_MID); set_px(t, 8, 13, NACHO_LIGHT)


def tile_trash_crushed_purple_can(t: np.ndarray):
    """Trash floor: crushed grape soda can."""
    tile_floor_plain_2(t)
    for y in range(7, 13):
        for x in range(6, 14):
            set_px(t, x, y, CAN_PURPLE_MID)
    for x in range(7, 13):
        set_px(t, x, 7, CAN_PURPLE_BRIGHT)
        set_px(t, x, 12, CAN_PURPLE_DARK)
    set_px(t, 5, 9, OUTLINE); set_px(t, 14, 9, OUTLINE)


def tile_trash_six_pack_ring(t: np.ndarray):
    """Trash floor: plastic 6-pack can holder ring flattened in dirt."""
    tile_floor_plain_3(t)
    # Clear translucent plastic rings
    ring_color = (0xE2, 0xE8, 0xF0, 200)
    for (cx, cy) in [(6, 7), (13, 7), (6, 13), (13, 13)]:
        set_px(t, cx - 1, cy, ring_color)
        set_px(t, cx + 1, cy, ring_color)
        set_px(t, cx, cy - 1, ring_color)
        set_px(t, cx, cy + 1, ring_color)


def tile_trash_flyer_fragment(t: np.ndarray):
    """Trash floor: soggy 2014 convention flyer fragment and Dorito chip."""
    tile_floor_plain_1(t)
    # Laminated paper scrap
    for y in range(8, 13):
        for x in range(5, 12):
            set_px(t, x, y, (0xE8, 0xE2, 0xD5, 255))
    set_px(t, 6, 9, OUTLINE); set_px(t, 7, 9, OUTLINE); set_px(t, 8, 9, OUTLINE)  # text scribble
    set_px(t, 6, 11, OUTLINE); set_px(t, 7, 11, OUTLINE)
    # Dorito chip triangle
    set_px(t, 14, 13, NACHO_LIGHT)
    set_px(t, 15, 14, NACHO_BRIGHT); set_px(t, 14, 14, NACHO_MID)
    set_px(t, 16, 15, NACHO_DARK); set_px(t, 15, 15, NACHO_MID); set_px(t, 14, 15, NACHO_DARK)


def tile_trash_dense_heap(t: np.ndarray):
    """Dense trash heap: layers of cans, plastics, and snack debris."""
    tile_floor_plain_4(t)
    # Multi-can pile
    for y in range(4, 16):
        for x in range(3, 17):
            set_px(t, x, y, ROCK_DARK)
    # Cans embedded
    for x in range(4, 9): set_px(t, x, 6, CAN_GREEN_BRIGHT); set_px(t, x, 7, CAN_GREEN_MID)
    for x in range(11, 16): set_px(t, x, 8, CAN_SILVER_BRIGHT); set_px(t, x, 9, CAN_SILVER_MID)
    for x in range(6, 13): set_px(t, x, 12, CAN_PURPLE_MID); set_px(t, x, 13, CAN_PURPLE_DARK)
    for x in range(8, 12): set_px(t, x, 14, NACHO_BRIGHT)


# --- Row 2: Can-Compacted Rock Wall & Wall Cap (Cave walls made of compressed cans) ---

def tile_wall_cap_left(t: np.ndarray):
    """Wall cap / top-left corner: rocky ledge with crushed cans embedded."""
    fill_rect(t, 0, 0, 20, 20, OUTLINE)
    # Rocky ledge cap (y: 6..19, x: 6..19)
    for y in range(6, 20):
        for x in range(6, 20):
            set_px(t, x, y, ROCK_MID)
    # Highlight lip
    for x in range(7, 20): set_px(t, x, 6, ROCK_BRIGHT)
    for y in range(7, 20): set_px(t, 6, y, ROCK_BRIGHT)
    # Crushed neon green and silver cans embedded in ledge lip
    set_px(t, 10, 6, CAN_GREEN_BRIGHT); set_px(t, 11, 6, CAN_GREEN_MID)
    set_px(t, 15, 6, CAN_SILVER_BRIGHT); set_px(t, 16, 6, CAN_SILVER_MID)
    set_px(t, 6, 11, CAN_PURPLE_BRIGHT); set_px(t, 6, 12, CAN_PURPLE_MID)


def tile_wall_cap_mid(t: np.ndarray):
    """Wall cap / top-center: horizontal rocky ledge with compressed cans showing."""
    fill_rect(t, 0, 0, 20, 5, OUTLINE)
    fill_rect(t, 0, 5, 20, 20, ROCK_MID)
    for x in range(0, 20):
        set_px(t, x, 5, ROCK_BRIGHT)
        set_px(t, x, 6, ROCK_LIGHT)
    # Protruding crushed cans along the top edge
    set_px(t, 3, 5, CAN_GREEN_BRIGHT); set_px(t, 4, 5, CAN_GREEN_MID)
    set_px(t, 8, 5, CAN_SILVER_BRIGHT); set_px(t, 9, 5, CAN_SILVER_MID)
    set_px(t, 14, 5, CAN_PURPLE_BRIGHT); set_px(t, 15, 5, CAN_PURPLE_MID)
    set_px(t, 18, 5, CAN_GREEN_BRIGHT)


def tile_wall_cap_right(t: np.ndarray):
    """Wall cap / top-right corner."""
    fill_rect(t, 0, 0, 20, 20, OUTLINE)
    for y in range(6, 20):
        for x in range(0, 14):
            set_px(t, x, y, ROCK_MID)
    for x in range(0, 13): set_px(t, x, 6, ROCK_BRIGHT)
    for y in range(7, 20): set_px(t, 13, y, ROCK_DARKEST)
    set_px(t, 4, 6, CAN_GREEN_BRIGHT); set_px(t, 5, 6, CAN_GREEN_MID)
    set_px(t, 9, 6, CAN_SILVER_BRIGHT); set_px(t, 10, 6, CAN_SILVER_MID)


def tile_wall_face_left(t: np.ndarray):
    """Wall face left edge: vertical rock wall with embedded can cylinders."""
    fill_rect(t, 0, 0, 6, 20, OUTLINE)
    fill_rect(t, 6, 0, 20, 20, ROCK_MID)
    for y in range(0, 20):
        set_px(t, 6, y, ROCK_BRIGHT)
        set_px(t, 7, y, ROCK_LIGHT)
    # Embedded cans
    for y in range(4, 8):
        set_px(t, 11, y, CAN_GREEN_MID); set_px(t, 12, y, CAN_GREEN_BRIGHT)
    for y in range(12, 16):
        set_px(t, 9, y, CAN_SILVER_MID); set_px(t, 10, y, CAN_SILVER_BRIGHT)


def tile_wall_face_center(t: np.ndarray):
    """Wall face center: dense rock face packed with compressed soda cans (geological layers)."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    # Rock stratification lines
    for x in range(0, 20):
        set_px(t, x, 4, ROCK_DARK)
        set_px(t, x, 11, ROCK_DARK)
        set_px(t, x, 17, ROCK_DARK)

    # Compressed Green Citrus Blast can (x: 2..9, y: 6..9)
    for x in range(3, 9):
        set_px(t, x, 6, CAN_GREEN_BRIGHT)
        set_px(t, x, 7, CAN_GREEN_MID)
        set_px(t, x, 8, CAN_GREEN_DARK)
    set_px(t, 2, 7, OUTLINE); set_px(t, 9, 7, OUTLINE)

    # Compressed Silver Can (x: 11..18, y: 7..10)
    for x in range(12, 18):
        set_px(t, x, 7, CAN_SILVER_BRIGHT)
        set_px(t, x, 8, CAN_SILVER_MID)
        set_px(t, x, 9, CAN_SILVER_DARK)
    set_px(t, 11, 8, OUTLINE); set_px(t, 18, 8, OUTLINE)

    # Compressed Purple Can (x: 5..14, y: 13..16)
    for x in range(6, 14):
        set_px(t, x, 13, CAN_PURPLE_BRIGHT)
        set_px(t, x, 14, CAN_PURPLE_MID)
        set_px(t, x, 15, CAN_PURPLE_DARK)
    set_px(t, 5, 14, OUTLINE); set_px(t, 14, 14, OUTLINE)


def tile_wall_face_right(t: np.ndarray):
    """Wall face right edge."""
    fill_rect(t, 0, 0, 14, 20, ROCK_MID)
    fill_rect(t, 14, 0, 20, 20, OUTLINE)
    for y in range(0, 20):
        set_px(t, 13, y, ROCK_DARKEST)
        set_px(t, 12, y, ROCK_DARK)
    # Embedded cans
    for y in range(5, 9):
        set_px(t, 5, y, CAN_GREEN_MID); set_px(t, 6, y, CAN_GREEN_BRIGHT)
    for y in range(13, 17):
        set_px(t, 7, y, CAN_SILVER_MID); set_px(t, 8, y, CAN_SILVER_BRIGHT)


def tile_wall_base_shadow(t: np.ndarray):
    """Wall base bottom shadow: transition where rock wall meets dirt floor."""
    fill_rect(t, 0, 0, 20, 12, ROCK_MID)
    # Dark shadow gradient at base
    for x in range(0, 20):
        set_px(t, x, 12, ROCK_DARK)
        set_px(t, x, 13, ROCK_DARKEST)
        set_px(t, x, 14, OUTLINE)
    fill_rect(t, 0, 15, 20, 20, ROCK_DARK)
    # Crumpled cans protruding at the wall base
    for x in range(4, 9): set_px(t, x, 11, CAN_GREEN_MID); set_px(t, x, 12, CAN_GREEN_DARK)
    for x in range(12, 17): set_px(t, x, 11, CAN_SILVER_MID); set_px(t, x, 12, CAN_SILVER_DARK)


def tile_wall_face_heavy_cans(t: np.ndarray):
    """Wall face variant: extremely compacted strata of crushed aluminum cans."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    # Band of green cans (y: 2..5)
    for x in range(1, 19):
        set_px(t, x, 2, CAN_GREEN_BRIGHT)
        set_px(t, x, 3, CAN_GREEN_MID)
        set_px(t, x, 4, CAN_GREEN_DARK)
    # Band of silver cans (y: 7..11)
    for x in range(0, 20):
        set_px(t, x, 8, CAN_SILVER_BRIGHT)
        set_px(t, x, 9, CAN_SILVER_MID)
        set_px(t, x, 10, CAN_SILVER_DARK)
    # Band of red/purple cans (y: 13..17)
    for x in range(2, 18):
        set_px(t, x, 14, CAN_RED_BRIGHT)
        set_px(t, x, 15, CAN_RED_MID)
        set_px(t, x, 16, CAN_RED_DARK)


# --- Row 3: Layered Nacho Stratum Wall Tiles (The famous nacho cheese geological strata) ---

def tile_wall_nacho_top_transition(t: np.ndarray):
    """Wall face transitioning from dark rock into the nacho stratum."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    # Wavy sedimentary transition boundary
    for x in range(0, 20):
        w = int(np.sin(x * 0.4) * 2.0)
        set_px(t, x, 8 + w, NACHO_DEEP)
        set_px(t, x, 9 + w, NACHO_DARK)
        set_px(t, x, 10 + w, NACHO_MID)
        set_px(t, x, 11 + w, NACHO_BRIGHT)
    for y in range(12, 20):
        for x in range(0, 20):
            set_px(t, x, y, NACHO_MID)
    # Embedded crushed can in the nacho stratum
    for x in range(8, 14):
        set_px(t, x, 15, CAN_GREEN_BRIGHT); set_px(t, x, 16, CAN_GREEN_MID)


def tile_wall_nacho_solid_stratum(t: np.ndarray):
    """Solid geological nacho cheese sediment layer."""
    fill_rect(t, 0, 0, 20, 20, NACHO_MID)
    # Horizontal sediment bedding layers of cheddar dust
    for x in range(0, 20):
        set_px(t, x, 2, NACHO_BRIGHT)
        set_px(t, x, 3, NACHO_LIGHT)
        set_px(t, x, 4, NACHO_HIGHLIGHT)
        set_px(t, x, 5, NACHO_BRIGHT)
        set_px(t, x, 9, NACHO_DARK)
        set_px(t, x, 10, NACHO_DEEP)
        set_px(t, x, 13, NACHO_BRIGHT)
        set_px(t, x, 14, NACHO_LIGHT)
        set_px(t, x, 17, NACHO_DARK)
    # Crushed silver can compressed in the nacho stratum
    for x in range(5, 12):
        set_px(t, x, 7, CAN_SILVER_BRIGHT)
        set_px(t, x, 8, CAN_SILVER_MID)


def tile_wall_nacho_bottom_transition(t: np.ndarray):
    """Nacho stratum transitioning back down into compacted rock."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    fill_rect(t, 0, 0, 20, 10, NACHO_MID)
    for x in range(0, 20):
        set_px(t, x, 3, NACHO_LIGHT)
        set_px(t, x, 7, NACHO_DARK)
        w = int(np.cos(x * 0.5) * 2.0)
        set_px(t, x, 10 + w, NACHO_DEEP)
        set_px(t, x, 11 + w, ROCK_DARKEST)
    fill_rect(t, 0, 13, 20, 20, ROCK_MID)


def tile_wall_nacho_dual_strata(t: np.ndarray):
    """Wall with two distinct thin nacho strata separated by can rock."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    # Top nacho stratum (y: 2..5)
    for x in range(0, 20):
        set_px(t, x, 2, NACHO_DARK)
        set_px(t, x, 3, NACHO_BRIGHT)
        set_px(t, x, 4, NACHO_LIGHT)
        set_px(t, x, 5, NACHO_DARK)
    # Mid rock with compressed cans
    for x in range(4, 10): set_px(t, x, 8, CAN_GREEN_BRIGHT); set_px(t, x, 9, CAN_GREEN_MID)
    # Bottom nacho stratum (y: 13..16)
    for x in range(0, 20):
        set_px(t, x, 13, NACHO_DEEP)
        set_px(t, x, 14, NACHO_MID)
        set_px(t, x, 15, NACHO_BRIGHT)
        set_px(t, x, 16, NACHO_DARK)


def tile_wall_nacho_cap(t: np.ndarray):
    """Wall cap with nacho dust along the rim."""
    tile_wall_cap_mid(t)
    # Nacho dust coating on top ledge
    for x in range(2, 18, 3):
        set_px(t, x, 6, NACHO_BRIGHT)
        set_px(t, x + 1, 6, NACHO_LIGHT)


def tile_inner_corner_left(t: np.ndarray):
    """Inner corner left wall tile."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    for y in range(0, 20):
        set_px(t, 5, y, ROCK_DARKEST)
        set_px(t, 6, y, ROCK_BRIGHT)
    for x in range(0, 20):
        set_px(t, x, 8, NACHO_MID)
        set_px(t, x, 9, NACHO_BRIGHT)


def tile_inner_corner_right(t: np.ndarray):
    """Inner corner right wall tile."""
    fill_rect(t, 0, 0, 20, 20, ROCK_MID)
    for y in range(0, 20):
        set_px(t, 14, y, ROCK_DARKEST)
        set_px(t, 13, y, ROCK_BRIGHT)
    for x in range(0, 20):
        set_px(t, x, 8, NACHO_MID)
        set_px(t, x, 9, NACHO_BRIGHT)


def tile_wall_nacho_fissure(t: np.ndarray):
    """Wall with vertical crack oozing ancient nacho powder."""
    tile_wall_face_center(t)
    fissure = [(8, 2), (9, 3), (8, 4), (8, 5), (9, 6), (10, 7), (10, 8), (11, 9), (10, 10), (10, 11), (9, 12), (9, 13), (8, 14), (8, 15)]
    for x, y in fissure:
        set_px(t, x, y, OUTLINE)
        set_px(t, x + 1, y, NACHO_HIGHLIGHT)
        set_px(t, x + 2, y, NACHO_BRIGHT)


# --- Row 4: Light Beam Overlay Tiles (Translucent golden rays & floor splash) ---

def tile_beam_source(t: np.ndarray):
    """Light beam ceiling hole source: conical emergence of light from darkness above."""
    # Translucent cone: narrow at top (x: 8..11), expanding at bottom (x: 4..15)
    for y in range(0, 20):
        half_w = 2 + int(y * 0.45)
        for x in range(10 - half_w, 10 + half_w + 1):
            dist = abs(x - 10) / float(half_w + 1)
            alpha = int(140 * (1.0 - dist * 0.6))
            set_px(t, x, y, (0xFF, 0xFD, 0xE8, alpha))
    # Floating dust motes
    set_px(t, 9, 6, (0xFF, 0xFF, 0xFF, 220))
    set_px(t, 11, 14, (0xFF, 0xFF, 0xFF, 220))


def tile_beam_body_left(t: np.ndarray):
    """Light beam body - left angled edge ray."""
    # Diagonal ray passing through from top to bottom
    for y in range(0, 20):
        edge_x = int(6 + y * 0.25)
        for x in range(edge_x, 20):
            dist = (x - edge_x) / 14.0
            alpha = int(110 * min(1.0, dist * 1.5))
            set_px(t, x, y, (0xFF, 0xFD, 0xD0, alpha))
        set_px(t, edge_x, y, (0xFF, 0xFF, 0xF0, 130))
    # Dust mote
    set_px(t, 12, 8, (0xFF, 0xFF, 0xFF, 230))


def tile_beam_body_center(t: np.ndarray):
    """Light beam body - bright luminous center column."""
    for y in range(0, 20):
        for x in range(0, 20):
            dist = abs(x - 10) / 10.0
            alpha = int(130 * (1.0 - dist * 0.4))
            set_px(t, x, y, (0xFF, 0xFA, 0xDC, alpha))
    # Floating dust motes catching the sun
    set_px(t, 6, 4, (0xFF, 0xFF, 0xFF, 240))
    set_px(t, 14, 11, (0xFF, 0xFF, 0xFF, 240))
    set_px(t, 8, 17, (0xFF, 0xFF, 0xFF, 240))


def tile_beam_body_right(t: np.ndarray):
    """Light beam body - right angled edge ray."""
    for y in range(0, 20):
        edge_x = int(14 - y * 0.25)
        for x in range(0, edge_x):
            dist = (edge_x - x) / 14.0
            alpha = int(110 * min(1.0, dist * 1.5))
            set_px(t, x, y, (0xFF, 0xFD, 0xD0, alpha))
        set_px(t, edge_x, y, (0xFF, 0xFF, 0xF0, 130))
    set_px(t, 7, 13, (0xFF, 0xFF, 0xFF, 230))


def tile_beam_floor_pool_center(t: np.ndarray):
    """Light beam floor pool - warm circular glow on cave ground."""
    for y in range(0, 20):
        for x in range(0, 20):
            dx = (x - 10) / 10.0
            dy = (y - 10) / 8.0
            d = np.sqrt(dx * dx + dy * dy)
            if d < 1.0:
                alpha = int(120 * (1.0 - d))
                set_px(t, x, y, (0xFF, 0xF5, 0xB0, alpha))


def tile_beam_floor_pool_left(t: np.ndarray):
    """Light beam floor pool - left fade."""
    for y in range(0, 20):
        for x in range(0, 20):
            dx = (x - 20) / 15.0
            dy = (y - 10) / 8.0
            d = np.sqrt(dx * dx + dy * dy)
            if d < 1.0:
                alpha = int(100 * (1.0 - d))
                set_px(t, x, y, (0xFF, 0xF5, 0xB0, alpha))


def tile_beam_floor_pool_right(t: np.ndarray):
    """Light beam floor pool - right fade."""
    for y in range(0, 20):
        for x in range(0, 20):
            dx = (x - 0) / 15.0
            dy = (y - 10) / 8.0
            d = np.sqrt(dx * dx + dy * dy)
            if d < 1.0:
                alpha = int(100 * (1.0 - d))
                set_px(t, x, y, (0xFF, 0xF5, 0xB0, alpha))


def tile_beam_dust_motes(t: np.ndarray):
    """Floating dust motes / particles overlay."""
    motes = [
        (3, 4, 180), (7, 12, 220), (14, 6, 240), (16, 15, 200),
        (10, 9, 255), (11, 17, 190), (4, 16, 170)
    ]
    for x, y, a in motes:
        set_px(t, x, y, (0xFF, 0xFF, 0xFF, a))
        set_px(t, x + 1, y, (0xFF, 0xFD, 0xD0, a // 2))


# --- Rows 5-7: Additional Structural, Threshold & Decal Tiles ---

def tile_outer_corner_wall_left(t: np.ndarray):
    """Outer corner wall top-left."""
    fill_rect(t, 0, 0, 20, 20, OUTLINE)
    fill_rect(t, 0, 0, 14, 14, ROCK_MID)
    for x in range(0, 14): set_px(t, x, 0, ROCK_BRIGHT)
    for y in range(0, 14): set_px(t, 0, y, ROCK_BRIGHT)
    for x in range(0, 14): set_px(t, x, 14, OUTLINE)
    for y in range(0, 14): set_px(t, 14, y, OUTLINE)


def tile_outer_corner_wall_right(t: np.ndarray):
    """Outer corner wall top-right."""
    fill_rect(t, 0, 0, 20, 20, OUTLINE)
    fill_rect(t, 6, 0, 20, 14, ROCK_MID)
    for x in range(6, 20): set_px(t, x, 0, ROCK_BRIGHT)
    for y in range(0, 14): set_px(t, 19, y, ROCK_DARKEST)
    for x in range(6, 20): set_px(t, x, 14, OUTLINE)
    for y in range(0, 14): set_px(t, 5, y, OUTLINE)


def tile_pillar_top(t: np.ndarray):
    """Cave rock pillar top."""
    fill_rect(t, 0, 0, 20, 20, OUTLINE)
    fill_rect(t, 4, 4, 16, 20, ROCK_MID)
    for x in range(4, 16): set_px(t, x, 4, ROCK_BRIGHT)
    set_px(t, 7, 6, CAN_GREEN_BRIGHT); set_px(t, 8, 6, CAN_GREEN_MID)


def tile_pillar_mid(t: np.ndarray):
    """Cave rock pillar mid with compressed cans."""
    fill_rect(t, 0, 0, 20, 20, OUTLINE)
    fill_rect(t, 4, 0, 16, 20, ROCK_MID)
    for y in range(0, 20):
        set_px(t, 4, y, ROCK_BRIGHT)
        set_px(t, 15, y, ROCK_DARKEST)
    # Compressed can in pillar
    for x in range(6, 14): set_px(t, x, 8, CAN_SILVER_BRIGHT); set_px(t, x, 9, CAN_SILVER_MID)


def tile_pillar_base(t: np.ndarray):
    """Cave rock pillar base meeting dirt floor."""
    fill_rect(t, 0, 0, 20, 20, OUTLINE)
    fill_rect(t, 3, 0, 17, 14, ROCK_MID)
    for x in range(3, 17): set_px(t, x, 14, OUTLINE)
    fill_rect(t, 1, 15, 19, 20, ROCK_DARK)


def tile_threshold_concrete_1(t: np.ndarray):
    """Cracked concrete threshold leading up to the Con entrance."""
    fill_rect(t, 0, 0, 20, 20, (0x4D, 0x3D, 0x59, 255))
    # Paver seams
    for x in range(0, 20): set_px(t, x, 10, (0x2D, 0x24, 0x35, 255))
    for y in range(0, 20): set_px(t, 10, y, (0x2D, 0x24, 0x35, 255))
    # Cracks
    set_px(t, 4, 3, (0x20, 0x18, 0x26, 255)); set_px(t, 5, 4, (0x20, 0x18, 0x26, 255))
    set_px(t, 6, 4, (0x68, 0x56, 0x78, 255))


def tile_threshold_concrete_2(t: np.ndarray):
    """Concrete threshold with dirt stains."""
    tile_threshold_concrete_1(t)
    # Dirt and nacho stains
    set_px(t, 14, 14, NACHO_MID); set_px(t, 15, 14, NACHO_BRIGHT)
    set_px(t, 3, 16, ROCK_DARK); set_px(t, 4, 16, ROCK_DARK)


def tile_vintage_logo_fragment(t: np.ndarray):
    """Ancient crushed Citrus Blast 2014 logo embedded in cave rock wall."""
    tile_wall_face_center(t)
    # "CITRUS" retro cursive lettering in neon green on wall
    logo = [(5, 8), (6, 7), (7, 7), (6, 9), (7, 9), (9, 7), (9, 8), (9, 9), (11, 7), (12, 7), (13, 8), (14, 9)]
    for x, y in logo:
        set_px(t, x, y, CAN_GREEN_BRIGHT)


def tile_decal_citrus_can(t: np.ndarray):
    """Standalone neon green Citrus Blast can decal (transparent bg)."""
    # Empty transparent bg
    for y in range(7, 14):
        for x in range(6, 14):
            set_px(t, x, y, CAN_GREEN_MID)
    for x in range(7, 13):
        set_px(t, x, 7, CAN_SILVER_BRIGHT)
        set_px(t, x, 8, CAN_GREEN_BRIGHT)
        set_px(t, x, 13, CAN_SILVER_DARK)
    set_px(t, 5, 9, OUTLINE); set_px(t, 5, 10, OUTLINE); set_px(t, 5, 11, OUTLINE)
    set_px(t, 14, 9, OUTLINE); set_px(t, 14, 10, OUTLINE); set_px(t, 14, 11, OUTLINE)


def tile_decal_silver_can(t: np.ndarray):
    """Standalone silver diet can decal (transparent bg)."""
    for y in range(7, 14):
        for x in range(6, 14):
            set_px(t, x, y, CAN_SILVER_MID)
    for x in range(7, 13):
        set_px(t, x, 7, CAN_SILVER_BRIGHT)
        set_px(t, x, 13, CAN_SILVER_DARK)
    set_px(t, 5, 10, OUTLINE); set_px(t, 14, 10, OUTLINE)


def tile_decal_dorito_chip(t: np.ndarray):
    """Standalone Dorito chip triangle decal (transparent bg)."""
    set_px(t, 10, 6, NACHO_LIGHT)
    set_px(t, 9, 7, NACHO_LIGHT); set_px(t, 10, 7, NACHO_BRIGHT); set_px(t, 11, 7, NACHO_MID)
    for x in range(8, 13): set_px(t, x, 8, NACHO_BRIGHT)
    for x in range(7, 14): set_px(t, x, 9, NACHO_MID)
    for x in range(6, 15): set_px(t, x, 10, NACHO_DARK)
    for x in range(7, 14): set_px(t, x, 11, OUTLINE)


def build_landing_tileset() -> Image.Image:
    arr = np.zeros((160, 160, 4), dtype=np.uint8)

    # Row 0: Base Cave Rock Floor Tiles
    draw_tile(arr, 0, 0, tile_floor_plain_1)
    draw_tile(arr, 1, 0, tile_floor_plain_2)
    draw_tile(arr, 2, 0, tile_floor_plain_3)
    draw_tile(arr, 3, 0, tile_floor_plain_4)
    draw_tile(arr, 4, 0, tile_floor_nacho_crumbs)
    draw_tile(arr, 5, 0, tile_floor_bottle_cap)
    draw_tile(arr, 6, 0, tile_floor_pull_tab)
    draw_tile(arr, 7, 0, tile_void_black)

    # Row 1: Trash Floor Tiles
    draw_tile(arr, 0, 1, tile_trash_crushed_citrus_can)
    draw_tile(arr, 1, 1, tile_trash_dorito_wrapper)
    draw_tile(arr, 2, 1, tile_trash_crushed_silver_can)
    draw_tile(arr, 3, 1, tile_trash_debris_mix)
    draw_tile(arr, 4, 1, tile_trash_crushed_purple_can)
    draw_tile(arr, 5, 1, tile_trash_six_pack_ring)
    draw_tile(arr, 6, 1, tile_trash_flyer_fragment)
    draw_tile(arr, 7, 1, tile_trash_dense_heap)

    # Row 2: Can-Compacted Rock Wall
    draw_tile(arr, 0, 2, tile_wall_cap_left)
    draw_tile(arr, 1, 2, tile_wall_cap_mid)
    draw_tile(arr, 2, 2, tile_wall_cap_right)
    draw_tile(arr, 3, 2, tile_wall_face_left)
    draw_tile(arr, 4, 2, tile_wall_face_center)
    draw_tile(arr, 5, 2, tile_wall_face_right)
    draw_tile(arr, 6, 2, tile_wall_base_shadow)
    draw_tile(arr, 7, 2, tile_wall_face_heavy_cans)

    # Row 3: Layered Nacho Stratum Wall
    draw_tile(arr, 0, 3, tile_wall_nacho_top_transition)
    draw_tile(arr, 1, 3, tile_wall_nacho_solid_stratum)
    draw_tile(arr, 2, 3, tile_wall_nacho_bottom_transition)
    draw_tile(arr, 3, 3, tile_wall_nacho_dual_strata)
    draw_tile(arr, 4, 3, tile_wall_nacho_cap)
    draw_tile(arr, 5, 3, tile_inner_corner_left)
    draw_tile(arr, 6, 3, tile_inner_corner_right)
    draw_tile(arr, 7, 3, tile_wall_nacho_fissure)

    # Row 4: Light Beam Overlay
    draw_tile(arr, 0, 4, tile_beam_source)
    draw_tile(arr, 1, 4, tile_beam_body_left)
    draw_tile(arr, 2, 4, tile_beam_body_center)
    draw_tile(arr, 3, 4, tile_beam_body_right)
    draw_tile(arr, 4, 4, tile_beam_floor_pool_center)
    draw_tile(arr, 5, 4, tile_beam_floor_pool_left)
    draw_tile(arr, 6, 4, tile_beam_floor_pool_right)
    draw_tile(arr, 7, 4, tile_beam_dust_motes)

    # Row 5: Structural & Corners
    draw_tile(arr, 0, 5, tile_outer_corner_wall_left)
    draw_tile(arr, 1, 5, tile_outer_corner_wall_right)
    draw_tile(arr, 2, 5, tile_pillar_top)
    draw_tile(arr, 3, 5, tile_pillar_mid)
    draw_tile(arr, 4, 5, tile_pillar_base)
    draw_tile(arr, 5, 5, tile_threshold_concrete_1)
    draw_tile(arr, 6, 5, tile_threshold_concrete_2)
    draw_tile(arr, 7, 5, tile_vintage_logo_fragment)

    # Row 6: Transitions & Floor variants
    draw_tile(arr, 0, 6, tile_floor_plain_1)
    draw_tile(arr, 1, 6, tile_floor_plain_2)
    draw_tile(arr, 2, 6, tile_floor_plain_3)
    draw_tile(arr, 3, 6, tile_trash_crushed_citrus_can)
    draw_tile(arr, 4, 6, tile_trash_dorito_wrapper)
    draw_tile(arr, 5, 6, tile_trash_crushed_silver_can)
    draw_tile(arr, 6, 6, tile_threshold_concrete_1)
    draw_tile(arr, 7, 6, tile_threshold_concrete_2)

    # Row 7: Decals & Transparent
    draw_tile(arr, 0, 7, tile_decal_citrus_can)
    draw_tile(arr, 1, 7, tile_decal_silver_can)
    draw_tile(arr, 2, 7, tile_decal_dorito_chip)
    draw_tile(arr, 3, 7, tile_beam_floor_pool_center)
    draw_tile(arr, 4, 7, tile_beam_dust_motes)
    draw_tile(arr, 5, 7, tile_floor_nacho_crumbs)
    draw_tile(arr, 6, 7, tile_floor_bottle_cap)
    # col 7, row 7 is transparent empty
    return Image.fromarray(arr)


# =============================================================================
# 4. PREVIEW GENERATOR (checks at 1x and 2x in tmp/)
# =============================================================================

def render_preview(landing_tiles: Image.Image, con_doors: Image.Image, plush_pile: Image.Image, out_path_1x: str, out_path_2x: str):
    """Renders a full scene mockup of R1 The Landing cave at 1x (320x240) and 2x (640x480)."""
    # 16x12 tiles (320x240 px native 1x)
    canvas = Image.new("RGBA", (320, 240), OUTLINE)

    # Slice tiles from landing_tiles
    def get_tile(col: int, row: int) -> Image.Image:
        return landing_tiles.crop((col * 20, row * 20, col * 20 + 20, row * 20 + 20))

    t_floor1 = get_tile(0, 0)
    t_floor2 = get_tile(1, 0)
    t_trash_can = get_tile(0, 1)
    t_trash_bag = get_tile(1, 1)
    t_trash_silver = get_tile(2, 1)

    t_wall_cap_l = get_tile(0, 2)
    t_wall_cap_m = get_tile(1, 2)
    t_wall_cap_r = get_tile(2, 2)
    t_wall_mid = get_tile(4, 2)
    t_wall_nacho = get_tile(1, 3)
    t_wall_base = get_tile(6, 2)

    t_beam_src = get_tile(0, 4)
    t_beam_l = get_tile(1, 4)
    t_beam_c = get_tile(2, 4)
    t_beam_r = get_tile(3, 4)
    t_beam_pool = get_tile(4, 4)

    # 1. Fill ground (rows 3..10, cols 1..14)
    for ry in range(3, 11):
        for rx in range(1, 15):
            tf = t_floor1 if (rx + ry) % 2 == 0 else t_floor2
            canvas.paste(tf, (rx * 20, ry * 20))

    # Scatter trash on floor
    canvas.paste(t_trash_can, (3 * 20, 6 * 20), t_trash_can)
    canvas.paste(t_trash_bag, (11 * 20, 7 * 20), t_trash_bag)
    canvas.paste(t_trash_silver, (12 * 20, 5 * 20), t_trash_silver)

    # 2. Build North cave wall with the nacho stratum
    # North wall is row 1..3, cols 1..6 and 10..14 (leaving cols 7..9 for Con doors)
    for rx in list(range(1, 7)) + list(range(10, 15)):
        canvas.paste(t_wall_cap_m, (rx * 20, 1 * 20))
        canvas.paste(t_wall_nacho, (rx * 20, 2 * 20))
        canvas.paste(t_wall_base, (rx * 20, 3 * 20))

    # Side walls
    for ry in range(2, 11):
        canvas.paste(t_wall_mid, (0 * 20, ry * 20))
        canvas.paste(t_wall_mid, (15 * 20, ry * 20))

    # 3. Paste Con front doors at north entrance (x: 130..190, y: 15..75)
    # Perfectly fits the center gap between x=140 and x=180
    canvas.paste(con_doors, (130, 15), con_doors)

    # 4. Light beam overlay from hole above
    # Beam enters at ceiling (x: 150, y: 0) down to plush pile at (x: 140, y: 120)
    canvas.paste(t_beam_src, (150, 0), t_beam_src)
    for by in range(1, 6):
        canvas.paste(t_beam_l, (130, by * 20), t_beam_l)
        canvas.paste(t_beam_c, (150, by * 20), t_beam_c)
        canvas.paste(t_beam_r, (170, by * 20), t_beam_r)
    canvas.paste(t_beam_pool, (150, 6 * 20), t_beam_pool)

    # 5. Paste Plush Pile under the light beam
    # State 1 (knocked out Floofy): row 0 of plush_pile (40x20)
    pile_floofy = plush_pile.crop((0, 0, 40, 20))
    canvas.paste(pile_floofy, (140, 130), pile_floofy)

    # Also show State 2 (fox dent) beside it for side-by-side verification
    pile_dent = plush_pile.crop((0, 20, 40, 40))
    canvas.paste(pile_dent, (60, 150), pile_dent)

    # Save 1x
    canvas.save(out_path_1x)
    print(f"Saved 1x preview to {out_path_1x} ({canvas.size})")

    # Save 2x (nearest-neighbor scaled for 640x480)
    canvas_2x = canvas.resize((canvas.width * 2, canvas.height * 2), Image.Resampling.NEAREST)
    canvas_2x.save(out_path_2x)
    print(f"Saved 2x preview to {out_path_2x} ({canvas_2x.size})")


def main():
    print("Generating Landing art assets (T03 / ART-183 #13-15)...")

    # 1. Generate plush pile
    plush_pile = build_plush_pile()
    plush_pile_path = os.path.join(REPO_ROOT, "assets", "props", "ruins_plush_pile.png")
    os.makedirs(os.path.dirname(plush_pile_path), exist_ok=True)
    plush_pile.save(plush_pile_path)
    print(f"Saved {plush_pile_path} ({plush_pile.size})")

    # 2. Write plush pile JSON
    plush_pile_json = get_plush_pile_json()
    plush_json_path = os.path.join(REPO_ROOT, "assets", "props", "ruins_plush_pile.json")
    with open(plush_json_path, "w", encoding="utf-8") as f:
        json.dump(plush_pile_json, f, indent=2)
        f.write("\n")
    print(f"Saved {plush_json_path}")

    # 3. Generate Con front doors
    con_doors = build_con_doors()
    con_doors_path = os.path.join(REPO_ROOT, "assets", "props", "ruins_con_doors.png")
    con_doors.save(con_doors_path)
    print(f"Saved {con_doors_path} ({con_doors.size})")

    # 4. Generate Landing tileset
    landing_tiles = build_landing_tileset()
    tiles_path = os.path.join(REPO_ROOT, "assets", "tiles", "landing.png")
    os.makedirs(os.path.dirname(tiles_path), exist_ok=True)
    landing_tiles.save(tiles_path)
    print(f"Saved {tiles_path} ({landing_tiles.size})")

    # 5. Generate 1x and 2x previews in tmp/
    tmp_dir = os.path.join(REPO_ROOT, "tmp")
    os.makedirs(tmp_dir, exist_ok=True)
    preview_1x = os.path.join(tmp_dir, "landing_preview_1x.png")
    preview_2x = os.path.join(tmp_dir, "landing_preview_2x.png")
    render_preview(landing_tiles, con_doors, plush_pile, preview_1x, preview_2x)
    print("Art generation complete!")


if __name__ == "__main__":
    main()
