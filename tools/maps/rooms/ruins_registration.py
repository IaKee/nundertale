"""ruins_registration: the room builder for data/maps/ruins_registration.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R1a: Registration (ruins_registration)
# Size: 18x15 tiles (720x600 px).
# Empty badge pickup room between the Landing and Panel Hall. The first petal is
# waiting near the far door; there are no NPCs, encounters, or entry cutscenes.
# =============================================================================
def build_ruins_registration():
    width = 18
    height = 15

    ground = [
        "##################",
        "##[=====  =====]##",
        "##{--v--  --k--}##",
        "##..7888888889..##",
        "##..4555555556..##",
        "##;.4555555556.;##",
        "##..4555555556..##",
        "##;.4555555556.;##",
        "##..4555555556..##",
        "##;.455555.556.;##",
        "##..4555555556..##",
        "##;.4555555556.;##",
        "##..1222222223..##",
        "##______  ______##",
        "########  ########",
    ]
    above = [
        "..................",
        "..................",
        "........LR........",
        "..................",
        "..................",
        "..................",
        "..................",
        "..................",
        "..................",
        "..................",
        "..................",
        "..................",
        "..................",
        "..................",
        "..................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 320, "h": 120, "name": "top_wall_left"},
        {"x": 400, "y": 0, "w": 320, "h": 120, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 80, "h": 600, "name": "left_wall"},
        {"x": 640, "y": 0, "w": 80, "h": 600, "name": "right_wall"},
        {"x": 0, "y": 520, "w": 320, "h": 80, "name": "bottom_wall_left"},
        {"x": 400, "y": 520, "w": 320, "h": 80, "name": "bottom_wall_right"},
    ]

    objects = [
        {
            "name": "from_landing",
            "type": "spawn",
            "point": True,
            "x": 340,
            "y": 500,
            "properties": {"facing": "up"},
        },
        {
            "name": "from_panelhall",
            "type": "spawn",
            "point": True,
            "x": 340,
            "y": 140,
            "properties": {"facing": "down"},
        },
        {
            "name": "door_to_landing",
            "type": "door",
            "x": 320,
            "y": 560,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_fall",
                "targetSpawn": "from_registration",
            },
        },
        {
            "name": "turnstile_locked",
            "type": "prop",
            "x": 320,
            "y": 80,
            "w": 80,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_reg_turnstile.png",
                "anim": "locked",
                "solid": True,
                "hideIf": "ruins.r1a.turnstile_open >= 1",
                "cutscene": "r1a_turnstile",
                "bumpCutscene": "r1a_turnstile",
                "lines": "* BADGE REQUIRED. NO BADGE, NO CON.",
            },
        },
        {
            "name": "turnstile_open",
            "type": "prop",
            "x": 320,
            "y": 80,
            "w": 80,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_reg_turnstile.png",
                "anim": "open",
                "solid": False,
                "showIf": "ruins.r1a.turnstile_open >= 1",
            },
        },
        {
            "name": "door_to_panelhall",
            "type": "door",
            "x": 320,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_panelhall",
                "targetSpawn": "from_registration",
                "showIf": "ruins.r1a.turnstile_open >= 1",
            },
        },
        {
            "name": "registration_desk",
            "type": "prop",
            "x": 176,
            "y": 118,
            "w": 128,
            "h": 84,
            "properties": {
                "sprite": "assets/props/ruins_reg_desk.png",
                "solid": False,
            },
        },
        {
            "name": "badge_box",
            "type": "prop",
            "x": 88,
            "y": 142,
            "w": 84,
            "h": 60,
            "properties": {
                "sprite": "assets/props/ruins_reg_badges.png",
                "solid": True,
                "counter": "ruins.r1a.badges",
                "cutscene": "r1a_items",
                "hideIf": "ruins.r1a.turnstile_tried >= 1",
                "lines": "* Unclaimed badges, 2014. Nobody came back for them.",
            },
        },
        {
            "name": "badge_box_invalid",
            "type": "prop",
            "x": 88,
            "y": 142,
            "w": 84,
            "h": 60,
            "properties": {
                "sprite": "assets/props/ruins_reg_badges.png",
                "solid": True,
                "showIf": "ruins.r1a.turnstile_tried >= 1",
                "lines": "* \"INVALID: EVENT ENDED\"",
            },
        },
        {
            "name": "rules_board",
            "type": "prop",
            "x": 424,
            "y": 114,
            "w": 96,
            "h": 88,
            "properties": {
                "sprite": "assets/props/ruins_reg_rules.png",
                "solid": True,
                "counter": "ruins.r1a.rules",
                "cutscene": "r1a_items",
                "lines": "* CON RULES: 1. No yiffing in the hallways. 2. Please. 3. We mean it.",
            },
        },
        {
            "name": "escalator",
            "type": "prop",
            "x": 84,
            "y": 384,
            "w": 168,
            "h": 108,
            "properties": {
                "sprite": "assets/props/ruins_reg_escalator.png",
                "solid": True,
                "counter": "ruins.r1a.escalator",
                "cutscene": "r1a_items",
                "lines": "* Out of order since 2014. The \"temporarily\" sign is laminated.",
            },
        },
        {
            "name": "lost_and_found",
            "type": "prop",
            "x": 540,
            "y": 424,
            "w": 92,
            "h": 72,
            "properties": {
                "sprite": "assets/props/ruins_reg_lostfound.png",
                "solid": True,
                "counter": "ruins.r1a.lostfound",
                "cutscene": "r1a_items",
                "hideIf": "ruins.r1a.lostfound_seen >= 1",
                "lines": "* One fursuit paw. A phone charger. Somebody's sense of shame. (Unclaimed.)",
            },
        },
        {
            # C1-029: from the second examine on the bin runs r1a_beacon (Take it / Leave it).
            "name": "lost_and_found_beacon",
            "type": "prop",
            "x": 540,
            "y": 424,
            "w": 92,
            "h": 72,
            "properties": {
                "sprite": "assets/props/ruins_reg_lostfound.png",
                "solid": True,
                "cutscene": "r1a_beacon",
                "showIf": "ruins.r1a.lostfound_seen >= 1",
                "lines": "* One fursuit paw. A phone charger. Somebody's sense of shame. (Unclaimed.)",
            },
        },
        {
            "name": "strange_beacon",
            "type": "prop",
            "x": 574,
            "y": 478,
            "w": 24,
            "h": 24,
            "properties": {
                "sprite": "assets/props/strange_beacon.png",
                "anim": "glow",
                "solid": False,
                "showIf": "ruins.r1a.lostfound_seen >= 1 && ruins.beacon.state == 0",
            },
        },
        {
            "name": "petal",
            "type": "prop",
            "x": 348,
            "y": 155,
            "w": 24,
            "h": 16,
            "properties": {
                "sprite": "assets/props/petal_trail.png",
                "solid": False,
                "counter": "ruins.r1a.petal",
                "cutscene": "r1a_items",
                "lines": "* A pink petal. It's warm.",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_registration",
        display_name="Ruins - Registration",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="ruins_quiet",
    )


# =============================================================================
# Room R1b: Panel Hall (ruins_panelhall)
# Size: 16x18 tiles (640x720 px).
# Closed panel rooms flank a petal trail to Floofy's lounge. This is the final
# quiet room and the last save point before the encounter.
# =============================================================================
