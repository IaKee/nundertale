# Visual identity kit for the art prompts: style images, per-sheet templates, style guide and project instructions.
import os
from PIL import Image, ImageDraw, ImageFont, ImageFilter

FONT_PATH = os.path.join("assets", "fonts", "8bitoperator.ttf")

def font(size):
    try:
        return ImageFont.truetype(FONT_PATH, size)
    except OSError:
        return ImageFont.load_default()

def hex_rgb(h):
    h = h.strip().lstrip("#")
    return (int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16))

# ---------------------------------------------------------------- anchor sprite (Pepe, front idle, 24x32 cell)
PEPE_ANCHOR = [
    "........................",
    "....OOOOO......OOOOO....",
    "...OGGGGGO....OGGGGGO...",
    "..OGGGGGGGOOOOGGGGGGGO..",
    ".OGOOOOOGGGGGGGGOOOOOGO.",
    ".OGOWWWOGGGGGGGGOWWWOGO.",
    ".OGOWPPOGGGGGGGGOWPPOGO.",
    ".OGGOOOGGGGGGGGGGOOOGGO.",
    ".OGGdddGGGGGGGGGGdddGGO.",
    ".OGGOOOOOOOOOOOOOOOOGGO.",
    ".OGOLLLLLLLLLLLLLLLLOGO.",
    ".OOLLOOOOOOOOOOOOOOLLOO.",
    ".OGOOllllllllllllllOOGO.",
    "..OGGOOOOOOOOOOOOOOGGO..",
    "...OGGGGGGGGGGGGGGGGO...",
    "....OOOOOOOOOOOOOOOO....",
    ".....OBBBBBBBBBBBBO.....",
    "....OBBBBBBBBBBBBBBO....",
    "...OBOBBBNBBBBBBBOBO....",
    "...OBObBBBBBBBNBbOBO....",
    "...OBObBBBBBBBBBbOBO....",
    "...OGObbbbbbbbbbbOGO....",
    "...OGOOOOOOOOOOOOOGO....",
    ".....OSSSSSSSSSSSO......",
    ".....OSSSSSsSSSSSO......",
    ".....OsssssOsssssO......",
    "......OGGGO.OGGGO.......",
    "......OGGGO.OGGGO.......",
    "......OGgGO.OGgGO.......",
    ".....OGGGGO.OGGGGO......",
    ".....OFFFFO.OFFFFO......",
    "......OOOO...OOOO.......",
]
PEPE_COLORS = {"O": "3D120E", "G": "5FA33A", "g": "3F7A2A", "d": "2E5E22", "W": "FFFFFF", "P": "141414", "L": "C8553D",
               "l": "A8432E", "B": "2F62D6", "b": "1F449E", "N": "F0A03A", "S": "67A4E0", "s": "4A80BC", "F": "2B2B2B"}

def anchor_sprite():
    im = Image.new("RGBA", (24, 32), (0, 0, 0, 0))
    for y, row in enumerate(PEPE_ANCHOR):
        for x, ch in enumerate(row):
            if ch != ".":
                im.putpixel((x, y), hex_rgb(PEPE_COLORS[ch]) + (255,))
    return im

def up(im, k):
    return im.resize((im.width * k, im.height * k), Image.NEAREST)

def text(d, xy, s, size=16, fill=(20, 20, 20)):
    d.text(xy, s, font=font(size), fill=fill)

# ---------------------------------------------------------------- style images
def img_palettes(sections, path):
    sw, gap, per_row = 44, 10, 9
    rows = []
    for title, items in sections:
        rows.append(("title", title))
        for i in range(0, len(items), per_row):
            rows.append(("sw", items[i:i + per_row]))
    H = 70 + sum(30 if k == "title" else sw + 46 for k, _ in rows) + 20
    W = 40 + per_row * (sw + 76)
    im = Image.new("RGB", (W, H), (245, 245, 245))
    d = ImageDraw.Draw(im)
    text(d, (20, 16), "nUNDERTALE - RUINS PALETTES (use only these hex colors)", 24)
    y = 70
    for kind, v in rows:
        if kind == "title":
            text(d, (20, y), v, 18, (60, 30, 100)); y += 30
            continue
        x = 20
        for label, hx in v:
            d.rectangle([x, y, x + sw, y + sw], fill=hex_rgb(hx), outline=(40, 40, 40))
            text(d, (x, y + sw + 4), "#" + hx.strip().lstrip("#").upper(), 14)
            text(d, (x, y + sw + 22), label[:18], 12, (90, 90, 90))
            x += sw + 76
        y += sw + 46
    im.save(path)

