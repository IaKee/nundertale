"""ruins_house_pepe: the room builder for data/maps/ruins_house_pepe.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R11-B: Pepe's Room (ruins_house_pepe)
# Size: 16x11 tiles (640x440 px).
# Cozy bedroom with bed, bedside lamp, closet with fursuit head, and beanbag.
# =============================================================================
def build_ruins_house_pepe():
    width = 16
    height = 11

    ground = [
        "#[============]#",
        "#{------------}#",
        "#.,;..;,..;.,..#",
        "#;.,..;,..;,..;#",
        "#.,;.78888889.,#",
        "#;.,.45555556;.#",
        "..;,.45555556.,#",
        "..;,.45555556;.#",
        "#.,;,12222223.,#",
        "#;.,..;.,..*.;.#",
        "################",
    ]

    above = ["." * width] * height

    collision = [
        {"x": 0, "y": 0, "w": 640, "h": 80, "name": "top_wall"},
        {"x": 0, "y": 400, "w": 640, "h": 40, "name": "bottom_wall"},
        {"x": 600, "y": 0, "w": 40, "h": 440, "name": "right_wall"},
        {"x": 0, "y": 0, "w": 40, "h": 240, "name": "left_wall_top"},
        {"x": 0, "y": 320, "w": 40, "h": 120, "name": "left_wall_bottom"},
        {"x": 480, "y": 250, "w": 60, "h": 50, "name": "col_beanbag"},
    ]

    objects = [
        # West spawns from the mirror hallway & default spawn
        {
            "name": "from_hallway",
            "type": "spawn",
            "point": True,
            "x": 100,
            "y": 240,
            "properties": {"facing": "right"},
        },
        {
            "name": "from_house",
            "type": "spawn",
            "point": True,
            "x": 100,
            "y": 240,
            "properties": {"facing": "right"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 100,
            "y": 240,
            "properties": {"facing": "right"},
        },
        # Door west back to ruins_house_hallway
        {
            "name": "door_to_hallway",
            "type": "door",
            "x": 0,
            "y": 240,
            "w": 40,
            "h": 80,
            "properties": {
                "targetRoom": "ruins_house_hallway",
                "targetSpawn": "from_pepe",
            },
        },
        # Bed (solid prop with nap cutscene)
        {
            "name": "bed",
            "type": "prop",
            "x": 120,
            "y": 80,
            "w": 80,
            "h": 60,
            "properties": {
                "sprite": "assets/props/ruins_bed.png",
                "solid": True,
                "cutscene": "r11_bed",
            },
        },
        # Lamp (solid prop)
        {
            "name": "lamp",
            "type": "prop",
            "x": 220,
            "y": 80,
            "w": 40,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_lamp.png",
                "anim": "lit",
                "solid": True,
                "lines": "* A small bedside lamp. It casts a cozy glow.",
            },
        },
        # Closet (solid prop with fursuit head cutscene)
        {
            "name": "closet",
            "type": "prop",
            "x": 360,
            "y": 80,
            "w": 60,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_closet.png",
                "anim": "closed",
                "altAnim": "open",
                "altAnimIf": "ruins.fursuit_head.taken >= 1",
                "solid": True,
                "cutscene": "r11_closet",
            },
        },
        # Beanbag examine
        {
            "name": "beanbag",
            "type": "examine",
            "x": 480,
            "y": 240,
            "w": 60,
            "h": 60,
            "properties": {
                "lines": "* A beanbag. It has a Grub-shaped dent already. Suspicious.",
            },
        },
        # Faux-Fur Bandana itemPickup (LORE v3.3, NAR-030, C1-011)
        {
            "name": "bandana_pickup",
            "type": "itemPickup",
            "x": 280,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "item": "Faux-Fur Bandana",
                "flag": "ruins.bandana.taken",
                "lines": "* (You take the Faux-Fur Bandana.)\n* So soft. Too soft. It's working on you.",
                "emptyLines": "* (The nightstand is empty.)",
                "fullLines": "* You're carrying too much.",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_house_pepe",
        display_name="Ruins - Furiell's House",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
    )


# =============================================================================
# Room R12: Basement Corridor (ruins_basement)
# Size: 16x38 tiles (640x1520 px).
# Long, dark, narrow corridor leading north to the iron door and ruins exit.
# =============================================================================
