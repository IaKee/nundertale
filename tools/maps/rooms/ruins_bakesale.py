"""ruins_bakesale: the room builder for data/maps/ruins_bakesale.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R10: Bowl, Commission Bake Sale and Mod Rock (ruins_bakesale)
# Size: 20x12 tiles (800x480 px).
# Dorito bowl, Commission Bake Sale booth with sample donut, mod rock,
# Furiell forced call trigger, save point, and dark doorway north to R11.
# =============================================================================
def build_ruins_bakesale():
    width = 20
    height = 12

    ground = [
        "#[================]#",
        "#{---v----k---}  {}#",
        "#.,;,..;,..,;,....;#",
        "#;.,..,.c..,;,..;,.#",
        "#.,;,..;.7889.,;,..#",
        "#;.,..,..4556;.,..;#",
        "#.,;,....1223.,;,..#",
        "#;.,..:B:.,;,..,;,.#",
        "#.,;...*..,;,....,;#",
        "#;.,...,;,..,;,..,;#",
        "#______  __________#",
        "#######..###########",
    ]

    above = [
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
        "....................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 600, "h": 80, "name": "top_wall_left"},
        {"x": 680, "y": 0, "w": 120, "h": 80, "name": "top_wall_right"},
        {"x": 0, "y": 440, "w": 280, "h": 40, "name": "bottom_wall_left"},
        {"x": 360, "y": 440, "w": 440, "h": 40, "name": "bottom_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 760, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
        {"x": 280, "y": 280, "w": 40, "h": 40, "name": "col_modrock"},
    ]

    objects = [
        # South spawn (from Barky's Arena) & default spawn
        {
            "name": "from_barky",
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
        # Spawn when entering from Furiell's House (north)
        {
            "name": "from_house",
            "type": "spawn",
            "point": True,
            "x": 640,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_barky
        {
            "name": "door_to_barky",
            "type": "door",
            "x": 280,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_barky",
                "targetSpawn": "from_bakesale",
            },
        },
        # Save point
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 180,
            "y": 340,
            "properties": {
                "lines": "* The smell of baked goods and spider-art fills you with DETERMINATION.",
            },
        },
        # Forced calls are queued instead of interrupting another cutscene/menu/battle.
        {
            "name": "trigger_call",
            "type": "trigger",
            "x": 40,
            "y": 320,
            "w": 720,
            "h": 40,
            "properties": {
                "phoneMessage": "r10_call",
                "once": "ruins.r10.call",
            },
        },
        {
            "name": "trigger_call2",
            "type": "trigger",
            "x": 40,
            "y": 240,
            "w": 720,
            "h": 40,
            "properties": {
                "phoneMessage": "r10_call2",
                "once": "ruins.r10.call2",
                "showIf": "ruins.r10.call >= 1",
            },
        },
        # Dorito Bowl (full/empty via altAnim)
        {
            "name": "dorito_bowl",
            "type": "prop",
            "x": 120,
            "y": 140,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_dorito_bowl.png",
                "anim": "full",
                "altAnim": "empty",
                "altAnimIf": "ruins.bowl.takes >= 3",
                "hideIf": "ruins.bowl.broken >= 1",
                "cutscene": "r10_bowl",
                "counter": "ruins.bowl.takes",
                "solid": True,
            },
        },
        # Dorito Bowl (knocked over once broken)
        {
            "name": "dorito_bowl_knocked",
            "type": "prop",
            "x": 120,
            "y": 140,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_dorito_bowl.png",
                "anim": "knocked",
                "showIf": "ruins.bowl.broken >= 1",
                "cutscene": "r10_bowl",
                "counter": "ruins.bowl.takes",
                "solid": True,
            },
        },
        # Sign above the bowl
        {
            "name": "dorito_sign",
            "type": "sign",
            "x": 120,
            "y": 90,
            "w": 40,
            "h": 40,
            "properties": {
                "lines": "* \"Take one Dorito.\"",
            },
        },
        # The Mod Rock
        {
            "name": "mod_rock",
            "type": "prop",
            "x": 280,
            "y": 280,
            "w": 40,
            "h": 40,
            "properties": {
                "speaker": "MOD ROCK",
                "counter": "ruins.modrock.talks",
                "lines": "Hey. No pushing without permission.",
                "lines2": "...Permission granted.",
                "lines3": "Don't push your luck.",
                "solid": True,
                "bumpCutscene": "r10_modrock_push",
            },
        },
        # Commission Bake Sale sample donut pickup
        {
            "name": "sample_donut",
            "type": "itemPickup",
            "x": 330,
            "y": 170,
            "w": 40,
            "h": 40,
            "properties": {
                "item": "Commissioned Donut",
                "flag": "ruins.bakesale.sample",
                "lines": "* (You take the free sample.)\n* ETA: 6-8 weeks. ...It's here already. Somehow.",
                "emptyLines": "* (The sample plate is empty. A business card reads: 'Slots closed.')",
                "fullLines": "* (You try to take the free sample, but you're carrying too much.)",
            },
        },
        # Commission Bake Sale Booth
        {
            "name": "bakesale_booth",
            "type": "prop",
            "x": 380,
            "y": 150,
            "w": 120,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_booth.png",
                "solid": True,
                "cutscene": "r10_shop",
            },
        },
        # Napstablurk: lies on the floor "lurking" in the hallway
        {
            "name": "napstablurk",
            "type": "examine",
            "x": 620,
            "y": 240,
            "w": 40,
            "h": 40,
            "properties": {
                "cutscene": "r10_napstablurk",
                "hideIf": "ruins.napstablurk.state == 1 || ruins.napstablurk.state == 2",
            },
        },
        # Post-battle trigger when spared (plays LORE ending line)
        {
            "name": "trigger_napstablurk",
            "type": "trigger",
            "x": 560,
            "y": 180,
            "w": 160,
            "h": 140,
            "properties": {
                "cutscene": "r10_napstablurk_spared",
                "once": "ruins.napstablurk.spared_cutscene",
                "showIf": "battle.napstablurk.spared >= 1 && ruins.napstablurk.state == 0",
            },
        },
        # Post-battle trigger when killed
        {
            "name": "trigger_napstablurk_killed",
            "type": "trigger",
            "x": 560,
            "y": 180,
            "w": 160,
            "h": 140,
            "properties": {
                "cutscene": "r10_napstablurk_killed",
                "once": "ruins.napstablurk.killed_cutscene",
                "showIf": "battle.napstablurk.killed >= 1 && ruins.napstablurk.state == 0",
            },
        },
        # Post-battle trigger when fled
        {
            "name": "trigger_napstablurk_fled",
            "type": "trigger",
            "x": 560,
            "y": 180,
            "w": 160,
            "h": 140,
            "properties": {
                "cutscene": "r10_napstablurk_fled",
                "once": "ruins.napstablurk.fled_cutscene",
                "showIf": "ruins.napstablurk.met >= 1 && battle.napstablurk.spared == 0 && battle.napstablurk.killed == 0 && ruins.napstablurk.state == 0",
            },
        },
        # Door north forward to ruins_house
        {
            "name": "door_to_house",
            "type": "door",
            "x": 600,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_house",
                "targetSpawn": "from_bakesale",
            },
        },
        {
            "name": "tip_jar",
            "type": "examine",
            "x": 520,
            "y": 160,
            "w": 40,
            "h": 40,
            "properties": {
                "lines": "* The tip jar accepts gold, exposure, and apologies.||* Exposure is crossed out twice.",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_bakesale",
        display_name="Ruins - Bake Sale",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
        save_place="* The smell of baked goods and spider-art fills you with DETERMINATION.",
        encounters="lurkeye:1,reply_guy:1,grasstoid:1,ruins_r10_trio:1",
        encounter_budget=1,
    )


# =============================================================================
# Room R11: Furiell's House - Living Room (ruins_house)
# Size: 20x12 tiles (800x480 px).
# Cozy living room with fireplace, armchair, bookshelf, desk with laptop and
# letterhead, Glitter Pop-Tart pickup, locked door to her room, basement stairs,
# and the east doorway to the mirror hallway.
# =============================================================================
