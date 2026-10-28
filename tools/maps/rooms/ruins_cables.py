"""ruins_cables: the room builder for data/maps/ruins_cables.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


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
