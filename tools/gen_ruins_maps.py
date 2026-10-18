"""Generate the first Ruins rooms as Tiled JSON maps (OVW-020, C1-001/003/008/009).

Usage: python tools/gen_ruins_maps.py   (writes data/maps/ruins_*.json)

The maps are plain Tiled 1.10 JSON, so they can be opened and edited in Tiled
afterwards; this script only exists to lay out the first pass consistently.
Tile ids follow docs/art_prompts/ruins/F01_tiles_ruins.md section 3
(gid = row * 8 + col + 1). Map cells are 40 room px (20 px tiles drawn at 2x).
"""

import json
import os

T = 40  # room px per map cell

# --- tileset gids -----------------------------------------------------------
STONE = [1, 2, 3]
CRACKED, GLITTER, WRAPPER, CRUMBS = 4, 5, 6, 7
CARPET = {"tl": 9, "t": 10, "tr": 11, "l": 12, "c": 13, "r": 14, "bl": 17, "b": 18, "br": 19}
CARPET_WORN, CARPET_STAIN, TAPE, MAT, PAW = 15, 16, 20, 21, 22
CAP_L, CAP_M, CAP_R = 25, 26, 27
FRONT_L, FRONT_M, FRONT_R, FRONT_CRACK, FRONT_VENT = 28, 29, 30, 31, 32
FRONT_HOOK, FRONT_OUTLET, WALL_SHADOW = 37, 38, 39
STAIRS_TOP = [41, 42, 43]
STAIRS_MID = [44, 45, 46]
ARCH_TL, ARCH_TR, ARCH_BL, ARCH_BR = 53, 54, 55, 56
PILLAR_TOP, PILLAR_MID, PILLAR_BASE, NEON = 57, 58, 59, 60
BEAM_TOP, BEAM_BOTTOM = 61, 62


class Room:
    def __init__(self, room_id, width, height, display_name):
        self.id = room_id
        self.w = width
        self.h = height
        self.display_name = display_name
        self.ground = [0] * (width * height)
        self.decor = [0] * (width * height)
        self.above = [0] * (width * height)
        self.collision = []
        self.objects = []
        self.props = {}
        self.next_id = 1

    # tiles ------------------------------------------------------------------
    def put(self, layer, x, y, gid):
        if 0 <= x < self.w and 0 <= y < self.h:
            getattr(self, layer)[y * self.w + x] = gid

    def floor(self, x0, y0, x1, y1):
        """Stone floor on the inclusive cell rect, with a deterministic A/B/C mix."""
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                n = (x * 7 + y * 13 + (x * y) % 5) % 9
                gid = STONE[0] if n < 5 else STONE[1] if n < 8 else STONE[2]
                self.put("ground", x, y, gid)

    def wall(self, x0, x1, cap_y, openings=()):
        """Top wall: a cap row at cap_y and a two-row brick front below it."""
        for x in range(x0, x1 + 1):
            cap = CAP_L if x == x0 else CAP_R if x == x1 else CAP_M
            self.put("ground", x, cap_y, cap)
            if x in openings:
                continue
            front = FRONT_L if x == x0 else FRONT_R if x == x1 else FRONT_M
            self.put("ground", x, cap_y + 1, front)
            self.put("ground", x, cap_y + 2, front)

    def arch(self, x, cap_y):
        """A 2-cell archway in the wall front (cells x, x+1)."""
        self.put("ground", x, cap_y + 1, ARCH_TL)
        self.put("ground", x + 1, cap_y + 1, ARCH_TR)
        self.put("ground", x, cap_y + 2, ARCH_BL)
        self.put("ground", x + 1, cap_y + 2, ARCH_BR)

    def carpet(self, x0, y0, x1, y1):
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                v = "t" if y == y0 else "b" if y == y1 else ""
                h = "l" if x == x0 else "r" if x == x1 else ""
                key = (v + h) or "c"
                if key in ("t", "b", "l", "r", "c"):
                    pass
                self.put("decor", x, y, CARPET[key])

    # objects ----------------------------------------------------------------
    def _obj(self, layer, name, otype, x, y, w=0, h=0, **props):
        obj = {
            "id": self.next_id,
            "name": name,
            "type": otype,
            "x": x, "y": y, "width": w, "height": h,
            "rotation": 0, "visible": True,
        }
        if props:
            obj["properties"] = [_prop(k, v) for k, v in props.items()]
        self.next_id += 1
        (self.collision if layer == "collision" else self.objects).append(obj)
        return obj

    def solid(self, name, x, y, w, h):
        return self._obj("collision", name, "collision", x, y, w, h)

    def obj(self, name, otype, x, y, w=0, h=0, **props):
        return self._obj("objects", name, otype, x, y, w, h, **props)

    def to_json(self, extra_props=None):
        def tile_layer(lid, name, data):
            return {"id": lid, "name": name, "type": "tilelayer", "width": self.w, "height": self.h,
                    "x": 0, "y": 0, "opacity": 1, "visible": True, "data": data}

        layers = [
            tile_layer(1, "ground", self.ground),
            tile_layer(2, "decor", self.decor),
            tile_layer(3, "above", self.above),
            {"id": 4, "name": "collision", "type": "objectgroup", "x": 0, "y": 0, "opacity": 1,
             "visible": True, "draworder": "topdown", "objects": self.collision},
            {"id": 5, "name": "objects", "type": "objectgroup", "x": 0, "y": 0, "opacity": 1,
             "visible": True, "draworder": "topdown", "objects": self.objects},
        ]
        props = [_prop("displayName", self.display_name)]
        for k, v in (extra_props or {}).items():
            props.append(_prop(k, v))
        return {
            "compressionlevel": -1, "infinite": False, "orientation": "orthogonal",
            "renderorder": "right-down", "tiledversion": "1.11.2", "type": "map", "version": "1.10",
            "width": self.w, "height": self.h, "tilewidth": T, "tileheight": T,
            "nextlayerid": 6, "nextobjectid": self.next_id,
            "properties": props,
            "tilesets": [{"firstgid": 1, "name": "ruins", "image": "assets/tiles/ruins.png",
                          "imagewidth": 160, "imageheight": 160, "tilewidth": 20, "tileheight": 20,
                          "columns": 8, "tilecount": 64, "margin": 0, "spacing": 0}],
            "layers": layers,
        }


