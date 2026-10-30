"""ruins_hallway: the room builder for data/maps/ruins_hallway.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


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
        {
            "name": "free_hugs_sticker",
            "type": "examine",
            "x": 440,
            "y": 160,
            "w": 40,
            "h": 80,
            "properties": {
                "lines": "* A sticker says \"FREE HUGS.\"||* Someone added \"NO REFUNDS\" in smaller letters.",
            },
        },
        {
            "name": "support_column_plaque",
            "type": "examine",
            "x": 920,
            "y": 160,
            "w": 40,
            "h": 80,
            "properties": {
                "lines": "* A plaque names this the Emotional Support Column.||* It is load-bearing. Emotionally, unclear.",
            },
        },
        {
            "name": "paw_direction_decals",
            "type": "examine",
            "x": 1400,
            "y": 160,
            "w": 40,
            "h": 80,
            "properties": {
                "lines": "* One paw-print decal points north.||* Another points back to the exit. The committee disagreed.",
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
