#!/usr/bin/env python3
"""build_ruins_maps.py - Author the first Ruins rooms for nUndertale (T2).

Generates Tiled JSON maps (40x40 px tiles, 2x from 20x20 ruins tileset):
  - data/maps/ruins_fall.json     (R1 The Fall Room)
  - data/maps/ruins_registration.json (R1a Registration)
  - data/maps/ruins_panelhall.json    (R1b Panel Hall)
  - data/maps/ruins_floofy.json       (R2 Floofy)
  - data/maps/ruins_entrance.json (R3 Ruins Entrance)
  - data/maps/ruins_notices.json  (R8 Notices, CRT, Art Show)
  - data/maps/ruins_barky.json    (R9 Barky's Arena, all data-driven: C1-009)
"""

import json
import os
import sys

# Tileset mapping for assets/tiles/ruins.png (160x160 px, 8 columns x 8 rows, firstgid=1)
# gid = 1 + row * 8 + col
TILE_CHARS = {
    # Void / empty
    ' ': 8,   # black void
    '#': 8,   # black void
    '0': 0,   # transparent / empty

    # Stone floor (row 0)
    '.': 1,   # stone A
    ',': 2,   # stone B
    ';': 3,   # stone C
    ':': 4,   # cracked floor
    '*': 5,   # glitter decal
    'w': 6,   # wrapper decal
    'c': 7,   # crumbs decal

    # Carpet (rows 1 & 2)
    '7': 9,   # carpet TL
    '8': 10,  # carpet top
    '9': 11,  # carpet TR
    '4': 12,  # carpet left
    '5': 13,  # carpet center
    '6': 14,  # carpet right
    'z': 15,  # carpet worn
    'x': 16,  # carpet stain
    '1': 17,  # carpet BL
    '2': 18,  # carpet bottom
    '3': 19,  # carpet BR
    't': 20,  # tape decal
    'm': 21,  # welcome mat
    'p': 22,  # paw print decal

    # Walls (rows 3 & 4)
    '[': 25,  # wall top L
    '=': 26,  # wall top M
    ']': 27,  # wall top R
    '{': 28,  # wall front L
    '-': 29,  # wall front M
    '}': 30,  # wall front R
    'k': 31,  # cracked front
    'v': 32,  # front w/ vent
    '<': 33,  # inner corner L
    '>': 34,  # inner corner R
    '(': 35,  # outer corner L
    ')': 36,  # outer corner R
    'h': 37,  # banner hook
    'o': 38,  # outlet+cable
    '_': 39,  # wall-base shadow
    '/': 40,  # dark wall

    # Stairs (row 5)
    'Q': 41,  # stairs top L
    'W': 42,  # stairs top M
    'E': 43,  # stairs top R
    'A': 44,  # stairs mid L
    'S': 45,  # stairs mid M
    'D': 46,  # stairs mid R
    'Z': 47,  # stairs base L
    'X': 48,  # stairs base R

    # Doors & Arches (row 6)
    'u': 49,  # door closed top
    'j': 50,  # door closed bottom
    'n': 51,  # door open top
    'N': 52,  # door open bottom
    'L': 53,  # arch TL
    'R': 54,  # arch TR
    'l': 55,  # arch BL
    'r': 56,  # arch BR

    # Pillars, light beams & details (row 7)
    'T': 57,  # pillar top
    'I': 58,  # pillar mid
    'B': 59,  # pillar base
    'G': 60,  # neon sign
    'Y': 61,  # light beam top
    'y': 62,  # light beam base
    '~': 63,  # ledge edge
}

ABOVE_CHARS = {
    ' ': 0,
    '.': 0,
    '0': 0,
    'Y': 61,  # light beam top
    'y': 62,  # light beam base
    'L': 53,  # arch TL
    'R': 54,  # arch TR
    'T': 57,  # pillar top
    'I': 58,  # pillar mid
    'B': 59,  # pillar base
}


def parse_grid(lines, char_map, width, height):
    """Parse list of string rows into a 1D list of GIDs."""
    if len(lines) != height:
        raise ValueError(f"Grid height {len(lines)} != expected {height}")
    data = []
    for r, line in enumerate(lines):
        if len(line) != width:
            raise ValueError(f"Grid line {r} width {len(line)} != expected {width}: '{line}'")
        for c, ch in enumerate(line):
            if ch not in char_map:
                raise KeyError(f"Unknown tile char '{ch}' at row {r}, col {c}")
            data.append(char_map[ch])
    return data