def _prop(name, value):
    if isinstance(value, bool):
        t = "bool"
    elif isinstance(value, int):
        t = "int"
    elif isinstance(value, float):
        t = "float"
    else:
        t = "string"
    return {"name": name, "type": t, "value": value}


def box_walls(room, x0, y0, x1, y1, gaps=()):
    """Solid rects around the walkable cell rect [x0..x1] x [y0..y1].

    gaps: list of (side, c0, c1) cell ranges left open, side in top/bottom/left/right.
    """
    W, H = room.w * T, room.h * T
    left, top, right, bottom = x0 * T, y0 * T, (x1 + 1) * T, (y1 + 1) * T

    def edge(side, start, end, fixed_a, fixed_b, horizontal):
        cuts = sorted((c0 * T, (c1 + 1) * T) for s, c0, c1 in gaps if s == side)
        pos = start
        n = 0
        for a, b in cuts + [(end, end)]:
            if a > pos:
                if horizontal:
                    room.solid(f"{side}_{n}", pos, fixed_a, a - pos, fixed_b - fixed_a)
                else:
                    room.solid(f"{side}_{n}", fixed_a, pos, fixed_b - fixed_a, a - pos)
                n += 1
            pos = max(pos, b)

    edge("top", 0, W, 0, top, True)
    edge("bottom", 0, W, bottom, H, True)
    edge("left", top, bottom, 0, left, False)
    edge("right", top, bottom, right, W, False)


def write(room, extra_props=None):
    path = os.path.join(os.environ.get("MAPS_OUT", os.path.join("data", "maps")), room.id + ".json")
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        json.dump(room.to_json(extra_props), f, indent=1)
        f.write("\n")
    print("wrote", path)


# --- R1: The Fall Room ---------------------------------------------------------
def ruins_fall():
    r = Room("ruins_fall", 16, 12, "Ruins - Fall Room")
    r.wall(2, 13, 1, openings=(7, 8))
    r.arch(7, 1)
    r.floor(2, 4, 13, 10)
    # crumbs and wrappers scattered around the bed of snack wrappers
    for (x, y) in [(5, 6), (10, 8), (6, 9), (11, 5)]:
        r.put("decor", x, y, CRUMBS)
    for (x, y) in [(8, 9), (4, 8), (12, 9)]:
        r.put("decor", x, y, WRAPPER)
    r.put("decor", 3, 5, CRACKED)
    # light beam from the hole in the ceiling, drawn over Pepe
    for x in (7, 8):
        r.put("above", x, 5, BEAM_TOP)
        r.put("above", x, 6, BEAM_BOTTOM)
    box_walls(r, 2, 4, 13, 10, gaps=[("top", 7, 8)])
    # the archway itself: walls on both sides of the 2-cell opening, door at the back
    r.solid("arch_back", 7 * T, 0, 2 * T, 2 * T + 20)
    r.obj("to_entrance", "door", 7 * T + 8, 2 * T + 20, 2 * T - 16, 20,
          targetRoom="ruins_entrance", targetSpawn="from_fall", facing="up")
    r.obj("default", "spawn", 8 * T - 20, 7 * T, facing="down")
    r.obj("from_entrance", "spawn", 8 * T - 20, 4 * T + 4, facing="down")
    r.obj("bed", "prop", 7 * T + 20, 7 * T, 40, 40,
          sprite="assets/props/ruins_wrapper_pile.png", solid=False,
          text="* A bed of discarded snack wrappers.|* It crinkles. It is still warm.")
    r.obj("can", "examine", 11 * T, 9 * T, 40, 40, solid=False,
          text="* Citrus Blast, 2014 edition. A vintage.",
          text2="* You consider drinking it.|* You consider your life.|* You leave it.")
    r.obj("savepoint", "savepoint", 4 * T + 20, 6 * T)
    write(r, {"music": "ruins"})


