"""ART-183 #5 / LORE 26.5: builds assets/portraits/floofy_soaked.png (the `EEP!!` portrait).

Derived from floofy_cute.png so the 1-bit style matches the rest of the set:
the cute eyes/mouth are cleared and redrawn as a shocked face (tiny pupils, open
mouth), the crown flowers get plastered down, and water drops run off the head.
Run from the worktree root:  C:/Python313/python.exe tools/art/portraits/build_floofy_soaked.py
Pass --preview <png> to also write a 8x preview.
"""
import sys
from PIL import Image

W = (255, 255, 255, 255)
K = (0, 0, 0, 255)
T = (0, 0, 0, 0)

src = Image.open("assets/portraits/floofy_cute.png").convert("RGBA")
im = src.copy()
px = im.load()


def rect(x0, y0, x1, y1, c):
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            px[x, y] = c


def dot(x, y, c):
    px[x, y] = c


# clear the cute face (eyes + muzzle), keep the ears, crown and outline
rect(11, 21, 39, 38, W)

# eyes: big round whites with tiny pupils (shock), brows slanted up
for cx, cy in ((17, 29), (32, 28)):
    rect(cx - 3, cy - 3, cx + 3, cy + 3, K)
    rect(cx - 2, cy - 2, cx + 2, cy + 2, W)
    rect(cx, cy, cx, cy + 1, K)  # pupil
for i in range(5):
    dot(13 + i, 23 - i // 2, K)   # left brow, inner end up
    dot(36 - i, 22 - i // 2, K)   # right brow

# open mouth "EEP"
rect(22, 33, 29, 38, K)
rect(23, 34, 28, 37, W)
rect(24, 36, 27, 37, K)

# plastered-down crown: soggy dark band replacing the perky flowers
rect(14, 14, 36, 15, K)
for x in range(15, 36, 4):
    dot(x, 16, K)
    dot(x, 17, K)

# water drops running down the face and off the chin
for x, y in ((9, 26), (9, 27), (41, 30), (41, 31), (41, 32), (22, 40), (22, 41), (30, 42)):
    dot(x, y, K)
for x, y in ((8, 26), (10, 26), (40, 30), (42, 30)):
    dot(x, y, W)

im.save("assets/portraits/floofy_soaked.png")

if "--preview" in sys.argv:
    out = sys.argv[sys.argv.index("--preview") + 1]
    bg = Image.new("RGBA", im.size, (60, 60, 70, 255))
    bg.alpha_composite(im)
    bg.resize((400, 400), Image.NEAREST).save(out)
