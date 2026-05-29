"""Re-process deformed 1x Ruins sprites through ChatGPT image_gen with macro-pixel guides.

Generalises barky_macro_pipeline.py to every OW sheet, battle sprite and portrait set.
The generic geometry lives in the chatgpt-imagegen skill (scripts/macro_pixel.py); this
script owns what is project-specific: which cells go in which guide, the briefs, the
spec palettes and the invariants imposed after decoding (duplicate frames, empty cells,
locked anchor frames, opaque silhouettes).

  python tools/art_prompts/macro_reprocess.py prepare [IDS...]
      -> assets/_gen/ruins/<ID>-macro<N>[a|b]-guide.png/.json/.prompt.txt, -brief.txt, jobs.json
  python ~/.claude/skills/chatgpt-imagegen/scripts/batch.py --jobs assets/_gen/ruins/_macro/jobs.json \
      --out assets/_gen/ruins/_macro
  python tools/art_prompts/macro_reprocess.py finish [IDS...]
      -> <ID>-macro-native.png (or one PNG per portrait), <ID>-macro-compare2x.png, contact sheets

Nothing under assets/sprites or assets/portraits is ever written.
"""
from __future__ import annotations

import json
import shutil
import subprocess
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[2]
GEN = ROOT / "assets" / "_gen" / "ruins"
BATCH = GEN / "_macro"
CARDS = ROOT / "docs" / "art_prompts" / "ruins" / "cards"
ANCHOR = ROOT / "docs" / "art_prompts" / "ruins" / "style" / "04_pepe_anchor_native.png"
MACRO = Path.home() / ".claude" / "skills" / "chatgpt-imagegen" / "scripts" / "macro_pixel.py"
KEY = (255, 0, 255)

PEPE_PAL = "3D120E,5FA33A,3F7A2A,2E5E22,FFFFFF,141414,C8553D,A8432E,2F62D6,1F449E,F0A03A,67A4E0,4A80BC,2B2B2B"
BW = "FFFFFF,000000"

PEPE_LOOK = ("Pepe, a cartoon frog man in the classic sad-frog internet comic look, a tired 26-year-old gamer: "
             "wide lumpy head clearly wider than his shoulders with two round eye domes bulging above it, large white "
             "eyes half covered by heavy droopy green eyelids with big black pupils resting on the lower lid, a faint "
             "darker-green line under each eye, very wide two-part reddish-brown lips (upper #C8553D, lower #A8432E, "
             "dark line between), medium frog-green skin #5FA33A with #3F7A2A shading, plain blue crew-neck T-shirt "
             "#2F62D6 with one or two tiny orange nacho stains, light blue shorts, black flip-flops, green hands "
             "(no orange or yellow on hands or feet), short pudgy adult body, slouched.")

BATTLE_RULES = ("Undertale battle style, pure black #000000 and white #FFFFFF only: keep the white areas and black "
                "areas of Image 1 where they are (white fur, faces and outlines stay white; black clothes and interior "
                "fill stay black; never invert). The outer edge of the silhouette is a continuous white line, and the "
                "whole silhouette is fully opaque, with no magenta holes inside it.")

