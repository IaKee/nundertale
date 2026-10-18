"""Helpers to turn the raw AI guide sheets in assets/_gen/ruins/ into native 1x pixel art.

Pipeline per sheet: chroma-key the noisy magenta, slice cells by background bands,
reduce each cell to the native grid by palette-snapped majority vote, then pack.
"""
from __future__ import annotations

import numpy as np
from PIL import Image


def hexrgb(h: str) -> tuple[int, int, int]:
    h = h.lstrip("#")
    return int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16)


def load(path: str) -> tuple[np.ndarray, np.ndarray]:
    """Return (rgb int array HxWx3, foreground bool mask)."""
    rgb = np.asarray(Image.open(path).convert("RGB")).astype(np.int32)
    r, g, b = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    bg = (r > 170) & (g < 110) & (b > 170)
    # anti-aliased fringe between magenta and dark outline (e.g. 120,0,120)
    lo = np.minimum(r, b)
    bg |= (lo > 90) & (g * 100 < lo * 35) & (np.abs(r - b) < 60)
    return rgb, ~bg


def bands(profile: np.ndarray, n: int, min_count: int = 3) -> list[tuple[int, int]]:
    """Split a 1D foreground-count profile into exactly n bands at the widest gaps."""
    on = profile >= min_count
    runs = []
    i = 0
    while i < len(on):
        if on[i]:
            j = i
            while j < len(on) and on[j]:
                j += 1
            runs.append([i, j])
            i = j
        else:
            i += 1
    if len(runs) < n:
        raise ValueError(f"found {len(runs)} bands, need {n}")
    # merge across the smallest gaps until n remain
    while len(runs) > n:
        gaps = [runs[k + 1][0] - runs[k][1] for k in range(len(runs) - 1)]
        k = int(np.argmin(gaps))
        runs[k] = [runs[k][0], runs[k + 1][1]]
        del runs[k + 1]
    return [tuple(r) for r in runs]


def slice_cells(fg: np.ndarray, cols_per_row: list[int], min_count: int = 3):
    """Return boxes[row][col] = (x0, y0, x1, y1), tight around foreground."""
    rows = bands(fg.sum(axis=1), len(cols_per_row), min_count)
    out = []
    for (y0, y1), ncols in zip(rows, cols_per_row):
        cols = bands(fg[y0:y1].sum(axis=0), ncols, min_count)
        row = []
        for x0, x1 in cols:
            sub = fg[y0:y1, x0:x1]
            ys = np.where(sub.sum(axis=1) >= 2)[0]
            row.append((x0, y0 + ys[0], x1, y0 + ys[-1] + 1))
        out.append(row)
    return out


def _redmean(a: np.ndarray, pal: np.ndarray) -> np.ndarray:
    """Perceptual-ish RGB distance from each pixel in a (N,3) to each palette color (K,3)."""
    rm = (a[:, None, 0] + pal[None, :, 0]) / 2.0
    d = a[:, None, :] - pal[None, :, :]
    return (2 + rm / 256) * d[..., 0] ** 2 + 4 * d[..., 1] ** 2 + (2 + (255 - rm) / 256) * d[..., 2] ** 2


def snap(rgb: np.ndarray, pal: np.ndarray) -> np.ndarray:
    """Index of the nearest palette color for every pixel."""
    flat = rgb.reshape(-1, 3).astype(np.float64)
    idx = np.empty(len(flat), dtype=np.int32)
    for s in range(0, len(flat), 200_000):
        idx[s:s + 200_000] = np.argmin(_redmean(flat[s:s + 200_000], pal), axis=1)
    return idx.reshape(rgb.shape[:2])


def reduce_region(idx: np.ndarray, fg: np.ndarray, ax: float, ay: float, scale: float,
                  ox: int, oy: int, w: int, h: int, k: int,
                  weights: np.ndarray | None = None, alpha_thr: float = 0.5) -> np.ndarray:
    """Majority-vote downsample into a w x h cell.

    Source point (ax, ay) lands on target point (ox, oy); one target pixel covers
    `scale` source pixels. Returns int array (h, w), -1 = transparent, else palette index.
    """
    if weights is None:
        weights = np.ones(k)
    out = np.full((h, w), -1, dtype=np.int32)
    H, W = idx.shape
    for ty in range(h):
        sy0 = ay + (ty - oy) * scale
        y0, y1 = int(round(sy0)), int(round(sy0 + scale))
        if y1 <= 0 or y0 >= H:
            continue
        y0, y1 = max(y0, 0), min(y1, H)
        for tx in range(w):
            sx0 = ax + (tx - ox) * scale
            x0, x1 = int(round(sx0)), int(round(sx0 + scale))
            if x1 <= 0 or x0 >= W:
                continue
            x0, x1 = max(x0, 0), min(x1, W)
            f = fg[y0:y1, x0:x1]
            n = f.size
            nf = int(f.sum())
            if n == 0 or nf < alpha_thr * n:
                continue
            counts = np.bincount(idx[y0:y1, x0:x1][f], minlength=k) * weights
            out[ty, tx] = int(np.argmax(counts))
    return out


