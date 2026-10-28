"""ruins_switches: the room builder for data/maps/ruins_switches.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R4: RGB Switches (ruins_switches)
# Size: 16x12 tiles (640x480 px).
# Three switches on the wall. Two correct ones marked with glitter (sw1, sw3).
# One wrong switch (sw2) with a note from Ruins Administration.
# Gate across north exit blocked by cables until both right switches are pressed.
# =============================================================================
def build_ruins_switches():
    width = 16
    height = 12

    ground = [
        "#[====]  [====]#",
        "#{---k}  {--v-}#",
        "#.,;,..,;,..,;.#",
        "#;.,...*,;,..,.#",
        "#.,;,..,.w..,;.#",
        "#;..,.c....,;..#",
        "#.,;,....,;,..,#",
        "#;.,..,p..,;,..#",
        "#.,;....,;,..,.#",
        "#;.,...,;,..,;.#",
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
        # South spawn (from entrance) & default spawn
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
        # North spawn (from dummy)
        {
            "name": "from_dummy",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_entrance
        {
            "name": "door_to_entrance",
            "type": "door",
            "x": 280,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_entrance",
                "targetSpawn": "from_switches",
            },
        },
        # Door north forward to ruins_dummy
        {
            "name": "door_to_dummy",
            "type": "door",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_dummy",
                "targetSpawn": "from_switches",
            },
        },
        # Entrance hint trigger
        {
            "name": "trigger_hint",
            "type": "trigger",
            "x": 240,
            "y": 380,
            "w": 160,
            "h": 40,
            "properties": {
                "cutscene": "r4_hint",
                "once": "ruins.r4.intro",
            },
        },
        # Switch 1: Right switch (marked with glitter)
        {
            "name": "sw1",
            "type": "prop",
            "x": 120,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "counter": "ruins.r4.sw1",
                "lines": "* You flipped the switch. Click! Glitter spilled everywhere.",
                "lines2": "* The switch is already flipped. There is glitter all over your hands.",
            },
        },
        {
            "name": "sw1_marked",
            "type": "prop",
            "x": 120,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "marked",
                "hideIf": "ruins.r4.sw1 >= 1",
                "solid": False,
            },
        },
        {
            "name": "sw1_on",
            "type": "prop",
            "x": 120,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "on",
                "showIf": "ruins.r4.sw1 >= 1",
                "solid": False,
            },
        },
        # Switch 2: Wrong switch
        {
            "name": "sw2",
            "type": "prop",
            "x": 200,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "counter": "ruins.r4.sw2",
                "lines": "* You flipped the switch. It made a hollow clunk. Nothing happened.",
                "lines2": "* Still nothing. It feels like pressing an unmapped key.",
            },
        },
        {
            "name": "sw2_off",
            "type": "prop",
            "x": 200,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "off",
                "hideIf": "ruins.r4.sw2 >= 1",
                "solid": False,
            },
        },
        {
            "name": "sw2_on",
            "type": "prop",
            "x": 200,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "on",
                "showIf": "ruins.r4.sw2 >= 1",
                "solid": False,
            },
        },
        # Note on wall next to wrong switch
        {
            "name": "note_switch2",
            "type": "examine",
            "x": 240,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "lines": "* \"Not this one. - Ruins Administration (not Furiell)\"",
            },
        },
        # Sign
        {
            "name": "sign_courage",
            "type": "sign",
            "x": 400,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "lines": "* \"Only the fearless may proceed. Brave ones, foolish ones. Both walk not the path of the noob.\"",
            },
        },
        # Switch 3: Right switch (marked with glitter)
        {
            "name": "sw3",
            "type": "prop",
            "x": 480,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "counter": "ruins.r4.sw3",
                "lines": "* You flipped the switch. Click! A distant latch clicked open.",
                "lines2": "* The switch is already on. It emits a faint RGB glow.",
            },
        },
        {
            "name": "sw3_marked",
            "type": "prop",
            "x": 480,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "marked",
                "hideIf": "ruins.r4.sw3 >= 1",
                "solid": False,
            },
        },
        {
            "name": "sw3_on",
            "type": "prop",
            "x": 480,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_switch.png",
                "anim": "on",
                "showIf": "ruins.r4.sw3 >= 1",
                "solid": False,
            },
        },
        # North gate: solid cables blocking exit when unsolved
        {
            "name": "gate_cables_left",
            "type": "prop",
            "x": 280,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
                "solid": True,
                "hideIf": "ruins.r4.sw1 >= 1 && ruins.r4.sw3 >= 1",
            },
        },
        {
            "name": "gate_cables_right",
            "type": "prop",
            "x": 320,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
                "solid": True,
                "hideIf": "ruins.r4.sw1 >= 1 && ruins.r4.sw3 >= 1",
            },
        },
        # Visible barrier one tile lower: the north arch (above layer) covers the y=80 row.
        {
            "name": "gate_front_left",
            "type": "prop",
            "x": 280,
            "y": 120,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
                "solid": True,
                "hideIf": "ruins.r4.sw1 >= 1 && ruins.r4.sw3 >= 1",
                "lines": "* A tangle of live cables blocks the way.\n* Somewhere, a switch is waiting.",
            },
        },
        {
            "name": "gate_front_right",
            "type": "prop",
            "x": 320,
            "y": 120,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_cables.png",
                "anim": "on",
                "solid": True,
                "hideIf": "ruins.r4.sw1 >= 1 && ruins.r4.sw3 >= 1",
                "lines": "* A tangle of live cables blocks the way.\n* Somewhere, a switch is waiting.",
            },
        },
    ]

    # C1-004 v3.1 replaces the old three independent switches above.  The five
    # key props are one Lights-Out keyboard; OverworldScene owns their atomic
    # neighbor toggle so a save can never observe half of a press.
    key_specs = [
        (0, "red"),
        (1, "yellow"),
        (2, "green"),
        (3, "blue"),
        (4, "magenta"),
    ]
    objects = [
        {
            "name": "from_entrance", "type": "spawn", "point": True,
            "x": 320, "y": 370, "properties": {"facing": "up"},
        },
        {
            "name": "spawn", "type": "spawn", "point": True,
            "x": 320, "y": 370, "properties": {"facing": "up"},
        },
        {
            "name": "from_dummy", "type": "spawn", "point": True,
            "x": 320, "y": 140, "properties": {"facing": "down"},
        },
        {
            "name": "door_to_entrance", "type": "door",
            "x": 280, "y": 440, "w": 80, "h": 40,
            "properties": {"targetRoom": "ruins_entrance", "targetSpawn": "from_switches"},
        },
        {
            "name": "door_to_dummy", "type": "door",
            "x": 280, "y": 40, "w": 80, "h": 40,
            "properties": {"targetRoom": "ruins_dummy", "targetSpawn": "from_switches"},
        },
        {
            "name": "trigger_hint", "type": "trigger",
            "x": 240, "y": 380, "w": 160, "h": 40,
            "properties": {"cutscene": "r4_hint", "once": "ruins.r4.intro"},
        },
        {
            "name": "rgb_keyboard_frame", "type": "decoration",
            "x": 220, "y": 160, "w": 200, "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_rgb_keyboard.png",
                "solid": False,
            },
        },
        {
            "name": "note_keyboard", "type": "examine",
            "x": 120, "y": 155, "w": 40, "h": 50,
            "properties": {"lines": "* \"Not this one. - Ruins Administration (not Furiell)\""},
        },
        {
            "name": "sign_courage", "type": "sign",
            "x": 480, "y": 155, "w": 40, "h": 50,
            "properties": {
                "lines": "* \"Only the fearless may proceed. Brave ones, foolish ones. Both walk not the path of the noob.\"",
            },
        },
    ]
    for key, color in key_specs:
        objects.append({
            "name": f"rgb_key_{key}", "type": "prop",
            "x": 224 + key * 38, "y": 144, "w": 36, "h": 50,
            "properties": {
                "rgbKey": key,
                "sprite": "assets/props/ruins_rgb_keys.png",
                "anim": f"{color}_off",
                "altAnim": f"{color}_on",
                "altAnimIf": f"ruins.r4.key{key} >= 1",
                "cutscene": "r4_keys",
                "solid": False,
            },
        })
    for suffix, x, y in (("cables_left", 280, 80), ("cables_right", 320, 80),
                         ("front_left", 280, 120), ("front_right", 320, 120)):
        properties = {
            "sprite": "assets/props/ruins_cables.png",
            "anim": "on",
            "solid": True,
            "hideIf": "ruins.r4.solved >= 1",
        }
        if suffix.startswith("front"):
            properties["lines"] = "* A tangle of live cables blocks the way.\n* Make the keyboard pink."
        objects.append({
            "name": f"gate_{suffix}", "type": "prop",
            "x": x, "y": y, "w": 40, "h": 40,
            "properties": properties,
        })

    return make_tiled_map(
        room_id="ruins_switches",
        display_name="Ruins - Switch Room",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
    )


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
