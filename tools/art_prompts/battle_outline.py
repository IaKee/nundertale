"""Enforce Undertale's battle-sprite edge rules on a white-on-black sheet.

Measured on the original battle sprites (Toriel, Papyrus, Sans, Loox, ...)
at native size, 2026-10-18:

1. the outer edge of every silhouette is a closed 1 px white line, also where
   the material is dark (Toriel's robe, Sans's jacket);
2. a 2 px opaque black keyline sits outside that white line, so white never
   touches the background and the arena grid stops short of the figure;
3. the inside is black with few, long white lines; no stray pixels.

usage:
  python tools/art_prompts/battle_outline.py --check SHEET.png --cell 60x80
  python tools/art_prompts/battle_outline.py SHEET.png --cell 60x80 --out FIXED.png
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
from PIL import Image

WHITE = np.array([255, 255, 255, 255], np.uint8)
BLACK = np.array([0, 0, 0, 255], np.uint8)
CLEAR = np.array([0, 0, 0, 0], np.uint8)

N4 = [(0, 1), (0, -1), (1, 0), (-1, 0)]
N8 = N4 + [(1, 1), (1, -1), (-1, 1), (-1, -1)]


def shift(mask: np.ndarray, dy: int, dx: int, fill: bool = False) -> np.ndarray:
    out = np.full_like(mask, fill)
    h, w = mask.shape
    ys, yd = (slice(0, h - dy), slice(dy, h)) if dy >= 0 else (slice(-dy, h), slice(0, h + dy))
    xs, xd = (slice(0, w - dx), slice(dx, w)) if dx >= 0 else (slice(-dx, w), slice(0, w + dx))
    out[yd, xd] = mask[ys, xs]
    return out


def classes(cell: np.ndarray) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """opaque, white, accent (any opaque color that is neither black nor white)."""
    opaque = cell[..., 3] >= 128
    rgb = cell[..., :3].astype(int)
    lum = rgb.sum(axis=2)
    white = opaque & (lum >= 600)
    black = opaque & (lum <= 90)
    accent = opaque & ~white & ~black
    return opaque, white, accent


def outside_reach(opaque: np.ndarray, white: np.ndarray) -> np.ndarray:
    """Pixels reachable from any transparent pixel without crossing white."""
    h, w = opaque.shape
    seen = np.zeros_like(opaque)
    stack = [tuple(p) for p in np.argwhere(~opaque)]
    while stack:
        y, x = stack.pop()
        if y < 0 or x < 0 or y >= h or x >= w or seen[y, x] or white[y, x]:
            continue
        seen[y, x] = True
        stack.extend((y + dy, x + dx) for dy, dx in N4)
    return seen


def distance_to_clear(opaque: np.ndarray, limit: int = 3) -> np.ndarray:
    """Chebyshev distance (capped) from each pixel to the nearest transparent pixel."""
    dist = np.where(opaque, limit, 0)
    clear = ~opaque
    grown = clear.copy()
    for d in range(1, limit):
        nxt = grown.copy()
        for dy, dx in N8:
            nxt |= shift(grown, dy, dx)
        dist[(nxt & ~grown) & opaque] = d
        grown = nxt
    return dist


def check_cell(cell: np.ndarray) -> dict:
    opaque, white, _ = classes(cell)
    if not opaque.any():
        return {"empty": True}
    reach = outside_reach(opaque, white)
    dist = distance_to_clear(opaque)
    edge = opaque & np.zeros_like(opaque)
    for dy, dx in N4:
        edge |= opaque & ~shift(opaque, dy, dx, fill=True)
    return {
        "white_on_edge": int((edge & white).sum()),
        # black reachable from outside that is neither keyline (near the background) nor next to the
        # white contour = the background runs into the body
        "leak_px": int((reach & opaque & (dist > 2) & (distance_to_clear(~white) > 2)).sum()),
        "isolated_white": int(isolated(white).sum()),
    }


def isolated(mask: np.ndarray) -> np.ndarray:
    count = np.zeros(mask.shape, int)
    for dy, dx in N8:
        count += shift(mask, dy, dx)
    return mask & (count == 0)


def fix_cell(cell: np.ndarray, keyline: int = 2) -> np.ndarray:
    out = cell.copy()
    opaque, white, accent = classes(out)
    if not opaque.any():
        return out
    # 1. drop stray single white pixels that are not touching anything white
    stray = isolated(white)
    out[stray] = BLACK
    # 2. normalize to pure black/white (accent colors such as the blush stay)
    opaque, white, accent = classes(out)
    out[opaque & ~white & ~accent] = BLACK
    out[white] = WHITE
    out[~opaque] = CLEAR
    # 3. the outer ring of the silhouette becomes the closed white contour
    #    (the cell border counts as outside, so feet on the baseline get a bottom line too)
    inner = opaque.copy()
    for dy, dx in N4:
        inner &= shift(opaque, dy, dx, fill=False)
    ring = opaque & ~inner
    result = out.copy()
    result[ring & ~accent] = WHITE
    # 4. keyline: grow the silhouette outward by `keyline` px (8-connected) in opaque black
    grown = opaque.copy()
    for _ in range(keyline):
        nxt = grown.copy()
        for dy, dx in N8:
            nxt |= shift(grown, dy, dx)
        grown = nxt
    result[grown & ~opaque] = BLACK
    return result


def cells(img: np.ndarray, cw: int, ch: int):
    for y in range(0, img.shape[0], ch):
        for x in range(0, img.shape[1], cw):
            yield y, x


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("sheet")
    ap.add_argument("--cell", required=True, help="WxH, e.g. 60x80")
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--out")
    ap.add_argument("--keyline", type=int, default=2)
    a = ap.parse_args()
    cw, ch = (int(v) for v in a.cell.lower().split("x"))
    img = np.array(Image.open(a.sheet).convert("RGBA"))
    if a.check:
        rep = {f"{y // ch},{x // cw}": check_cell(img[y:y + ch, x:x + cw]) for y, x in cells(img, cw, ch)}
        rep = {k: v for k, v in rep.items() if not v.get("empty")}
        ok = all(v["leak_px"] == 0 and v["white_on_edge"] == 0 for v in rep.values())
        print(json.dumps({"ok": ok, "cells": rep}, indent=1))
        raise SystemExit(0 if ok else 1)
    out = img.copy()
    for y, x in cells(img, cw, ch):
        out[y:y + ch, x:x + cw] = fix_cell(img[y:y + ch, x:x + cw], a.keyline)
    dst = Path(a.out or a.sheet)
    Image.fromarray(out).save(dst)
    print(f"wrote {dst}")


if __name__ == "__main__":
    main()