def to_rgba(cell: np.ndarray, pal: np.ndarray) -> np.ndarray:
    h, w = cell.shape
    img = np.zeros((h, w, 4), dtype=np.uint8)
    m = cell >= 0
    img[m, :3] = pal[cell[m]]
    img[m, 3] = 255
    return img


def fill_holes(cell: np.ndarray, fill: int) -> np.ndarray:
    """Make transparent pixels not connected to the cell border opaque."""
    h, w = cell.shape
    out = cell.copy()
    seen = np.zeros_like(cell, dtype=bool)
    stack = [(y, x) for y in range(h) for x in (0, w - 1)] + [(y, x) for x in range(w) for y in (0, h - 1)]
    while stack:
        y, x = stack.pop()
        if 0 <= y < h and 0 <= x < w and not seen[y, x] and cell[y, x] < 0:
            seen[y, x] = True
            stack += [(y + 1, x), (y - 1, x), (y, x + 1), (y, x - 1)]
    out[(cell < 0) & ~seen] = fill
    return out


def drop_specks(cell: np.ndarray, min_size: int = 2) -> np.ndarray:
    """Remove opaque 4-connected islands smaller than min_size pixels."""
    h, w = cell.shape
    out = cell.copy()
    seen = np.zeros_like(cell, dtype=bool)
    for sy in range(h):
        for sx in range(w):
            if cell[sy, sx] < 0 or seen[sy, sx]:
                continue
            comp, stack = [], [(sy, sx)]
            seen[sy, sx] = True
            while stack:
                y, x = stack.pop()
                comp.append((y, x))
                for ny, nx in ((y + 1, x), (y - 1, x), (y, x + 1), (y, x - 1)):
                    if 0 <= ny < h and 0 <= nx < w and not seen[ny, nx] and cell[ny, nx] >= 0:
                        seen[ny, nx] = True
                        stack.append((ny, nx))
            if len(comp) < min_size:
                for y, x in comp:
                    out[y, x] = -1
    return out


def outline(cell: np.ndarray, color: int) -> np.ndarray:
    """Recolor every opaque pixel that touches transparency (4-neighbour) to `color`."""
    h, w = cell.shape
    pad = np.pad(cell, 1, constant_values=-1)
    edge = (pad[:-2, 1:-1] < 0) | (pad[2:, 1:-1] < 0) | (pad[1:-1, :-2] < 0) | (pad[1:-1, 2:] < 0)
    out = cell.copy()
    out[(cell >= 0) & edge] = color
    return out


def foot_anchor(fg: np.ndarray, box, frac: float = 0.15) -> tuple[float, float]:
    """Source point under the feet: x = centroid of the lowest `frac` of the figure, y = bottom."""
    x0, y0, x1, y1 = box
    sub = fg[y0:y1, x0:x1]
    band = sub[int((y1 - y0) * (1 - frac)):]
    xs = np.where(band)[1]
    return x0 + (xs.mean() + 0.5 if len(xs) else (x1 - x0) / 2), float(y1)


def preview(guide_path: str, native: Image.Image, out_path: str, zoom: int = 4, guide_h: int | None = None):
    """Side-by-side: AI guide (left) and the native result upscaled with nearest (right)."""
    big = native.resize((native.width * zoom, native.height * zoom), Image.NEAREST)
    g = Image.open(guide_path).convert("RGB")
    gh = guide_h or big.height
    g = g.resize((max(1, round(g.width * gh / g.height)), gh), Image.LANCZOS)
    pad = 16
    canvas = Image.new("RGB", (g.width + big.width + pad * 3, max(g.height, big.height) + pad * 2), (48, 48, 56))
    canvas.paste(g, (pad, pad))
    # checker behind the native art so transparency reads
    chk = Image.new("RGB", big.size, (90, 90, 100))
    px = chk.load()
    for y in range(0, big.height, zoom * 2):
        for x in range(0, big.width, zoom * 2):
            for yy in range(y, min(y + zoom, big.height)):
                for xx in range(x, min(x + zoom, big.width)):
                    px[xx, yy] = (110, 110, 120)
    chk.paste(big, (0, 0), big if big.mode == "RGBA" else None)
    canvas.paste(chk, (g.width + pad * 2, pad))
    canvas.save(out_path)


def mode_filter(idx: np.ndarray, size: int) -> np.ndarray:
    """Remove AI texture noise finer than `size` source pixels from an index image."""
    if size < 3:
        return idx
    from PIL import ImageFilter
    im = Image.fromarray(idx.astype(np.uint8))
    return np.asarray(im.filter(ImageFilter.ModeFilter(size | 1))).astype(np.int32)


