#!/usr/bin/env python3
"""build_ruins_maps.py - Author the first Ruins rooms for nUndertale (T2).

Generates Tiled JSON maps (40x40 px tiles, 2x from 20x20 ruins tileset):
  - data/maps/ruins_fall.json     (R1 The Fall Room)
  - data/maps/ruins_floofy.json   (R2 Floofy)
  - data/maps/ruins_entrance.json (R3 Ruins Entrance)
  - data/maps/ruins_notices.json  (R8 Notices, CRT, Art Show)
  - data/maps/ruins_barky.json    (R9 Barky's Arena, legacyBarky: true)
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
    legacy_barky=False,
    encounters=None,
    encounter_budget=0,
    background_image=None,
    tile_size=40,
):
    """Build a complete Tiled JSON map dict."""
    layers = []
    layer_id = 1
    next_obj_id = 1

    # Optional background image layer (legacy barky)
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
    if legacy_barky:
        map_properties.insert(0, {"name": "legacyBarky", "type": "bool", "value": True})

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
# Room R1: The Fall Room (ruins_fall)
# Size: 16x12 tiles (640x480 px).
# Dark, small room. Pepe wakes up on a snack wrapper bed under a light beam.
# Examinable can, save point, door north to Floofy.
# =============================================================================
def build_ruins_fall():
    width = 16
    height = 12

    ground = [
        "################",
        "#[====]  [====]#",
        "#{-v-k}  {---o}#",
        "#.,;,..,;,..,;.#",
        "#;..,*...,;,..;#",
        "#.,;,....,;,..;#",
        "#;,..,,.,;,..,.#",
        "#..;,..,;,..w,;#",
        "#;,..,.p..,;,..#",
        "#.,;,..,;,..,;.#",
        "#______________#",
        "################",
    ]

    above = [
        "................",
        "................",
        ".......LR.......",
        "................",
        "................",
        ".......YY.......",
        ".......yy.......",
        "................",
        "................",
        "................",
        "................",
        "................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 280, "h": 120, "name": "top_wall_left"},
        {"x": 360, "y": 0, "w": 280, "h": 120, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 600, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
        {"x": 0, "y": 420, "w": 640, "h": 60, "name": "bottom_wall"},
    ]

    objects = [
        # Default spawn & R1 wake position
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 240,
            "properties": {"facing": "up"},
        },
        # Spawn when coming back from Floofy
        {
            "name": "from_floofy",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door north -> ruins_floofy
        {
            "name": "door_to_floofy",
            "type": "door",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_floofy",
                "targetSpawn": "from_fall",
            },
        },
        # Bed of snack wrappers under the beam
        {
            "name": "wrapper_bed",
            "type": "prop",
            "x": 280,
            "y": 230,
            "w": 80,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_wrapper_pile.png",
                "anim": "bed",
                "solid": False,
                "lines": "* A bed of ancient snack wrappers.\n* History smells like cheese dust.",
            },
        },
        # Save point star
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 180,
            "y": 320,
        },
        # Examinable can decal on floor (col 12, row 7 -> x: 480, y: 280)
        {
            "name": "can_decal",
            "type": "examine",
            "x": 480,
            "y": 280,
            "w": 40,
            "h": 40,
            "properties": {
                "lines": "* Citrus Blast, 2014 edition. A vintage.",
            },
        },
        # Wake-up cutscene trigger (covers spawn on the bed)
        {
            "name": "trigger_wake",
            "type": "trigger",
            "x": 270,
            "y": 210,
            "w": 100,
            "h": 90,
            "properties": {
                "cutscene": "r1_wake",
                "once": "ruins.r1.woke",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_fall",
        display_name="Ruins - Fall Room",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
        save_place="* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.",
    )


# =============================================================================
# Room R2: Floofy (ruins_floofy)
# Size: 16x18 tiles (640x720 px).
# Taller corridor with convention carpet. Floofy in the center.
# Trigger across the path, cutscene r2_floofy, Furiell arrives.
# =============================================================================
def build_ruins_floofy():
    width = 16
    height = 18

    ground = [
        "################",
        "###[==]  [==]###",
        "###{-v}  {-k}###",
        "###.;.7889.,.###",
        "###.,.4556.;.###",
        "###;.,4556.,.###",
        "###.,*45z6.;.###",
        "###;.,4556.w.###",
        "###.,.4556.,.###",
        "###;.,4x56.;.###",
        "###.,t4556.,.###",
        "###;.,4556.,.###",
        "###.,.4556.;.###",
        "###;.,4556.p.###",
        "###.,.4556.,.###",
        "###;..1223.;.###",
        "###___    ___###",
        "######    ######",
    ]

    above = [
        "................",
        "................",
        "......LR........",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 280, "h": 120, "name": "top_wall_left"},
        {"x": 360, "y": 0, "w": 280, "h": 120, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 120, "h": 720, "name": "left_void"},
        {"x": 520, "y": 0, "w": 120, "h": 720, "name": "right_void"},
        {"x": 0, "y": 640, "w": 280, "h": 80, "name": "bottom_wall_left"},
        {"x": 360, "y": 640, "w": 280, "h": 80, "name": "bottom_wall_right"},
    ]

    objects = [
        # Default spawn & spawn from fall room
        {
            "name": "from_fall",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 620,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 620,
            "properties": {"facing": "up"},
        },
        # Spawn when entering from entrance room (north)
        {
            "name": "from_entrance",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_fall
        {
            "name": "door_to_fall",
            "type": "door",
            "x": 280,
            "y": 680,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_fall",
                "targetSpawn": "from_floofy",
            },
        },
        # Door north forward to ruins_entrance
        {
            "name": "door_to_entrance",
            "type": "door",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_entrance",
                "targetSpawn": "from_floofy",
            },
        },
        # Floofy NPC (disappears when met)
        {
            "name": "floofy",
            "type": "npc",
            "x": 300,
            "y": 340,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/sprites/ow/floofy.png",
                "anim": "idle",
                "hideIf": "ruins.floofy.met >= 1",
                "solid": True,
            },
        },
        # Encounter trigger across corridor
        {
            "name": "trigger_floofy",
            "type": "trigger",
            "x": 120,
            "y": 420,
            "w": 400,
            "h": 40,
            "properties": {
                "cutscene": "r2_floofy",
                "once": "ruins.floofy.met.seen",
            },
        },
        # Furiell NPC after cutscene, stands near north exit
        {
            "name": "furiell",
            "type": "npc",
            "x": 220,
            "y": 130,
            "w": 64,
            "h": 80,
            "properties": {
                "sprite": "assets/sprites/ow/furiell.png",
                "anim": "stand_down",
                "showIf": "ruins.floofy.met >= 1",
                "speaker": "FURIELL",
                "lines": "Welcome to your new home, my child. ...Do you like blankets? I have forty.",
                "solid": True,
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_floofy",
        display_name="Ruins - Flower Bed",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
    )


# =============================================================================
# Room R3: Ruins Entrance (ruins_entrance)
# Size: 22x14 tiles (880x560 px).
# Iconic double staircase up to a landing doorway. Banners, Dorito pile, savepoint.
# Furiell intro trigger & cutscene r3_furiell.
# =============================================================================
def build_ruins_entrance():
    width = 22
    height = 14

    ground = [
        "######################",
        "#[=======]  [=======]#",
        "#{-v---k-}  {---o---}#",
        "#.,.,.QE......QE.,.,.#",
        "#;.,..AD[====]AD..;,;#",
        "#.,;..ZX{-hh-}ZX.,;,;#",
        "#;.,...,______,..;,..#",
        "#.,.7888888888889.;..#",
        "#;.,4555555555556.,;.#",
        "#.,.1222222222223.;..#",
        "#;.,...,;,..,;,..;.,.#",
        "#.,;,..,;,..,;,..,;,;#",
        "#_________  _________#",
        "##########  ##########",
    ]

    above = [
        "......................",
        "..........LR..........",
        "......................",
        "......................",
        "......................",
        "......................",
        "......................",
        "......................",
        "......................",
        "......................",
        "......................",
        "......................",
        "......................",
        "......................",
    ]

    collision = [
        # Top walls flanking doorway
        {"x": 0, "y": 0, "w": 400, "h": 120, "name": "top_wall_left"},
        {"x": 480, "y": 0, "w": 400, "h": 120, "name": "top_wall_right"},
        # Side voids
        {"x": 0, "y": 0, "w": 40, "h": 560, "name": "left_void"},
        {"x": 840, "y": 0, "w": 40, "h": 560, "name": "right_void"},
        # Bottom walls flanking doorway south
        {"x": 0, "y": 480, "w": 400, "h": 80, "name": "bottom_wall_left"},
        {"x": 480, "y": 480, "w": 400, "h": 80, "name": "bottom_wall_right"},
        # Central wall between the double stairs
        {"x": 320, "y": 160, "w": 240, "h": 80, "name": "central_wall"},
    ]

    objects = [
        # South spawn (from Floofy) & default
        {
            "name": "from_floofy",
            "type": "spawn",
            "point": True,
            "x": 420,
            "y": 460,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 420,
            "y": 460,
            "properties": {"facing": "up"},
        },
        # North spawn (from Switches, Dummy, and legacy Notices)
        {
            "name": "from_switches",
            "type": "spawn",
            "point": True,
            "x": 420,
            "y": 140,
            "properties": {"facing": "down"},
        },
        {
            "name": "from_dummy",
            "type": "spawn",
            "point": True,
            "x": 420,
            "y": 140,
            "properties": {"facing": "down"},
        },
        {
            "name": "from_notices",
            "type": "spawn",
            "point": True,
            "x": 420,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_floofy
        {
            "name": "door_to_floofy",
            "type": "door",
            "x": 400,
            "y": 520,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_floofy",
                "targetSpawn": "from_entrance",
            },
        },
        # Door north forward to ruins_switches
        {
            "name": "door_to_switches",
            "type": "door",
            "x": 400,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_switches",
                "targetSpawn": "from_entrance",
            },
        },
        # Welcome Banner on central wall
        {
            "name": "banner_welcome",
            "type": "prop",
            "x": 380,
            "y": 155,
            "w": 120,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_banner_welcome.png",
                "solid": False,
                "lines": "* \"FurNation Con '14 - Welcome!\"",
            },
        },
        # Deodorant Banner below Welcome banner on central wall
        {
            "name": "banner_deodorant",
            "type": "prop",
            "x": 380,
            "y": 215,
            "w": 120,
            "h": 32,
            "properties": {
                "sprite": "assets/props/ruins_banner_deodorant.png",
                "solid": False,
                "lines": "* \"Deodorant is NOT optional.\"",
            },
        },
        # Dorito bag pile prop on the lower floor
        {
            "name": "dorito_pile",
            "type": "prop",
            "x": 220,
            "y": 380,
            "w": 80,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_wrapper_pile.png",
                "anim": "pile",
                "solid": False,
                "lines": "* A pile of Dorito bags. *crunch*",
            },
        },
        # Savepoint star
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 300,
            "y": 380,
        },
        # Furiell intro trigger near entrance
        {
            "name": "trigger_furiell",
            "type": "trigger",
            "x": 360,
            "y": 440,
            "w": 160,
            "h": 40,
            "properties": {
                "cutscene": "r3_furiell",
                "once": "ruins.furiell.r3",
            },
        },
        # Furiell NPC after intro cutscene
        {
            "name": "furiell_entrance",
            "type": "npc",
            "x": 520,
            "y": 350,
            "w": 64,
            "h": 80,
            "properties": {
                "sprite": "assets/sprites/ow/furiell.png",
                "anim": "stand_down",
                "showIf": "ruins.furiell.r3 >= 1",
                "speaker": "FURIELL",
                "lines": "The surface is full of cruel people with cameras. Down here, nobody will ever laugh at you.||You can always accept a hug. It is the fastest way to make friends.",
                "solid": True,
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_entrance",
        display_name="Ruins - Entrance",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.ruins_entrance",
        save_place="* The distant hum of dial-up internet fills you with DETERMINATION.",
    )


# =============================================================================
# Room R4: RGB Switches (ruins_switches)
# Size: 16x12 tiles (640x480 px).
# Three switches on the wall. Two correct ones marked with glitter (sw1, sw3).
# One wrong switch (sw2) with a note from Ruins Administration.
# Gate across north exit blocked by cables until both right switches are pressed.
# =============================================================================
def build_ruins_switches():
    width = 16
    height = 12

    ground = [
        "#[====]  [====]#",
        "#{---k}  {--v-}#",
        "#.,;,..,;,..,;.#",
        "#;.,...*,;,..,.#",
        "#.,;,..,.w..,;.#",
        "#;..,.c....,;..#",
        "#.,;,....,;,..,#",
        "#;.,..,p..,;,..#",
        "#.,;....,;,..,.#",
        "#;.,...,;,..,;.#",
        "#______  ______#",
        "#######  #######",
    ]

    above = [
        "................",
        "................",
        ".......LR.......",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 280, "h": 120, "name": "top_wall_left"},
        {"x": 360, "y": 0, "w": 280, "h": 120, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 600, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
        {"x": 0, "y": 440, "w": 280, "h": 40, "name": "bottom_wall_left"},
        {"x": 360, "y": 440, "w": 280, "h": 40, "name": "bottom_wall_right"},
    ]

    objects = [
        # South spawn (from entrance) & default spawn
        {
            "name": "from_entrance",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 370,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 370,
            "properties": {"facing": "up"},
        },
        # North spawn (from dummy)
        {
            "name": "from_dummy",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_entrance
        {
            "name": "door_to_entrance",
            "type": "door",
            "x": 280,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_entrance",
                "targetSpawn": "from_switches",
            },
        },
        # Door north forward to ruins_dummy
        {
            "name": "door_to_dummy",
            "type": "door",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_dummy",
                "targetSpawn": "from_switches",
            },
        },
        # Entrance hint trigger
        {
            "name": "trigger_hint",
            "type": "trigger",
            "x": 240,
            "y": 380,
            "w": 160,
            "h": 40,
            "properties": {
                "cutscene": "r4_hint",
                "once": "ruins.r4.intro",
            },
        },
        # Switch 1: Right switch (marked with glitter)
        {
            "name": "sw1",
            "type": "prop",
            "x": 120,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "counter": "ruins.r4.sw1",
                "lines": "* You flipped the switch. Click! Glitter spilled everywhere.",
                "lines2": "* The switch is already flipped. There is glitter all over your hands.",
            },
        },
        {
            "name": "sw1_marked",
            "type": "prop",
            "x": 120,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "marked",
                "hideIf": "ruins.r4.sw1 >= 1",
                "solid": False,
            },
        },
        {
            "name": "sw1_on",
            "type": "prop",
            "x": 120,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "on",
                "showIf": "ruins.r4.sw1 >= 1",
                "solid": False,
            },
        },
        # Switch 2: Wrong switch
        {
            "name": "sw2",
            "type": "prop",
            "x": 200,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "counter": "ruins.r4.sw2",
                "lines": "* You flipped the switch. It made a hollow clunk. Nothing happened.",
                "lines2": "* Still nothing. It feels like pressing an unmapped key.",
            },
        },
        {
            "name": "sw2_off",
            "type": "prop",
            "x": 200,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "off",
                "hideIf": "ruins.r4.sw2 >= 1",
                "solid": False,
            },
        },
        {
            "name": "sw2_on",
            "type": "prop",
            "x": 200,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "on",
                "showIf": "ruins.r4.sw2 >= 1",
                "solid": False,
            },
        },
        # Note on wall next to wrong switch
        {
            "name": "note_switch2",
            "type": "examine",
            "x": 240,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "lines": "* \"Not this one. - Ruins Administration (not Furiell)\"",
            },
        },
        # Sign
        {
            "name": "sign_courage",
            "type": "sign",
            "x": 400,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "lines": "* \"Only the fearless may proceed. Brave ones, foolish ones. Both walk not the path of the noob.\"",
            },
        },
        # Switch 3: Right switch (marked with glitter)
        {
            "name": "sw3",
            "type": "prop",
            "x": 480,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "counter": "ruins.r4.sw3",
                "lines": "* You flipped the switch. Click! A distant latch clicked open.",
                "lines2": "* The switch is already on. It emits a faint RGB glow.",
            },
        },
        {
            "name": "sw3_marked",
            "type": "prop",
            "x": 480,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "marked",
                "hideIf": "ruins.r4.sw3 >= 1",
                "solid": False,
            },
        },
        {
            "name": "sw3_on",
            "type": "prop",
            "x": 480,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "on",
                "showIf": "ruins.r4.sw3 >= 1",
                "solid": False,
            },
        },
        # North gate: solid cables blocking exit when unsolved
        {
            "name": "gate_cables_left",
            "type": "prop",
            "x": 280,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
                "solid": True,
                "hideIf": "ruins.r4.sw1 >= 1 && ruins.r4.sw3 >= 1",
            },
        },
        {
            "name": "gate_cables_right",
            "type": "prop",
            "x": 320,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
                "solid": True,
                "hideIf": "ruins.r4.sw1 >= 1 && ruins.r4.sw3 >= 1",
            },
        },
        # Visible barrier one tile lower: the north arch (above layer) covers the y=80 row.
        {
            "name": "gate_front_left",
            "type": "prop",
            "x": 280,
            "y": 120,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
                "solid": True,
                "hideIf": "ruins.r4.sw1 >= 1 && ruins.r4.sw3 >= 1",
                "lines": "* A tangle of live cables blocks the way.\n* Somewhere, a switch is waiting.",
            },
        },
        {
            "name": "gate_front_right",
            "type": "prop",
            "x": 320,
            "y": 120,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
                "solid": True,
                "hideIf": "ruins.r4.sw1 >= 1 && ruins.r4.sw3 >= 1",
                "lines": "* A tangle of live cables blocks the way.\n* Somewhere, a switch is waiting.",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_switches",
        display_name="Ruins - Switch Room",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
    )


# =============================================================================
# Room R5: Fursuit Dummy (ruins_dummy)
# Size: 16x12 tiles (640x480 px).
# Furiell's battle tutorial room between R3 and R8.
# Features:
#   - Carpet patch in center with Fursuit Dummy prop and Furiell NPC.
#   - North door to ruins_notices, south door to ruins_entrance.
#   - Intro trigger running r5_dummy (Furiell lines + startBattle fursuit_dummy).
#   - Post-battle reaction triggers with mutually-exclusive showIf conditions,
#     sharing once: "ruins.dummy.reacted".
# =============================================================================
def build_ruins_dummy():
    width = 16
    height = 12

    ground = [
        "################",
        "#[====]  [====]#",
        "#{---k}  {--v-}#",
        "#.,;,..,;,..,;.#",
        "#;.,...*,;,..,.#",
        "#.,;.788889.,;.#",
        "#;,..455z56..,.#",
        "#..;.122223.w,;#",
        "#;,..,.p..,;,..#",
        "#.,;,..,;,..,;.#",
        "#______  ______#",
        "#######  #######",
    ]

    above = [
        "................",
        "................",
        ".......LR.......",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 280, "h": 120, "name": "top_wall_left"},
        {"x": 360, "y": 0, "w": 280, "h": 120, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 600, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
        {"x": 0, "y": 440, "w": 280, "h": 40, "name": "bottom_wall_left"},
        {"x": 360, "y": 440, "w": 280, "h": 40, "name": "bottom_wall_right"},
    ]

    objects = [
        # South spawn (from switches, entrance) & default spawn
        {
            "name": "from_switches",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 370,
            "properties": {"facing": "up"},
        },
        {
            "name": "from_entrance",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 370,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 370,
            "properties": {"facing": "up"},
        },
        # North spawn (from cables, hallway, notices)
        {
            "name": "from_cables",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 140,
            "properties": {"facing": "down"},
        },
        {
            "name": "from_hallway",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 140,
            "properties": {"facing": "down"},
        },
        {
            "name": "from_notices",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_switches
        {
            "name": "door_to_switches",
            "type": "door",
            "x": 280,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_switches",
                "targetSpawn": "from_dummy",
            },
        },
        # Door north forward to ruins_cables
        {
            "name": "door_to_cables",
            "type": "door",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_cables",
                "targetSpawn": "from_dummy",
            },
        },
        # Fursuit Dummy prop on carpet
        {
            "name": "dummy",
            "type": "prop",
            "x": 250,
            "y": 220,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/sprites/ow/fursuit_dummy.png",
                "anim": "idle",
                "solid": True,
                "hideIf": "battle.fursuit_dummy.killed >= 1",
                "lines": "* A mannequin wearing a fursuit head. Smells like the previous owner.",
            },
        },
        # Furiell NPC standing beside dummy
        {
            "name": "furiell",
            "type": "npc",
            "x": 350,
            "y": 200,
            "w": 60,
            "h": 60,
            "properties": {
                "sprite": "assets/sprites/ow/furiell.png",
                "anim": "stand_down",
                "speaker": "FURIELL",
                "lines": "A warm embrace is always the best choice, my child.",
                "solid": True,
            },
        },
        # Intro trigger across the path to the dummy
        {
            "name": "trigger_intro",
            "type": "trigger",
            "x": 40,
            "y": 320,
            "w": 560,
            "h": 40,
            "properties": {
                "cutscene": "r5_dummy",
                "once": "ruins.dummy.intro",
            },
        },
        # Post-battle reaction triggers covering the area around the dummy.
        # Exactly one will be visible based on the battle outcome; all share once: "ruins.dummy.reacted".
        {
            "name": "trigger_react_hug",
            "type": "trigger",
            "x": 40,
            "y": 120,
            "w": 560,
            "h": 315,
            "properties": {
                "cutscene": "r5_dummy_hug",
                "once": "ruins.dummy.reacted",
                "showIf": "ruins.dummy.hugged >= 1 && battle.fursuit_dummy.killed == 0",
            },
        },
        {
            "name": "trigger_react_talk",
            "type": "trigger",
            "x": 40,
            "y": 120,
            "w": 560,
            "h": 315,
            "properties": {
                "cutscene": "r5_dummy_talk",
                "once": "ruins.dummy.reacted",
                "showIf": "ruins.dummy.talked >= 1 && ruins.dummy.hugged == 0 && battle.fursuit_dummy.killed == 0",
            },
        },
        {
            "name": "trigger_react_fight",
            "type": "trigger",
            "x": 40,
            "y": 120,
            "w": 560,
            "h": 315,
            "properties": {
                "cutscene": "r5_dummy_fight",
                "once": "ruins.dummy.reacted",
                "showIf": "battle.fursuit_dummy.killed >= 1",
            },
        },
        {
            "name": "trigger_react_flee",
            "type": "trigger",
            "x": 40,
            "y": 120,
            "w": 560,
            "h": 315,
            "properties": {
                "cutscene": "r5_dummy_other",
                "once": "ruins.dummy.reacted",
                "showIf": "fleesTotal >= 1 && ruins.dummy.hugged == 0 && ruins.dummy.talked == 0 && battle.fursuit_dummy.killed == 0",
            },
        },
        {
            "name": "trigger_react_spare",
            "type": "trigger",
            "x": 40,
            "y": 120,
            "w": 560,
            "h": 315,
            "properties": {
                "cutscene": "r5_dummy_other",
                "once": "ruins.dummy.reacted",
                "showIf": "battle.fursuit_dummy.spared >= 1 && ruins.dummy.hugged == 0 && ruins.dummy.talked == 0 && battle.fursuit_dummy.killed == 0",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_dummy",
        display_name="Ruins - Training Room",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
    )


# =============================================================================
# Room R6: Cable Maze (ruins_cables)
# Size: 20x12 tiles (800x480 px).
# Live network cables crossed on the floor with hazard damage.
# Clear readable zig-zag path across the room.
# Furiell stands near entrance and guides Pepe across via cutscene r6_guide.
# Sign on the wall with Ruins Administration note.
# =============================================================================
def build_ruins_cables():
    width = 20
    height = 12

    # Col 0: # (void), Cols 1-14: wall top, Cols 15-16: exit doorway, Cols 17-18: wall top, Col 19: # (void)
    ground = [
        "#[============]  []#",
        "#{h-v---k---v-}  {}#",
        "#.,;,..;,..,;,....;#",
        "#;.,..,.c..,;,..;,.#",
        "#.,;,..;,..;,..,;,.#",
        "#;.,...,;,..;.,...,#",
        "#.,;,....,;,....,;,#",
        "#;.,..,p..,;,..;.,.#",
        "#.,;....,;,..,.w.,;#",
        "#;.,...,;,..,;,..,.#",
        "#_  _______________#",
        "##  ################",
    ]

    above = [
        "....................",
        "....................",
        "...............LR...",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 600, "h": 80, "name": "top_wall_left"},
        {"x": 680, "y": 0, "w": 120, "h": 80, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 760, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
        {"x": 0, "y": 440, "w": 80, "h": 40, "name": "bottom_wall_left"},
        {"x": 160, "y": 440, "w": 640, "h": 40, "name": "bottom_wall_right"},
    ]

    objects = [
        # South spawn (from dummy) & default spawn
        {
            "name": "from_dummy",
            "type": "spawn",
            "point": True,
            "x": 120,
            "y": 370,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 120,
            "y": 370,
            "properties": {"facing": "up"},
        },
        # North spawn (from hallway)
        {
            "name": "from_hallway",
            "type": "spawn",
            "point": True,
            "x": 640,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_dummy
        {
            "name": "door_to_dummy",
            "type": "door",
            "x": 80,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_dummy",
                "targetSpawn": "from_cables",
            },
        },
        # Door north forward to ruins_hallway
        {
            "name": "door_to_hallway",
            "type": "door",
            "x": 600,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_hallway",
                "targetSpawn": "from_cables",
            },
        },
        # Furiell NPC standing near entrance
        {
            "name": "furiell",
            "type": "npc",
            "x": 140,
            "y": 260,
            "w": 60,
            "h": 60,
            "properties": {
                "sprite": "assets/sprites/ow/furiell.png",
                "anim": "stand_down",
                "speaker": "FURIELL",
                "lines": "* She is watching your step with great care.",
                "solid": True,
                "hideIf": "ruins.r6.guided >= 1",
            },
        },
        # Entry trigger for Furiell guide cutscene
        {
            "name": "trigger_guide",
            "type": "trigger",
            "x": 60,
            "y": 320,
            "w": 140,
            "h": 60,
            "properties": {
                "cutscene": "r6_guide",
                "once": "ruins.r6.guide",
            },
        },
        # Examinable note on wall
        {
            "name": "sign_admin",
            "type": "sign",
            "x": 80,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "lines": "* NOTICE FROM RUINS ADMINISTRATION:\n* Do not unplug the blue cables.\n* The Minecraft server runs on this hallway.||* (The note is taped down with duct tape.)",
            },
        },
        # Hazards covering the 4 cable barriers
        {
            "name": "hazard_barrier_1",
            "type": "hazard",
            "x": 200,
            "y": 80,
            "w": 40,
            "h": 240,
            "properties": {
                "damage": 2,
                "lines": "* Ow. That's a Cat5e. Uncomfortably live.",
            },
        },
        {
            "name": "hazard_barrier_2",
            "type": "hazard",
            "x": 320,
            "y": 200,
            "w": 40,
            "h": 200,
            "properties": {
                "damage": 2,
                "lines": "* Ow. That's a Cat5e. Uncomfortably live.",
            },
        },
        {
            "name": "hazard_barrier_3",
            "type": "hazard",
            "x": 440,
            "y": 80,
            "w": 40,
            "h": 240,
            "properties": {
                "damage": 2,
                "lines": "* Ow. That's a Cat5e. Uncomfortably live.",
            },
        },
        {
            "name": "hazard_barrier_4",
            "type": "hazard",
            "x": 560,
            "y": 200,
            "w": 40,
            "h": 200,
            "properties": {
                "damage": 2,
                "lines": "* Ow. That's a Cat5e. Uncomfortably live.",
            },
        },
    ]

    # Add animated cable props for each barrier
    # Barrier 1: x=200, rows 2..7 (y=80, 120, 160, 200, 240, 280)
    for i, y in enumerate(range(80, 320, 40)):
        objects.append({
            "name": f"cable_b1_{i}",
            "type": "prop",
            "x": 200,
            "y": y,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
            },
        })

    # Barrier 2: x=320, rows 5..9 (y=200, 240, 280, 320, 360)
    for i, y in enumerate(range(200, 400, 40)):
        objects.append({
            "name": f"cable_b2_{i}",
            "type": "prop",
            "x": 320,
            "y": y,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
            },
        })

    # Barrier 3: x=440, rows 2..7 (y=80, 120, 160, 200, 240, 280)
    for i, y in enumerate(range(80, 320, 40)):
        objects.append({
            "name": f"cable_b3_{i}",
            "type": "prop",
            "x": 440,
            "y": y,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
            },
        })

    # Barrier 4: x=560, rows 5..9 (y=200, 240, 280, 320, 360)
    for i, y in enumerate(range(200, 400, 40)):
        objects.append({
            "name": f"cable_b4_{i}",
            "type": "prop",
            "x": 560,
            "y": y,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
            },
        })

    return make_tiled_map(
        room_id="ruins_cables",
        display_name="Ruins - Cable Maze",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
    )


# =============================================================================
# Room R7: Long Hallway (ruins_hallway)
# Size: 52x12 tiles (2080x480 px, > 3 screens wide).
# Independence test: Furiell gives Pepe the CELL flip phone at entrance.
# Long hallway with pillars, decals, and Furiell hiding behind a pillar near the end.
# Phone rings right before north exit door.
# =============================================================================
def build_ruins_hallway():
    width = 52
    height = 12

    # Col 0: # (void), Cols 1-44: wall, Cols 45-46: exit doorway, Cols 47-50: wall, Col 51: # (void)
    ground = [
        "#[==========================================]  [==]#",
        "#{---k-v-----v----k----v-----k----v-----k---}  {--}#",
        "#.,;,..,;,..,;,..,;,..,;,..,;,..,;,..,;,..,;...,.;.#",
        "#;.,...*,;,..,.c..;.,...,;,..,.w..;.,...,;,..,;;.,.#",
        "#.,;....,;,I,..,;,..,;,I,..,;,..,;,I,..,;,I,..,.,;,#",
        "#;..,.c....B;..,;..,...B;..,;..,...B;..,..B,;,;;..,#",
        "#.,;,....,;,..,.,;,....,;,..,.,;,....,;,..,.,.,.,;,#",
        "#;.,..,p..,;,..;.,..,p..,;,..;.,..,p..,;,..;..;;,..#",
        "#.,;....,;,..,.w.,;....,;,..,.c.,;....,;,..,..,..,.#",
        "#;.,...,;,..,;.,;.,...,;,..,;.,;.,...,;,..,;.c.;,;.#",
        "#_  _______________________________________________#",
        "##  ################################################",
    ]

    above = [
        "....................................................",
        "....................................................",
        ".............................................LR.....",
        "...........T...........T...........T......T.........",
        "..........................................I.........",
        "..........................................B.........",
        "....................................................",
        "....................................................",
        "....................................................",
        "....................................................",
        "....................................................",
        "....................................................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 1800, "h": 120, "name": "top_wall_left"},
        {"x": 1880, "y": 0, "w": 200, "h": 120, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 2040, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
        {"x": 0, "y": 440, "w": 80, "h": 40, "name": "bottom_wall_left"},
        {"x": 160, "y": 440, "w": 1920, "h": 40, "name": "bottom_wall_right"},
        {"x": 440, "y": 160, "w": 40, "h": 80, "name": "pillar_1"},
        {"x": 920, "y": 160, "w": 40, "h": 80, "name": "pillar_2"},
        {"x": 1400, "y": 160, "w": 40, "h": 80, "name": "pillar_3"},
        {"x": 1680, "y": 160, "w": 40, "h": 80, "name": "pillar_4"},
    ]

    objects = [
        # South spawn (from cables, dummy) & default spawn
        {
            "name": "from_cables",
            "type": "spawn",
            "point": True,
            "x": 120,
            "y": 370,
            "properties": {"facing": "up"},
        },
        {
            "name": "from_dummy",
            "type": "spawn",
            "point": True,
            "x": 120,
            "y": 370,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 120,
            "y": 370,
            "properties": {"facing": "up"},
        },
        # North spawn (from notices)
        {
            "name": "from_notices",
            "type": "spawn",
            "point": True,
            "x": 1840,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_cables
        {
            "name": "door_to_cables",
            "type": "door",
            "x": 80,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_cables",
                "targetSpawn": "from_hallway",
            },
        },
        # Door north forward to ruins_notices
        {
            "name": "door_to_notices",
            "type": "door",
            "x": 1800,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_notices",
                "targetSpawn": "from_hallway",
            },
        },
        # Trigger at entrance giving the CELL
        {
            "name": "trigger_cell",
            "type": "trigger",
            "x": 80,
            "y": 260,
            "w": 160,
            "h": 40,
            "properties": {
                "cutscene": "r7_cell",
                "once": "ruins.r7.intro",
            },
        },
        # Furiell hiding behind pillar 4
        {
            "name": "furiell_hiding",
            "type": "prop",
            "x": 1695,
            "y": 160,
            "w": 40,
            "h": 60,
            "properties": {
                "sprite": "assets/sprites/ow/furiell.png",
                "anim": "stand_left",
                "solid": False,
            },
        },
        # Examine object on pillar 4 seeing her tail
        {
            "name": "tail_examine",
            "type": "examine",
            "x": 1670,
            "y": 150,
            "w": 70,
            "h": 90,
            "properties": {
                "lines": "* (You can see her tail. She can see you seeing her tail.)",
            },
        },
        # Trigger before north exit for forced phone call
        {
            "name": "trigger_call",
            "type": "trigger",
            "x": 1740,
            "y": 120,
            "w": 50,
            "h": 240,
            "properties": {
                "cutscene": "r7_call",
                "once": "ruins.r7.call",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_hallway",
        display_name="Ruins - Long Hallway",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
    )


# =============================================================================
# Room R8: Notices, CRT and Art Show (ruins_notices)
# Size: 20x14 tiles (800x560 px).
# Notice board, flickering CRT with hidden laser pointer, Art Show booth and 18+ curtain.
# =============================================================================
def build_ruins_notices():
    width = 20
    height = 14

    ground = [
        "####################",
        "#[======]  [======]#",
        "#{--v-k-}  {---o--}#",
        "#.,;,..,.788889.,;.#",
        "#;.,..,.c455556.,..#",
        "#.,;,..,.455556;.w.#",
        "#;.,..,;.455556.,;.#",
        "#.,;,..*.455556;.,.#",
        "#;.,..,.p455556.,;.#",
        "#.,;,..,.122223;.,.#",
        "#;.,..,;,..,;,..;.,#",
        "#.,;,..,;,..,;,..,;#",
        "#________  ________#",
        "#########  #########",
    ]

    above = [
        "....................",
        ".........LR.........",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 360, "h": 120, "name": "top_wall_left"},
        {"x": 440, "y": 0, "w": 360, "h": 120, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 560, "name": "left_void"},
        {"x": 760, "y": 0, "w": 40, "h": 560, "name": "right_void"},
        {"x": 0, "y": 480, "w": 360, "h": 80, "name": "bottom_wall_left"},
        {"x": 440, "y": 480, "w": 360, "h": 80, "name": "bottom_wall_right"},
    ]

    objects = [
        # South spawn (from Hallway, Dummy, and legacy Entrance) & default
        {
            "name": "from_hallway",
            "type": "spawn",
            "point": True,
            "x": 380,
            "y": 460,
            "properties": {"facing": "up"},
        },
        {
            "name": "from_dummy",
            "type": "spawn",
            "point": True,
            "x": 380,
            "y": 460,
            "properties": {"facing": "up"},
        },
        {
            "name": "from_entrance",
            "type": "spawn",
            "point": True,
            "x": 380,
            "y": 460,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 380,
            "y": 460,
            "properties": {"facing": "up"},
        },
        # North spawn (from Barky's arena)
        {
            "name": "from_barky",
            "type": "spawn",
            "point": True,
            "x": 380,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_hallway
        {
            "name": "door_to_hallway",
            "type": "door",
            "x": 360,
            "y": 520,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_hallway",
                "targetSpawn": "from_notices",
            },
        },
        # Door north forward to ruins_barky
        {
            "name": "door_to_barky",
            "type": "door",
            "x": 360,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_barky",
                "targetSpawn": "from_notices",
            },
        },
        # Notice board on the west wall
        {
            "name": "notice_board",
            "type": "prop",
            "x": 140,
            "y": 90,
            "w": 80,
            "h": 60,
            "properties": {
                "sprite": "assets/props/ruins_notice_board.png",
                "solid": True,
                "lines": "* NOTICE FROM RUINS ADMINISTRATION: Please do not shed in the ventilation shafts. Violators will be banned from the Minecraft server.",
            },
        },
        # CRT Monitor with BSOD (anim flicker)
        {
            "name": "crt_monitor",
            "type": "prop",
            "x": 220,
            "y": 240,
            "w": 80,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_crt.png",
                "anim": "flicker",
                "solid": True,
                "lines": "* A blue screen of death. The error message is in Comic Sans.",
            },
        },
        # Item pickup behind CRT: Toy Laser Pointer
        {
            "name": "laser_pickup",
            "type": "itemPickup",
            "x": 240,
            "y": 210,
            "w": 40,
            "h": 20,
            "properties": {
                "item": "Toy Laser Pointer",
                "flag": "ruins.laser.taken",
                "lines": "* You found a Toy Laser Pointer behind the CRT.",
                "emptyLines": "* Just dust bunnies. Synthetic ones.",
            },
        },
        # Art Show 18+ curtain on the north-east wall
        {
            "name": "curtain_18",
            "type": "prop",
            "x": 600,
            "y": 60,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_curtain_18.png",
                "solid": True,
                "counter": "ruins.curtain.looks",
                "lines": "* You decide to respect the curtain.",
                "lines2": "* You respect the curtain. For a while.",
                "lines3": "* (...No.)",
            },
        },
        # Art Show booth / bid sheet table
        {
            "name": "art_show_booth",
            "type": "prop",
            "x": 520,
            "y": 220,
            "w": 120,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_booth.png",
                "solid": True,
                "lines": "* A bid sheet. Highest bid: 400G. For a drawing of a \"very tired raccoon\". You don't ask.",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_notices",
        display_name="Ruins - Notice Hall",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
        encounters="froggsona:2,shysona:1,modsmal:1",
        encounter_budget=1,
    )


# =============================================================================
# Room R9: Barky's Arena (ruins_barky)
# Size: 16x12 tiles (640x480 px).
# Keeps legacyBarky: true, preserves all named legacy objects:
# sign, dust, door, npc_barky, bowl, savepoint, from_notices, to_notices.
# Adds ruins_lamp prop.
# =============================================================================
def build_ruins_barky():
    # OVW-020: real tiles instead of the old test.bmp; the legacy objects keep their positions.
    width = 16
    height = 12

    ground = [
        "#[=====  =====]#",
        "#{--v-}  {k--o}#",
        "#.,;,..,;,..,;.#",
        "#;..,*...,;,..;#",
        "#;,I,..,;,..I,.#",
        "#.,B..,;,..,B;.#",
        "#;..,....,;,..;#",
        "#.,;,....,;,..;#",
        "#;,..,,.,;,..,.#",
        "#..;,..,;,..c,;#",
        "#;,..,.p..,;,..#",
        "#######..#######",
    ]

    above = [
        "................",
        ".......LR.......",
        "................",
        "...T........T...",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 640, "h": 80, "name": "top_wall"},
        {"x": 0, "y": 440, "w": 640, "h": 40, "name": "bottom_wall"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 600, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
        {"x": 120, "y": 160, "w": 40, "h": 80, "name": "left_pillar"},
        {"x": 480, "y": 160, "w": 40, "h": 80, "name": "right_pillar"},
    ]

    objects = [
        # Legacy objects
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 320,
        },
        {
            "name": "sign",
            "type": "interactable",
            "x": 100,
            "y": 70,
            "w": 40,
            "h": 30,
        },
        {
            "name": "dust",
            "type": "interactable",
            "x": 490,
            "y": 90,
            "w": 40,
            "h": 40,
        },
        {
            "name": "door",
            "type": "interactable",
            "x": 280,
            "y": 50,
            "w": 80,
            "h": 30,
        },
        {
            "name": "npc_barky",
            "type": "interactable",
            "x": 490,
            "y": 80,
            "w": 40,
            "h": 50,
        },
        {
            "name": "bowl",
            "type": "interactable",
            "x": 200,
            "y": 160,
            "w": 40,
            "h": 30,
        },
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 320,
            "y": 260,
        },
        {
            "name": "from_notices",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 370,
            "properties": {"facing": "up"},
        },
        {
            "name": "to_notices",
            "type": "door",
            "x": 280,
            "y": 410,
            "w": 80,
            "h": 30,
            "properties": {
                "targetRoom": "ruins_notices",
                "targetSpawn": "from_barky",
            },
        },
        # T2 addition: Lamp prop (Barky's natural enemy)
        {
            "name": "ruins_lamp",
            "type": "prop",
            "x": 530,
            "y": 140,
            "w": 40,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_lamp.png",
                "anim": "lit",
                "solid": True,
                "lines": "* Barky's natural enemy. Or his moon. He hasn't decided.",
            },
        },
        # Door north forward to ruins_bakesale (revealed once Barky is resolved)
        {
            "name": "to_bakesale",
            "type": "door",
            "x": 280,
            "y": 70,
            "w": 80,
            "h": 20,
            "properties": {
                "targetRoom": "ruins_bakesale",
                "targetSpawn": "from_barky",
                "showIf": "ruins.barky.state >= 1",
            },
        },
        # Spawn returning from ruins_bakesale
        {
            "name": "from_bakesale",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 100,
            "properties": {"facing": "down"},
        },
    ]

    return make_tiled_map(
        room_id="ruins_barky",
        display_name="Ruins - Barky's Arena",
        width=width,
        height=height,
        ground_grid=ground,
        above_grid=above,
        collision_boxes=collision,
        objects=objects,
        legacy_barky=True,
        tile_size=40,
    )


# =============================================================================
# Room R10: Bowl, Commission Bake Sale and Mod Rock (ruins_bakesale)
# Size: 20x12 tiles (800x480 px).
# Dorito bowl, Commission Bake Sale booth with sample donut, mod rock,
# Furiell forced call trigger, save point, and dark doorway north to R11.
# =============================================================================
def build_ruins_bakesale():
    width = 20
    height = 12

    ground = [
        "#[================]#",
        "#{---v----k---}  {}#",
        "#.,;,..;,..,;,....;#",
        "#;.,..,.c..,;,..;,.#",
        "#.,;,..;.7889.,;,..#",
        "#;.,..,..4556;.,..;#",
        "#.,;,....1223.,;,..#",
        "#;.,..:B:.,;,..,;,.#",
        "#.,;...*..,;,....,;#",
        "#;.,...,;,..,;,..,;#",
        "#______  __________#",
        "#######..###########",
    ]

    above = [
        "....................",
        "...............LR...",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 600, "h": 80, "name": "top_wall_left"},
        {"x": 680, "y": 0, "w": 120, "h": 80, "name": "top_wall_right"},
        {"x": 0, "y": 440, "w": 280, "h": 40, "name": "bottom_wall_left"},
        {"x": 360, "y": 440, "w": 440, "h": 40, "name": "bottom_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 760, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
        {"x": 280, "y": 280, "w": 40, "h": 40, "name": "col_modrock"},
    ]

    objects = [
        # South spawn (from Barky's Arena) & default spawn
        {
            "name": "from_barky",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 380,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 380,
            "properties": {"facing": "up"},
        },
        # Spawn when entering from Furiell's House (north)
        {
            "name": "from_house",
            "type": "spawn",
            "point": True,
            "x": 640,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_barky
        {
            "name": "door_to_barky",
            "type": "door",
            "x": 280,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_barky",
                "targetSpawn": "from_bakesale",
            },
        },
        # Save point
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 180,
            "y": 340,
            "properties": {
                "lines": "* The smell of baked goods and spider-art fills you with DETERMINATION.",
            },
        },
        # Trigger partway through the room for Furiell's forced phone call
        {
            "name": "trigger_call",
            "type": "trigger",
            "x": 40,
            "y": 320,
            "w": 720,
            "h": 40,
            "properties": {
                "cutscene": "r10_call",
                "once": "ruins.r10.call",
            },
        },
        # Dorito Bowl (full/empty via altAnim)
        {
            "name": "dorito_bowl",
            "type": "prop",
            "x": 120,
            "y": 140,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_dorito_bowl.png",
                "anim": "full",
                "altAnim": "empty",
                "altAnimIf": "ruins.bowl.takes >= 3",
                "hideIf": "ruins.bowl.broken >= 1",
                "cutscene": "r10_bowl",
                "counter": "ruins.bowl.takes",
                "solid": True,
            },
        },
        # Dorito Bowl (knocked over once broken)
        {
            "name": "dorito_bowl_knocked",
            "type": "prop",
            "x": 120,
            "y": 140,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_dorito_bowl.png",
                "anim": "knocked",
                "showIf": "ruins.bowl.broken >= 1",
                "cutscene": "r10_bowl",
                "counter": "ruins.bowl.takes",
                "solid": True,
            },
        },
        # Sign above the bowl
        {
            "name": "dorito_sign",
            "type": "sign",
            "x": 120,
            "y": 90,
            "w": 40,
            "h": 40,
            "properties": {
                "lines": "* \"Take one Dorito.\"",
            },
        },
        # The Mod Rock
        {
            "name": "mod_rock",
            "type": "prop",
            "x": 280,
            "y": 280,
            "w": 40,
            "h": 40,
            "properties": {
                "speaker": "MOD ROCK",
                "counter": "ruins.modrock.talks",
                "lines": "Hey. No pushing without permission.",
                "lines2": "...Permission granted.",
                "lines3": "Don't push your luck.",
                "solid": True,
                "bumpCutscene": "r10_modrock_push",
            },
        },
        # Commission Bake Sale sample donut pickup
        {
            "name": "sample_donut",
            "type": "itemPickup",
            "x": 330,
            "y": 170,
            "w": 40,
            "h": 40,
            "properties": {
                "item": "Commissioned Donut",
                "flag": "ruins.bakesale.sample",
                "lines": "* (You take the free sample.)\n* ETA: 6-8 weeks. ...It's here already. Somehow.",
                "emptyLines": "* (The sample plate is empty. A business card reads: 'Slots closed.')",
                "fullLines": "* (You try to take the free sample, but you're carrying too much.)",
            },
        },
        # Commission Bake Sale Booth
        {
            "name": "bakesale_booth",
            "type": "prop",
            "x": 380,
            "y": 150,
            "w": 120,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_booth.png",
                "solid": True,
                "lines": "* \"All proceeds go to spider artists. Slots open. ETA: yes.\"",
            },
        },
        # Door north forward to ruins_house
        {
            "name": "door_to_house",
            "type": "door",
            "x": 600,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_house",
                "targetSpawn": "from_bakesale",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_bakesale",
        display_name="Ruins - Bake Sale",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
        save_place="* The smell of baked goods and spider-art fills you with DETERMINATION.",
        encounters="froggsona:2,shysona:1,modsmal:1",
        encounter_budget=1,
    )


# =============================================================================
# Room R11: Furiell's House - Living Room (ruins_house)
# Size: 20x12 tiles (800x480 px).
# Cozy living room with fireplace, armchair, bookshelf, desk with laptop and
# letterhead, Glitter Pop-Tart pickup, locked door to her room, basement stairs,
# and the mirror.
# =============================================================================
def build_ruins_house():
    width = 20
    height = 12

    ground = [
        "#[=]u[============]#",
        "#{--}j{--QWE-----o}#",
        "#.,;..;,..ASD...;,;#",
        "#;.,..;,..,;,..;.,.#",
        "#.,.::.78888889.,;.#",
        "#;..c..45555556;.,.#",
        "#.,.,..455z5556.,;,#",
        "#;.,...45555p56;.,.#",
        "#.,;,..12222223.,;.#",
        "#;.,..*....*....;..#",
        "#________  ________#",
        "#########..#########",
    ]

    above = [
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
        "....................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 800, "h": 80, "name": "top_wall"},
        {"x": 0, "y": 440, "w": 360, "h": 40, "name": "bottom_wall_left"},
        {"x": 440, "y": 440, "w": 360, "h": 40, "name": "bottom_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 760, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
        {"x": 80, "y": 180, "w": 160, "h": 40, "name": "col_desk"},
    ]

    objects = [
        # South spawn (from Bake Sale) & default spawn
        {
            "name": "from_bakesale",
            "type": "spawn",
            "point": True,
            "x": 400,
            "y": 380,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 400,
            "y": 380,
            "properties": {"facing": "up"},
        },
        # Door south back to ruins_bakesale
        {
            "name": "door_to_bakesale",
            "type": "door",
            "x": 360,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_bakesale",
                "targetSpawn": "from_house",
            },
        },
        # Locked door to her room
        {
            "name": "furiell_room_door",
            "type": "examine",
            "x": 120,
            "y": 40,
            "w": 40,
            "h": 40,
            "properties": {
                "lines": "* It's locked. You hear a printer.",
            },
        },
        # Stairs to basement blocked with examine
        {
            "name": "stairs_basement",
            "type": "examine",
            "x": 240,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "lines": "* (The basement. Not yet.)",
            },
        },
        # Table / desk area - Laptop examine
        {
            "name": "laptop",
            "type": "examine",
            "x": 80,
            "y": 180,
            "w": 50,
            "h": 40,
            "properties": {
                "lines": "* She closes a tab very fast.",
            },
        },
        # Table / desk area - Letterhead examine
        {
            "name": "letterhead",
            "type": "examine",
            "x": 130,
            "y": 180,
            "w": 50,
            "h": 40,
            "properties": {
                "lines": "* \"RUINS ADMINISTRATION\". In her handwriting. Hm.",
            },
        },
        # Table / desk area - Glitter Pop-Tart itemPickup
        {
            "name": "poptart_pickup",
            "type": "itemPickup",
            "x": 180,
            "y": 180,
            "w": 50,
            "h": 40,
            "properties": {
                "item": "Glitter Pop-Tart",
                "flag": "ruins.house.poptart",
                "lines": "* (You take the Glitter Pop-Tart.)\n* It sparkles aggressively.",
                "emptyLines": "* (The plate is empty.)",
                "fullLines": "* You're carrying too much.",
            },
        },
        # Soft Armchair
        {
            "name": "armchair",
            "type": "prop",
            "x": 410,
            "y": 140,
            "w": 60,
            "h": 60,
            "properties": {
                "sprite": "assets/props/ruins_armchair.png",
                "solid": True,
                "lines": "* A very soft armchair.\n* It smells faintly of vanilla and dryer sheets.",
            },
        },
        # Crackling Fireplace
        {
            "name": "fireplace",
            "type": "prop",
            "x": 480,
            "y": 40,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_fireplace.png",
                "anim": "burn",
                "solid": True,
                "lines": "* A cozy fire crackles in the fireplace.",
            },
        },
        # Bookshelf with book line
        {
            "name": "bookshelf",
            "type": "prop",
            "x": 580,
            "y": 40,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_bookshelf.png",
                "solid": True,
                "lines": "* \"Fanfics of Questionable Quality, Vol. 7\". You read one sentence. You put it down very carefully.",
            },
        },
        # Mirror with normal and fur lines
        {
            "name": "mirror",
            "type": "prop",
            "x": 680,
            "y": 40,
            "w": 40,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_mirror.png",
                "anim": "normal",
                "altAnim": "fur",
                "altAnimIf": "fur >= 20",
                "solid": True,
                "lines": "* It's you!",
                "furMin": 20,
                "furLines": "* It's... still you. With ears.",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_house",
        display_name="Ruins - Furiell's House",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
    )


def main():
    repo_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    maps_dir = os.path.join(repo_root, "data", "maps")
    os.makedirs(maps_dir, exist_ok=True)

    builders = {
        "ruins_fall.json": build_ruins_fall,
        "ruins_floofy.json": build_ruins_floofy,
        "ruins_entrance.json": build_ruins_entrance,
        "ruins_switches.json": build_ruins_switches,
        "ruins_dummy.json": build_ruins_dummy,
        "ruins_cables.json": build_ruins_cables,
        "ruins_hallway.json": build_ruins_hallway,
        "ruins_notices.json": build_ruins_notices,
        "ruins_barky.json": build_ruins_barky,
        "ruins_bakesale.json": build_ruins_bakesale,
        "ruins_house.json": build_ruins_house,
    }

    print("Building Ruins maps...")
    for filename, builder in builders.items():
        filepath = os.path.join(maps_dir, filename)
        map_data = builder()
        with open(filepath, "w", encoding="utf-8") as f:
            json.dump(map_data, f, indent=2)
            f.write("\n")
        print(f"  -> Wrote {filename} ({map_data['width']}x{map_data['height']} tiles)")
    print("Done!")


if __name__ == "__main__":
    main()