# props/petal_trail_pieces sheet anims (16x16 cells, drawn at 2x = 32 px, centred on the pivot)
PETAL_STRAIGHT_V = "p6"
PETAL_STRAIGHT_H = "p7"
PETAL_CORNERS = ("p3", "p4", "p5", "p8")
PETAL_SCATTER = ("p0", "p1", "p2", "p9", "p10", "p11")
PETAL_SHEET = "assets/props/petal_trail_pieces.png"


def lay_petal_trail(tiles, name="petal_trail", scatter_tiles=(), tile_size=40):
    """Turn a path of (col, row) tiles into petal decal props (Floofy's sign, LORE v3.5 §6.5).

    Consecutive tiles must be 4-connected. A tile whose neighbours lie on one axis gets a
    straight piece; a tile where the path turns gets a corner piece; scatter_tiles get loose
    petals. Returns walkable, non-solid prop objects named <name>_00, <name>_01, ...; the
    caller adds one examine object for the interaction text.
    """
    tiles = [tuple(t) for t in tiles]
    objects = []

    def decal(col, row, anim):
        cx = col * tile_size + tile_size // 2
        cy = row * tile_size + tile_size // 2
        size = 32
        objects.append({
            "name": "%s_%02d" % (name, len(objects)),
            "type": "prop",
            "x": cx - size // 2,
            "y": cy - size,
            "w": size,
            "h": size,
            "properties": {"sprite": PETAL_SHEET, "anim": anim, "solid": False},
        })

    corner_index = 0
    for i, (col, row) in enumerate(tiles):
        prev_t = tiles[i - 1] if i > 0 else None
        next_t = tiles[i + 1] if i + 1 < len(tiles) else None
        for other in (prev_t, next_t):
            if other and abs(other[0] - col) + abs(other[1] - row) != 1:
                raise ValueError("petal trail tiles must be 4-connected: %r -> %r" % ((col, row), other))
        neighbours = [t for t in (prev_t, next_t) if t]
        horizontal = any(t[1] == row for t in neighbours)
        vertical = any(t[0] == col for t in neighbours)
        if horizontal and vertical:
            anim = PETAL_CORNERS[corner_index % len(PETAL_CORNERS)]
            corner_index += 1
        elif horizontal:
            anim = PETAL_STRAIGHT_H
        else:
            anim = PETAL_STRAIGHT_V
        decal(col, row, anim)
    for i, (col, row) in enumerate(scatter_tiles):
        decal(col, row, PETAL_SCATTER[i % len(PETAL_SCATTER)])
    return objects


WALL_CAP_CHARS = "[=]"
WALL_FRONT_CHARS = "{-}khvo"


def cut_doorways(ground_grid, above_grid):
    """Shared doorway helper (BUG-41): cut every 2-wide gap into the wall.

    Rooms mark an exit with a 2-wide void gap in the wall plus an 'L','R' pair in the above
    grid. The old arch overlay (tiles 53/54) was drawn one row off the wall, so it stood on the
    floor. This turns each pair into an open doorway: tile 51 (lintel) on the wall's cap row and
    tile 52 (dark opening, sill on the floor line) on the wall's bottom row, and gives the wall
    ends beside the gap their jamb tiles. The overlay chars are dropped from the above grid.
    Also checks that closed doors ('u'/'j') and open ones ('n'/'N') are top-over-bottom.
    Returns (ground_grid, above_grid) as new lists of strings."""
    ground = [list(row) for row in ground_grid]
    above = [list(row) for row in above_grid] if above_grid else None

    if above:
        for ar, arow in enumerate(above):
            for ac in range(len(arow) - 1):
                if arow[ac] != 'L' or arow[ac + 1] != 'R':
                    continue
                # the gap in the ground is the truth; the overlay may be a column off (ruins_floofy)
                for shift in (0, 1, -1):
                    cols = (ac + shift, ac + shift + 1)
                    left, right = cols[0] - 1, cols[1] + 1
                    # rows where the gap cuts through wall (neighbours on both sides are wall)
                    rows = [
                        r for r in range(len(ground))
                        if ground[r][left] in WALL_CAP_CHARS + WALL_FRONT_CHARS
                        and ground[r][right] in WALL_CAP_CHARS + WALL_FRONT_CHARS
                        and all(ground[r][c] in ' #' for c in cols)
                    ]
                    # the sill is the wall's bottom gap row; the row above it is the cap row
                    # (open gap, or already solid cap that then simply becomes the lintel tile)
                    if rows and rows[-1] - 1 >= 0 and all(
                        ground[rows[-1] - 1][c] in WALL_CAP_CHARS + ' #' for c in cols
                    ) and (len(rows) == 1 or rows[0] == rows[-1] - 1):
                        break
                else:
                    raise ValueError(
                        f"doorway near col {ac}: expected a 2-row wall gap (cap + front)")
                sill = rows[-1]
                top = sill - 1
                for c in cols:
                    ground[top][c] = 'n'
                    ground[sill][c] = 'N'
                for c in (ac, ac + 1):
                    arow[c] = '.'
                # jamb tiles on the wall ends beside the gap (plain bricks only)
                for r, plain, jamb_l, jamb_r in (
                    (top, '=', ']', '['),
                    (sill, '-', '}', '{'),
                ):
                    if ground[r][left] == plain:
                        ground[r][left] = jamb_l
                    if ground[r][right] == plain:
                        ground[r][right] = jamb_r

    for r in range(len(ground)):
        for c, ch in enumerate(ground[r]):
            for upper, lower in (('u', 'j'), ('n', 'N')):
                if ch == upper and (r + 1 >= len(ground) or ground[r + 1][c] != lower):
                    raise ValueError(f"door tile '{upper}' at row {r}, col {c} needs '{lower}' right below it")
                if ch == lower and (r == 0 or ground[r - 1][c] != upper):
                    raise ValueError(f"door tile '{lower}' at row {r}, col {c} needs '{upper}' right above it")

    return (
        [''.join(row) for row in ground],
        [''.join(row) for row in above] if above else above_grid,
    )