def _pixel_circle(r, fg, bg=None):
    n = 2 * r + 1
    im = Image.new("RGBA", (n, n), bg or (0, 0, 0, 0))
    for y in range(n):
        for x in range(n):
            dx, dy = x - r, y - r
            dd = dx * dx + dy * dy
            if dd <= r * r:
                im.putpixel((x, y), fg)
    return im

def img_rules(path):
    W, H = 1180, 1010
    im = Image.new("RGB", (W, H), (245, 245, 245))
    d = ImageDraw.Draw(im)
    text(d, (20, 14), "nUNDERTALE - RENDERING RULES (GOOD = do this, BAD = never)", 24)
    pepe = anchor_sprite()
    good, bad = (20, 140, 40), (200, 30, 30)

    def panel(x, y, title):
        d.rectangle([x, y, x + 560, y + 300], outline=(160, 160, 160))
        text(d, (x + 10, y + 8), title, 16, (40, 40, 40))

    # 1 pixels
    x, y = 20, 60
    panel(x, y, "1. HARD SQUARE PIXELS, NO ANTI-ALIASING")
    c = _pixel_circle(9, (124, 255, 58, 255))
    ring = Image.new("RGBA", c.size, (0, 0, 0, 0))
    for yy in range(c.height):
        for xx in range(c.width):
            if c.getpixel((xx, yy))[3]:
                nb = [(xx + 1, yy), (xx - 1, yy), (xx, yy + 1), (xx, yy - 1)]
                edge = any(not (0 <= a < c.width and 0 <= b < c.height) or not c.getpixel((a, b))[3] for a, b in nb)
                ring.putpixel((xx, yy), (61, 18, 14, 255) if edge else (124, 255, 58, 255))
    im.paste(up(ring, 10), (x + 30, y + 50), up(ring, 10))
    smooth = Image.new("RGBA", (190, 190), (0, 0, 0, 0))
    sd = ImageDraw.Draw(smooth)
    sd.ellipse([4, 4, 186, 186], fill=(124, 255, 58, 255), outline=(61, 18, 14, 255), width=10)
    smooth = smooth.filter(ImageFilter.GaussianBlur(5))
    im.paste(smooth, (x + 330, y + 50), smooth)
    text(d, (x + 60, y + 255), "GOOD: visible pixel blocks", 14, good)
    text(d, (x + 320, y + 255), "BAD: smooth, blurry edge", 14, bad)

    # 2 outline + shading (anchor vs blurred)
    x, y = 600, 60
    panel(x, y, "2. 1-PX DARK OUTLINE + 1 SHADOW TONE")
    big = up(pepe, 7)
    im.paste(big, (x + 40, y + 40), big)
    blur = pepe.resize((24 * 7, 32 * 7), Image.BICUBIC).filter(ImageFilter.GaussianBlur(2))
    im.paste(blur, (x + 330, y + 40), blur)
    text(d, (x + 30, y + 272), "GOOD: anchor sprite", 14, good)
    text(d, (x + 320, y + 272), "BAD: soft / painted", 14, bad)

    # 3 battle fill
    x, y = 20, 380
    panel(x, y, "3. BATTLE: WHITE LINES + OPAQUE BLACK FILL")
    for ox in (x + 40, x + 320):
        for gx in range(0, 210, 21):
            d.line([ox + gx, y + 50, ox + gx, y + 260], fill=(0, 120, 0))
        for gy in range(0, 210, 21):
            d.line([ox, y + 50 + gy, ox + 210, y + 50 + gy], fill=(0, 120, 0))
    blob = _pixel_circle(10, (255, 255, 255, 255))
    filled = blob.copy()
    hollow = Image.new("RGBA", blob.size, (0, 0, 0, 0))
    for yy in range(blob.height):
        for xx in range(blob.width):
            if blob.getpixel((xx, yy))[3]:
                nb = [(xx + 1, yy), (xx - 1, yy), (xx, yy + 1), (xx, yy - 1)]
                edge = any(not (0 <= a < blob.width and 0 <= b < blob.height) or not blob.getpixel((a, b))[3] for a, b in nb)
                filled.putpixel((xx, yy), (255, 255, 255, 255) if edge else (0, 0, 0, 255))
                if edge:
                    hollow.putpixel((xx, yy), (255, 255, 255, 255))
    for eye in ((7, 8), (13, 8)):
        filled.putpixel(eye, (255, 255, 255, 255)); hollow.putpixel(eye, (255, 255, 255, 255))
    im.paste(up(filled, 9), (x + 50, y + 60), up(filled, 9))
    im.paste(up(hollow, 9), (x + 330, y + 60), up(hollow, 9))
    text(d, (x + 40, y + 272), "GOOD: fill hides the grid", 14, good)
    text(d, (x + 320, y + 272), "BAD: grid shows through", 14, bad)

    # 4 edges
    x, y = 600, 380
    panel(x, y, "4. FULLY OPAQUE EDGES, NO HALO")
    d.rectangle([x + 30, y + 40, x + 270, y + 290], fill=(0x3B, 0x1F, 0x5C))
    d.rectangle([x + 310, y + 40, x + 550, y + 290], fill=(0x3B, 0x1F, 0x5C))
    im.paste(big, (x + 66, y + 44), big)
    a = big.getchannel("A").filter(ImageFilter.MaxFilter(9)).filter(ImageFilter.GaussianBlur(3))
    white =Image.new("RGBA", big.size, (255, 255, 255, 0)); white.putalpha(a.point(lambda v: int(v * 0.6)))
    im.paste(white, (x + 346, y + 44), white)
    im.paste(big, (x + 346, y + 44), big)
    text(d, (x + 40, y + 292 - 20), "GOOD", 14, (255, 255, 255))
    text(d, (x + 320, y + 292 - 20), "BAD: white fringe", 14, (255, 160, 160))

    # 5 layout
    x, y = 20, 700
    d.rectangle([x, y, x + 1140, y + 290], outline=(160, 160, 160))
    text(d, (x + 10, y + 8), "5. SPRITE SHEET: ONE ROW PER ANIMATION, FRAMES START AT COLUMN 0, EQUAL CELLS, NO GAPS", 16, (40, 40, 40))
    cw, ch = 60, 70
    labels = [("row 0  walk_down", 4), ("row 1  walk_up", 4), ("row 2  walk_left", 2)]
    for r, (lab, n) in enumerate(labels):
        yy = y + 40 + r * (ch + 8)
        text(d, (x + 20, yy + 25), lab, 16)
        for c in range(4):
            xx = x + 240 + c * cw
            used = c < n
            d.rectangle([xx, yy, xx + cw - 1, yy + ch - 1], fill=(205, 205, 205) if used else (255, 255, 255), outline=(90, 90, 90))
            if used:
                d.line([xx, yy + ch - 6, xx + cw - 1, yy + ch - 6], fill=(220, 0, 0), width=2)
            else:
                d.line([xx, yy, xx + cw - 1, yy + ch - 1], fill=(220, 220, 220))
    text(d, (x + 520, y + 50), "- every cell has the same size (see the ficha)", 16)
    text(d, (x + 520, y + 80), "- unused cells stay transparent", 16)
    text(d, (x + 520, y + 110), "- variants/states go in NEW ROWS, never in", 16)
    text(d, (x + 520, y + 132), "  later columns of the same row", 16)
    text(d, (x + 520, y + 162), "- feet / base touch the red pivot line in", 16)
    text(d, (x + 520, y + 184), "  every frame", 16)
    text(d, (x + 520, y + 214), "- background: transparent or flat #FF00FF", 16)
    im.save(path)

