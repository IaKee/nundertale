#!/usr/bin/env python3
"""Build Floofy Bass Shake battle sprites for ART-183 #3-4.

Generates:
  - assets/sprites/battle/floofy_close.png (360x480, 4 expressions x 3 lean columns)
  - assets/sprites/battle/floofy_close.json
  - assets/sprites/battle/floofy_wide.png (144x128, 3 sway frames + hurt frame)
  - assets/sprites/battle/floofy_wide.json

Meets Undertale battle sprite conventions:
  - Pure white mass + crisp black keylines/details on transparent background
  - 2px opaque black keyline surrounding the silhouette
  - 0 white pixels touching the outer edge of cells
"""
from __future__ import annotations

import json
import math
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[3]
CLOSE_PNG = ROOT / "assets/sprites/battle/floofy_close.png"
CLOSE_JSON = ROOT / "assets/sprites/battle/floofy_close.json"
WIDE_PNG = ROOT / "assets/sprites/battle/floofy_wide.png"
WIDE_JSON = ROOT / "assets/sprites/battle/floofy_wide.json"

WHITE = (255, 255, 255, 255)
BLACK = (0, 0, 0, 255)
CLEAR = (0, 0, 0, 0)


def rot_pt(x: float, y: float, cx: float, cy: float, angle_deg: float) -> tuple[int, int]:
    """Rotate a point (x, y) around (cx, cy) by angle_deg (clockwise in image coords)."""
    rad = math.radians(angle_deg)
    cos_a = math.cos(rad)
    sin_a = math.sin(rad)
    nx = cx + (x - cx) * cos_a - (y - cy) * sin_a
    ny = cy + (x - cx) * sin_a + (y - cy) * cos_a
    return int(round(nx)), int(round(ny))


def enforce_keyline(cell: Image.Image, keyline: int = 2) -> Image.Image:
    """Enforce 1px white silhouette + 2px black keyline on black/transparent background."""
    arr = np.array(cell.convert("RGBA"))
    opaque = arr[..., 3] >= 128
    if not opaque.any():
        return cell

    rgb = arr[..., :3].astype(int)
    lum = rgb.sum(axis=2)
    white = opaque & (lum >= 400)
    black = opaque & ~white

    n8 = [(0, 1), (0, -1), (1, 0), (-1, 0), (1, 1), (1, -1), (-1, 1), (-1, -1)]
    h, w = opaque.shape

    grown = opaque.copy()
    for _ in range(keyline):
        nxt = grown.copy()
        for dy, dx in n8:
            ys, yd = (slice(0, h - dy), slice(dy, h)) if dy >= 0 else (slice(-dy, h), slice(0, h + dy))
            xs, xd = (slice(0, w - dx), slice(dx, w)) if dx >= 0 else (slice(-dx, w), slice(0, w + dx))
            shifted = np.zeros_like(grown)
            shifted[yd, xd] = grown[ys, xs]
            nxt |= shifted
        grown = nxt

    inner = opaque.copy()
    for dy, dx in [(0, 1), (0, -1), (1, 0), (-1, 0)]:
        ys, yd = (slice(0, h - dy), slice(dy, h)) if dy >= 0 else (slice(-dy, h), slice(0, h + dy))
        xs, xd = (slice(0, w - dx), slice(dx, w)) if dx >= 0 else (slice(-dx, w), slice(0, w + dx))
        shifted = np.zeros_like(inner)
        shifted[yd, xd] = inner[ys, xs]
        inner &= shifted
    ring = opaque & ~inner

    out = np.zeros_like(arr)
    out[grown] = np.array(BLACK, np.uint8)
    out[opaque] = np.array(BLACK, np.uint8)
    out[white] = np.array(WHITE, np.uint8)
    out[ring] = np.array(WHITE, np.uint8)
    out[black & ~ring] = np.array(BLACK, np.uint8)

    # Clean borders
    out[0, :] = np.where(out[0, :, 3:] > 0, np.array(BLACK, np.uint8), out[0, :])
    out[-1, :] = np.where(out[-1, :, 3:] > 0, np.array(BLACK, np.uint8), out[-1, :])
    out[:, 0] = np.where(out[:, 0, 3:] > 0, np.array(BLACK, np.uint8), out[:, 0])
    out[:, -1] = np.where(out[:, -1, 3:] > 0, np.array(BLACK, np.uint8), out[:, -1])

    return Image.fromarray(out)


