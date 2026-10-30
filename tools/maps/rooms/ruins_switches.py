"""ruins_switches: the room builder for data/maps/ruins_switches.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R4: RGB Switches (ruins_switches)
# Size: 16x12 tiles (640x480 px).
# C1-004 v3.5 (BUG-60): mechanical-switch keyboard, floor cables, shutter door.
# Five mechanical keys on the wall. Lights Out toggles neighbors.
# Floor cables lead from keyboard to shutter door and light up when solved.
# Shutter door animates up (5 frames) when solved and stays open on return.
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
            "name": "from_entrance", "type": "spawn", "point": True,
            "x": 320, "y": 370, "properties": {"facing": "up"},
        },
        {
            "name": "spawn", "type": "spawn", "point": True,
            "x": 320, "y": 370, "properties": {"facing": "up"},
        },
        # North spawn (from dummy)
        {
            "name": "from_dummy", "type": "spawn", "point": True,
            "x": 320, "y": 140, "properties": {"facing": "down"},
        },
        # Door south back to ruins_entrance
        {
            "name": "door_to_entrance", "type": "door",
            "x": 280, "y": 440, "w": 80, "h": 40,
            "properties": {"targetRoom": "ruins_entrance", "targetSpawn": "from_switches"},
        },
        # Door north forward to ruins_dummy
        {
            "name": "door_to_dummy", "type": "door",
            "x": 280, "y": 40, "w": 80, "h": 40,
            "properties": {"targetRoom": "ruins_dummy", "targetSpawn": "from_switches"},
        },
        # Entrance hint trigger
        {
            "name": "trigger_hint", "type": "trigger",
            "x": 240, "y": 380, "w": 160, "h": 40,
            "properties": {"cutscene": "r4_hint", "once": "ruins.r4.intro"},
        },
        # Mechanical keyboard frame (136x28 px at 2x = 272x56 px, centered at 320)
        {
            "name": "rgb_keyboard_frame", "type": "decoration",
            "x": 184, "y": 160, "w": 272, "h": 56,
            "properties": {
                "sprite": "assets/props/ruins_mech_keyboard.png",
                "solid": False,
            },
        },
        # Note on wall next to keyboard
        {
            "name": "note_keyboard", "type": "examine",
            "x": 120, "y": 155, "w": 40, "h": 50,
            "properties": {"lines": "* \"Not this one. - Ruins Administration (not Furiell)\""},
        },
        # Courage sign
        {
            "name": "sign_courage", "type": "sign",
            "x": 480, "y": 155, "w": 40, "h": 50,
            "properties": {
                "lines": "* \"Only the fearless may proceed. Brave ones, foolish ones. Both walk not the path of the noob.\"",
            },
        },
        # Shutter door across north exit: closed -> opening (5 frames) -> open
        {
            "name": "shutter_door", "type": "prop",
            "x": 280, "y": 60, "w": 80, "h": 74,
            "properties": {
                "sprite": "assets/props/ruins_switch_door.png",
                "anim": "closed",
                "altAnim": "opening",
                "altAnimIf": "ruins.r4.solved >= 1",
                "solid": False,
            },
        },
        # Shutter barrier: blocks doorway while unsolved, retracts when solved
        {
            "name": "shutter_door_barrier", "type": "prop",
            "x": 280, "y": 80, "w": 80, "h": 40,
            "properties": {
                "solid": True,
                "hideIf": "ruins.r4.solved >= 1",
                "lines": "* A heavy security shutter blocks the exit.\n* The cables lead back to the RGB keyboard.",
            },
        },
    ]

    # Mechanical switches: 5 keys centered on sockets, off (up) / on (down)
    key_specs = [
        (0, "red", 230),
        (1, "yellow", 268),
        (2, "green", 304),
        (3, "blue", 340),
        (4, "magenta", 378),
    ]
    for key, color, kx in key_specs:
        objects.append({
            "name": f"rgb_key_{key}", "type": "prop",
            "x": kx, "y": 158, "w": 36, "h": 48,
            "properties": {
                "rgbKey": key,
                "sprite": "assets/props/ruins_mech_keys.png",
                "anim": f"{color}_off",
                "altAnim": f"{color}_on",
                "altAnimIf": f"ruins.r4.key{key} >= 1",
                "cutscene": "r4_keys",
                "solid": False,
            },
        })

    # Floor cables: run from keyboard right side along floor to the north door.
    # Horizontal segments switch from straight_h to lit_h when ruins.r4.solved >= 1.
    cable_segments = [
        ("cable_v", 440, 160, "straight_v", None),
        ("cable_corner", 440, 120, "corner", None),
        ("cable_h1", 400, 120, "straight_h", "lit_h"),
        ("cable_h2", 360, 120, "straight_h", "lit_h"),
        ("cable_door", 320, 120, "straight_h", "lit_h"),
        ("cable_door_left", 280, 120, "straight_h", "lit_h"),
    ]
    for cname, cx, cy, anim, alt in cable_segments:
        props = {
            "sprite": "assets/props/ruins_floor_cables.png",
            "anim": anim,
            "solid": False,
        }
        if alt is not None:
            props["altAnim"] = alt
            props["altAnimIf"] = "ruins.r4.solved >= 1"
        objects.append({
            "name": cname, "type": "prop",
            "x": cx, "y": cy, "w": 40, "h": 40,
            "properties": props,
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
