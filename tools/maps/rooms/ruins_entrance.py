"""R3: the abandoned main hall of FurNation Con '14."""

from ruins_common import *  # noqa: F401,F403 (shared map helpers)


# The round-8 sheet is useful material rather than a clean autotile grid. Keep the
# hand-picked cells here: broad floor/wall cells repeat safely, while the four
# carpet cells (25-28) are always kept together across the grand stair.
HALL_TILESET = {
    "columns": 8,
    "firstgid": 1,
    "image": "assets/tiles/ruins_hall.png",
    "imageheight": 80,
    "imagewidth": 160,
    "name": "ruins_hall",
    "tilecount": 32,
    "tileheight": 20,
    "tilewidth": 20,
}


def build_ruins_entrance():
    width = 22
    height = 14

    # GIDs from ruins_hall.png. The dark openings at north/south are the route
    # through the building; the side openings read as halls without adding exits.
    # Repeating 25-28 as an intact strip makes the bright, ruined carpet describe
    # the broad grand staircase where the source art is not a conventional grid.
    hall_ground = [
        [9, 10, 9, 11, 9, 10, 9, 11, 9, 15, 24, 24, 16, 9, 10, 9, 11, 9, 10, 9, 11, 9],
        [17, 18, 17, 19, 18, 20, 17, 18, 19, 23, 24, 24, 23, 17, 18, 19, 20, 17, 18, 19, 17, 18],
        [17, 18, 19, 20, 21, 18, 17, 19, 20, 23, 24, 24, 23, 18, 17, 20, 19, 18, 21, 17, 19, 18],
        [1, 2, 4, 5, 7, 1, 2, 4, 5, 7, 1, 2, 4, 5, 7, 1, 2, 4, 5, 7, 1, 2],
        [24, 24, 4, 5, 7, 25, 26, 27, 28, 25, 26, 27, 28, 25, 26, 27, 28, 4, 5, 7, 24, 24],
        [24, 24, 5, 7, 1, 25, 26, 27, 28, 25, 26, 27, 28, 25, 26, 27, 28, 5, 7, 1, 24, 24],
        [16, 16, 7, 1, 2, 25, 26, 27, 28, 25, 26, 27, 28, 25, 26, 27, 28, 7, 1, 2, 15, 15],
        [1, 2, 4, 5, 7, 25, 26, 27, 28, 25, 26, 27, 28, 25, 26, 27, 28, 4, 5, 7, 1, 2],
        [2, 4, 5, 7, 1, 4, 5, 7, 1, 2, 4, 5, 7, 1, 2, 4, 5, 7, 1, 2, 4, 5],
        [4, 5, 7, 1, 2, 5, 7, 1, 2, 4, 5, 7, 1, 2, 4, 5, 7, 1, 2, 4, 5, 7],
        [5, 7, 1, 2, 4, 7, 1, 2, 4, 5, 7, 1, 2, 4, 5, 7, 1, 2, 4, 5, 7, 1],
        [7, 1, 2, 4, 5, 1, 2, 4, 5, 7, 1, 2, 4, 5, 7, 1, 2, 4, 5, 7, 1, 2],
        [17, 18, 19, 20, 17, 18, 19, 20, 17, 23, 24, 24, 23, 18, 19, 20, 17, 18, 19, 20, 17, 18],
        [9, 10, 11, 9, 10, 11, 9, 10, 9, 15, 24, 24, 16, 9, 10, 11, 9, 10, 11, 9, 10, 9],
    ]

    # The route through R3 is unchanged: R2 enters at the south main doors and R4
    # enters at the north stair landing. Side halls are architectural silhouettes.
    collision = [
        {"x": 0, "y": 0, "w": 400, "h": 120, "name": "top_wall_left"},
        {"x": 480, "y": 0, "w": 400, "h": 120, "name": "top_wall_right"},
        {"x": 0, "y": 0, "w": 40, "h": 560, "name": "left_void"},
        {"x": 840, "y": 0, "w": 40, "h": 560, "name": "right_void"},
        {"x": 0, "y": 480, "w": 400, "h": 80, "name": "bottom_wall_left"},
        {"x": 480, "y": 480, "w": 400, "h": 80, "name": "bottom_wall_right"},
    ]

    objects = [
        # R2/R4 continuity (names, targets and coordinates intentionally unchanged).
        {"name": "from_floofy", "type": "spawn", "point": True, "x": 420, "y": 460,
         "properties": {"facing": "up"}},
        {"name": "spawn", "type": "spawn", "point": True, "x": 420, "y": 460,
         "properties": {"facing": "up"}},
        {"name": "from_switches", "type": "spawn", "point": True, "x": 420, "y": 140,
         "properties": {"facing": "down"}},
        {"name": "from_dummy", "type": "spawn", "point": True, "x": 420, "y": 140,
         "properties": {"facing": "down"}},
        {"name": "from_notices", "type": "spawn", "point": True, "x": 420, "y": 140,
         "properties": {"facing": "down"}},
        {"name": "door_to_floofy", "type": "door", "x": 400, "y": 520, "w": 80, "h": 40,
         "properties": {"targetRoom": "ruins_floofy", "targetSpawn": "from_entrance"}},
        {"name": "door_to_switches", "type": "door", "x": 400, "y": 40, "w": 80, "h": 40,
         "properties": {"targetRoom": "ruins_switches", "targetSpawn": "from_entrance"}},

        # Left side: the collapsed welcome area and a place where the attendee left a tail.
        {"name": "hall_welcome_sign", "type": "prop", "x": 92, "y": 408, "w": 64, "h": 64,
         "properties": {
             "sprite": "assets/props/ruins_hall_welcome_sign.png", "solid": True,
             "counter": "ruins.r3.welcome", "cutscene": "r3_hall",
             "lines": "* \"FurNation Con '14 — Welcome!\"\n* The sign is face-down. It saw enough.",
         }},
        {"name": "hall_bench", "type": "prop", "x": 92, "y": 252, "w": 64, "h": 64,
         "properties": {
             "sprite": "assets/props/ruins_hall_bench.png", "solid": True,
             "counter": "ruins.r3.bench", "cutscene": "r3_hall",
             "lines": "* A fursuit tail is curled under the bench.\n* (You decide not to check whether it is attached.)",
         }},

        # Right side: dead registration hardware and a cold abandoned snack table.
        {"name": "hall_scanner", "type": "prop", "x": 692, "y": 408, "w": 64, "h": 64,
         "properties": {
             "sprite": "assets/props/ruins_hall_scanner.png", "solid": True,
             "counter": "ruins.r3.scanner", "cutscene": "r3_hall",
             "lines": "* A dead badge scanner. It still expects an attendee from 2014.",
         }},
        {"name": "hall_schedule", "type": "prop", "x": 700, "y": 224, "w": 64, "h": 64,
         "properties": {
             "sprite": "assets/props/ruins_hall_schedule.png", "solid": True,
             "counter": "ruins.r3.schedule", "cutscene": "r3_hall",
             "lines": "* The schedule still says 2014.\n* Every panel after noon is marked \"TBA\".",
         }},
        {"name": "hall_snacks", "type": "prop", "x": 620, "y": 348, "w": 64, "h": 64,
         "properties": {
             "sprite": "assets/props/ruins_hall_snacks.png", "solid": True,
             "counter": "ruins.r3.snacks", "cutscene": "r3_hall",
             "lines": "* The snack table is cold.\n* The cheese dip has developed boundaries.",
         }},

        # Fluorescent fixtures: two dead tubes and one weak survivor. The round-8
        # tube is a single frame, so the existing gentle lamp animation supplies the
        # soft flicker without adding code or editing the asset.
        {"name": "hall_light_dead_west", "type": "prop", "x": 222, "y": 136, "w": 64, "h": 32,
         "properties": {"sprite": "assets/props/ruins_hall_light.png", "solid": False}},
        {"name": "hall_light", "type": "prop", "x": 408, "y": 136, "w": 64, "h": 32,
         "properties": {
             "sprite": "assets/props/ruins_hall_light.png", "solid": False,
             "counter": "ruins.r3.light", "cutscene": "r3_hall",
             "lines": "* One fluorescent tube is still trying.\n* The others established boundaries.",
         }},
        {"name": "hall_light_dead_east", "type": "prop", "x": 594, "y": 136, "w": 64, "h": 32,
         "properties": {"sprite": "assets/props/ruins_hall_light.png", "solid": False}},
        {"name": "hall_light_flicker_soft", "type": "prop", "x": 514, "y": 194, "w": 20, "h": 40,
         "properties": {
             "sprite": "assets/props/ruins_lamp.png", "anim": "flicker", "solid": False,
         }},

        # The two redrawn legacy jokes sit in the dusty central route.
        {"name": "banner_deodorant", "type": "prop", "x": 380, "y": 292, "w": 120, "h": 40,
         "properties": {
             "sprite": "assets/props/ruins_banner_deodorant.png", "solid": False,
             "counter": "ruins.r3.banner", "cutscene": "r3_items",
             "lines": "* \"Deodorant is NOT optional.\"",
         }},
        {"name": "dorito_pile", "type": "prop", "x": 246, "y": 374, "w": 80, "h": 40,
         "properties": {
             "sprite": "assets/props/ruins_wrapper_pile.png", "anim": "pile", "solid": False,
             "counter": "ruins.r3.doritos", "cutscene": "r3_items",
             "lines": "* A pile of Dorito bags. *crunch*", "sound": "assets/sfx/step_crunch.ogg",
         }},
        {"name": "savepoint", "type": "savepoint", "point": True, "x": 300, "y": 380},

        # The hand-in-hand arrival remains exactly where it was in the route.
        {"name": "trigger_furiell", "type": "trigger", "x": 360, "y": 440, "w": 160, "h": 40,
         "properties": {"cutscene": "r3_furiell", "once": "ruins.furiell.r3"}},
        {"name": "furiell_entrance", "type": "npc", "x": 520, "y": 350, "w": 64, "h": 80,
         "properties": {
             "sprite": "assets/sprites/ow/furiell.png", "anim": "stand_down",
             "showIf": "ruins.furiell.r3 >= 1", "speaker": "FURIELL",
             "lines": "The surface is full of cruel people with cameras. Down here, nobody will ever laugh at you.||You can always accept a hug. It is the fastest way to make friends.",
             "solid": True,
         }},
    ]

    # make_tiled_map supplies the stable Tiled schema and object/property encoding.
    # Replace its default Ruins ground data and tileset only for this room.
    hall_map = make_tiled_map(
        room_id="ruins_entrance",
        display_name="Ruins - Abandoned Main Hall",
        width=width,
        height=height,
        ground_grid=["." * width for _ in range(height)],
        collision_boxes=collision,
        objects=objects,
        music="overworld.ruins_entrance",
        save_place="* The distant hum of dial-up internet fills you with DETERMINATION.",
    )
    hall_map["layers"][0]["data"] = [gid for row in hall_ground for gid in row]
    hall_map["tilesets"] = [HALL_TILESET]
    return hall_map