# ---------------------------------------------------------------------------
# Close-up drawing (~120x120)
# ---------------------------------------------------------------------------

def draw_rose(draw: ImageDraw.ImageDraw, cx: int, cy: int, r: int, whip_dx: int = 0) -> None:
    """Draw a stylized white rose with black spiral petals."""
    draw.ellipse((cx - r, cy - r, cx + r, cy + r), fill=WHITE, outline=BLACK, width=1)
    if r >= 6:
        draw.arc((cx - r + 2, cy - r + 2, cx + r - 2, cy + r - 2), 20, 220, fill=BLACK, width=1)
        draw.arc((cx - r + 3 + whip_dx, cy - r + 3, cx + r - 1 + whip_dx, cy + r - 1), 150, 350, fill=BLACK, width=1)
        draw.ellipse((cx - 1 + whip_dx, cy - 1, cx + 2 + whip_dx, cy + 2), fill=BLACK)
    elif r >= 4:
        draw.arc((cx - r + 1, cy - r + 1, cx + r - 1, cy + r - 1), 30, 240, fill=BLACK, width=1)
        draw.point((cx + whip_dx, cy), fill=BLACK)


def draw_leaf(draw: ImageDraw.ImageDraw, x1: int, y1: int, x2: int, y2: int) -> None:
    """Draw a leaf pointing from (x1, y1) to (x2, y2)."""
    mx = (x1 + x2) // 2
    my = (y1 + y2) // 2
    dx = (y2 - y1) // 3
    dy = -(x2 - x1) // 3
    poly = [(x1, y1), (mx + dx, my + dy), (x2, y2), (mx - dx, my - dy)]
    draw.polygon(poly, fill=WHITE, outline=BLACK)
    draw.line((x1, y1, x2, y2), fill=BLACK, width=1)


def draw_floating_petal(draw: ImageDraw.ImageDraw, x: int, y: int, angle_deg: float) -> None:
    """Draw a single flying rose petal."""
    p1 = rot_pt(x - 4, y - 2, x, y, angle_deg)
    p2 = rot_pt(x + 4, y - 1, x, y, angle_deg)
    p3 = rot_pt(x + 3, y + 3, x, y, angle_deg)
    p4 = rot_pt(x - 3, y + 2, x, y, angle_deg)
    draw.polygon([p1, p2, p3, p4], fill=WHITE, outline=BLACK)
    draw.line([p1, p3], fill=BLACK, width=1)