def make_tiled_map(
    room_id,
    display_name,
    width,
    height,
    ground_grid,
    collision_boxes,
    objects,
    above_grid=None,
    music="overworld.default",
    save_place=None,
    encounters=None,
    encounter_budget=0,
    background_image=None,
    tile_size=40,
):
    """Build a complete Tiled JSON map dict."""
    ground_grid, above_grid = cut_doorways(ground_grid, above_grid)
    layers = []
    layer_id = 1
    next_obj_id = 1

    # Optional background image layer
    if background_image:
        layers.append({
            "id": layer_id,
            "image": background_image,
            "name": "background",
            "offsetx": 0,
            "offsety": 0,
            "opacity": 1,
            "type": "imagelayer",
            "visible": True,
            "x": 0,
            "y": 0,
        })
        layer_id += 1

    # Ground tile layer
    ground_data = parse_grid(ground_grid, TILE_CHARS, width, height)
    layers.append({
        "data": ground_data,
        "height": height,
        "id": layer_id,
        "name": "ground",
        "opacity": 1,
        "type": "tilelayer",
        "visible": True,
        "width": width,
        "x": 0,
        "y": 0,
    })
    layer_id += 1

    # Collision layer
    collision_objs = []
    for c in collision_boxes:
        collision_objs.append({
            "height": c["h"],
            "id": next_obj_id,
            "name": c.get("name", f"col_{next_obj_id}"),
            "rotation": 0,
            "type": "collision",
            "visible": True,
            "width": c["w"],
            "x": c["x"],
            "y": c["y"],
        })
        next_obj_id += 1

    layers.append({
        "draworder": "topdown",
        "id": layer_id,
        "name": "collision",
        "objects": collision_objs,
        "opacity": 1,
        "type": "objectgroup",
        "visible": True,
        "x": 0,
        "y": 0,
    })
    layer_id += 1

    # Objects layer
    tiled_objs = []
    for obj in objects:
        tiled_obj = {
            "height": obj.get("h", 0),
            "id": next_obj_id,
            "name": obj.get("name", ""),
            "rotation": 0,
            "type": obj.get("type", ""),
            "visible": True,
            "width": obj.get("w", 0),
            "x": obj.get("x", 0),
            "y": obj.get("y", 0),
        }
        if obj.get("point", False):
            tiled_obj["point"] = True

        props = []
        for prop_name, prop_val in obj.get("properties", {}).items():
            if isinstance(prop_val, bool):
                prop_type = "bool"
            elif isinstance(prop_val, int):
                prop_type = "int"
            elif isinstance(prop_val, float):
                prop_type = "float"
            else:
                prop_type = "string"
            props.append({
                "name": prop_name,
                "type": prop_type,
                "value": prop_val,
            })
        if props:
            tiled_obj["properties"] = props

        tiled_objs.append(tiled_obj)
        next_obj_id += 1

    layers.append({
        "draworder": "topdown",
        "id": layer_id,
        "name": "objects",
        "objects": tiled_objs,
        "opacity": 1,
        "type": "objectgroup",
        "visible": True,
        "x": 0,
        "y": 0,
    })
    layer_id += 1

    # Optional above layer
    if above_grid:
        above_data = parse_grid(above_grid, ABOVE_CHARS, width, height)
        layers.append({
            "data": above_data,
            "height": height,
            "id": layer_id,
            "name": "above",
            "opacity": 1,
            "properties": [
                {"name": "above", "type": "bool", "value": True}
            ],
            "type": "tilelayer",
            "visible": True,
            "width": width,
            "x": 0,
            "y": 0,
        })
        layer_id += 1

    # Map-level properties
    map_properties = [
        {"name": "displayName", "type": "string", "value": display_name},
        {"name": "music", "type": "string", "value": music},
    ]
    if save_place:
        map_properties.append({"name": "savePlace", "type": "string", "value": save_place})
    if encounters:
        # ENC-003/ENC-006: weighted random encounters with a per-room budget
        map_properties.append({"name": "encounters", "type": "string", "value": encounters})
        map_properties.append({"name": "encounterBudget", "type": "int", "value": encounter_budget})

    tilesets = [
        {
            "columns": 8,
            "firstgid": 1,
            "image": "assets/tiles/ruins.png",
            "imageheight": 160,
            "imagewidth": 160,
            "name": "ruins",
            "tilecount": 64,
            "tileheight": 20,
            "tilewidth": 20,
        }
    ]

    tiled_map = {
        "compressionlevel": -1,
        "height": height,
        "infinite": False,
        "layers": layers,
        "nextlayerid": layer_id,
        "nextobjectid": next_obj_id,
        "orientation": "orthogonal",
        "properties": map_properties,
        "renderorder": "right-down",
        "tiledversion": "1.11.2",
        "tileheight": tile_size,
        "tilesets": tilesets,
        "tilewidth": tile_size,
        "type": "map",
        "version": "1.10",
        "width": width,
    }
    return tiled_map


