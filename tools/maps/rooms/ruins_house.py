"""ruins_house: the room builder for data/maps/ruins_house.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R11: Furiell's House - Living Room (ruins_house)
# Size: 20x12 tiles (800x480 px).
# Cozy living room with fireplace, armchair, bookshelf, desk with laptop and
# letterhead, Glitter Pop-Tart pickup, locked door to her room, basement stairs,
# and the east doorway to the mirror hallway.
# =============================================================================
def build_ruins_house():
    width = 20
    height = 12

    ground = [
        "#[==]u[===========]#",
        "#{--}j{--QWE-----o}#",
        "#.,;..;,..ASD...;,;#",
        "#;.,..;,..,;,..;.,.#",
        "#.,.::.78888889.,;.#",
        "#;..c..45555556;.,.#",
        "#.,.,..455z5556.,;,.",
        "#;.,...45555p56;.,..",
        "#.,;,..12222223.,;.#",
        "#;.,..*....*....;..#",
        "#________  ________#",
        "#########..#########",
    ]

    above = [
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
        {"x": 0, "y": 0, "w": 360, "h": 80, "name": "top_wall_left"},
        {"x": 480, "y": 0, "w": 320, "h": 80, "name": "top_wall_right"},
        {"x": 360, "y": 0, "w": 120, "h": 40, "name": "top_wall_stairs_back"},
        {"x": 0, "y": 440, "w": 360, "h": 40, "name": "bottom_wall_left"},
        {"x": 440, "y": 440, "w": 360, "h": 40, "name": "bottom_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 480, "name": "left_wall"},
        {"x": 760, "y": 0, "w": 40, "h": 240, "name": "right_wall_top"},
        {"x": 760, "y": 320, "w": 40, "h": 160, "name": "right_wall_bottom"},
        {"x": 80, "y": 180, "w": 160, "h": 40, "name": "col_desk"},
    ]

    objects = [
        # South spawn (from Bake Sale) & default spawn
        {
            "name": "from_bakesale",
            "type": "spawn",
            "point": True,
            "x": 400,
            "y": 380,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 400,
            "y": 380,
            "properties": {"facing": "up"},
        },
        # Door south back to ruins_bakesale
        {
            "name": "door_to_bakesale",
            "type": "door",
            "x": 360,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_bakesale",
                "targetSpawn": "from_house",
            },
        },
        # Locked door to her room
        {
            "name": "furiell_room_door",
            "type": "examine",
            "x": 120,
            "y": 40,
            "w": 40,
            "h": 40,
            "properties": {
                "lines": "* It's locked. You hear a printer.",
            },
        },
        # Package by her door (LORE v3, D-19, C1-011)
        {
            "name": "furiell_package",
            "type": "prop",
            "x": 160,
            "y": 40,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_package.png",
                "solid": False,
                "lines": "* A package for Furiell. Discreet packaging. The logo is not discreet.",
            },
        },
        # Spawn returning from Basement (stairs)
        {
            "name": "from_basement",
            "type": "spawn",
            "point": True,
            "x": 420,
            "y": 90,
            "properties": {"facing": "down"},
        },
        # Door to basement on the stairs
        {
            "name": "door_to_basement",
            "type": "door",
            "x": 360,
            "y": 40,
            "w": 120,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_basement",
                "targetSpawn": "from_house",
            },
        },
        # Spawns returning from the mirror hallway (east)
        {
            "name": "from_hallway",
            "type": "spawn",
            "point": True,
            "x": 700,
            "y": 240,
            "properties": {"facing": "left"},
        },
        {
            "name": "from_pepe",
            "type": "spawn",
            "point": True,
            "x": 700,
            "y": 240,
            "properties": {"facing": "left"},
        },
        # Door east to the mirror hallway
        {
            "name": "door_to_hallway",
            "type": "door",
            "x": 760,
            "y": 240,
            "w": 40,
            "h": 80,
            "properties": {
                "targetRoom": "ruins_house_hallway",
                "targetSpawn": "from_house",
            },
        },
        # Table / desk area - Laptop examine
        {
            "name": "laptop",
            "type": "examine",
            "x": 80,
            "y": 180,
            "w": 50,
            "h": 40,
            "properties": {
                "lines": "* She closes a tab very fast.",
            },
        },
        # Table / desk area - Letterhead examine
        {
            "name": "letterhead",
            "type": "examine",
            "x": 130,
            "y": 180,
            "w": 50,
            "h": 40,
            "properties": {
                "lines": "* \"RUINS ADMINISTRATION\". In her handwriting. Hm.",
            },
        },
        # Table / desk area - Glitter Pop-Tart itemPickup
        {
            "name": "poptart_pickup",
            "type": "itemPickup",
            "x": 180,
            "y": 180,
            "w": 50,
            "h": 40,
            "properties": {
                "item": "Glitter Pop-Tart",
                "flag": "ruins.house.poptart",
                "lines": "* (You take the Glitter Pop-Tart.)\n* It sparkles aggressively.",
                "emptyLines": "* (The plate is empty.)",
                "fullLines": "* You're carrying too much.",
            },
        },
        # Soft Armchair
        {
            "name": "armchair",
            "type": "prop",
            "x": 410,
            "y": 140,
            "w": 60,
            "h": 60,
            "properties": {
                "sprite": "assets/props/ruins_armchair.png",
                "solid": True,
                "lines": "* A very soft armchair.\n* It smells faintly of vanilla and dryer sheets.",
            },
        },
        # Crackling Fireplace
        {
            "name": "fireplace",
            "type": "prop",
            "x": 480,
            "y": 40,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_fireplace.png",
                "anim": "burn",
                "solid": True,
                "lines": "* A cozy fire crackles in the fireplace.",
            },
        },
        # Bookshelf with book line
        {
            "name": "bookshelf",
            "type": "prop",
            "x": 580,
            "y": 40,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_bookshelf.png",
                "solid": True,
                "lines": "* \"Fanfics of Questionable Quality, Vol. 7\". You read one sentence. You put it down very carefully.",
            },
        },
        # Furiell NPC in the living room (anchor for Suite 63 Tide Pod regression test)
        {
            "name": "furiell",
            "type": "npc",
            "x": 340,
            "y": 140,
            "w": 64,
            "h": 80,
            "properties": {
                "sprite": "assets/sprites/ow/furiell.png",
                "anim": "stand_down",
                "showIf": "0 == 1",
                "solid": False,
                "cutscene": "r11_furiell",
            },
        },
        # Furiell NPC in the living room (active)
        {
            "name": "furiell",
            "type": "npc",
            "x": 340,
            "y": 140,
            "w": 64,
            "h": 80,
            "properties": {
                "sprite": "assets/sprites/ow/furiell.png",
                "anim": "stand_down",
                "showIf": "ruins.house.talk_home == 0 && ruins.r12.walk == 0 && ruins.furiell.killed == 0",
                "solid": True,
                "cutscene": "r11_talk",
            },
        },
        {
            "name": "industrial_dryer_sheets",
            "type": "examine",
            "x": 300,
            "y": 80,
            "w": 40,
            "h": 40,
            "properties": {
                "lines": "* A stack of dryer sheets is labeled \"AIR FRESHENER: INDUSTRIAL.\"||* The top sheet has surrendered.",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_house",
        display_name="Ruins - Furiell's House",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="overworld.default",
    )


# =============================================================================
# Room R11-H: Mirror Hallway (ruins_house_hallway)
# Size: 16x12 tiles (640x480 px).
# Short hallway between the living room and Grub's bedroom with the mirror on
# the north wall.
# =============================================================================
