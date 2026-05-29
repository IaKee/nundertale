"""Spec-driven reduction for the non-core Ruins sheets.

Reads the prompt sheet (docs/art_prompts/ruins/<ID>_*.md): final path, cell, grid, origin,
single-image size or portrait list (section 3), palette (section 4) and JSON (section 8),
then reduces the newest guide for that sheet (manifest.json, else the highest -vN file).
These are first passes for hand touch-up, not final art.
"""
from __future__ import annotations

import glob
import json
import os
import re

import numpy as np
from PIL import Image

import reduce_lib as rl

DOCS = "docs/art_prompts/ruins"
GEN = "assets/_gen/ruins"


# ------------------------------------------------------------------ spec parsing
def section(md: str, num: int) -> str:
    m = re.search(rf"^## {num}\. .*?$(.*?)(?=^## \d+\. |\Z)", md, re.S | re.M)
    return m.group(1) if m else ""


def parse_spec(md_path: str) -> dict:
    md = open(md_path, encoding="utf-8").read()
    s3 = section(md, 3)
    spec = {"id": os.path.basename(md_path).split("_")[0], "stem": os.path.basename(md_path)[:-3]}
    spec["final"] = re.search(r"^\| Arquivo final \| `([^`]+)`", md, re.M).group(1)
    if m := re.search(r"Célula \(um frame\):\*\* (\d+)×(\d+)", s3):
        spec["cell"] = (int(m.group(1)), int(m.group(2)))
        g = re.search(r"Grade:\*\* (\d+) coluna\(s\) × (\d+) linha", s3)
        spec["grid"] = (int(g.group(1)), int(g.group(2)))
        o = re.search(r"Pivô \(`origin`\):\*\* \[(\d+), (\d+)\]", s3)
        spec["origin"] = (int(o.group(1)), int(o.group(2))) if o else (spec["cell"][0] // 2, spec["cell"][1] - 1)
    elif m := re.search(r"\*\*Tamanho:\*\* (\d+)×(\d+)", s3):
        spec["cell"] = (int(m.group(1)), int(m.group(2)))
        spec["grid"] = (1, 1)
        spec["origin"] = None
    elif m := re.search(r"Tamanho de cada arquivo:\*\* (\d+)×(\d+)", s3):
        spec["cell"] = (int(m.group(1)), int(m.group(2)))
        spec["files"] = re.findall(r"^\| `(assets/portraits/[^`]+)`", s3, re.M)
        spec["grid"] = (len(spec["files"]), 1)
        spec["origin"] = None
    spec["idle_bob"] = "Frame 2 do idle muda só" in md
    spec["palette"] = re.findall(r"`(#[0-9A-Fa-f]{6})`", section(md, 4))
    spec["json"] = None
    for k in range(7, 10):
        sec = section(md, k)
        if sec and "Salvar como" in sec:
            spec["json_path"] = re.search(r"Salvar como `([^`]+)`", sec).group(1)
            spec["json"] = json.loads(re.search(r"```json\n(.*?)```", sec, re.S).group(1))
    return spec


def frames_per_row(spec) -> list[int]:
    cols, rows = spec["grid"]
    if not spec["json"]:
        return [cols] * rows
    per = [0] * rows
    for a in spec["json"]["anims"].values():
        r = a["row"]
        if r < rows:
            per[r] = max(per[r], a.get("frames", 1))
    return [p or 1 for p in per]


def source(stem: str) -> str | None:
    try:
        man = json.load(open(f"{GEN}/manifest.json", encoding="utf-8"))
        for f in (man.get(stem) or {}).get("files") or []:
            if f.lower().endswith(".png") and os.path.exists(f):
                return f
    except (OSError, ValueError):
        pass
    vs = sorted(glob.glob(f"{GEN}/{stem}-v*.png"), key=lambda p: int(re.search(r"-v(\d+)\.png$", p).group(1)))
    if vs:
        return vs[-1]
    p = f"{GEN}/{stem}.png"
    return p if os.path.exists(p) else None


# ------------------------------------------------------------------ slicing
def detect_cells(fg, min_count=3, min_gap=8):
    """All cells in reading order, by background bands of at least min_gap px."""
    def runs(profile):
        on = profile >= min_count
        out, i = [], 0
        while i < len(on):
            if on[i]:
                j = i
                while j < len(on) and on[j]:
                    j += 1
                if out and i - out[-1][1] < min_gap:
                    out[-1][1] = j
                else:
                    out.append([i, j])
                i = j
            else:
                i += 1
        return out

    rows = runs(fg.sum(axis=1))
    cells = []
    for y0, y1 in rows:
        row = []
        for x0, x1 in runs(fg[y0:y1].sum(axis=0)):
            sub = fg[y0:y1, x0:x1]
            ys = np.where(sub.sum(axis=1) >= 2)[0]
            if len(ys) and sub.sum() > 50:
                row.append((x0, y0 + ys[0], x1, y0 + ys[-1] + 1))
        if row:
            cells.append(row)
    return cells


def assign(fg, per_row):
    """Map guide cells onto target (row, col). Exact grid match first, else reading order."""
    try:
        boxes = rl.slice_cells(fg, per_row)
        return {(r, c): b for r, row in enumerate(boxes) for c, b in enumerate(row)}, "grid"
    except ValueError:
        pass
    flat = [b for row in detect_cells(fg) for b in row]
    want = [(r, c) for r, n in enumerate(per_row) for c in range(n)]
    # drop the smallest blobs (stray labels, specks) if there are too many
    if len(flat) > len(want):
        keep = sorted(sorted(range(len(flat)), key=lambda i: -(flat[i][2] - flat[i][0]) * (flat[i][3] - flat[i][1]))[:len(want)])
        flat = [flat[i] for i in keep]
    return dict(zip(want, flat)), f"reading-order {len(flat)}/{len(want)}"


# ------------------------------------------------------------------ reduction
def reduce_sheet(spec, guide):
    pal = np.array([rl.hexrgb(h) for h in dict.fromkeys(h.upper() for h in spec["palette"])], dtype=np.int32)
    rgb, fg = rl.load(guide)
    idx = rl.snap(rgb, pal)
    per_row = frames_per_row(spec)
    cells, how = assign(fg, per_row)
    cw, ch = spec["cell"]
    origin = spec["origin"]
    based = origin is not None and origin[1] >= ch - 1          # pivot on the feet / base
    # one scale per sheet so every frame keeps the same pixel density
    margin = 0 if spec.get("files") is None and origin is None else 1
    scale = max(max((b[2] - b[0]) / (cw - margin), (b[3] - b[1]) / (ch - margin)) for b in cells.values())
    mono = set(map(tuple, pal.tolist())) <= {(255, 255, 255), (0, 0, 0), (255, 119, 200)}
    if not mono:  # white-on-black line art keeps thin strokes only without the mode filter
        idx = rl.mode_filter(idx, min(7, int(scale * 0.6)))  # bigger windows are slow and add little
    # accents: rare palette colors get a vote bonus so 1-px details survive
    counts = np.bincount(idx[fg], minlength=len(pal)).astype(float) + 1
    w = np.clip((np.median(counts) / counts) ** 0.3, 1.0, 2.5)
    black = next((i for i, c in enumerate(pal.tolist()) if c == [0, 0, 0]), None)
    white = next((i for i, c in enumerate(pal.tolist()) if c == [255, 255, 255]), None)
    pink = next((i for i, c in enumerate(pal.tolist()) if c == [255, 119, 200]), None)
    if mono:
        # white wins at ~1/3 coverage: strokes are thinner than one 1x pixel in most guides
        w = np.array([2.0 if c == [255, 255, 255] else 1.0 for c in pal.tolist()])
    athr = 0.35 if spec["id"][0] == "D" else 0.5  # bullets are tiny: keep thin handles and tips
    out = {}
    for (r, c), (x0, y0, x1, y1) in cells.items():
        m = np.zeros_like(fg)
        m[y0:y1, x0:x1] = fg[y0:y1, x0:x1]
        if based:
            ax, ay = rl.foot_anchor(fg, (x0, y0, x1, y1))
            ox, oy = origin[0], ch
        else:
            ax, ay = (x0 + x1) / 2, (y0 + y1) / 2
            ox, oy = (origin if origin else (cw / 2, ch / 2))
            if origin:
                ox, oy = ox + 0.5 * (cw % 2 == 1), oy + 0.5 * (ch % 2 == 1)
        if mono and black is not None and white is not None:
            cell = rl.reduce_mono(idx, m, white, black, ax, ay, scale, ox, oy, cw, ch, accent=pink, alpha_thr=athr)
        else:
            cell = rl.reduce_region(idx, m, ax, ay, scale, ox, oy, cw, ch, len(pal), w, athr)
        cell = rl.drop_specks(cell, 2)
        cell = rl.despeckle(cell, protect=tuple(int(i) for i in np.where(w > 1.6)[0]))
        if mono and black is not None and spec["id"][0] in "BCD":
            cell = rl.fill_holes(cell, black)  # characters and bullets: opaque black inside
        out[(r, c)] = cell
    if spec.get("idle_bob") and (0, 0) in out and (0, 1) in out:
        out[(0, 1)] = squash(out[(0, 0)])
    return pal, out, how, scale


def squash(cell):
    """Idle frame 2 = frame 1 squashed by one pixel (spec: it may only move 1-2 px).

    The least distinctive row in the middle band is dropped and everything above it moves down 1 px,
    so the feet stay on the baseline; the lower quarter gets 1 px wider by repeating the
    center column (the right half moves out 1 px), so the pivot does not move."""
    rows = np.where((cell >= 0).any(axis=1))[0]
    if len(rows) < 4:
        return cell.copy()
    top, bottom = rows[0], rows[-1]
    # drop the row (between 35% and 65% of the height) that repeats its neighbour most,
    # so faces and details survive the squash
    span = range(top + int((bottom - top) * 0.35), top + int((bottom - top) * 0.65))
    cut = min(span, key=lambda y: int((cell[y] != cell[y + 1]).sum()))
    out = np.full_like(cell, -1)
    out[1:cut + 1] = cell[0:cut]
    out[cut + 1:] = cell[cut + 1:]
    base = bottom - (bottom - top) // 4
    mid = cell.shape[1] // 2
    out[base:, mid + 1:] = out[base:, mid:-1].copy()
    return out


def run(spec, ctx):
    guide = source(spec["stem"])
    if guide is None:
        print(f"  no guide for {spec['stem']}")
        return
    pal, cells, how, scale = reduce_sheet(spec, guide)
    cw, ch = spec["cell"]
    print(f"  {spec['stem']}: {os.path.basename(guide)} {how}, scale {scale:.1f}")
    if spec.get("files"):
        strip = np.full((ch, cw * len(spec["files"])), -1, dtype=np.int32)
        for i, path in enumerate(spec["files"]):
            c = cells.get((0, i), np.full((ch, cw), -1, dtype=np.int32))
            strip[:, i * cw:(i + 1) * cw] = c
            ctx.save_img(Image.fromarray(rl.to_rgba(c, pal)), path)
        ctx.preview(guide, Image.fromarray(rl.to_rgba(strip, pal)), spec["stem"], 4)
        return
    cols, rows = spec["grid"]
    sheet = np.full((rows * ch, cols * cw), -1, dtype=np.int32)
    for (r, c), cell in cells.items():
        sheet[r * ch:(r + 1) * ch, c * cw:(c + 1) * cw] = cell
    img = Image.fromarray(rl.to_rgba(sheet, pal))
    ctx.save_img(img, spec["final"])
    if spec["json"]:
        ctx.save_json(spec["json"], spec["json_path"])
    zoom = max(2, min(8, 480 // max(img.width, img.height)))
    ctx.preview(guide, img, spec["stem"], zoom)


def all_specs(ids=None):
    for p in sorted(glob.glob(f"{DOCS}/[A-G][0-9][0-9]_*.md")):
        sid = os.path.basename(p).split("_")[0]
        if ids is None or sid in ids:
            yield parse_spec(p)