# --- R3: Ruins Entrance ------------------------------------------------------------
def ruins_entrance():
    r = Room("ruins_entrance", 16, 14, "Ruins - Entrance")
    r.wall(1, 14, 1, openings=(7, 8))
    r.arch(7, 1)
    r.floor(1, 4, 14, 12)
    r.floor(7, 13, 8, 13)  # corridor down to the fall room
    # double staircase up to the arch, both sides (solid, decorative like Undertale's)
    for x0 in (3, 10):
        for i, gid in enumerate(STAIRS_TOP):
            r.put("ground", x0 + i, 4, gid)
        for i, gid in enumerate(STAIRS_MID):
            r.put("ground", x0 + i, 5, gid)
            r.put("ground", x0 + i, 6, gid)
        r.solid(f"stairs_{x0}", x0 * T, 4 * T, 3 * T, 3 * T)
    # welcome mat leading to the arch and a worn carpet runner
    r.carpet(6, 7, 9, 11)
    r.put("decor", 7, 4, MAT)
    r.put("decor", 8, 4, MAT)
    for (x, y) in [(2, 10), (13, 8), (12, 12)]:
        r.put("decor", x, y, CRUMBS)
    r.put("decor", 2, 7, CRACKED)
    r.put("decor", 13, 11, GLITTER)
    box_walls(r, 1, 4, 14, 12, gaps=[("top", 7, 8), ("bottom", 7, 8)])
    r.solid("corridor_l", 0, 13 * T, 7 * T, T)
    r.solid("corridor_r", 9 * T, 13 * T, 7 * T, T)
    r.solid("arch_back", 7 * T, 0, 2 * T, 2 * T + 20)
    r.obj("to_notices", "door", 7 * T + 8, 2 * T + 20, 2 * T - 16, 20,
          targetRoom="ruins_notices", targetSpawn="from_entrance", facing="right")
    r.obj("to_fall", "door", 7 * T + 8, 14 * T - 12, 2 * T - 16, 12,
          targetRoom="ruins_fall", targetSpawn="from_entrance", facing="down")
    r.obj("from_fall", "spawn", 8 * T - 20, 12 * T, facing="up")
    r.obj("default", "spawn", 8 * T - 20, 10 * T, facing="up")
    r.obj("from_notices", "spawn", 8 * T - 20, 4 * T + 4, facing="down")
    # banners on the wall front (the wall is already solid)
    r.obj("banner_welcome", "prop", 3 * T + 20, T + 20, 120, 80,
          sprite="assets/props/ruins_banner_welcome.png", solid=False,
          text="* \"FurNation Con '14 - Welcome!\"|* The banner is faded. The glitter is not.")
    r.obj("banner_deodorant", "prop", 10 * T + 20, 2 * T + 20, 120, 32,
          sprite="assets/props/ruins_banner_deodorant.png", solid=False,
          text="* \"Deodorant is NOT optional.\"|* Someone underlined NOT four times.")
    r.obj("dorito_pile", "prop", 12 * T, 9 * T, 80, 80,
          sprite="assets/props/ruins_wrapper_pile.png", solid=False,
          text="* A pile of empty chip bags.|* *crunch*",
          text2="* *crunch*|* ...You stepped in it on purpose, didn't you.")
    r.obj("savepoint", "savepoint", 3 * T, 10 * T)
    write(r, {"music": "ruins"})