def render_closeup_cell(expression: str, column: int) -> Image.Image:
    """Render a single 120x120 cell for Floofy close-up.

    column 0: Lean Left  (angle -11 deg, ear tufts/petals whipping to Right)
    column 1: Center     (angle 0 deg)
    column 2: Lean Right (angle +11 deg, ear tufts/petals whipping to Left)
    """
    im = Image.new("RGBA", (120, 120), CLEAR)
    draw = ImageDraw.Draw(im)

    angle = -11.0 if column == 0 else (11.0 if column == 2 else 0.0)
    whip = 1 if column == 0 else (-1 if column == 2 else 0)
    whip_dx = 8 * whip
    cx_head = 60 + (-5 if column == 0 else (5 if column == 2 else 0))
    cy_head = 64

    def R(x: float, y: float) -> tuple[int, int]:
        return rot_pt(x + (cx_head - 60), y, cx_head, cy_head, angle)

    # 1. Fluffy Chest Ruff / Neck (seamlessly connecting head to body)
    chest_pts = [
        R(34, 82), R(26, 94), R(30, 106), R(40, 115),
        R(52, 117), R(60, 118), R(68, 117), R(80, 115),
        R(90, 106), R(94, 94), R(86, 82)
    ]
    draw.polygon(chest_pts, fill=WHITE, outline=BLACK)
    # Scalloped chest fur layers
    draw.line([R(44, 96), R(60, 108), R(76, 96)], fill=BLACK, width=1)
    draw.line([R(50, 102), R(60, 113), R(70, 102)], fill=BLACK, width=1)
    draw.line([R(36, 98), R(46, 106)], fill=BLACK, width=1)
    draw.line([R(84, 98), R(74, 106)], fill=BLACK, width=1)

    # 2. Paws (held up at chest level)
    # Left paw
    lp_pts = [R(24, 98), R(20, 106), R(26, 113), R(36, 110), R(34, 100)]
    draw.polygon(lp_pts, fill=WHITE, outline=BLACK)
    draw.line([R(25, 107), R(27, 102)], fill=BLACK, width=1)
    draw.line([R(30, 108), R(31, 103)], fill=BLACK, width=1)
    # Right paw
    rp_pts = [R(86, 100), R(84, 110), R(94, 113), R(100, 106), R(96, 98)]
    draw.polygon(rp_pts, fill=WHITE, outline=BLACK)
    draw.line([R(89, 103), R(90, 108)], fill=BLACK, width=1)
    draw.line([R(93, 102), R(95, 107)], fill=BLACK, width=1)

    # 3. Fox Ears with whipping ear tufts
    # Left Ear: tall & pointed (tip reaches y=10..12)
    le_tip_x = 28 + (whip_dx if whip > 0 else 0)
    le_outer = [R(30, 48), R(24, 30), R(le_tip_x, 10), R(40, 24), R(48, 38)]
    draw.polygon(le_outer, fill=WHITE, outline=BLACK)
    # Left inner ear
    draw.polygon([R(32, 42), R(27, 28), R(le_tip_x + 2, 14), R(38, 26), R(43, 38)], fill=BLACK)
    # Inner ear tufts whipping dynamically (sharp flowing fur points)
    tuft_lx = 18 + (whip_dx * 2 if whip > 0 else -1)
    draw.polygon([R(32, 34), R(tuft_lx, 26), R(28, 42)], fill=WHITE, outline=BLACK)
    draw.polygon([R(34, 40), R(tuft_lx + 4, 36), R(32, 46)], fill=WHITE, outline=BLACK)
    if whip > 0:  # extra whipping tuft trailing right
        draw.polygon([R(42, 28), R(52, 24), R(46, 32)], fill=WHITE, outline=BLACK)

    # Right Ear: tall & pointed
    re_tip_x = 92 + (whip_dx if whip < 0 else 0)
    re_outer = [R(72, 38), R(80, 24), R(re_tip_x, 10), R(96, 30), R(90, 48)]
    draw.polygon(re_outer, fill=WHITE, outline=BLACK)
    # Right inner ear
    draw.polygon([R(77, 38), R(82, 26), R(re_tip_x - 2, 14), R(93, 28), R(88, 42)], fill=BLACK)
    # Inner ear tufts whipping dynamically
    tuft_rx = 102 + (whip_dx * 2 if whip < 0 else 1)
    draw.polygon([R(88, 34), R(tuft_rx, 26), R(92, 42)], fill=WHITE, outline=BLACK)
    draw.polygon([R(86, 40), R(tuft_rx - 4, 36), R(88, 46)], fill=WHITE, outline=BLACK)
    if whip < 0:  # extra whipping tuft trailing left
        draw.polygon([R(78, 28), R(68, 24), R(74, 32)], fill=WHITE, outline=BLACK)

    # 4. Head Silhouette & Cheek Tufts (fox cheeks, no whiskers!)
    l_flare1 = 20 if whip > 0 else (26 if whip < 0 else 23)
    l_flare2 = 18 if whip > 0 else (24 if whip < 0 else 21)
    r_flare1 = 100 if whip > 0 else (94 if whip < 0 else 97)
    r_flare2 = 102 if whip > 0 else (96 if whip < 0 else 99)

    head_poly = [
        R(38, 38), R(60, 34), R(82, 38),              # top forehead
        R(88, 44), R(90, 54),                          # upper right temple
        R(r_flare1, 64), R(r_flare1 - 3, 70),          # top cheek tuft
        R(r_flare2, 76), R(r_flare2 - 4, 82),          # bottom cheek tuft
        R(84, 88), R(74, 91), R(60, 92), R(46, 91), R(36, 88), # chin & jaw
        R(l_flare2 - 4, 82), R(l_flare2, 76),          # bottom left cheek tuft
        R(l_flare1 - 3, 70), R(l_flare1, 64),          # top left cheek tuft
        R(30, 54), R(32, 44)                           # upper left temple
    ]
    draw.polygon(head_poly, fill=WHITE, outline=BLACK)

    # Short notches at cheek tuft clefts (just 3 px, no whiskers!)
    draw.line([R(l_flare1 - 1, 70), R(l_flare1 + 3, 70)], fill=BLACK, width=1)
    draw.line([R(l_flare2 - 2, 82), R(l_flare2 + 2, 82)], fill=BLACK, width=1)
    draw.line([R(r_flare1 + 1, 70), R(r_flare1 - 3, 70)], fill=BLACK, width=1)
    draw.line([R(r_flare2 + 2, 82), R(r_flare2 - 2, 82)], fill=BLACK, width=1)

    # 5. Flower Crown (5 lush roses across forehead + leaves)
    draw_leaf(draw, *R(40, 32), *R(32 + whip_dx, 26))
    draw_leaf(draw, *R(52, 30), *R(48 + whip_dx, 22))
    draw_leaf(draw, *R(68, 30), *R(72 + whip_dx, 22))
    draw_leaf(draw, *R(80, 32), *R(88 + whip_dx, 26))

    # Roses
    p_whip = whip_dx // 2
    draw_rose(draw, *R(36, 40), 6, p_whip)
    draw_rose(draw, *R(48, 35), 7, p_whip)
    draw_rose(draw, *R(60, 32), 8, p_whip)  # center rose
    draw_rose(draw, *R(72, 35), 7, p_whip)
    draw_rose(draw, *R(84, 40), 6, p_whip)

    # Flying loose petals on lean frames (whipping with bass drops)
    if column == 0:  # leaning Left -> petals whip to Right
        draw_floating_petal(draw, *R(96, 32), 30.0)
        draw_floating_petal(draw, *R(104, 44), -20.0)
        draw_floating_petal(draw, *R(100, 18), 45.0)
    elif column == 2:  # leaning Right -> petals whip to Left
        draw_floating_petal(draw, *R(24, 32), -30.0)
        draw_floating_petal(draw, *R(16, 44), 20.0)
        draw_floating_petal(draw, *R(20, 18), -45.0)

    # 6. Muzzle & Black Triangular Nose
    nose_pts = [R(56, 67), R(64, 67), R(60, 71)]
    draw.polygon(nose_pts, fill=BLACK)
    draw.line([R(60, 71), R(60, 74)], fill=BLACK, width=1)

    # 7. Expressions
    if expression == "smug":
        # Smug: half-lidded eyelids, sharp lashes, arched eyebrows, smug cat smile
        draw.arc((R(36, 52)[0] - 8, R(36, 52)[1] - 6, R(36, 52)[0] + 8, R(36, 52)[1] + 8), 190, 350, fill=BLACK, width=2)
        draw.chord((R(42, 54)[0] - 6, R(42, 54)[1] - 4, R(42, 54)[0] + 6, R(42, 54)[1] + 6), 0, 180, fill=BLACK)
        draw.point(R(40, 56), fill=WHITE)
        draw.line([R(32, 51), R(36, 53)], fill=BLACK, width=2)
        draw.line([R(33, 49), R(37, 51)], fill=BLACK, width=1)
        draw.arc((R(42, 45)[0] - 8, R(42, 45)[1] - 4, R(42, 45)[0] + 8, R(42, 45)[1] + 4), 200, 340, fill=BLACK, width=2)

        draw.arc((R(84, 52)[0] - 8, R(84, 52)[1] - 6, R(84, 52)[0] + 8, R(84, 52)[1] + 8), 190, 350, fill=BLACK, width=2)
        draw.chord((R(78, 54)[0] - 6, R(78, 54)[1] - 4, R(78, 54)[0] + 6, R(78, 54)[1] + 6), 0, 180, fill=BLACK)
        draw.point(R(76, 56), fill=WHITE)
        draw.line([R(88, 51), R(84, 53)], fill=BLACK, width=2)
        draw.line([R(87, 49), R(83, 51)], fill=BLACK, width=1)
        draw.arc((R(78, 45)[0] - 8, R(78, 45)[1] - 4, R(78, 45)[0] + 8, R(78, 45)[1] + 4), 200, 340, fill=BLACK, width=2)

        # Smug smile: cat :3 grin with cute tooth
        draw.arc((R(52, 74)[0] - 6, R(52, 74)[1] - 4, R(52, 74)[0] + 6, R(52, 74)[1] + 4), 10, 160, fill=BLACK, width=2)
        draw.arc((R(68, 74)[0] - 6, R(68, 74)[1] - 4, R(68, 74)[0] + 6, R(68, 74)[1] + 4), 20, 170, fill=BLACK, width=2)
        draw.polygon([R(64, 77), R(66, 80), R(68, 77)], fill=WHITE, outline=BLACK)

    elif expression == "wink":
        # Left eye: wide open, sparkling with star highlight
        draw.ellipse((R(42, 54)[0] - 8, R(42, 54)[1] - 8, R(42, 54)[0] + 8, R(42, 54)[1] + 8), fill=BLACK)
        draw.ellipse((R(40, 52)[0] - 3, R(40, 52)[1] - 3, R(40, 52)[0] + 3, R(40, 52)[1] + 3), fill=WHITE)
        draw.ellipse((R(44, 57)[0] - 1, R(44, 57)[1] - 1, R(44, 57)[0] + 1, R(44, 57)[1] + 1), fill=WHITE)
        draw.arc((R(42, 52)[0] - 10, R(42, 52)[1] - 9, R(42, 52)[0] + 10, R(42, 52)[1] + 7), 190, 350, fill=BLACK, width=2)
        draw.line([R(32, 50), R(35, 52)], fill=BLACK, width=2)
        draw.arc((R(42, 43)[0] - 8, R(42, 43)[1] - 4, R(42, 43)[0] + 8, R(42, 43)[1] + 4), 200, 340, fill=BLACK, width=2)

        # Right eye: playful curved wink arc with eyelashes & star
        p_c = R(78, 55)
        draw.line([R(70, 53), p_c, R(86, 52)], fill=BLACK, width=2)
        draw.line([R(84, 49), R(86, 52)], fill=BLACK, width=2)
        draw.line([R(87, 55), R(84, 53)], fill=BLACK, width=2)
        sp = R(92, 54)
        draw.line([(sp[0] - 3, sp[1]), (sp[0] + 3, sp[1])], fill=BLACK, width=1)
        draw.line([(sp[0], sp[1] - 3), (sp[0], sp[1] + 3)], fill=BLACK, width=1)
        draw.arc((R(78, 46)[0] - 7, R(78, 46)[1] - 3, R(78, 46)[0] + 7, R(78, 46)[1] + 3), 200, 340, fill=BLACK, width=2)

        # Mouth: open happy smile :D
        mouth_pts = [R(54, 75), R(66, 75), R(64, 83), R(56, 83)]
        draw.polygon(mouth_pts, fill=BLACK)
        draw.ellipse((R(60, 82)[0] - 4, R(60, 82)[1] - 2, R(60, 82)[0] + 4, R(60, 82)[1] + 2), fill=WHITE)

    elif expression == "pout":
        # Big glossy anime eyes looking slightly up, adorable dramatic pout
        for ex, side in [(42, -1), (78, 1)]:
            draw.ellipse((R(ex, 55)[0] - 8, R(ex, 55)[1] - 9, R(ex, 55)[0] + 8, R(ex, 55)[1] + 9), fill=BLACK)
            draw.ellipse((R(ex - 2, 52)[0] - 4, R(ex - 2, 52)[1] - 4, R(ex - 2, 52)[0] + 4, R(ex - 2, 52)[1] + 4), fill=WHITE)
            draw.ellipse((R(ex + 2, 58)[0] - 2, R(ex + 2, 58)[1] - 2, R(ex + 2, 58)[0] + 2, R(ex + 2, 58)[1] + 2), fill=WHITE)
            draw.arc((R(ex, 52)[0] - 9, R(ex, 52)[1] - 9, R(ex, 52)[0] + 9, R(ex, 52)[1] + 7), 190, 350, fill=BLACK, width=2)
            draw.line([R(ex - 6 * side, 44), R(ex + 6 * side, 46)], fill=BLACK, width=2)

        # Soft blush dots
        draw.line([R(34, 68), R(38, 65)], fill=BLACK, width=1)
        draw.line([R(82, 65), R(86, 68)], fill=BLACK, width=1)

        # Classic kissy-pout 3 mouth
        draw.arc((R(56, 76)[0] - 4, R(56, 76)[1] - 4, R(56, 76)[0] + 4, R(56, 76)[1] + 4), 70, 270, fill=BLACK, width=2)
        draw.arc((R(64, 76)[0] - 4, R(64, 76)[1] - 4, R(64, 76)[0] + 4, R(64, 76)[1] + 4), 270, 110, fill=BLACK, width=2)

    elif expression == "smirk":
        # Left eye: sharp eye with lowered cocky brow
        draw.chord((R(42, 55)[0] - 7, R(42, 55)[1] - 6, R(42, 55)[0] + 7, R(42, 55)[1] + 6), 0, 180, fill=BLACK)
        draw.point(R(41, 56), fill=WHITE)
        draw.line([R(34, 53), R(49, 53)], fill=BLACK, width=2)
        draw.line([R(34, 46), R(48, 48)], fill=BLACK, width=2)

        # Right eye: arched eye with high cocked brow
        draw.arc((R(82, 53)[0] - 7, R(82, 53)[1] - 6, R(82, 53)[0] + 7, R(82, 53)[1] + 6), 190, 350, fill=BLACK, width=2)
        draw.chord((R(80, 54)[0] - 6, R(80, 54)[1] - 5, R(80, 54)[0] + 6, R(80, 54)[1] + 5), 0, 180, fill=BLACK)
        draw.point(R(78, 55), fill=WHITE)
        draw.arc((R(80, 42)[0] - 8, R(80, 42)[1] - 4, R(80, 42)[0] + 8, R(80, 42)[1] + 4), 210, 330, fill=BLACK, width=2)

        # One-sided smirk: curves up sharply on the right
        draw.line([R(53, 76), R(60, 75)], fill=BLACK, width=2)
        draw.arc((R(65, 71)[0] - 7, R(65, 71)[1] - 6, R(65, 71)[0] + 7, R(65, 71)[1] + 6), 10, 140, fill=BLACK, width=2)
        draw.line([R(71, 71), R(75, 66)], fill=BLACK, width=2)
        draw.polygon([R(68, 73), R(71, 76), R(72, 73)], fill=WHITE, outline=BLACK)  # cheeky little tooth

    return enforce_keyline(im, keyline=2)


