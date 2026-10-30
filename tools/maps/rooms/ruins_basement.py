"""ruins_basement: the room builder for data/maps/ruins_basement.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R12: Basement Corridor (ruins_basement)
# Size: 16x38 tiles (640x1520 px).
# Long, dark, narrow corridor leading north to the iron door and ruins exit (via R12a VIP Lounge).
# =============================================================================
def build_ruins_basement():
    width = 16
    height = 38

    ground = [
        "#####[]nn[]#####",
        "#####{}NN{}#####",
        "#####{....}#####",
        "#####{....}#####",
        "#####{.;..}#####",
        "#####{...,}#####",
        "#####{....}#####",
        "#####{..c.}#####",
        "#####{....}#####",
        "#####{.;..}#####",
        "#####{....}#####",
        "#####{...,}#####",
        "#####{....}#####",
        "#####{..w.}#####",
        "#####{....}#####",
        "#####{.;..}#####",
        "#####{....}#####",
        "#####{...,}#####",
        "#####{....}#####",
        "#####{:...}#####",
        "#####{....}#####",
        "#####{.;..}#####",
        "#####{....}#####",
        "#####{...,}#####",
        "#####{....}#####",
        "#####{..c.}#####",
        "#####{....}#####",
        "#####{.;..}#####",
        "#####{....}#####",
        "#####{...,}#####",
        "#####{....}#####",
        "#####{.;..}#####",
        "#####{....}#####",
        "#####{....}#####",
        "#####{....}#####",
        "#####{-QWE}#####",
        "#####{-ASD}#####",
        "################",
    ]

    above = ["." * width] * height

    collision = [
        {"x": 0, "y": 0, "w": 240, "h": 1520, "name": "left_void_wall"},
        {"x": 400, "y": 0, "w": 240, "h": 1520, "name": "right_void_wall"},
        {"x": 240, "y": 0, "w": 40, "h": 80, "name": "top_wall_left"},
        {"x": 360, "y": 0, "w": 40, "h": 80, "name": "top_wall_right"},
        {"x": 240, "y": 1480, "w": 40, "h": 40, "name": "bottom_wall_left"},
        {"x": 360, "y": 1480, "w": 40, "h": 40, "name": "bottom_wall_right"},
    ]

    objects = [
        # South spawn (from house) & default spawn
        {
            "name": "from_house",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 1380,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 1380,
            "properties": {"facing": "up"},
        },
        # South door back to ruins_house
        {
            "name": "door_to_house",
            "type": "door",
            "x": 280,
            "y": 1440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_house",
                "targetSpawn": "from_basement",
            },
        },
        # North door forward to ruins_viplounge (R12a)
        {
            "name": "door_to_viplounge",
            "type": "",
            "class": "door",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_viplounge",
                "targetSpawn": "from_basement",
            },
        },
        # North spawn (returning from ruins_viplounge)
        {
            "name": "from_viplounge",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 120,
            "properties": {"facing": "down"},
        },
        # Legacy/alias north spawn (from_exit)
        {
            "name": "from_exit",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 120,
            "properties": {"facing": "down"},
        },
        # Furiell NPC (visible only before fight is resolved)
        {
            "name": "furiell",
            "type": "npc",
            "x": 288,
            "y": 1240,
            "w": 64,
            "h": 80,
            "properties": {
                "sprite": "assets/sprites/ow/furiell.png",
                "anim": "stand_up",
                "showIf": "ruins.furiell.spared == 0 && ruins.furiell.killed == 0",
                "solid": False,
            },
        },
        # Entry trigger for Furiell walk-ahead cutscene
        {
            "name": "trigger_r12_entry",
            "type": "trigger",
            "x": 240,
            "y": 1340,
            "w": 160,
            "h": 40,
            "properties": {
                "cutscene": "r12_entry",
                "once": "ruins.r12.walk",
                "showIf": "ruins.furiell.spared == 0 && ruins.furiell.killed == 0",
            },
        },
        # Stop 1 trigger
        {
            "name": "trigger_r12_stop1",
            "type": "trigger",
            "x": 240,
            "y": 1060,
            "w": 160,
            "h": 40,
            "properties": {
                "cutscene": "r12_stop1",
                "once": "ruins.r12.stop1",
                "showIf": "ruins.furiell.spared == 0 && ruins.furiell.killed == 0",
            },
        },
        # Stop 2 trigger
        {
            "name": "trigger_r12_stop2",
            "type": "trigger",
            "x": 240,
            "y": 780,
            "w": 160,
            "h": 40,
            "properties": {
                "cutscene": "r12_stop2",
                "once": "ruins.r12.stop2",
                "showIf": "ruins.furiell.spared == 0 && ruins.furiell.killed == 0",
            },
        },
        # Stop 3 trigger
        {
            "name": "trigger_r12_stop3",
            "type": "trigger",
            "x": 240,
            "y": 500,
            "w": 160,
            "h": 40,
            "properties": {
                "cutscene": "r12_stop3",
                "once": "ruins.r12.stop3",
                "showIf": "ruins.furiell.spared == 0 && ruins.furiell.killed == 0",
            },
        },
        # North fight trigger before iron door
        {
            "name": "trigger_r12_boss",
            "type": "trigger",
            "x": 240,
            "y": 240,
            "w": 160,
            "h": 40,
            "properties": {
                "cutscene": "r12_boss",
                "showIf": "ruins.furiell.spared == 0 && ruins.furiell.killed == 0",
            },
        },
        {
            "name": "staff_only_notice",
            "type": "examine",
            "x": 280,
            "y": 200,
            "w": 80,
            "h": 40,
            "properties": {
                "lines": "* A wall notice reads \"STAFF ONLY.\"||* Someone added \"emotionally.\"",
            },
        },
        {
            "name": "lanyard_dust_bunny",
            "type": "examine",
            "x": 280,
            "y": 600,
            "w": 80,
            "h": 40,
            "properties": {
                "lines": "* A dust bunny has a STAFF lanyard.||* It outranks you.",
            },
        },
        {
            "name": "nice_tally",
            "type": "examine",
            "x": 280,
            "y": 1000,
            "w": 80,
            "h": 40,
            "properties": {
                "lines": "* The tally marks stop at sixty-nine.||* A second hand wrote \"nice.\"",
            },
        },
        {
            "name": "almost_home_sticker",
            "type": "examine",
            "x": 280,
            "y": 1360,
            "w": 80,
            "h": 40,
            "properties": {
                "lines": "* A paw-print sticker says \"YOU ARE ALMOST HOME.\"||* It points both ways.",
            },
        },
    ]

    tiled_map = make_tiled_map(
        room_id="ruins_basement",
        display_name="Ruins - Basement",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
    )
    for layer in tiled_map["layers"]:
        if layer["type"] == "objectgroup":
            for obj in layer["objects"]:
                if obj.get("name") == "door_to_viplounge":
                    obj["type"] = ""
                    obj["class"] = "door"
    return tiled_map
