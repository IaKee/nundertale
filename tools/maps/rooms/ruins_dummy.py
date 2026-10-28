"""ruins_dummy: the room builder for data/maps/ruins_dummy.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


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
                "hideIf": "ruins.r5.walked >= 1",
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
                "showIf": "ruins.dummy.talked >= 1 && ruins.dummy.hugged == 0 && battle.fursuit_dummy.spared == 0 && battle.fursuit_dummy.killed == 0",
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
                "cutscene": "r5_dummy_spare",
                "once": "ruins.dummy.reacted",
                "showIf": "battle.fursuit_dummy.spared >= 1 && ruins.dummy.hugged == 0 && battle.fursuit_dummy.killed == 0",
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
