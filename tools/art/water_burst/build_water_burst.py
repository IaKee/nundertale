#!/usr/bin/env python3
"""Generate assets/ui/water_burst.png and assets/ui/water_burst.json for T17 / ART-183.

Produces:
- 3 frames ~24px water-burst animation.
- Row 0: Battle variant (crisp white droplets for battle use on dark backgrounds).
- Row 1: Overworld variant (light-blue water tint droplets for overworld use).
- Associated SpriteSheet metadata JSON.
- Optional preview rendering at 4x scale.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from PIL import Image

# Worktree root is 3 levels up from tools/art/water_burst
ROOT = Path(__file__).resolve().parents[3]
UI_DIR = ROOT / "assets" / "ui"
PNG_PATH = UI_DIR / "water_burst.png"
JSON_PATH = UI_DIR / "water_burst.json"

FRAME_SIZE = 24
FRAME_COUNT = 3
ROW_COUNT = 2

# Colors
CLEAR = (0, 0, 0, 0)
WHITE = (255, 255, 255, 255)

# Overworld palette: light-blue water tones with white highlights
BLUE_DARK = (64, 140, 210, 255)
BLUE_MID  = (110, 195, 255, 255)
BLUE_LGT  = (185, 235, 255, 255)
BLUE_HIL  = (240, 250, 255, 255)

# Frame 0: Initial compact spritz impact / nozzle squirt
FRAME_0_PIXELS = {
    # Core cluster
    (11, 11): BLUE_HIL,
    (12, 11): BLUE_HIL,
    (11, 12): BLUE_MID,
    (12, 12): BLUE_MID,
    (13, 12): BLUE_MID,
    (12, 13): BLUE_DARK,
    (11, 13): BLUE_DARK,
    # Small inner splatters
    (10, 10): BLUE_LGT,
    (13, 10): BLUE_LGT,
    (9, 12): BLUE_MID,
    (14, 12): BLUE_MID,
    (12, 9): BLUE_LGT,
    (12, 15): BLUE_DARK,
    # First outward droplet specks
    (8, 10): BLUE_LGT,
    (15, 10): BLUE_LGT,
    (11, 8): BLUE_HIL,
    (13, 8): BLUE_HIL,
    (10, 14): BLUE_DARK,
    (14, 14): BLUE_DARK,
}

# Frame 1: Peak radial burst / splash starburst
FRAME_1_PIXELS = {
    # Center dispersing
    (11, 11): BLUE_LGT,
    (12, 12): BLUE_LGT,
    # Upper arcing droplets (expanding fan)
    (12, 5): BLUE_HIL,
    (11, 6): BLUE_HIL,
    (13, 6): BLUE_HIL,
    (9, 7): BLUE_LGT,
    (10, 7): BLUE_LGT,
    (14, 7): BLUE_LGT,
    (15, 7): BLUE_LGT,
    (7, 9): BLUE_MID,
    (8, 9): BLUE_MID,
    (16, 9): BLUE_MID,
    (17, 9): BLUE_MID,
    # Side bursts
    (5, 11): BLUE_MID,
    (6, 11): BLUE_LGT,
    (17, 11): BLUE_LGT,
    (18, 11): BLUE_MID,
    (6, 13): BLUE_MID,
    (17, 13): BLUE_MID,
    # Lower falling droplets
    (9, 15): BLUE_DARK,
    (10, 16): BLUE_MID,
    (14, 15): BLUE_DARK,
    (13, 16): BLUE_MID,
    (11, 17): BLUE_MID,
    (12, 18): BLUE_DARK,
    # Outlying spray specks
    (4, 8): BLUE_LGT,
    (19, 8): BLUE_LGT,
    (8, 4): BLUE_HIL,
    (16, 4): BLUE_HIL,
}

# Frame 2: Dispersing spray & falling droplets
FRAME_2_PIXELS = {
    # Center is now hollowed out
    # Top droplets thinning & moving out
    (7, 3): BLUE_HIL,
    (17, 3): BLUE_HIL,
    (12, 4): BLUE_LGT,
    # Wide side droplets
    (3, 10): BLUE_MID,
    (4, 11): BLUE_LGT,
    (20, 11): BLUE_LGT,
    (21, 10): BLUE_MID,
    (4, 14): BLUE_DARK,
    (19, 14): BLUE_DARK,
    # Downward gravity droplets
    (7, 17): BLUE_MID,
    (8, 18): BLUE_DARK,
    (16, 17): BLUE_MID,
    (15, 18): BLUE_DARK,
    (10, 19): BLUE_MID,
    (11, 20): BLUE_DARK,
    (13, 20): BLUE_DARK,
    (14, 19): BLUE_MID,
    (12, 22): BLUE_DARK,
    # Scattered fine specks
    (6, 6): BLUE_LGT,
    (18, 6): BLUE_LGT,
    (10, 14): BLUE_LGT,
    (14, 14): BLUE_LGT,
}

FRAMES = [FRAME_0_PIXELS, FRAME_1_PIXELS, FRAME_2_PIXELS]


def build_spritesheet() -> Image.Image:
    sheet_w = FRAME_SIZE * FRAME_COUNT  # 72 px
    sheet_h = FRAME_SIZE * ROW_COUNT    # 48 px
    img = Image.new("RGBA", (sheet_w, sheet_h), CLEAR)

    for frame_idx, frame_data in enumerate(FRAMES):
        ox = frame_idx * FRAME_SIZE

        # Row 0: Battle variant (all droplets pure white)
        oy_battle = 0
        for (x, y), _ in frame_data.items():
            img.putpixel((ox + x, oy_battle + y), WHITE)

        # Row 1: Overworld variant (light-blue water tint)
        oy_ow = FRAME_SIZE
        for (x, y), color in frame_data.items():
            img.putpixel((ox + x, oy_ow + y), color)

    return img


def build_json() -> dict:
    return {
        "image": "water_burst.png",
        "cell": [FRAME_SIZE, FRAME_SIZE],
        "origin": [FRAME_SIZE // 2, FRAME_SIZE // 2],
        "anims": {
            "battle": { "row": 0, "frames": FRAME_COUNT, "fps": 12.0, "loop": False },
            "burst_battle": { "row": 0, "frames": FRAME_COUNT, "fps": 12.0, "loop": False },
            "white": { "row": 0, "frames": FRAME_COUNT, "fps": 12.0, "loop": False },
            "overworld": { "row": 1, "frames": FRAME_COUNT, "fps": 12.0, "loop": False },
            "burst_overworld": { "row": 1, "frames": FRAME_COUNT, "fps": 12.0, "loop": False },
            "blue": { "row": 1, "frames": FRAME_COUNT, "fps": 12.0, "loop": False }
        }
    }


def render_preview(sheet: Image.Image, out_path: Path, scale: int = 4) -> None:
    # 3 frames x 2 rows, displayed on a contrasting background
    pad = 4
    canvas_w = (FRAME_SIZE * FRAME_COUNT + pad * (FRAME_COUNT + 1)) * scale
    canvas_h = (FRAME_SIZE * ROW_COUNT + pad * (ROW_COUNT + 1)) * scale
    preview = Image.new("RGBA", (canvas_w, canvas_h), (20, 16, 24, 255))

    for r in range(ROW_COUNT):
        # Draw battle row on dark background; overworld row with slight dark purple tint
        bg_col = (10, 10, 10, 255) if r == 0 else (36, 24, 48, 255)
        for f in range(FRAME_COUNT):
            cell = sheet.crop((f * FRAME_SIZE, r * FRAME_SIZE, (f + 1) * FRAME_SIZE, (r + 1) * FRAME_SIZE))
            cell_scaled = cell.resize((FRAME_SIZE * scale, FRAME_SIZE * scale), Image.Resampling.NEAREST)

            bx = (pad + f * (FRAME_SIZE + pad)) * scale
            by = (pad + r * (FRAME_SIZE + pad)) * scale
            
            # Fill slot background
            slot_bg = Image.new("RGBA", (FRAME_SIZE * scale, FRAME_SIZE * scale), bg_col)
            preview.paste(slot_bg, (bx, by))
            preview.paste(cell_scaled, (bx, by), cell_scaled)

    preview.save(out_path)
    print(f"Saved preview: {out_path} ({preview.size[0]}x{preview.size[1]})")


def main():
    parser = argparse.ArgumentParser(description="Generate water burst sprite sheet and json.")
    parser.add_argument("--preview", action="store_true", help="Render preview PNG")
    args = parser.parse_args()

    UI_DIR.mkdir(parents=True, exist_ok=True)
    sheet = build_spritesheet()
    sheet.save(PNG_PATH, format="PNG")
    print(f"Saved sprite sheet: {PNG_PATH} ({sheet.size[0]}x{sheet.size[1]})")

    metadata = build_json()
    with open(JSON_PATH, "w", encoding="utf-8") as f:
        json.dump(metadata, f, indent=2)
    print(f"Saved sprite metadata: {JSON_PATH}")

    preview_path = Path(__file__).resolve().parent / "water_burst_preview_4x.png"
    render_preview(sheet, preview_path, scale=4)


if __name__ == "__main__":
    main()
