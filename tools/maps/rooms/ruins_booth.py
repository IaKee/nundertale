"""ruins_booth: the room builder for data/maps/ruins_booth.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R9a: the Apurrture booth (ruins_booth), C1-014
# Size: 16x12 tiles (640x480 px).
# Only reachable from R9 once Barky was spared (LORE §12 R9a, §19.10). GLaDOwOS hangs from the
# ceiling over the test floor; the choice Sacrifice / Refuse happens when Grub walks up to it
# (r9a_booth_entry). Save point at the entrance, panel / cube / sign examinables, and a
# powered-down host when Barky is dead. Flag ruins.booth.state: 0 not entered, 1 entered
# (refusing leaves it at 1), 2 Barky sacrificed.
# =============================================================================
def build_ruins_booth():
    width = 16
    height = 12

    ground = [
        "#[============]#",
        "#{-----v-----o}#",
        "#.,.,p.,.,.,.,.#",
        "#,.,.,.,.,.,.,.#",
        "#.,.,.,.,.,.,.,#",
        "#,.,.,.,.,.p.,.#",
        "#.,.,.,.,.,.,.,#",
        "#,.,.,.,.,.,.,.#",
        "#.,.,.,.,.,.,.,#",
        "#,.,.,.,.,.,.,.#",
        "#.,.,.,.,.,.,.,#",
        "#######..#######",
    ]

    above = [
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
        {"x": 0, "y": 0, "w": 640, "h": 80, "name": "top_wall"},
        {"x": 0, "y": 440, "w": 640, "h": 40, "name": "bottom_wall"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 600, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
    ]

    objects = [
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 340,
        },
        {
            "name": "from_barky",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 340,
            "properties": {"facing": "up"},
        },
        {
            "name": "to_barky",
            "type": "door",
            "x": 280,
            "y": 410,
            "w": 80,
            "h": 30,
            "properties": {
                "targetRoom": "ruins_barky",
                "targetSpawn": "from_booth",
            },
        },
        # Save point at the entrance (the WELCOME HOME of the fight returns here)
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 200,
            "y": 340,
            "properties": {
                "lines": "* The faint hum of a very patient machine fills you with DETERMINATION.",
            },
        },
        # The test starts when Grub walks up to the host: no once-flag, ruins.booth.state == 0 is
        # the gate (r9a_booth_entry sets it), so a lost fight can be retried with the host below.
        {
            "name": "trigger_booth_test",
            "type": "trigger",
            "x": 40,
            "y": 190,
            "w": 560,
            "h": 50,
            "properties": {
                "cutscene": "r9a_booth_entry",
                "showIf": "ruins.booth.state == 0 && ruins.barky.state == 1",
            },
        },
        # GLaDOwOS: talk to it to take the test again (state 1, Barky alive)
        {
            "name": "npc_glad",
            "type": "npc",
            "x": 280,
            "y": 140,
            "w": 80,
            "h": 60,
            "properties": {
                "sprite": "assets/props/ruins_apurrture_host.png",
                "anim": "idle",
                "noTurn": True,
                "showIf": "ruins.barky.state == 1",
                "cutscene": "r9a_booth_entry",
            },
        },
        # Powered-down variant (Barky is dead): LORE §12 R9a
        {
            "name": "npc_glad_off",
            "type": "npc",
            "x": 280,
            "y": 140,
            "w": 80,
            "h": 60,
            "properties": {
                "sprite": "assets/props/ruins_apurrture_host.png",
                "anim": "idle",
                "noTurn": True,
                "showIf": "ruins.barky.state == 2",
                "lines": "* There's nobody left to test.",
            },
        },
        # Barky waits by the entrance side of the test floor
        {
            "name": "npc_barky_booth",
            "type": "npc",
            "x": 200,
            "y": 250,
            "w": 40,
            "h": 50,
            "properties": {
                "sprite": "assets/sprites/ow/barky.png",
                "anim": "stand_down",
                "speaker": "BARKY",
                "showIf": "ruins.barky.state == 1",
                "lines": "* Dude. A talking booth. I've waited my whole life for this.||* Don't tell it about the jacket.",
            },
        },
        {
            "name": "incinerator",
            "type": "prop",
            "x": 480,
            "y": 150,
            "w": 80,
            "h": 50,
            "properties": {
                "sprite": "assets/props/ruins_apurrture_incinerator.png",
                "anim": "idle",
                "solid": True,
                "lines": "* The Emergency Fursuit Incinerator. It's warm.||* It looks hungry.",
            },
        },
        # Companion cube display (LORE §12 R9a)
        {
            "name": "cube",
            "type": "prop",
            "x": 90,
            "y": 150,
            "w": 60,
            "h": 50,
            "properties": {
                "sprite": "assets/props/ruins_apurrture_cube.png",
                "anim": "idle",
                "solid": True,
                "lines": "* A cube with a paw print on it. Someone has been hugging it.",
            },
        },
        # Loose panel: graffiti THE HUG IS A LIE (both choices)
        {
            "name": "panel",
            "type": "prop",
            "x": 185,
            "y": 70,
            "w": 50,
            "h": 70,
            "properties": {
                "sprite": "assets/props/ruins_apurrture_panel.png",
                "anim": "closed",
                "altAnim": "open",
                "altAnimIf": "ruins.booth.panel >= 1",
                "cutscene": "r9a_panel",
            },
        },
        # Booth sign (LORE §12 R9a)
        {
            "name": "booth_sign",
            "type": "prop",
            "x": 380,
            "y": 60,
            "w": 80,
            "h": 30,
            "properties": {
                "sprite": "assets/props/ruins_apurrture_sign.png",
                "lines": "* \"APURRTURE SCIENCE. Testing tomorrow's fandom, today.\"||* Someone added \":3\".",
            },
        },
        # What is left of Barky after the sacrifice
        {
            "name": "booth_dust",
            "type": "prop",
            "x": 490,
            "y": 210,
            "w": 40,
            "h": 30,
            "properties": {
                "sprite": "assets/props/ruins_wrapper_pile.png",
                "anim": "pile",
                "showIf": "ruins.booth.state == 2",
                "lines": "* (A small mound of gray fur and gamer dust.)||* (It is still warm.)",
            },
        },
        {
            "name": "test_waiver",
            "type": "examine",
            "x": 280,
            "y": 300,
            "w": 80,
            "h": 40,
            "properties": {
                "lines": "* A waiver has one checkbox: \"I AM EMOTIONALLY REPLACEABLE.\"||* It is already checked.",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_booth",
        display_name="Ruins - Apurrture Booth",
        width=width,
        height=height,
        ground_grid=ground,
        above_grid=above,
        collision_boxes=collision,
        objects=objects,
        tile_size=40,
        save_place="* The faint hum of a very patient machine fills you with DETERMINATION.",
    )