# Each target: asset(s), cell, palette, guides. A guide is either the whole sheet or a packed
# list of source cells (col,row) laid out in a row, so tall sheets keep an 8x scale.
TARGETS = {
    "A03_ow_furiell": dict(
        kind="ow", asset="assets/sprites/ow/furiell.png", cell=(32, 44),
        palette="2A1640,F4F0EA,CFC3DD,E6D8B8,C9A0DC,5E3A87,E6B422,9A9AAA,3B1F5C",
        subject=("Furiell, a tall motherly adult woman in a fluffy white goat fursuit: oversized soft costume head with "
                 "small cream horns and long floppy ears, a fabric fold at the neck, small gold half-moon reading "
                 "glasses, long purple dress under a lilac apron with a dark purple paw print, paws clasped, gentle "
                 "posture. On the back views a silver costume zipper (1 native pixel wide) runs down the middle of her back."),
        improve=("Keep horns, ears and head shape identical between frames; clean continuous 1-pixel dark outline "
                 "#2A1640; readable glasses and paw print; clear leg/hem motion in the step frames."),
        layout=["row 1: facing down - idle, left step, exact duplicate of idle, right step",
                "row 2: facing up (zipper visible) - idle, left step, exact duplicate of idle, right step",
                "row 3: facing left - idle, step, then two completely empty magenta cells",
                "row 4: facing right - idle, step, then two completely empty magenta cells",
                "row 5: facing down talking - mouth closed, mouth open, then two completely empty magenta cells"],
        dup=[((2, 0), (0, 0)), ((2, 1), (0, 1))],
        empty=[(2, 2), (3, 2), (2, 3), (3, 3), (2, 4), (3, 4)]),
    "A05_ow_floofy": dict(
        kind="ow", asset="assets/sprites/ow/floofy.png", cell=(24, 24),
        palette="2A1A08,FFD23F,E6A800,3FA34D,2C7A38,F4F0EA,CFC3DD,111111,FFFFFF,9A9AAA,000000",
        subject=("Floofy, a golden yellow flower with big rounded petals and a green stem with two leaves, whose face "
                 "is a tiny white baby-goat mascot costume head with shiny black plastic eyes and a silver zipper "
                 "running down the middle of the face from the top of the head to the chin (pull tab at the chin). "
                 "No mouth."),
        improve=("Rounder, evenly sized petals; the mascot head and zipper must read clearly at this size; "
                 "identical flower between frames except the described motion; stem base at the bottom centre of each cell."),
        layout=["row 1: idle - two frames swaying 1 native pixel side to side, then one completely empty magenta cell",
                "row 2: talk - two frames, no mouth: the head bobs 1 pixel and the zipper pull tab swings, then one empty cell",
                "row 3: sinister - face zipper pulled open revealing only pitch-black #000000 darkness and two small "
                "white dot eyes (frame 2: eyes 1 pixel smaller), then one empty cell",
                "row 4: sink - whole flower, sunk to half height, only petals visible above the ground line"],
        empty=[(2, 0), (2, 1), (2, 2)]),
    "A01_ow_pepe_frog": dict(
        kind="ow", asset="assets/sprites/ow/pepe_frog.png", cell=(24, 32), palette=PEPE_PAL,
        subject=PEPE_LOOK,
        improve=("Keep the existing face design (eyelids, two lips, eye domes) identical in every frame; only the "
                 "body and feet move. The body rises 1 pixel in step frames. Feet on the bottom row of each cell."),
        layout=["row 1: facing down - idle, left step, exact duplicate of idle, right step",
                "row 2: facing up (back of head with eye domes, back of T-shirt) - idle, left step, duplicate of idle, right step",
                "row 3: facing left (lips protrude forward) - idle, step, then two completely empty magenta cells",
                "row 4: facing right - mirror of row 3: idle, step, then two completely empty magenta cells"],
        dup=[((2, 0), (0, 0)), ((2, 1), (0, 1))],
        empty=[(2, 2), (3, 2), (2, 3), (3, 3)],
        anchor=True, mirror_rows=(2, 3), head_lock=(0, 1, 2), shirt="2F62D6"),
    "A02_ow_pepe_wakeup": dict(
        kind="ow", asset="assets/sprites/ow/pepe_wakeup.png", cell=(32, 32), palette=PEPE_PAL,
        subject=PEPE_LOOK,
        improve=("Same character and face design as the existing frames; clearer poses; feet or body resting on the "
                 "bottom row of each cell."),
        layout=["row 1: lying flat on his back, eyes closed, arms spread (3/4 top-down view), then two completely empty magenta cells",
                "row 2: sitting up rubbing one eye, kneeling and pushing himself up, standing facing the viewer slouched"],
        empty=[(1, 0), (2, 0)], wake_anchor=True),
    "B01_battle_barky": dict(
        kind="battle", asset="assets/sprites/battle/barky.png", cell=(60, 80), palette=BW + ",FF77C8",
        subject=("Barky, an adult man in a cheap gray wolf fursuit: huge pointed wolf ears, emo side-swept fringe over "
                 "one eye, studded faux-leather jacket, plastic chains, fingerless gloves, a fluffy wolf tail held on "
                 "with a big safety pin, a visible costume zipper. Dramatic try-hard edgy posture."),
        improve="Cleaner continuous lines, readable jacket studs and chains, no stray single pixels.",
        guides={
            "a": dict(cells=[(0, 0), (1, 0), (0, 1)], palette=BW, layout=[
                "cell 1: idle - edgy pose, arms crossed",
                "cell 2: idle frame 2 - same as cell 1, but the left ear folds 2 pixels and the tail swings to the other side",
                "cell 3: hurt - eyes squeezed shut, ears flattened, recoiling"]),
            "b": dict(cells=[(0, 2), (0, 3)], layout=[
                "cell 1: blush - blushing, looking aside, paws covering the zipper; pink cheeks #FF77C8 (the only "
                "non-black/white colour, only in this cell)",
                "cell 2: spared - relaxed, shy smile"]),
        },
        empty=[(1, 1), (1, 2), (1, 3)], pink_only=[(0, 2)]),
    "B02_battle_furiell": dict(
        kind="battle", asset="assets/sprites/battle/furiell.png", cell=(80, 110), palette=BW,
        subject=("Furiell, a motherly adult woman in a fluffy white goat fursuit (big soft costume head, small horns, "
                 "long floppy ears), half-moon glasses, apron with a paw print, framed from the knees up, centred."),
        improve="Cleaner face and glasses, smooth continuous outlines, solid black clothing areas, no stray pixels.",
        guides={
            "a": dict(cells=[(0, 0), (1, 0)], layout=[
                "cell 1: idle - paws together in front of the apron",
                "cell 2: idle frame 2 - breathing (shoulders 1 pixel higher) and a glint on the glasses"]),
            "b": dict(cells=[(0, 1), (0, 2)], layout=[
                "cell 1: hurt - eyes shut, recoiling, glasses crooked",
                "cell 2: sad - head lowered, one paw on her chest"]),
        },
        empty=[(1, 1), (1, 2)]),
    "B03_battle_floofy": dict(
        kind="battle", asset="assets/sprites/battle/floofy.png", cell=(40, 40), palette=BW + ",FFD23F",
        subject=("Floofy, a flower with big petals whose face is a little goat mascot costume head with a zipper "
                 "down the middle of the face and no mouth. Petals may be yellow #FFD23F as in Image 1."),
        improve="Even, rounder petals; clear zipper; identical flower between frames except the described motion.",
        layout=["row 1: idle, 2 frames (gentle sway)",
                "row 2: talking, 2 frames (head bob, zipper tab swings, still no mouth)",
                "row 3: sinister, 2 frames - zipper open, only pitch-black darkness and two white eyes"],
        empty=[]),
    "B04_battle_fursuit_dummy": dict(
        kind="battle", asset="assets/sprites/battle/fursuit_dummy.png", cell=(40, 60), palette=BW,
        subject="Fursuit Dummy, a training mannequin on a stand wearing a floppy fursuit head (a Training Dummy parody).",
        improve="Clean seams, readable fursuit head and stand, no stray pixels.",
        layout=["row 1: idle, 2 frames - standing still / the head fabric drooping 1 pixel",
                "row 2: hurt, 1 frame in the left cell - tilted, stuffing coming out of a seam; right cell completely empty"],
        empty=[(1, 1)]),
    "B05_battle_froggsona": dict(
        kind="battle", asset="assets/sprites/battle/froggsona.png", cell=(40, 40), palette=BW,
        subject="Froggsona, a little frog in a homemade frog fursuit with huge costume eyes, a cat mouth ':3' and a collar with a bell.",
        improve="Clear big eyes and ':3' mouth, clean outline, no stray pixels.",
        layout=["row 1: idle, 2 frames (small bob)",
                "row 2: hurt, 1 frame in the left cell (squashed, eyes shut); right cell completely empty"],
        empty=[(1, 1)]),
    "B06_battle_shysona": dict(
        kind="battle", asset="assets/sprites/battle/shysona.png", cell=(40, 40), palette=BW,
        subject=("Shysona, a shy little creature with moth wings and droopy antennae, hiding half of its teary face "
                 "behind a sketchbook."),
        improve="Readable wings, sketchbook and the one visible teary eye; clean shapes; no stray pixels.",
        layout=["row 1: idle, 2 frames (wings flutter slightly)",
                "row 2: hurt, 1 frame in the left cell (cowering behind the sketchbook); right cell completely empty"],
        empty=[(1, 1)]),
    "B07_battle_modsmal": dict(
        kind="battle", asset="assets/sprites/battle/modsmal.png", cell=(40, 30), palette=BW,
        subject=("Modsmal, a pudding-shaped jelly blob with a sleepy little face, a small shield badge on top and a "
                 "tiny jelly ban hammer."),
        improve="Smooth jelly outline, readable sleepy face, hammer and badge; no stray pixels.",
        layout=["row 1: idle, 2 frames (wobble)",
                "row 2: hurt, 1 frame in the left cell (splattered flat); right cell completely empty"],
        empty=[(1, 1)]),
    "B08_battle_lurkeye": dict(
        kind="battle", asset="assets/sprites/battle/lurkeye.png", cell=(50, 50), palette=BW,
        subject=("Lurkeye, a round creature that is basically one giant eye with tired eye bags, short legs, holding "
                 "a glowing phone, with a small '...' speech balloon beside it."),
        improve="Perfectly round body, clean big eye, readable phone; no stray pixels.",
        layout=["row 1: idle, 2 frames (blink / balloon dots)",
                "row 2: hurt, 1 frame in the left cell (eye squeezed shut, phone dropped); right cell completely empty"],
        empty=[(1, 1)]),
    "B10_battle_grasstoid": dict(
        kind="battle", asset="assets/sprites/battle/grasstoid.png", cell=(50, 60), palette=BW,
        subject=("Grasstoid, a living tuft of grass on a square patch of turf, with a lonely hopeful little face in "
                 "the middle and two small grass arms reaching for a hug."),
        improve="Cleaner, fewer and longer grass blades, readable face and arms; no stray pixels.",
        layout=["row 1: idle, 2 frames (blades sway)",
                "row 2: hurt, 1 frame in the left cell (wilted, flattened); right cell completely empty"],
        empty=[(1, 1)]),
    "C01_portraits_furiell": dict(
        kind="portrait", cell=(50, 50), palette=BW, cols=3,
        files=["furiell_neutral", "furiell_happy", "furiell_sad", "furiell_angry", "furiell_flustered"],
        subject=("Dialogue portraits of Furiell, a motherly adult woman in a fluffy white goat fursuit (small horns, "
                 "long floppy ears, half-moon glasses), head and shoulders, the head about 70% of the height, in "
                 "exactly the same position in every cell."),
        improve="Cleaner face lines and glasses, clearly different expressions, identical head outline in all five.",
        layout=["row 1: neutral (soft smile, half-closed eyes); happy (eyes closed ^^, open smile); sad (eyebrows up, mouth curving down)",
                "row 2: angry (narrow eyes, glint on the glasses, flat mouth); flustered (wide eyes, sweat drop, looking aside); "
                "third cell completely empty magenta"]),
    "C02_portraits_floofy": dict(
        kind="portrait", cell=(50, 50), palette=BW + ",FFD23F", cols=3,
        files=["floofy_cute", "floofy_smug", "floofy_sinister"], white_petals=True,
        subject=("Dialogue portraits of Floofy, a flower whose face is a little goat mascot costume head with a zipper "
                 "down the middle of the face, head and petals filling about 70% of the cell, same position in every "
                 "cell. Petals yellow #FFD23F with white outline, like the Floofy battle sprite."),
        improve="Even petals, clear zipper, clearly different expressions, identical outline in all three.",
        layout=["cute (big shiny mascot eyes, no mouth); smug (half-lidded eyes and a crooked one-line smirk, the "
                "only mouth Floofy ever has); sinister (zipper open: only darkness and two white eyes)"]),
    "C03_portraits_barky": dict(
        kind="portrait", cell=(50, 50), palette=BW + ",FF77C8", cols=3,
        files=["barky_edgy", "barky_embarrassed", "barky_happy"],
        subject=("Dialogue portraits of Barky, an adult man in a cheap gray wolf fursuit (huge pointed ears, emo "
                 "fringe over one eye, studded jacket collar), head and shoulders, head about 70% of the height, "
                 "same position in every cell."),
        improve="Cleaner fringe and ears, readable eyes, clearly different expressions.",
        layout=["edgy (dark sideways glance, fringe over the eye); embarrassed (blushing with pink cheeks #FF77C8, "
                "ears pulled back - pink only in this cell); happy (open smile, ears up)"],
        pink_only_files=["barky_embarrassed"]),
}