# ---------------------------------------------------------------------------
# Wide shot drawing (48x64, 3 sway frames + 1 hurt frame)
# ---------------------------------------------------------------------------

def render_wide_cell(frame_type: str, column: int) -> Image.Image:
    """Render a single 48x64 cell for Floofy wide shot.

    frame_type == "idle":
      column 0: Lean Left  (bouquet tail sways out to Right)
      column 1: Center     (balanced upright)
      column 2: Lean Right (bouquet tail sways back/Left)
    frame_type == "hurt":
      column 0: Hurt/dizzy recoil pose
      column 1..2: Empty
    """
    im = Image.new("RGBA", (48, 64), CLEAR)
    if frame_type == "hurt" and column > 0:
        return im

    draw = ImageDraw.Draw(im)

    if frame_type == "idle":
        sway_dx = -3 if column == 0 else (3 if column == 2 else 0)
        tail_dx = 5 if column == 0 else (-4 if column == 2 else 0)
        head_angle = -5.0 if column == 0 else (5.0 if column == 2 else 0.0)

        cx_b = 24 + sway_dx // 2
        cy_b = 38

        def WB(x: float, y: float) -> tuple[int, int]:
            return rot_pt(x + sway_dx, y, cx_b, cy_b, head_angle)

        # 1. Bouquet Tail (large fluffy mass of flowers & leaves behind hip)
        tail_x = 33 + tail_dx
        tail_y = 36
        draw.polygon([(tail_x - 3, tail_y + 8), (tail_x + 9, tail_y + 14), (tail_x + 3, tail_y + 18)], fill=WHITE, outline=BLACK)
        draw.polygon([(tail_x + 1, tail_y - 8), (tail_x + 10, tail_y - 2), (tail_x + 3, tail_y + 4)], fill=WHITE, outline=BLACK)
        draw_rose(draw, tail_x, tail_y + 8, 4)
        draw_rose(draw, tail_x + 4, tail_y, 5)
        draw_rose(draw, tail_x - 1, tail_y - 6, 4)

        # 2. Legs & Feet (planted wide on ground y=60)
        draw.polygon([(17, 46), (22, 46), (20, 60), (14, 60)], fill=WHITE, outline=BLACK)
        draw.polygon([(26, 46), (31, 46), (34, 60), (28, 60)], fill=WHITE, outline=BLACK)

        # 3. Torso, Fluffy Neck Ruff & Arms
        torso_pts = [WB(18, 30), WB(30, 30), (30, 48), (18, 48)]
        draw.polygon(torso_pts, fill=WHITE, outline=BLACK)
        # Neck ruff directly merging head with torso
        draw.polygon([WB(16, 26), WB(24, 32), WB(32, 26), WB(24, 24)], fill=WHITE, outline=BLACK)

        # Arms & Paws
        draw.polygon([WB(15, 34), WB(12, 42), WB(17, 44), WB(19, 36)], fill=WHITE, outline=BLACK)
        draw.polygon([WB(29, 36), WB(32, 42), WB(28, 45), WB(26, 38)], fill=WHITE, outline=BLACK)
        # Rose held in right paw
        draw.line([WB(30, 40), WB(35, 35)], fill=BLACK, width=1)
        draw_rose(draw, *WB(36, 33), 3)

        # 4. Head, Cheek Fluff & Ears
        head_pts = [
            WB(16, 26), WB(13, 20), WB(18, 14), WB(30, 14),
            WB(35, 20), WB(32, 26), WB(24, 29)
        ]
        draw.polygon(head_pts, fill=WHITE, outline=BLACK)

        # Ears
        draw.polygon([WB(16, 16), WB(12, 4), WB(21, 12)], fill=WHITE, outline=BLACK)
        draw.polygon([WB(16, 14), WB(13, 7), WB(19, 12)], fill=BLACK)
        draw.polygon([WB(27, 12), WB(36, 4), WB(32, 16)], fill=WHITE, outline=BLACK)
        draw.polygon([WB(29, 12), WB(35, 7), WB(32, 14)], fill=BLACK)

        # Flower crown
        draw_rose(draw, *WB(18, 14), 3)
        draw_rose(draw, *WB(24, 12), 3)
        draw_rose(draw, *WB(30, 14), 3)

        # Eyes & Muzzle
        draw.ellipse((WB(19, 19)[0] - 1, WB(19, 19)[1] - 2, WB(19, 19)[0] + 1, WB(19, 19)[1] + 2), fill=BLACK)
        draw.ellipse((WB(29, 19)[0] - 1, WB(29, 19)[1] - 2, WB(29, 19)[0] + 1, WB(29, 19)[1] + 2), fill=BLACK)
        draw.point(WB(24, 23), fill=BLACK)
        draw.line([WB(22, 25), WB(24, 26), WB(26, 25)], fill=BLACK, width=1)

    elif frame_type == "hurt":
        # Recoil pose, startled eyes, askew crown
        draw_rose(draw, 34, 46, 4)
        draw_rose(draw, 38, 40, 4)

        # Buckled legs
        draw.polygon([(16, 46), (21, 46), (18, 60), (13, 60)], fill=WHITE, outline=BLACK)
        draw.polygon([(26, 46), (31, 46), (32, 60), (27, 60)], fill=WHITE, outline=BLACK)

        # Body recoiled
        draw.polygon([(17, 30), (29, 30), (28, 48), (17, 48)], fill=WHITE, outline=BLACK)
        # Paws thrown up
        draw.polygon([(14, 28), (11, 36), (16, 38), (18, 32)], fill=WHITE, outline=BLACK)
        draw.polygon([(28, 32), (34, 36), (32, 40), (26, 34)], fill=WHITE, outline=BLACK)

        # Head tilted back
        head_pts = [
            (16, 26), (13, 20), (19, 14), (31, 14),
            (35, 20), (33, 26), (24, 30)
        ]
        draw.polygon(head_pts, fill=WHITE, outline=BLACK)
        # Drooping ears
        draw.polygon([(16, 16), (10, 8), (19, 13)], fill=WHITE, outline=BLACK)
        draw.polygon([(27, 13), (37, 9), (32, 17)], fill=WHITE, outline=BLACK)

        # Crooked flower crown with falling petals
        draw_rose(draw, 17, 16, 3)
        draw_rose(draw, 23, 12, 3)
        draw_rose(draw, 31, 16, 3)
        draw_floating_petal(draw, 36, 24, 30.0)
        draw_floating_petal(draw, 12, 22, -25.0)

        # Dizzy swirly / shocked eyes
        draw.line([(18, 18), (22, 22)], fill=BLACK, width=1)
        draw.line([(22, 18), (18, 22)], fill=BLACK, width=1)
        draw.line([(27, 18), (31, 22)], fill=BLACK, width=1)
        draw.line([(31, 18), (27, 22)], fill=BLACK, width=1)
        # Open mouth ("EEP!")
        draw.ellipse((23, 25, 26, 28), fill=BLACK)

    return enforce_keyline(im, keyline=2)