def img_scale(path):
    W, H = 1180, 820
    im = Image.new("RGB", (W, H), (245, 245, 245))
    d = ImageDraw.Draw(im)
    text(d, (20, 14), "nUNDERTALE - SCALE LINEUP (sizes in native art pixels; the game draws everything at 2x)", 22)
    # overworld at 4x
    k = 4
    base = 300
    text(d, (20, 60), "OVERWORLD (shown at 4x) - colorful, 3/4 top-down", 18, (60, 30, 100))
    d.rectangle([20, 90, W - 20, base + 30], fill=(0x3B, 0x1F, 0x5C))
    items = [("Pepe 20x30", (24, 32), "anchor"), ("Barky 22x32", (22, 32), None), ("Furiell 26x40", (26, 40), None),
             ("Floofy 20x20", (20, 20), None), ("save point 20x28", (20, 28), None), ("tile 20x20", (20, 20), None)]
    x = 50
    for label, (w, h), kind in items:
        top = base - h * k
        if kind == "anchor":
            sp = up(anchor_sprite(), k)
            im.paste(sp, (x, top), sp)
        else:
            d.rectangle([x, top, x + w * k, base], outline=(244, 227, 255), width=2)
        text(d, (x, base + 8), label, 14, (244, 227, 255))
        x += w * k + 70
    # battle at 2x
    k = 2
    base = 760
    text(d, (20, 360), "BATTLE (shown at 2x) - white lines, opaque black fill; the battle box is 287 px wide", 18, (60, 30, 100))
    d.rectangle([20, 390, W - 20, base + 40], fill=(0, 0, 0))
    items = [("Barky", 60, 80), ("Furiell", 80, 110), ("Floofy", 40, 40), ("Dummy", 40, 60), ("Froggsona", 40, 40), ("Shysona", 40, 40),
             ("Modsmal", 40, 30), ("Lurkeye", 50, 50), ("Reply Guy", 40, 56), ("Grasstoid", 50, 60)]
    x = 40
    for label, w, h in items:
        top = base - h * k
        d.rectangle([x, top, x + w * k, base], outline=(255, 255, 255), width=2)
        text(d, (x, base + 8), f"{label}", 12, (255, 255, 255))
        text(d, (x, base + 22), f"{w}x{h}", 12, (180, 180, 180))
        x += w * k + 14
    d.line([40, 410, 40 + 287 * k, 410], fill=(255, 255, 0), width=2)
    text(d, (40 + 287 * k + 10, 402), "battle box width (287)", 12, (255, 255, 0))
    im.save(path)

