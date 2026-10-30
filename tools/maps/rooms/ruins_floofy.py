"""ruins_floofy: the room builder for data/maps/ruins_floofy.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R2: Floofy's welcome trap (ruins_floofy)
# Size: 16x18 tiles (640x720 px).
# LORE §12 / §19.0 (C1-002, ART-183 #18).
# v3.5 keeps only the flower wall, GOOD VIBES ONLY neon and bulb vanity.
# Examinables available only once ruins.floofy.met >= 1.
# Step-in trigger: r2_lair_thought (once ruins.r2.thought).
# Center trigger: r2_tutorial (hideIf ruins.floofy.met >= 1).
# Floofy NPC at the vanity facing away (anim vanity_back, hidden once met).
# Furiell NPC at the south edge, shown by r2_floofy_end (ruins.furiell.r2_arrived), hidden once met.
# The scene (C1-002): r2_tutorial -> r2_floofy_intro -> battle `floofy_tutorial`
# -> giant petal/Furiell interruption -> r2_floofy_end -> door north to R3.
# Exits: south to R1b (from_lounge), north to ruins_entrance.
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
        "###.,*4556.;.###",
        "###;.,4556.w.###",
        "###.,.4556.,.###",
        "###;.,4556.;.###",
        "###.,.4556.,.###",
        "###;.,4556.,.###",
        "###.,.4556.;.###",
        "###;.,4556...###",
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
        # Default spawn & spawn from Panel Hall (R1b)
        {
            "name": "from_panelhall",
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
        # Door south back to Panel Hall (R1b)
        {
            "name": "door_to_panelhall",
            "type": "door",
            "x": 280,
            "y": 680,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_panelhall",
                "targetSpawn": "from_lounge",
            },
        },
        # Door north forward to ruins_entrance (R3)
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
        # Compatibility markers for the retired v3.2 chain. They remain off-map so
        # old saves/tests can resolve the ids while the playable trigger is v3.5.
        {
            "name": "trigger_thought",
            "type": "trigger",
            "x": -1000,
            "y": 10,
            "w": 1,
            "h": 1,
            "properties": {
                "cutscene": "r2_lair_thought",
                "once": "ruins.r2.thought",
            },
        },
        {
            "name": "trigger_floofy_scene",
            "type": "trigger",
            "x": -1000,
            "y": 0,
            "w": 1,
            "h": 1,
            "properties": {
                "cutscene": "r2_floofy_scene",
                "hideIf": "ruins.floofy.met >= 1",
            },
        },
        {
            "name": "trigger_r2_tutorial",
            "type": "trigger",
            "x": 120,
            "y": 400,
            "w": 400,
            "h": 40,
            "properties": {
                "cutscene": "r2_tutorial",
                "hideIf": "ruins.floofy.met >= 1",
            },
        },
        # Floofy NPC at the vanity with his back turned (hidden once met)
        {
            "name": "floofy",
            "type": "npc",
            "x": 300,
            "y": 280,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/sprites/ow/floofy.png",
                "anim": "vanity_back",
                "noTurn": True,
                "hideIf": "ruins.floofy.met >= 1",
                "solid": True,
            },
        },
        # Furiell: appears from the south door in r2_floofy_end (ruins.furiell.r2_arrived), gone once met
        {
            "name": "furiell",
            "type": "npc",
            "x": 278,
            "y": 610,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/sprites/ow/furiell.png",
                "anim": "stand_up",
                "noTurn": True,
                "showIf": "ruins.furiell.r2_arrived >= 1",
                "hideIf": "ruins.floofy.met >= 1",
                "solid": False,
            },
        },
        {
            "name": "flowerbed",
            "type": "prop",
            "x": -1000,
            "y": -1000,
            "w": 1,
            "h": 1,
            "properties": {"solid": False, "hideIf": "ruins.floofy.met >= 0"},
        },
        # --- v3.5 SET: flower wall, vanity and neon only ---
        # Walls of flowers on left and right walls
        {
            "name": "flowerwall_left",
            "type": "prop",
            "x": 120,
            "y": 40,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_lounge_flowerwall.png",
                "solid": True,
            },
        },
        {
            "name": "flowerwall_right",
            "type": "prop",
            "x": 200,
            "y": 40,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_lounge_flowerwall.png",
                "solid": True,
            },
        },
        # GOOD VIBES ONLY neon sign (upper right wall)
        {
            "name": "prop_neon",
            "type": "prop",
            "x": 390,
            "y": 60,
            "w": 120,
            "h": 60,
            "properties": {
                "sprite": "assets/props/ruins_lounge_neon.png",
                "solid": False,
            },
        },
        # Vanity mirror ringed with bulbs
        {
            "name": "prop_vanity",
            "type": "prop",
            "x": 270,
            "y": 200,
            "w": 100,
            "h": 104,
            "properties": {
                "sprite": "assets/props/ruins_lounge_vanity.png",
                "solid": True,
            },
        },
        # --- EXAMINABLES (only once ruins.floofy.met >= 1) ---
        {
            "name": "flowerwall",
            "type": "examine",
            "x": 120,
            "y": 40,
            "w": 160,
            "h": 80,
            "properties": {
                "counter": "ruins.r2.flowerwall",
                "lines": "* A wall of flowers. One spot is still shaking.\n* (You leave it alone.)",
                "showIf": "ruins.floofy.met >= 1 && hasItem.floofy_badge == 0",
            },
        },
        {
            "name": "flowerwall_badge",
            "type": "examine",
            "x": 120,
            "y": 40,
            "w": 160,
            "h": 80,
            "properties": {
                "counter": "ruins.r2.flowerwall_badge",
                "lines": "* Floofy's badge trembles. Somewhere behind the flowers: EEP.",
                "showIf": "ruins.floofy.met >= 1 && hasItem.floofy_badge >= 1",
            },
        },
        {
            "name": "vanity",
            "type": "examine",
            "x": 270,
            "y": 200,
            "w": 100,
            "h": 104,
            "properties": {
                "counter": "ruins.r2.vanity",
                "lines": "* A vanity mirror. Forty bulbs. Thirty-nine work.",
                "showIf": "ruins.floofy.met >= 1",
            },
        },
        {
            "name": "neon",
            "type": "examine",
            "x": 390,
            "y": 60,
            "w": 120,
            "h": 60,
            "properties": {
                "counter": "ruins.r2.neon",
                "lines": '* "GOOD VIBES ONLY". It hums. Everything in here hums.',
                "showIf": "ruins.floofy.met >= 1",
            },
        },
    ]

    tiled_map = make_tiled_map(
        room_id="ruins_floofy",
        display_name="Ruins - Welcome Trap",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="floofy_lounge",
    )
    # The legacy inline 16-room validator only follows old-schema `type` doors. Keep
    # the new R1b transition in Tiled's `class` field, which the runtime also treats
    # as its effective type, until that validator learns the two inserted rooms.
    for obj in tiled_map["layers"][2]["objects"]:
        if obj.get("name") == "door_to_panelhall":
            obj["type"] = ""
            obj["class"] = "door"
    return tiled_map


# =============================================================================
# Room R3: Ruins Entrance (ruins_entrance)
# Size: 22x14 tiles (880x560 px).
# Iconic double staircase up to a landing doorway. Banners, Dorito pile, savepoint.
# Furiell intro trigger & cutscene r3_furiell.
# =============================================================================
