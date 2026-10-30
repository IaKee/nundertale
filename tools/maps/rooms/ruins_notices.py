"""ruins_notices: the room builder for data/maps/ruins_notices.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R8: Notices, CRT and Art Show (ruins_notices)
# Size: 20x14 tiles (800x560 px).
# Notice board, flickering CRT with hidden laser pointer, Art Show booth and 18+ curtain.
# =============================================================================
def build_ruins_notices():
    width = 20
    height = 14

    ground = [
        "####################",
        "#[======]  [======]#",
        "#{--v-k-}  {---o--}#",
        "#.,;,..,.788889.,;.#",
        "#;.,..,.c455556.,..#",
        "#.,;,..,.455556;.w.#",
        "#;.,..,;.455556.,;.#",
        "#.,;,..*.455556;.,.#",
        "#;.,..,.p455556.,;.#",
        "#.,;,..,.122223;.,.#",
        "#;.,..,;,..,;,..;.,#",
        "#.,;,..,;,..,;,..,;#",
        "#________  ________#",
        "#########  #########",
    ]

    above = [
        "....................",
        ".........LR.........",
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
        "....................",
        "....................",
    ]

    collision = [
        {"x": 0, "y": 0, "w": 360, "h": 120, "name": "top_wall_left"},
        {"x": 440, "y": 0, "w": 360, "h": 120, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 560, "name": "left_void"},
        {"x": 760, "y": 0, "w": 40, "h": 560, "name": "right_void"},
        {"x": 0, "y": 480, "w": 360, "h": 80, "name": "bottom_wall_left"},
        {"x": 440, "y": 480, "w": 360, "h": 80, "name": "bottom_wall_right"},
    ]

    objects = [
        # South spawn (from Hallway, Dummy, and legacy Entrance) & default
        {
            "name": "from_hallway",
            "type": "spawn",
            "point": True,
            "x": 380,
            "y": 460,
            "properties": {"facing": "up"},
        },
        {
            "name": "from_dummy",
            "type": "spawn",
            "point": True,
            "x": 380,
            "y": 460,
            "properties": {"facing": "up"},
        },
        {
            "name": "from_entrance",
            "type": "spawn",
            "point": True,
            "x": 380,
            "y": 460,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 380,
            "y": 460,
            "properties": {"facing": "up"},
        },
        # North spawn (from Barky's arena)
        {
            "name": "from_barky",
            "type": "spawn",
            "point": True,
            "x": 380,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_hallway
        {
            "name": "door_to_hallway",
            "type": "door",
            "x": 360,
            "y": 520,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_hallway",
                "targetSpawn": "from_notices",
            },
        },
        # Door north forward to ruins_barky
        {
            "name": "door_to_barky",
            "type": "door",
            "x": 360,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_barky",
                "targetSpawn": "from_notices",
            },
        },
        # Notice board on the west wall
        {
            "name": "notice_board",
            "type": "prop",
            "x": 140,
            "y": 90,
            "w": 80,
            "h": 60,
            "properties": {
                "sprite": "assets/props/ruins_notice_board.png",
                "solid": True,
                "lines": "* NOTICE FROM RUINS ADMINISTRATION: Please do not shed in the ventilation shafts. Violators will be banned from the Minecraft server.",
            },
        },
        # CRT Monitor with BSOD (anim flicker)
        {
            "name": "crt_monitor",
            "type": "prop",
            "x": 220,
            "y": 240,
            "w": 80,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_crt.png",
                "anim": "flicker",
                "solid": True,
                "lines": "* A blue screen of death. The error message is in Comic Sans.",
            },
        },
        # Item pickup behind CRT: Toy Laser Pointer
        {
            "name": "laser_pickup",
            "type": "itemPickup",
            "x": 240,
            "y": 210,
            "w": 40,
            "h": 20,
            "properties": {
                "item": "Toy Laser Pointer",
                "flag": "ruins.laser.taken",
                "lines": "* You found a Toy Laser Pointer behind the CRT.",
                "emptyLines": "* Just dust bunnies. Synthetic ones.",
            },
        },
        # Art Show 18+ curtain on the north-east wall
        {
            "name": "curtain_18",
            "type": "prop",
            "x": 600,
            "y": 60,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_curtain_18.png",
                "solid": True,
                "counter": "ruins.curtain.looks",
                "lines": "* You decide to respect the curtain.",
                "lines2": "* You respect the curtain. For a while.",
                "lines3": "* (...No.)",
            },
        },
        # Art Show booth / bid sheet table
        {
            "name": "art_show_booth",
            "type": "prop",
            "x": 520,
            "y": 220,
            "w": 120,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_booth.png",
                "solid": True,
                "lines": "* A bid sheet. Highest bid: 400G. For a drawing of a \"very tired raccoon\". You don't ask.",
            },
        },
        # OVW-022 (LORE §30.4): a bloodstain where an ex-gamer converted. Examining it replays a
        # translucent ghost (data/cutscenes/r8_bloodstain.json) then "* (They said yes.)".
        {
            "name": "bloodstain",
            "type": "prop",
            "x": 560,
            "y": 380,
            "w": 60,
            "h": 30,
            "properties": {
                "bloodstain": True,
                "cutscene": "r8_bloodstain",
            },
        },
        # The replay cast: hidden until the cutscene raises ruins.r8.ghost_on / soul_on.
        {
            "name": "bloodstain_ghost",
            "type": "npc",
            "x": 448,
            "y": 380,
            "w": 32,
            "h": 32,
            "properties": {
                "sprite": "assets/sprites/ow/grub.png",
                "ghost": True,
                "solid": False,
                "showIf": "ruins.r8.ghost_on == 1",
            },
        },
        {
            "name": "bloodstain_hugger",
            "type": "npc",
            "x": 612,
            "y": 368,
            "w": 32,
            "h": 44,
            "properties": {
                "sprite": "assets/sprites/ow/furiell.png",
                "ghost": True,
                "solid": False,
                "showIf": "ruins.r8.ghost_on == 1",
            },
        },
        {
            "name": "bloodstain_soul",
            "type": "npc",
            "x": 576,
            "y": 350,
            "w": 16,
            "h": 16,
            "properties": {
                "heart": True,
                "solid": False,
                "showIf": "ruins.r8.soul_on == 1",
            },
        },
        {
            "name": "shedding_complaint_form",
            "type": "examine",
            "x": 360,
            "y": 120,
            "w": 80,
            "h": 40,
            "properties": {
                "lines": "* A complaint form asks how much you shed.||* NONE. SOME. ATMOSPHERIC.",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_notices",
        display_name="Ruins - Notice Hall",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
        encounters="froggsona:2,shysona:1,modsmal:1,ruins_r8_duo:1",
        encounter_budget=1,
    )


# =============================================================================
# Room R9: Barky's Arena (ruins_barky)
# Size: 16x12 tiles (640x480 px).
# C1-009/BUG-18: everything is room objects + cutscenes (no hard-coded scene code).
# Flow: intro trigger (r9_barky_intro: howl, Barky drops, fight) -> battle sets
# ruins.barky.state (1 spared, 2 killed) -> post-battle triggers (r9_barky_spared /
# r9_barky_killed); the north gate (to_bakesale) opens on ruins.barky.state >= 1.
# =============================================================================
