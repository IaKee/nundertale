"""ruins_panelhall: the room builder for data/maps/ruins_panelhall.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R1b: Panel Hall (ruins_panelhall)
# Size: 16x18 tiles (640x720 px).
# Closed panel rooms flank a petal trail to Floofy's lounge. This is the final
# quiet room and the last save point before the encounter.
# =============================================================================
def build_ruins_panelhall():
    width = 16
    height = 18

    ground = [
        "################",
        "##[====  ====]##",
        "##{--v-  -k--}##",
        "##..78888889..##",
        "##;.45555556.;##",
        "##..4555.556..##",
        "##;.45555556.;##",
        "##..45555556..##",
        "##;.45555556.;##",
        "##..45555.56..##",
        "##;.45555556.;##",
        "##..45555556..##",
        "##;.45555556.;##",
        "##..45555556..##",
        "##;.45555556.;##",
        "##..12222223..##",
        "##____  ____####",
        "######  ########",
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
        {"x": 0, "y": 0, "w": 80, "h": 720, "name": "left_wall"},
        {"x": 560, "y": 0, "w": 80, "h": 720, "name": "right_wall"},
        {"x": 0, "y": 640, "w": 280, "h": 80, "name": "bottom_wall_left"},
        {"x": 360, "y": 640, "w": 280, "h": 80, "name": "bottom_wall_right"},
    ]

    objects = [
        {
            "name": "from_registration",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 620,
            "properties": {"facing": "up"},
        },
        {
            "name": "from_lounge",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 140,
            "properties": {"facing": "down"},
        },
        {
            "name": "door_to_registration",
            "type": "door",
            "x": 280,
            "y": 680,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_registration",
                "targetSpawn": "from_panelhall",
            },
        },
        {
            "name": "door_to_lounge",
            "type": "door",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_floofy",
                "targetSpawn": "from_panelhall",
            },
        },
        # BUG-50: the three panel-room doors stand in the north wall, two left of the lounge door, one right
        {
            "name": "panel_door_1",
            "type": "prop",
            "x": 80,
            "y": 6,
            "w": 86,
            "h": 116,
            "properties": {
                "sprite": "assets/props/ruins_panel_door_1.png",
                "solid": False,
            },
        },
        {
            "name": "panel_door_2",
            "type": "prop",
            "x": 166,
            "y": 6,
            "w": 86,
            "h": 116,
            "properties": {
                "sprite": "assets/props/ruins_panel_door_2.png",
                "solid": False,
            },
        },
        {
            "name": "panel_door_3",
            "type": "prop",
            "x": 430,
            "y": 6,
            "w": 86,
            "h": 116,
            "properties": {
                "sprite": "assets/props/ruins_panel_door_3.png",
                "solid": False,
            },
        },
        {
            "name": "panel_maintenance",
            "type": "examine",
            "x": 80,
            "y": 50,
            "w": 86,
            "h": 84,
            "properties": {
                "counter": "ruins.r1b.maintenance",
                "cutscene": "r1b_items",
                "lines": "* \"Fursuit Maintenance 101\". CANCELLED (smell).",
            },
        },
        {
            "name": "panel_parents",
            "type": "examine",
            "x": 166,
            "y": 50,
            "w": 86,
            "h": 84,
            "properties": {
                "counter": "ruins.r1b.parents",
                "cutscene": "r1b_items",
                "lines": "* \"Explaining Your Fursona to Your Parents\". FULL.",
            },
        },
        {
            "name": "panel_gamers",
            "type": "examine",
            "x": 430,
            "y": 50,
            "w": 86,
            "h": 84,
            "properties": {
                "counter": "ruins.r1b.gamers",
                "cutscene": "r1b_items",
                "lines": "* \"Gamers: Are They Real?\". 4 PM, 2014. The room is still booked.",
            },
        },
        # BUG-59 / C1-016: the trail follows the corridor from the south door, jogs one tile
        # right and runs up to the lounge door; the examine zone covers its walkable lane
        {
            "name": "petal_trail",
            "type": "examine",
            "x": 280,
            "y": 160,
            "w": 80,
            "h": 440,
            "properties": {
                "counter": "ruins.r1b.petals",
                "cutscene": "r1b_items",
                "lines": "* The petals lead to the door at the end.\n* (You've played enough games to know what that means.)",
            },
        },
        *lay_petal_trail(
            [(7, 14), (7, 13), (7, 12), (7, 11), (7, 10), (8, 10)]
            + [(8, r) for r in range(9, 1, -1)],
            scatter_tiles=[(6, 12), (9, 8), (7, 6), (9, 4)],
        ),
        {
            "name": "lounge_door",
            "type": "prop",
            "x": 286,
            "y": 56,
            "w": 68,
            "h": 66,
            "properties": {
                "sprite": "assets/props/ruins_panel_lounge_door.png",
                "solid": False,
                "counter": "ruins.r1b.lounge",
                "cutscene": "r1b_items",
                "lines": "* \"HEADLESS LOUNGE — SUITERS ONLY\". Someone taped \"FLOOFY'S\" over it.",
            },
        },
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 430,
            "y": 530,
        },
    ]

    return make_tiled_map(
        room_id="ruins_panelhall",
        display_name="Ruins - Panel Hall",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="ruins_panel_leak",
        save_place="* The muffled thump of music behind a door fills you with DETERMINATION. Mostly.",
    )


# =============================================================================
# Room R2: Floofy's Lair: Headless Lounge (ruins_floofy)
# Size: 16x18 tiles (640x720 px).
# LORE §12 / §19.0 (C1-002, ART-183 #18).
# Props from T07: flower walls, fairy lights, heart beanbag, velvet rope,
# GOOD VIBES ONLY neon, bulb vanity, plush throne, fursuit-head rack, flowerbed.
# Examinables available only once ruins.floofy.met >= 1.
# Step-in trigger: r2_lair_thought (once ruins.r2.thought).
# Center trigger: r2_floofy_scene (hideIf ruins.floofy.met >= 1, no once flag: BUG-43).
# Floofy NPC at the vanity facing away (anim vanity_back, hidden once met).
# Furiell NPC at the south edge, shown by r2_floofy_end (ruins.furiell.r2_arrived), hidden once met.
# The scene (C1-002): r2_floofy_scene -> r2_floofy_retry (deaths >= 1) | r2_floofy_intro -> battle `floofy`
# (the battle picks the variant) -> scripted end -> r2_floofy_end -> door north to R3.
# Exits: south to R1b (from_lounge), north to ruins_entrance.
# =============================================================================
