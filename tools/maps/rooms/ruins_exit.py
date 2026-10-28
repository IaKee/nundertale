"""ruins_exit: the room builder for data/maps/ruins_exit.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


def build_ruins_exit():
    width = 16
    height = 12

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
        "###;..1223.;.###",
        "###___    ___###",
        "######    ######",
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
        {"x": 0, "y": 0, "w": 280, "h": 80, "name": "top_wall_left"},
        {"x": 360, "y": 0, "w": 280, "h": 80, "name": "top_wall_right"},
        {"x": 280, "y": 0, "w": 80, "h": 80, "name": "door_blocker"},
        {"x": 0, "y": 0, "w": 120, "h": 480, "name": "left_void"},
        {"x": 520, "y": 0, "w": 120, "h": 480, "name": "right_void"},
        {"x": 0, "y": 400, "w": 280, "h": 80, "name": "bottom_wall_left"},
        {"x": 360, "y": 400, "w": 280, "h": 80, "name": "bottom_wall_right"},
    ]

    objects = [
        # South spawn (from Basement) & default spawn
        {
            "name": "from_basement",
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
        # Door south back to ruins_basement
        {
            "name": "door_to_basement",
            "type": "door",
            "x": 280,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_basement",
                "targetSpawn": "from_exit",
            },
        },
        # Aurora glimpse prop in the iron doorway
        {
            "name": "prop_aurora_glimpse",
            "type": "prop",
            "x": 280,
            "y": 0,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_aurora_glimpse.png",
                "solid": True,
            },
        },
        # Big iron door examine
        {
            "name": "examine_iron_door",
            "type": "examine",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 60,
            "properties": {
                "cutscene": "r13_door",
            },
        },
        # Save point
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 180,
            "y": 200,
            "properties": {
                "lines": "* The cold air and the faint smell of ozone fill you with DETERMINATION.",
            },
        },
        # Floofy NPC (appears on entry for cutscene, hides afterwards)
        {
            "name": "floofy",
            "type": "npc",
            "x": 300,
            "y": 220,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/sprites/ow/floofy.png",
                "anim": "idle",
                "showIf": "ruins.r13.floofy_pop == 1",
                "solid": True,
            },
        },
        # Entry trigger for Floofy cutscene
        {
            "name": "trigger_r13_floofy",
            "type": "trigger",
            "x": 120,
            "y": 360,
            "w": 400,
            "h": 80,
            "properties": {
                "cutscene": "r13_floofy",
                "once": "ruins.r13.floofy",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_exit",
        display_name="Ruins - Exit",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
        save_place="* The cold air and the faint smell of ozone fill you with DETERMINATION.",
    )


# =============================================================================
# Room R9a: the Apurrture booth (ruins_booth), C1-014
# Size: 16x12 tiles (640x480 px).
# Only reachable from R9 once Barky was spared (LORE §12 R9a, §19.10). GLaDOwOS hangs from the
# ceiling over the test floor; the choice Sacrifice / Refuse happens when Grub walks up to it
# (r9a_booth_entry). Save point at the entrance, panel / cube / sign examinables, and a
# powered-down host when Barky is dead. Flag ruins.booth.state: 0 not entered, 1 entered
# (refusing leaves it at 1), 2 Barky sacrificed.
# =============================================================================