def img_anchor(path, native_path):
    sp = anchor_sprite()
    sp.save(native_path)
    W, H = 760, 460
    im = Image.new("RGB", (W, H), (245, 245, 245))
    d = ImageDraw.Draw(im)
    text(d, (20, 14), "ANCHOR SPRITE - Pepe, front idle (cell 24x32)", 22)
    text(d, (20, 44), "Every generated sprite must match this pixel density, outline and shading.", 14, (80, 80, 80))
    d.rectangle([20, 80, 20 + 240 + 20, 80 + 320 + 20], fill=(0x3B, 0x1F, 0x5C))
    im.paste(up(sp, 10), (30, 90), up(sp, 10))
    text(d, (310, 90), "10x zoom  ->  1 block = 1 art pixel", 16)
    text(d, (310, 120), "native size (1x):", 16)
    im.paste(sp, (480, 116), sp)
    text(d, (310, 150), "in game (2x):", 16)
    im.paste(up(sp, 2), (480, 146), up(sp, 2))
    notes = ["- wide lumpy head with two eye domes on top", "- heavy eyelids cut the eyes in half", "- two lips, slight frown",
             "- 1-px dark brown outline #3D120E", "- flat colors, one shadow tone", "- pivot: bottom center, y = 31"]
    for i, n in enumerate(notes):
        text(d, (310, 230 + i * 26), n, 14)
    im.save(path)