# --- R8: Notices, CRT and Art Show ---------------------------------------------------
def ruins_notices():
    r = Room("ruins_notices", 22, 12, "Ruins - Notice Hall")
    r.wall(0, 21, 1)
    r.floor(0, 4, 21, 10)
    r.carpet(5, 6, 16, 8)
    r.put("decor", 9, 7, CARPET_STAIN)
    r.put("decor", 13, 6, CARPET_WORN)
    r.put("decor", 3, 9, TAPE)
    r.put("decor", 18, 9, PAW)
    r.put("decor", 19, 5, CRUMBS)
    r.put("ground", 6, 2, FRONT_VENT)
    r.put("ground", 11, 3, FRONT_OUTLET)
    r.put("ground", 17, 2, FRONT_CRACK)
    box_walls(r, 0, 4, 21, 10, gaps=[("left", 5, 8), ("right", 6, 8)])
    r.obj("to_entrance", "door", 0, 5 * T, 12, 4 * T,
          targetRoom="ruins_entrance", targetSpawn="from_notices", facing="down")
    r.obj("to_arena", "door", 22 * T - 12, 6 * T, 12, 3 * T,
          targetRoom="ruins_arena", targetSpawn="from_notices", facing="right")
    r.obj("from_entrance", "spawn", T, 6 * T + 10, facing="right")
    r.obj("default", "spawn", 4 * T, 6 * T + 10, facing="right")
    r.obj("from_arena", "spawn", 20 * T, 7 * T, facing="left")
    r.obj("notice", "prop", 3 * T, 2 * T + 10, 80, 60,
          sprite="assets/props/ruins_notice_board.png", solid=False,
          text="* NOTICE FROM RUINS ADMINISTRATION:|* Please do not shed in the ventilation shafts.\n* Violators will be banned from the Minecraft server.")
    r.obj("crt", "prop", 8 * T + 20, 4 * T + 10, 40, 40,
          sprite="assets/props/ruins_crt.png", solid=True,
          text="* A blue screen of death.|* The error message is in Comic Sans.",
          text2="* Something is wedged behind the monitor.|* ...It's a red dot. It's a whole laser pointer.\n* (You can't reach it. The cable is in the way.)")
    r.obj("curtain", "prop", 14 * T, 2 * T + 20, 80, 80,
          sprite="assets/props/ruins_curtain_18.png", solid=False,
          text="* A black curtain. A sign says \"ART SHOW - 18+\".|* You decide to respect the curtain.",
          text2="* You look at the curtain again.|* The curtain looks back.",
          text3="* The curtain has seen things.|* You will not be one of them.")
    r.obj("bid_table", "prop", 18 * T, 6 * T + 10, 120, 40,
          sprite="assets/props/ruins_booth.png", solid=True,
          text="* A bid sheet.|* Highest bid: 400G. For a drawing of a \"very tired raccoon\".\n* You don't ask.")
    write(r, {"music": "ruins", "encounters": "ruins_r8"})


# --- R9: Barky's Arena ----------------------------------------------------------------
def ruins_arena():
    r = Room("ruins_arena", 16, 14, "Ruins - Barky's Arena")
    r.wall(1, 14, 1, openings=(7, 8))
    r.arch(7, 1)
    r.floor(0, 4, 14, 12)
    r.carpet(4, 7, 11, 11)
    for (x, y) in [(2, 5), (12, 11), (13, 6)]:
        r.put("decor", x, y, CRACKED)
    r.put("decor", 6, 9, PAW)
    r.put("decor", 9, 8, PAW)
    # pillars in the corners, top drawn over Pepe
    for px in (3, 12):
        r.put("decor", px, 6, PILLAR_BASE)
        r.put("above", px, 5, PILLAR_MID)
        r.put("above", px, 4, PILLAR_TOP)
        r.solid(f"pillar_{px}", px * T + 6, 6 * T + 10, T - 12, T - 10)
    box_walls(r, 0, 4, 14, 12, gaps=[("left", 7, 9), ("top", 7, 8)])
    r.solid("arch_back", 7 * T, 0, 2 * T, 2 * T + 20)
    r.obj("to_notices", "door", 0, 7 * T, 12, 3 * T,
          targetRoom="ruins_notices", targetSpawn="from_arena", facing="left")
    r.obj("from_notices", "spawn", T, 8 * T, facing="right")
    r.obj("default", "spawn", T, 8 * T, facing="right")
    # Barky's trigger and NPC (name-based behaviours in OverworldScene)
    r.obj("door", "interactable", 7 * T, 3 * T, 2 * T, T)
    r.obj("npc_barky", "interactable", 10 * T, 5 * T, 40, 50)
    r.obj("dust", "interactable", 10 * T, 5 * T + 10, 40, 40)
    r.obj("bowl", "interactable", 2 * T, 10 * T, 40, 30)
    r.obj("lamp", "prop", 6 * T + 20, 5 * T, 40, 24,
          sprite="assets/props/ruins_lamp.png", solid=True,
          text="* A floor lamp.|* Barky's natural enemy. Or his moon. He hasn't decided.")
    write(r, {"music": "ruins"})


if __name__ == "__main__":
    ruins_fall()
    ruins_entrance()
    ruins_notices()
    ruins_arena()
