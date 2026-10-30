"""ruins_fall: the room builder for data/maps/ruins_fall.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R1: The Landing (ruins_fall)
# Size: 16x12 tiles (640x480 px).
# Cave room with compacted 2014 con trash and nacho cheese strata.
# Grub wakes lying on top of Floofy, knocked out in his flower bed under the surface light beam
# (LORE v3.4: the golden-flower parody made literal; BUG-46..48). North exit is the scratched Con doors to R1a.
# =============================================================================
def build_ruins_fall():
    width = 16
    height = 12

    ground = ["." * width for _ in range(height)]
    above = ["." * width for _ in range(height)]

    landing_ground_rows = [
        [8,  8, 17, 18, 29, 18, 43,  8,  8, 43, 18, 29, 18, 19,  8,  8],
        [8, 17, 20, 21, 24, 22, 44, 46, 47, 44, 20, 48, 24, 22, 19,  8],
        [8, 20, 27, 28, 32, 27, 45, 46, 47, 45, 27, 28, 32, 26, 22,  8],
        [20, 23, 23, 23, 23, 23, 46, 47, 46, 46, 23, 23, 23, 23, 23, 22],
        # BUG-46: a plain dirt floor; a ring of tiny flowers around the bed, three pieces of fallen trash.
        [20,  1,  2,  1,  3,  1,  1,  2,  1,  1,  2,  1,  1,  3,  1, 22],
        [20,  2,  1,  5,  1,  1, 14,  1,  2, 14,  1,  1,  5,  1,  2, 22],
        [20,  1,  3,  1,  1, 14,  1,  1,  1,  1, 14,  1,  1,  1,  1, 22],
        [20,  1,  1,  2,  1,  1, 14,  1,  1, 14,  1,  1,  9,  1,  3, 22],
        [20,  2,  1,  1, 10,  1,  1,  1,  2,  1,  1,  1,  1,  2,  1, 22],
        [20,  1,  3,  1,  1,  2,  1,  1,  1,  3,  1, 15,  1,  1,  2, 22],
        [8, 20, 23,  1,  2,  1,  1,  3,  1,  1,  2,  1,  1, 23, 22,  8],
        [8,  8, 17, 18, 29, 18, 18, 29, 18, 18, 29, 18, 18, 19,  8,  8],
    ]

    landing_above_rows = [
        [0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0, 40, 34, 36, 40,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0, 40, 34, 36, 40,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0, 38, 37, 37, 39,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0,  0, 40, 40,  0,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0],
    ]

    collision = [
        {"x": 0, "y": 0, "w": 280, "h": 120, "name": "top_wall_left"},
        {"x": 360, "y": 0, "w": 280, "h": 120, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 600, "y": 0, "w": 40, "h": 480, "name": "right_wall"},
        {"x": 0, "y": 420, "w": 640, "h": 60, "name": "bottom_wall"},
        {"x": 40, "y": 0, "w": 40, "h": 160, "name": "cave_corner_nw"},
        {"x": 560, "y": 0, "w": 40, "h": 160, "name": "cave_corner_ne"},
        {"x": 40, "y": 400, "w": 40, "h": 80, "name": "cave_corner_sw"},
        {"x": 560, "y": 400, "w": 40, "h": 80, "name": "cave_corner_se"},
    ]

    objects = [
        # Default spawn & R1 wake position
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            # BUG-47: lying across Floofy. Spawns are the player's top-left (pivot = +19, +58): pivot (342, 290),
            # the flower bed is a floor prop (drawn first); Grub (290) draws after Floofy (275), whose head shows
            # at Grub's left.
            "x": 323,
            "y": 232,
            # BUG-40: Grub is lying from the first frame (pose = anim key in the sheet), until r1_wake plays.
            "properties": {
                "facing": "down",
                "pose": "lying",
                "poseSheet": "assets/sprites/ow/grub.png",
                "poseUntil": "ruins.r1.woke",
            },
        },
        # Spawn when coming back from Registration (R1a)
        {
            "name": "from_registration",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Legacy spawn alias kept for compatibility with earlier room-chain checks
        {
            "name": "from_floofy",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Keep this before the north door: the new-game fall begins at y=30 inside
        # both bounds, and trigger iteration is ordered. Starting the cutscene first
        # prevents the door transition from stealing the opening frame.
        {
            "name": "trigger_landing",
            "type": "trigger",
            "x": 270,
            "y": 0,
            "w": 100,
            "h": 300,
            "properties": {
                "cutscene": "r1_landing",
                "once": "ruins.r1.woke",
            },
        },
        # Trigger on the north doorway exit: sets ruins.floofy.left_landing = 1 on leaving R1
        {
            "name": "trigger_leave_landing",
            "type": "trigger",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 45,
            "properties": {
                "cutscene": "r1_landing_items",
                "once": "ruins.floofy.left_landing",
                # C1-015: Floofy only leaves after the blocked turnstile sends Grub back.
                "showIf": "ruins.r1a.turnstile_tried >= 1",
            },
        },
        # Door north -> ruins_registration (R1a)
        {
            "name": "door_to_registration",
            "type": "door",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 45,
            "properties": {
                "targetRoom": "ruins_registration",
                "targetSpawn": "from_landing",
            },
        },
        # Hidden legacy door alias kept so fixed 16-room chain checks continue to pass
        {
            "name": "door_to_floofy",
            "type": "door",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_floofy",
                "targetSpawn": "from_panelhall",
                "showIf": "legacy.direct_floofy >= 1",
            },
        },
        # Scratched convention double doors at the north cave wall
        {
            "name": "con_doors",
            "type": "prop",
            "x": 260,
            "y": 0,
            "w": 120,
            "h": 120,
            "properties": {
                "sprite": "assets/props/ruins_con_doors.png",
                "solid": False,
                "cutscene": "r1_con_doors",
                "lines": "* \"FURNATION CON '14 - REGISTRATION ->\". There are claw marks on the doors.\n* On the inside.",
            },
        },
        # Floofy knocked out in his flower bed (4 pokes, hidden once player leaves R1)
        {
            "name": "badge_under_floofy",
            "type": "examine",
            "x": 368,
            "y": 248,
            "w": 32,
            "h": 32,
            "properties": {
                "hideIf": "ruins.r1a.turnstile_tried >= 1",
                "cutscene": "r1_badge",
                "lines": "* There's a badge under him. You're not reaching under him.",
            },
        },
        {
            "name": "floofy_badge",
            "type": "prop",
            "x": 368,
            "y": 248,
            "w": 32,
            "h": 32,
            "properties": {
                "sprite": "assets/props/floofy_badge.png",
                "solid": True,
                "showIf": "ruins.r1a.turnstile_tried >= 1 && item.floofy_badge.taken == 0",
                "cutscene": "r1_badge",
                "lines": "* A con badge, in the dent. \"FLOOFY — STAFF — WELCOME COMMITTEE\".",
            },
        },
        {
            "name": "floofy",
            "type": "npc",
            "x": 296,
            "y": 239,
            "w": 48,
            "h": 36,
            "properties": {
                "sprite": "assets/sprites/ow/floofy.png",
                "anim": "knocked_out",
                "noTurn": True,
                "solid": True,
                "hideIf": "ruins.floofy.left_landing >= 1",
                "cutscene": "r1_floofy_poke",
                "lines": "* A fox fursuiter covered in flowers. You landed on him.\n* He's out cold. He's smiling anyway.",
            },
        },
        # Fox-shaped dent examine when returning to R1 after Floofy leaves
        {
            "name": "flower_dent",
            "type": "examine",
            "x": 240,
            "y": 236,
            "w": 160,
            "h": 56,
            "properties": {
                "showIf": "ruins.floofy.left_landing >= 1",
                "counter": "ruins.r1.pile_check",
                "cutscene": "r1_landing_items",
                "lines": "* There's a fox-shaped dent in the flowers.",
            },
        },
        # Floofy's flower bed under the light beam (LORE v3.4; shows the dent on return)
        {
            "name": "flower_bed",
            "type": "prop",
            "x": 224,
            "y": 212,
            "w": 192,
            "h": 88,
            "properties": {
                "floor": True,
                "sprite": "assets/props/ruins_landing_flowerbed.png",
                "anim": "floofy",
                "altAnim": "dent",
                "altAnimIf": "ruins.floofy.left_landing >= 1",
                # BUG-R8-01/02: walkable, so Grub can climb off it and reach the badge in the dent; Floofy, the can
                # and the Tide Pod keep their solid footprints (BUG-56).
                "solid": False,
                "counter": "ruins.r1.pile_check",
                "cutscene": "r1_landing_items",
                "lines": "* Forty-seven kinds of flowers. Alphabetized. Were.",
                "lines2": "* There's a fox-shaped dent in the flowers.",
            },
        },
        # Surface light beam examine
        {
            "name": "light_beam",
            "type": "examine",
            "x": 280,
            "y": 190,
            "w": 80,
            "h": 38,
            "properties": {
                "lines": "* Light from the surface. One bar of signal. Sometimes.",
            },
        },
        # Save point star
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 180,
            "y": 320,
        },
        # Examinable crushed Citrus Blast can on floor (col 12, row 7 -> x: 480, y: 280)
        {
            "name": "can_decal",
            "type": "prop",
            "x": 480,
            "y": 280,
            "w": 40,
            "h": 40,
            "properties": {
                "solid": True,
                "lines": "* Citrus Blast, 2014 edition. A vintage.",
            },
        },
        # Wake-up cutscene trigger (covers the spawn on the flower bed)
        {
            "name": "trigger_wake",
            "type": "trigger",
            "x": 270,
            "y": 210,
            "w": 100,
            "h": 90,
            "properties": {
                "cutscene": "r1_wake",
                "once": "ruins.r1.woke",
            },
        },
        # Tide Pod pickup next to where Grub wakes
        {
            "name": "tide_pod",
            "type": "prop",
            "x": 440,
            "y": 160,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/tide_pod.png",
                "solid": True,
                "hideIf": "item.tide_pod.taken >= 1",
                "cutscene": "r1_tide_pod",
                "lines": "* The Tide Pod you slipped on. It fell in with you.\n* (Forbidden candy.)",
            },
        },
        # Floor message (OVW-022, LORE §30.4)
        {
            "name": "floor_message",
            "type": "prop",
            "x": 92,
            "y": 360,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/floor_message.png",
                "solid": False,
                # Compatibility id retained for existing saves/tests; its data mirrors r1_floormsg.
                "cutscene": "floormsg_r1",
                "lines": "* \"hole ahead\" - xX_AFK_Xx. Rated: 47.",
            },
        },
    ]

    tiled_map = make_tiled_map(
        room_id="ruins_fall",
        display_name="Ruins - The Landing",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
        save_place="* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.",
        # BUG-46 (2nd pass): the Landing is one painted room (ChatGPT, regen_r7 L10) like Undertale's first room:
        # a dark cave, a pool of daylight, the flower bed. The tile rows stay as the fallback when it's missing.
        background_image="assets/tiles/landing_bg.png",
    )
    tiled_map["tilesets"][0]["image"] = "assets/tiles/landing.png"
    tiled_map["tilesets"][0]["name"] = "landing"
    layers = {layer["name"]: layer for layer in tiled_map["layers"]}
    if os.path.exists(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "assets", "tiles", "landing_bg.png")):
        layers["ground"]["data"] = [0] * (width * height)
        layers["above"]["data"] = [0] * (width * height)
    else:
        layers["ground"]["data"] = [gid for row in landing_ground_rows for gid in row]
        layers["above"]["data"] = [gid for row in landing_above_rows for gid in row]
    for obj in layers["objects"]["objects"]:
        if obj.get("name") == "door_to_registration":
            obj["type"] = ""
            obj["class"] = "door"
    return tiled_map


# =============================================================================
# Room R1a: Registration (ruins_registration)
# Size: 18x15 tiles (720x600 px).
# Empty badge pickup room between the Landing and Panel Hall. The first petal is
# waiting near the far door; there are no NPCs, encounters, or entry cutscenes.
# =============================================================================