def img_legacy(src, path):
    im = Image.open(src).convert("RGBA")
    px = im.load()
    for y in range(im.height):
        for x in range(im.width):
            if px[x, y][:3] == (255, 0, 255):
                px[x, y] = (0, 0, 0, 0)
    out = Image.new("RGBA", (im.width * 6 + 40, im.height * 6 + 70), (245, 245, 245, 255))
    d = ImageDraw.Draw(out)
    text(d, (20, 10), "LEGACY Pepe sheet (pepe_char.bmp, 3x3 cells of 22x30) - identity reference only", 14)
    big = up(im, 6)
    out.paste(big, (20, 50), big)
    out.save(path)

# ---------------------------------------------------------------- per-ficha templates
def _rows_from_anims(anims):
    rows = {}
    for name, row, frames, fps, loop in anims or []:
        r = rows.setdefault(row, [[], 0])
        r[0].append(name)
        r[1] = max(r[1], frames)
    return rows

def template(a, path):
    TITLE = (20, 20, 20)
    if "files" in a:
        w, h = a["single"]
        n = len(a["files"])
        k = 6
        W = 40 + n * (w * k + 30)
        H = 120 + h * k + 60
        im = Image.new("RGB", (W, H), (255, 255, 255))
        d = ImageDraw.Draw(im)
        text(d, (20, 12), f"{a['id']} TEMPLATE - {n} portraits, each {w}x{h}, same framing", 18, TITLE)
        text(d, (20, 40), "Layout guide only: do NOT draw the gray boxes, the guide circle or the labels.", 14, (200, 0, 0))
        for i, (f, _) in enumerate(a["files"]):
            x = 20 + i * (w * k + 30)
            y = 80
            d.rectangle([x, y, x + w * k, y + h * k], fill=(210, 210, 210), outline=(90, 90, 90))
            d.ellipse([x + w * k * 0.18, y + h * k * 0.06, x + w * k * 0.82, y + h * k * 0.76], outline=(150, 150, 150), width=2)
            text(d, (x, y + h * k + 8), f, 12, TITLE)
        im.save(path)
        return
    if "single" in a:
        w, h = a["single"]
        k = max(1, min(40, 700 // max(w, h)))
        W, H = max(w * k + 40, 560), h * k + 130
        im = Image.new("RGB", (W, H), (255, 255, 255))
        d = ImageDraw.Draw(im)
        text(d, (20, 12), f"{a['id']} TEMPLATE - single image, canvas {w}x{h} (shown at {k}x)", 18, TITLE)
        text(d, (20, 40), "Canvas proportions only: do NOT draw the gray box or labels.", 14, (200, 0, 0))
        d.rectangle([20, 80, 20 + w * k, 80 + h * k], fill=(210, 210, 210), outline=(90, 90, 90))
        im.save(path)
        return
    (cw, ch), (cols, rows) = a["cell"], a["grid"]
    k = max(2, min(40, 640 // max(cols * cw, rows * ch)))
    k = max(k, -(-30 // min(cw, ch)))
    rmap = _rows_from_anims(a.get("anims"))
    LM = 300 if rmap else 90
    TM, BM = 100, 50
    W = LM + cols * cw * k + 30
    H = TM + rows * ch * k + BM
    W = max(W, 620)
    im = Image.new("RGB", (W, H), (255, 255, 255))
    d = ImageDraw.Draw(im)
    text(d, (20, 12), f"{a['id']} TEMPLATE - cell {cw}x{ch}, {cols} cols x {rows} rows, PNG {cols*cw}x{rows*ch} (shown at {k}x)", 16, TITLE)
    text(d, (20, 38), "Layout guide only: do NOT draw the grid, gray cells, red marks or labels.", 14, (200, 0, 0))
    for c in range(cols):
        text(d, (LM + c * cw * k + 4, TM - 22), f"col {c}", 12, (90, 90, 90))
    origin = a.get("origin") if a["style"] in ("ow", "battle", "prop") else None
    for r in range(rows):
        y0 = TM + r * ch * k
        used = rmap[r][1] if rmap and r in rmap else (cols if not rmap else 0)
        if rmap:
            names = " / ".join(rmap[r][0]) if r in rmap else "(unused)"
            fs = 14 if len(names) < 30 else 12
            text(d, (16, y0 + ch * k // 2 - 18), f"row {r}", 14, TITLE)
            text(d, (16, y0 + ch * k // 2), f"{names}", fs, (60, 30, 100))
            text(d, (16, y0 + ch * k // 2 + 18), f"{used} frame(s)", 12, (90, 90, 90))
        else:
            text(d, (16, y0 + ch * k // 2 - 6), f"row {r}", 14, TITLE)
        for c in range(cols):
            x0 = LM + c * cw * k
            box = [x0, y0, x0 + cw * k - 1, y0 + ch * k - 1]
            if c < used:
                d.rectangle(box, fill=(212, 212, 212) if (r + c) % 2 == 0 else (196, 196, 196), outline=(100, 100, 100))
                if origin:
                    py = y0 + origin[1] * k + k // 2
                    d.line([x0, py, x0 + cw * k - 1, py], fill=(220, 0, 0), width=max(1, k // 4))
                    px = x0 + origin[0] * k + k // 2
                    rr = max(2, k // 2)
                    d.ellipse([px - rr, py - rr, px + rr, py + rr], fill=(220, 0, 0))
            else:
                d.rectangle(box, fill=(255, 255, 255), outline=(200, 200, 200))
                d.line([box[0], box[1], box[2], box[3]], fill=(230, 230, 230))
                d.line([box[0], box[3], box[2], box[1]], fill=(230, 230, 230))
    if origin:
        text(d, (20, H - 34), f"red line = feet/base row (y={origin[1]}), red dot = pivot (x={origin[0]}); white cells = leave transparent", 12, (200, 0, 0))
    im.save(path)

# ---------------------------------------------------------------- per-ficha style card (density + palette + size + rules in ONE image)
CARD_RULES = {
    "ow": ["3/4 top-down view, colorful", "1-px dark outline, one shadow tone per color", "hard square pixels, no anti-aliasing",
           "fully opaque edges, no halo", "background: flat exact #FF00FF"],
    "prop": ["3/4 top-down view, Ruins palette", "1-px dark outline, one shadow tone per color", "hard square pixels, no anti-aliasing",
             "fully opaque edges, no halo", "background: flat exact #FF00FF"],
    "battle": ["white #FFFFFF line art only", "fully OPAQUE: light parts white, dark parts black", "no gray; color only where listed",
               "hard square pixels, no anti-aliasing", "background: flat exact #FF00FF"],
    "ui": ["simple solid shapes, readable at 2x", "hard square pixels, no anti-aliasing", "fully opaque edges, no halo",
           "only the listed colors", "background: flat exact #FF00FF"],
    "tile": ["20x20 tiles on an exact grid", "floor tiles seamless in every direction", "3/4 top-down, Ruins palette only",
             "hard square pixels, no anti-aliasing", "no labels, no grid lines in the output"],
    "card": ["single image, no transparency", "only the listed sepia colors", "crisp pixel art, Undertale intro style",
             "no real faces, no brand logos", "only text allowed: BEGONE!"],
}

def card(a, path, pal):
    W, H = 1200, 660
    im = Image.new("RGB", (W, H), (250, 250, 250))
    d = ImageDraw.Draw(im)
    style = a["style"]
    text(d, (20, 12), f"{a['id']} STYLE CARD - {a['file'].rsplit('/', 1)[-1]}  (reference only: do not draw this card)", 20)
    # panel A: density reference
    d.rectangle([20, 50, 380, 520], outline=(170, 170, 170))
    if style == "card":
        text(d, (30, 58), "TONE REFERENCE (neighbor intro card)", 14)
        try:
            ref = Image.open(os.path.join("assets", "img", "lore1.bmp")).convert("RGB")
            ref = ref.resize((340, int(340 * ref.height / ref.width)), Image.NEAREST)
            im.paste(ref, (30, 90))
        except OSError:
            pass
    elif style == "battle":
        text(d, (30, 58), "BATTLE STYLE: white line + opaque fill", 14)
        for gx in range(0, 190, 19):
            d.line([60 + gx, 90, 60 + gx, 280], fill=(0, 120, 0))
        for gy in range(0, 190, 19):
            d.line([60, 90 + gy, 250, 90 + gy], fill=(0, 120, 0))
        blob = _pixel_circle(10, (255, 255, 255, 255))
        for yy in range(blob.height):
            for xx in range(blob.width):
                if blob.getpixel((xx, yy))[3]:
                    nb = [(xx + 1, yy), (xx - 1, yy), (xx, yy + 1), (xx, yy - 1)]
                    edge = any(not (0 <= p < blob.width and 0 <= q < blob.height) or not blob.getpixel((p, q))[3] for p, q in nb)
                    if not edge:
                        blob.putpixel((xx, yy), (0, 0, 0, 255))
        for eye in ((7, 8), (13, 8)):
            blob.putpixel(eye, (255, 255, 255, 255))
        im.paste(up(blob, 8), (70, 100), up(blob, 8))
        text(d, (30, 300), "PIXEL DENSITY (1 block = 1 art pixel):", 14)
        sp = up(anchor_sprite(), 6)
        im.paste(sp, (110, 325), sp)
    else:
        text(d, (30, 58), "PIXEL DENSITY: 1 block = 1 art pixel", 14)
        d.rectangle([40, 90, 360, 510], fill=(0x3B, 0x1F, 0x5C))
        sp = up(anchor_sprite(), 12)
        im.paste(sp, (56, 106), sp)
    if style != "card" and not a["id"] in ("A1", "A2"):
        d.rectangle([24, 486, 376, 516], fill=(255, 235, 235))
        text(d, (32, 492), "density/style only - do NOT draw a frog", 14, (200, 0, 0))
    # panel B: palette
    d.rectangle([400, 50, 780, 520], outline=(170, 170, 170))
    text(d, (410, 58), "PALETTE - use only these colors", 14)
    y = 86
    for i, (label, hx) in enumerate(pal[:16]):
        col, row = i // 8, i % 8
        x0, y0 = 412 + col * 186, y + row * 52
        d.rectangle([x0, y0, x0 + 40, y0 + 40], fill=hex_rgb(hx), outline=(40, 40, 40))
        text(d, (x0 + 48, y0 + 2), "#" + hx.strip().lstrip("#").upper(), 14)
        text(d, (x0 + 48, y0 + 20), label[:16], 12, (90, 90, 90))
    # panel C: size
    d.rectangle([800, 50, 1180, 520], outline=(170, 170, 170))
    if "cell" in a:
        w, h = a["cell"]
        label = f"one cell: {w}x{h}"
    else:
        w, h = a["single"]
        label = f"canvas: {w}x{h}"
    text(d, (810, 58), "SIZE (same zoom as the anchor)", 14)
    if style == "card" or max(w, h) > 140:
        text(d, (810, 100), label, 16)
        text(d, (810, 126), "full image; see the template for proportions", 12, (90, 90, 90))
    else:
        z = max(1, min(10, 380 // max(h, 32), 160 // max(w, 24)))
        base = 480
        anc = up(anchor_sprite(), z)
        im.paste(anc, (820, base - anc.height), anc)
        bx = 840 + anc.width
        d.rectangle([bx, base - h * z, bx + w * z, base], outline=(220, 0, 0), width=2)
        text(d, (820, base + 8), "Pepe 24x32", 12)
        text(d, (bx, base + 8), label, 12, (220, 0, 0))
    # rules
    d.rectangle([20, 540, 1180, 645], outline=(170, 170, 170))
    for i, rule in enumerate(CARD_RULES[style]):
        text(d, (34 + (i % 3) * 380, 556 + (i // 3) * 40), "- " + rule, 15)
    im.save(path)