def despeckle(cell: np.ndarray, protect=(), passes: int = 1) -> np.ndarray:
    """Recolor opaque pixels that share their color with no 4-neighbour (except `protect` colors)."""
    out = cell.copy()
    h, w = cell.shape
    for _ in range(passes):
        src = out.copy()
        for y in range(h):
            for x in range(w):
                c = src[y, x]
                if c < 0 or c in protect:
                    continue
                n4 = [src[ny, nx] for ny, nx in ((y - 1, x), (y + 1, x), (y, x - 1), (y, x + 1))
                      if 0 <= ny < h and 0 <= nx < w]
                if c in n4:
                    continue
                n8 = [src[ny, nx] for ny in (y - 1, y, y + 1) for nx in (x - 1, x, x + 1)
                      if (ny, nx) != (y, x) and 0 <= ny < h and 0 <= nx < w and src[ny, nx] >= 0]
                if n8:
                    out[y, x] = max(set(n8), key=n8.count)
    return out


def prune_color(cell: np.ndarray, color: int, min_size: int, keep_largest: bool = False) -> np.ndarray:
    """Recolor 4-connected islands of `color` smaller than min_size (or all but the largest)
    to their most common other neighbour."""
    h, w = cell.shape
    out = cell.copy()
    seen = np.zeros_like(cell, dtype=bool)
    comps = []
    for sy in range(h):
        for sx in range(w):
            if cell[sy, sx] != color or seen[sy, sx]:
                continue
            comp, stack, ring = [], [(sy, sx)], []
            seen[sy, sx] = True
            while stack:
                y, x = stack.pop()
                comp.append((y, x))
                for ny, nx in ((y + 1, x), (y - 1, x), (y, x + 1), (y, x - 1)):
                    if 0 <= ny < h and 0 <= nx < w:
                        if cell[ny, nx] == color and not seen[ny, nx]:
                            seen[ny, nx] = True
                            stack.append((ny, nx))
                        elif cell[ny, nx] != color:
                            ring.append(int(cell[ny, nx]))
            comps.append((comp, ring))
    biggest = max((len(c) for c, _ in comps), default=0)
    for comp, ring in comps:
        if len(comp) < min_size or (keep_largest and len(comp) < biggest):
            fill = max(set(ring), key=ring.count) if ring else -1
            for y, x in comp:
                out[y, x] = fill
    return out


def reduce_mono(idx: np.ndarray, fg: np.ndarray, white: int, black: int, ax: float, ay: float, scale: float,
                ox: float, oy: float, w: int, h: int, accent: int | None = None,
                alpha_thr: float = 0.5, ctx: int = 2) -> np.ndarray:
    """White-on-black line art: adaptive threshold so thin strokes of either color survive.

    A pixel turns white when its white share beats the white share of its neighbourhood,
    clamped to [0.25, 0.75]: white lines on black need ~25%, black lines on white need ~75%."""
    frac = np.zeros((h, w))
    acc = np.zeros((h, w))
    opaque = np.zeros((h, w), dtype=bool)
    H, W = idx.shape
    for ty in range(h):
        y0, y1 = int(round(ay + (ty - oy) * scale)), int(round(ay + (ty - oy + 1) * scale))
        if y1 <= 0 or y0 >= H:
            continue
        y0, y1 = max(y0, 0), min(y1, H)
        for tx in range(w):
            x0, x1 = int(round(ax + (tx - ox) * scale)), int(round(ax + (tx - ox + 1) * scale))
            if x1 <= 0 or x0 >= W:
                continue
            x0, x1 = max(x0, 0), min(x1, W)
            f = fg[y0:y1, x0:x1]
            if f.size == 0 or f.sum() < alpha_thr * f.size:
                continue
            v = idx[y0:y1, x0:x1][f]
            opaque[ty, tx] = True
            frac[ty, tx] = np.mean((v == white) | ((v == accent) if accent is not None else False))
            if accent is not None:
                acc[ty, tx] = np.mean(v == accent)
    pad_f = np.pad(np.where(opaque, frac, 0.0), ctx)
    pad_o = np.pad(opaque.astype(float), ctx)
    k = 2 * ctx + 1
    s_f = sum(pad_f[dy:dy + h, dx:dx + w] for dy in range(k) for dx in range(k))
    s_o = sum(pad_o[dy:dy + h, dx:dx + w] for dy in range(k) for dx in range(k))
    local = np.clip(s_f / np.maximum(s_o, 1), 0.25, 0.75)
    out = np.full((h, w), -1, dtype=np.int32)
    out[opaque] = black
    out[opaque & (frac > local)] = white
    if accent is not None:
        out[opaque & (acc > 0.3)] = accent
    return out