# =============================================================================
# Room R13: Ruins Exit (ruins_exit) - R10-C (T3)
# Size: 16x18 tiles (640x720 px).
# =============================================================================
def ensure_aurora_glimpse_prop(repo_root):
    prop_path = os.path.join(repo_root, "assets", "props", "ruins_aurora_glimpse.png")
    if os.path.exists(prop_path):
        return
    try:
        from PIL import Image, ImageDraw
        w, h = 40, 40
        im = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        draw = ImageDraw.Draw(im)

        # Sky background (dark blue #0A0F2C)
        for y in range(h):
            draw.line([(0, y), (w - 1, y)], fill=(10, 15, 44, 255))

        # Aurora bands: #FF4FD8, #4FD8FF, #7CFF3A
        for x in range(w):
            y = int(6 + 2 * (x % 7) / 7.0)
            draw.point((x, y), fill=(255, 79, 216, 230))
            draw.point((x, y + 1), fill=(255, 79, 216, 255))

        for x in range(w):
            y = int(9 + 3 * ((x + 3) % 9) / 9.0)
            draw.point((x, y), fill=(79, 216, 255, 230))
            draw.point((x, y + 1), fill=(79, 216, 255, 255))

        for x in range(w):
            y = int(13 + 2 * ((x + 5) % 8) / 8.0)
            draw.point((x, y), fill=(124, 255, 58, 230))
            draw.point((x, y + 1), fill=(124, 255, 58, 255))

        # Distant pine trees silhouette
        trees = [(8, 24), (16, 22), (24, 25), (32, 23)]
        for tx, ty in trees:
            for dy in range(12):
                span = dy // 2
                for dx in range(-span, span + 1):
                    if 0 <= tx + dx < w and ty + dy < h:
                        draw.point((tx + dx, ty + dy), fill=(5, 8, 20, 255))

        # Pale snow ground #E8F0FF (232, 240, 255)
        for y in range(30, h):
            draw.line([(0, y), (w - 1, y)], fill=(232, 240, 255, 255))
        draw.line([(0, 30), (w - 1, 30)], fill=(200, 215, 240, 255))

        # Iron door frame on the sides
        for y in range(h):
            draw.point((0, y), fill=(50, 50, 65, 255))
            draw.point((1, y), fill=(65, 65, 80, 255))
            draw.point((w - 2, y), fill=(65, 65, 80, 255))
            draw.point((w - 1, y), fill=(50, 50, 65, 255))

        os.makedirs(os.path.dirname(prop_path), exist_ok=True)
        im.save(prop_path)
    except Exception as e:
        pass
