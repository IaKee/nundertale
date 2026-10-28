"""ruins_entrance: the room builder for data/maps/ruins_entrance.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


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
                "sound": "assets/sfx/step_crunch.ogg",
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