def rgba(path: Path) -> np.ndarray:
    return np.asarray(Image.open(path).convert("RGBA")).copy()


def hexes(p: str) -> list[tuple[int, int, int]]:
    return [tuple(int(h[i:i + 2], 16) for i in (0, 2, 4)) for h in p.split(",")]


def cell_get(a, cell, cr):
    (cw, ch), (c, r) = cell, cr
    return a[r * ch:(r + 1) * ch, c * cw:(c + 1) * cw]


def cell_set(a, cell, cr, v):
    (cw, ch), (c, r) = cell, cr
    a[r * ch:(r + 1) * ch, c * cw:(c + 1) * cw] = v


def guides_of(tid: str, t: dict) -> dict:
    """{suffix: (base RGBA array, cell list or None)} where cell list maps packed slot k -> source cell."""
    cw, ch = t["cell"]
    if t["kind"] == "portrait":
        tiles = [rgba(ROOT / "assets" / "portraits" / f"{f}.png") for f in t["files"]]
        cols = t["cols"]
        rows = -(-len(tiles) // cols)
        base = np.zeros((rows * ch, cols * cw, 4), np.uint8)
        for k, tile in enumerate(tiles):
            cell_set(base, (cw, ch), (k % cols, k // cols), tile)
        return {"": (base, None)}
    sheet = rgba(ROOT / t["asset"])
    if "guides" not in t:
        return {"": (sheet, None)}
    out = {}
    for suf, g in t["guides"].items():
        base = np.zeros((ch, cw * len(g["cells"]), 4), np.uint8)
        for k, cr in enumerate(g["cells"]):
            cell_set(base, (cw, ch), (k, 0), cell_get(sheet, (cw, ch), cr))
        out[suf] = (base, g["cells"])
    return out


def brief(tid, t, suf, geometry: str, scale: int, grid: str) -> str:
    g = t.get("guides", {}).get(suf, {})
    layout = g.get("layout", t.get("layout", []))
    pal = ", ".join("#" + h for h in g.get("palette", t["palette"]).split(","))
    kind = {"ow": "overworld sprite sheet", "battle": "battle sprite sheet", "portrait": "dialogue portrait sheet"}[t["kind"]]
    lines = [
        "Use case: precise-object-edit",
        f"Asset type: {scale}x macro-pixel {kind} for nUndertale (an Undertale parody), decoded deterministically back to native game pixels.",
        "",
        "Input images:",
        f"- Image 1 is the EDIT TARGET and the exact macro-pixel canvas. Keep its exact dimensions, magenta margins, "
        f"sprite positions, {grid} cell layout, empty cells and poses.",
        "- Image 2 is the style card: palette, native pixel density and rules only. Never draw the card, its labels, "
        "its grid or any frog shown on it.",
        "",
        f"Primary request: redraw and improve the sprite inside Image 1, treating every {scale}x{scale} square as one "
        "indivisible native pixel. The current sprite is a noisy reduction: broken lines, uneven shapes and scattered "
        "single pixels. Make it clean and readable at native size: fewer, deliberate pixels; continuous outlines; "
        "no stray dots. " + t["improve"],
        "",
        "Subject: " + t["subject"],
    ]
    if t["kind"] in ("battle", "portrait"):
        lines += ["", BATTLE_RULES]
    lines += ["", geometry.rstrip(), "", "Layout (left to right, top to bottom):"] + [f"- {s}" for s in layout]
    lines += ["", f"Palette only: {pal}, plus #FF00FF for background and empty native pixels. No other colours."]
    lines += ["", "Constraints: change only the pixel design inside the already occupied cells. One flat colour per "
              f"{scale}x{scale} block. Keep each figure's base on the bottom of its cell and centred like Image 1. "
              "No grid lines, labels, text (except words named above), watermark, extra characters, background "
              "shadows, gradients, blur, antialiasing, glow, dithering noise or semitransparency."]
    return "\n".join(lines) + "\n"


def prepare(ids):
    GEN.mkdir(parents=True, exist_ok=True)
    BATCH.mkdir(parents=True, exist_ok=True)
    jobs_path = BATCH / "jobs.json"
    jobs = json.loads(jobs_path.read_text()) if jobs_path.exists() else []
    names = {j["name"] for j in jobs}
    stems_path = BATCH / "guides.json"
    stems = json.loads(stems_path.read_text()) if stems_path.exists() else {}
    for tid in ids:
        t = TARGETS[tid]
        for suf, (base, cells) in guides_of(tid, t).items():
            tmp = GEN / f"{tid}-macro-base{suf}.png"
            Image.fromarray(base).save(tmp)
            probe = GEN / f"{tid}-macro-probe.png"
            cw, ch = t["cell"]
            r = subprocess.run([sys.executable, str(MACRO), "prepare", "--base", str(tmp), "--out", str(probe),
                                "--cell", f"{cw}x{ch}"], capture_output=True, text=True, check=True)
            n = json.loads(r.stdout)["scale"]
            stem = f"{tid}-macro{n}{suf}"
            for ext in (".png", ".json", ".prompt.txt"):
                probe.with_suffix(ext).replace(GEN / f"{stem}-guide{ext}")
            stems.setdefault(tid, {})[suf] = stem
            tmp.replace(GEN / f"{stem}-before.png")
            geometry = (GEN / f"{stem}-guide.prompt.txt").read_text()
            gcols, grows = base.shape[1] // cw, base.shape[0] // ch
            text = brief(tid, t, suf, geometry, n, f"{gcols}x{grows}")
            (GEN / f"{stem}-brief.txt").write_text(text, encoding="utf-8")
            card = CARDS / f"{tid}.png"
            job = {"name": f"{stem}-source", "prompt_file": str(GEN / f"{stem}-brief.txt"),
                   "images": [str(GEN / f"{stem}-guide.png"), str(card)]}
            if job["name"] not in names:
                jobs.append(job)
                names.add(job["name"])
            print(stem, f"scale {n}", f"{base.shape[1]}x{base.shape[0]}")
    jobs_path.write_text(json.dumps(jobs, indent=1))
    stems_path.write_text(json.dumps(stems, indent=1))
    print(jobs_path, len(jobs), "jobs")


def opaque_fill(a: np.ndarray, cell, black=(0, 0, 0)):
    """Battle/portrait rule: no transparent holes inside the silhouette (fill enclosed holes black)."""
    from collections import deque
    cw, ch = cell
    H, W = a.shape[:2]
    for r in range(H // ch):
        for c in range(W // cw):
            sub = a[r * ch:(r + 1) * ch, c * cw:(c + 1) * cw]
            if not sub[..., 3].any():
                continue
            seen = np.zeros((ch, cw), bool)
            q = deque((y, x) for y in range(ch) for x in range(cw)
                      if (y in (0, ch - 1) or x in (0, cw - 1)) and sub[y, x, 3] == 0)
            for y, x in q:
                seen[y, x] = True
            while q:
                y, x = q.popleft()
                for yy, xx in ((y + 1, x), (y - 1, x), (y, x + 1), (y, x - 1)):
                    if 0 <= yy < ch and 0 <= xx < cw and not seen[yy, xx] and sub[yy, xx, 3] == 0:
                        seen[yy, xx] = True
                        q.append((yy, xx))
            holes = (sub[..., 3] == 0) & ~seen
            sub[holes] = (*black, 255)


def recolor(sub, src, dst):
    m = (sub[..., 3] > 0) & (sub[..., :3] == src).all(-1)
    sub[m, :3] = dst


def decode_guide(stem: str, palette: str) -> np.ndarray | None:
    manifest = json.loads((BATCH / "manifest.json").read_text(encoding="utf-8"))
    rep = manifest.get(f"{stem}-source", {})
    if not rep.get("ok"):
        print(stem, "no generated source yet")
        return None
    src = GEN / f"{stem}-source.png"
    shutil.copyfile(rep["files"][0], src)
    r = subprocess.run([sys.executable, str(MACRO), "decode", str(src), "--meta", str(GEN / f"{stem}-guide.json"),
                        "--out", str(GEN / f"{stem}-native.png"), "--palette", palette,
                        "--compare", str(GEN / f"{stem}-before.png")], capture_output=True, text=True)
    if r.returncode:
        print(stem, "decode failed:", r.stdout, r.stderr)
        return None
    rep = json.loads(r.stdout)
    (GEN / f"{stem}-decode.json").write_text(json.dumps(rep, indent=1))
    print(stem, {k: rep.get(k) for k in ("source_size", "rescaled", "grid_shift_px", "block_purity_mean",
                                          "blocks_clean_90", "changed_vs_compare")})
    return rgba(GEN / f"{stem}-native.png")


def line_vote(stem: str, palette: str, line=(255, 255, 255), thr=0.35) -> np.ndarray:
    """Re-vote a decoded B/W guide over the WHOLE block, keeping the line colour when it covers >= thr.
    The model draws Undertale outlines thinner than one macro block, so macro_pixel's interior vote
    (20-80%) drops them in some blocks and outlines come out dotted. Uses the same grid shift."""
    g = json.loads((GEN / f"{stem}-guide.json").read_text())
    (W, H), n, (cw, ch), (ox, oy) = g["native"], g["scale"], g["canvas"], g["offset"]
    dx, dy = json.loads((GEN / f"{stem}-decode.json").read_text())["grid_shift_px"]
    img = np.asarray(Image.open(GEN / f"{stem}-source.png").convert("RGB")).astype(np.int32)
    sc = img.shape[1] / cw
    pal = [c for c in hexes(palette) if c != KEY] + [KEY]
    li = pal.index(line)
    P = np.asarray(pal, np.int32)
    idx = np.empty(img.shape[:2], np.uint8)
    for y in range(0, img.shape[0], 256):
        idx[y:y + 256] = ((img[y:y + 256, :, None] - P[None, None]) ** 2).sum(-1).argmin(-1)
    gx, gy, st = ox * sc + dx, oy * sc + dy, n * sc
    out = np.zeros((H, W, 4), np.uint8)
    for j in range(H):
        for i in range(W):
            blk = idx[int(gy + j * st):int(gy + (j + 1) * st), int(gx + i * st):int(gx + (i + 1) * st)].ravel()
            c = np.bincount(blk, minlength=len(pal)) / max(1, len(blk))
            k = li if c[li] >= thr else int(c.argmax())
            if k != len(pal) - 1:
                out[j, i, :3], out[j, i, 3] = pal[k], 255
    Image.fromarray(out).save(GEN / f"{stem}-native-lines.png")
    return out


def finish_one(tid) -> list[tuple[str, np.ndarray, np.ndarray]] | None:
    """Returns [(label, before, after)] for the contact sheet."""
    t = TARGETS[tid]
    cell = t["cell"]
    cw, ch = cell
    guides = guides_of(tid, t)
    decoded = {}
    stems = json.loads((BATCH / "guides.json").read_text()).get(tid, {})
    for suf, (base, cells) in guides.items():
        if suf not in stems:
            return None
        out = decode_guide(stems[suf], t["palette"])
        if out is None:
            return None
        if t["kind"] in ("battle", "portrait"):
            out = line_vote(stems[suf], t["palette"])
        decoded[suf] = (out, cells)
    pink = (255, 119, 200)

    if t["kind"] == "portrait":
        out, _ = decoded[""]
        res = []
        for k, f in enumerate(t["files"]):
            tile = cell_get(out, cell, (k % t["cols"], k // t["cols"])).copy()
            opaque_fill(tile, cell)
            if pink in hexes(t["palette"]) and f not in t.get("pink_only_files", []):
                recolor(tile, pink, (0, 0, 0))
            if t.get("white_petals"):
                # Spec C02: yellow only if B03's petals are yellow; B03's are white, so default to white
                # and keep the yellow render as an alternative.
                Image.fromarray(tile).save(GEN / f"{tid}-macro-yellow-{f}.png")
                recolor(tile, (255, 210, 63), (255, 255, 255))
            Image.fromarray(tile).save(GEN / f"{tid}-macro-{f}.png")
            res.append((f, rgba(ROOT / "assets" / "portraits" / f"{f}.png"), tile))
        return res

    before = rgba(ROOT / t["asset"])
    if "guides" in t:
        sheet = np.zeros_like(before)
        for suf, (out, cells) in decoded.items():
            for k, cr in enumerate(cells):
                cell_set(sheet, cell, cr, cell_get(out, cell, (k, 0)))
    else:
        sheet = decoded[""][0].copy()
    if t.get("anchor"):
        cell_set(sheet, cell, (0, 0), rgba(ANCHOR))
    for r in t.get("head_lock", ()):
        lock_head(sheet, cell, r, hexes(t["shirt"])[0])
    for dst, src in t.get("dup", []):
        cell_set(sheet, cell, dst, cell_get(sheet, cell, src))
    if t.get("mirror_rows"):
        a, b = t["mirror_rows"]
        for c in range(sheet.shape[1] // cw):
            cell_set(sheet, cell, (c, b), cell_get(sheet, cell, (c, a))[:, ::-1])
    for cr in t.get("empty", []):
        cell_set(sheet, cell, cr, 0)
    if t.get("wake_anchor"):
        last = np.zeros((ch, cw, 4), np.uint8)
        anc = rgba(ANCHOR)
        last[ch - anc.shape[0]:, (cw - anc.shape[1]) // 2:(cw - anc.shape[1]) // 2 + anc.shape[1]] = anc
        cell_set(sheet, cell, (2, 1), last)
    if t["kind"] == "battle":
        opaque_fill(sheet, cell)
        if pink in hexes(t["palette"]):
            for r in range(sheet.shape[0] // ch):
                for c in range(sheet.shape[1] // cw):
                    if (c, r) not in t.get("pink_only", []):
                        recolor(cell_get(sheet, cell, (c, r)), pink, (0, 0, 0))
    used = {tuple(p[:3]) for p in sheet.reshape(-1, 4) if p[3]}
    assert sheet.shape == before.shape and used <= set(hexes(t["palette"])), (tid, used - set(hexes(t["palette"])))
    Image.fromarray(sheet).save(GEN / f"{tid}-macro-native.png")
    save_compare(GEN / f"{tid}-macro-compare2x.png", [(tid, before, sheet)])
    return [(tid, before, sheet)]


def lock_head(sheet, cell, row, shirt):
    """Spec A01: copy the col-0 head into every frame of the row; only body and feet move.
    The head is everything above the first T-shirt row; it may sit 1 px higher in step frames."""
    cw, ch = cell
    ref = cell_get(sheet, cell, (0, row))
    ys = [y for y in range(ch) if (ref[y, :, :3] == shirt).all(-1).any()]
    if not ys:
        return
    hh = ys[0]
    head = ref[:hh]
    for c in range(1, sheet.shape[1] // cw):
        sub = cell_get(sheet, cell, (c, row))
        if not sub[..., 3].any():
            continue
        best = min((-1, 0, 1), key=lambda dy: (sub[max(0, dy):hh + dy] != head[max(0, -dy):]).any(-1).sum())
        sub[:hh + best] = 0
        sub[max(0, best):hh + best] = head[max(0, -best):]


def checker(w, h, s=8):
    bg = Image.new("RGB", (w, h), (52, 52, 60))
    d = ImageDraw.Draw(bg)
    for y in range(0, h, s):
        for x in range(0, w, s):
            if (x // s + y // s) % 2:
                d.rectangle((x, y, x + s - 1, y + s - 1), fill=(68, 68, 78))
    return bg


def save_compare(path: Path, rows, z=2, dark=False):
    pad, label_h = 10, 14
    font = ImageFont.load_default()
    widths = [b.shape[1] * z for _, b, _ in rows]
    heights = [b.shape[0] * z for _, b, _ in rows]
    W = max(widths) * 2 + pad * 3
    H = sum(h + label_h + pad for h in heights) + pad
    out = Image.new("RGB", (W, H), (24, 24, 30))
    d = ImageDraw.Draw(out)
    y = pad
    for (label, b, a), w, h in zip(rows, widths, heights):
        d.text((pad, y), f"{label}: OLD", fill="white", font=font)
        d.text((pad * 2 + max(widths), y), "NEW (macro-pixel)", fill=(140, 255, 140), font=font)
        y += label_h
        for k, img in enumerate((b, a)):
            big = Image.fromarray(img).resize((w, h), Image.NEAREST)
            bg = Image.new("RGB", (w, h), (0, 0, 0)) if dark else checker(w, h)
            bg.paste(big, mask=big.getchannel("A"))
            out.paste(bg, (pad + k * (max(widths) + pad), y))
        y += h + pad
    out.save(path)


def save_portrait_grid(path: Path, rows, z=2):
    """Portraits per character: an OLD row over a NEW row, on black like the dialogue box."""
    groups = {}
    for label, b, a in rows:
        groups.setdefault(label.split("_")[0], []).append((label, b, a))
    pad, lh, side = 10, 14, 50 * z
    cols = max(len(g) for g in groups.values())
    W = pad + cols * (side + pad) + 40
    H = pad + sum(2 * (side + lh + pad) for _ in groups)
    out = Image.new("RGB", (W, H), (24, 24, 30))
    d = ImageDraw.Draw(out)
    font = ImageFont.load_default()
    y = pad
    for g in groups.values():
        for k, tag in enumerate(("OLD", "NEW")):
            for i, (label, b, a) in enumerate(g):
                x = pad + i * (side + pad)
                d.text((x, y), f"{label.split('_', 1)[1]} {tag}", fill="white" if k == 0 else (140, 255, 140), font=font)
                big = Image.fromarray((b, a)[k]).resize((side, side), Image.NEAREST)
                bg = Image.new("RGB", (side, side))
                bg.paste(big, mask=big.getchannel("A"))
                out.paste(bg, (x, y + lh))
            y += side + lh + pad
    out.save(path)


def finish(ids):
    sheets = {"ow": [], "battle": [], "portrait": []}
    for tid in ids:
        rows = finish_one(tid)
        if rows:
            sheets[TARGETS[tid]["kind"]] += rows
    # Reply Guy was already re-processed (macro12, no lines) in the first test; show it with the battle set.
    rg = GEN / "B09_battle_reply_guy-macro12-native.png"
    if rg.exists():
        a = rgba(rg)
        cell_set(a, (40, 56), (1, 1), 0)
        opaque_fill(a, (40, 56))
        Image.fromarray(a).save(GEN / "B09_battle_reply_guy-macro-native.png")
        sheets["battle"].append(("B09_battle_reply_guy (12x test)", rgba(ROOT / "assets/sprites/battle/reply_guy.png"), a))
    if sheets["portrait"]:
        save_portrait_grid(GEN / "_macro_contact_portrait_2x.png", sheets.pop("portrait"))
        print(GEN / "_macro_contact_portrait_2x.png")
    for kind, rows in sheets.items():
        if rows:
            save_compare(GEN / f"_macro_contact_{kind}_2x.png", rows, dark=kind != "ow")
            print(GEN / f"_macro_contact_{kind}_2x.png")


def retouch(tid, suf, correction, extra=()):
    """Targeted retouch (chatgpt-imagegen skill): previous attempt attached LAST, one concrete correction.
    Writes a new stem <stem>r and points guides.json at it; the first attempt stays on disk."""
    stems = json.loads((BATCH / "guides.json").read_text())
    stem = stems[tid][suf]
    new = stem + "r"
    for tail in ("-guide.png", "-guide.json", "-before.png"):
        shutil.copyfile(GEN / f"{stem.rstrip('r')}{tail}", GEN / f"{new}{tail}")
    text = (GEN / f"{stem}-brief.txt").read_text(encoding="utf-8").rstrip()
    refs = "".join(f"\nImage {3 + k} is a style reference from the same character: {why}" for k, (_, why) in enumerate(extra))
    text += (f"{refs}\n\nThe LAST attached image is the previous attempt: keep its layout, poses and design, "
             f"change only: {correction}\n")
    (GEN / f"{new}-brief.txt").write_text(text, encoding="utf-8")
    jobs = json.loads((BATCH / "jobs.json").read_text())
    images = [str(GEN / f"{new}-guide.png"), str(CARDS / f"{tid}.png")] + [str(GEN / e) for e, _ in extra]
    images.append(str(GEN / f"{stem}-source.png"))
    jobs = [j for j in jobs if j["name"] != f"{new}-source"] + [
        {"name": f"{new}-source", "prompt_file": str(GEN / f"{new}-brief.txt"), "images": images}]
    (BATCH / "jobs.json").write_text(json.dumps(jobs, indent=1))
    stems[tid][suf] = new
    (BATCH / "guides.json").write_text(json.dumps(stems, indent=1))
    print(new)


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in ("prepare", "finish"):
        sys.exit(__doc__)
    ids = sys.argv[2:] or list(TARGETS)
    (prepare if sys.argv[1] == "prepare" else finish)(ids)


if __name__ == "__main__":
    main()