# ---------------------------------------------------------------------------
# Sheet Assembly & Output
# ---------------------------------------------------------------------------

def build_sheets() -> None:
    # 1. Close-up Sheet: 3 columns x 4 rows of 120x120 = 360 x 480
    close_sheet = Image.new("RGBA", (360, 480), CLEAR)
    expressions = ["smug", "wink", "pout", "smirk"]

    for row_idx, expr in enumerate(expressions):
        for col_idx in range(3):
            cell = render_closeup_cell(expr, col_idx)
            close_sheet.paste(cell, (col_idx * 120, row_idx * 120))

    close_sheet.save(CLOSE_PNG, optimize=True)
    print(f"Generated {CLOSE_PNG.relative_to(ROOT)} ({close_sheet.size})")

    close_metadata = {
        "image": "floofy_close.png",
        "cell": [120, 120],
        "origin": [60, 120],
        "anims": {
            "smug": {"row": 0, "frames": 3, "fps": 1},
            "wink": {"row": 1, "frames": 3, "fps": 1},
            "pout": {"row": 2, "frames": 3, "fps": 1},
            "smirk": {"row": 3, "frames": 3, "fps": 1},
            "shake": {"row": 0, "frames": 3, "fps": 12}
        }
    }
    CLOSE_JSON.write_text(json.dumps(close_metadata, indent=2) + "\n", encoding="utf-8")
    print(f"Generated {CLOSE_JSON.relative_to(ROOT)}")

    # 2. Wide Sheet: 3 columns x 2 rows of 48x64 = 144 x 128
    wide_sheet = Image.new("RGBA", (144, 128), CLEAR)
    # Row 0: idle (3 sway frames)
    for col_idx in range(3):
        cell = render_wide_cell("idle", col_idx)
        wide_sheet.paste(cell, (col_idx * 48, 0))

    # Row 1: hurt (1 frame in col 0)
    hurt_cell = render_wide_cell("hurt", 0)
    wide_sheet.paste(hurt_cell, (0, 64))

    wide_sheet.save(WIDE_PNG, optimize=True)
    print(f"Generated {WIDE_PNG.relative_to(ROOT)} ({wide_sheet.size})")

    wide_metadata = {
        "image": "floofy_wide.png",
        "cell": [48, 64],
        "origin": [24, 64],
        "anims": {
            "idle": {"row": 0, "frames": 3, "fps": 3},
            "hurt": {"row": 1, "frames": 1}
        }
    }
    WIDE_JSON.write_text(json.dumps(wide_metadata, indent=2) + "\n", encoding="utf-8")
    print(f"Generated {WIDE_JSON.relative_to(ROOT)}")


if __name__ == "__main__":
    build_sheets()
