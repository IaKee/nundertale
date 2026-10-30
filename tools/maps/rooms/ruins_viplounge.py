"""ruins_viplounge: the room builder for data/maps/ruins_viplounge.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R12a: VIP Lounge (ruins_viplounge)
# Size: 16x18 tiles (640x720 px).
# LORE §12 / §19.0b (C1-028, v3.5).
# Floofy's real lair past Furiell's basement.
# Lair props: flower walls, fairy lights, heart beanbag, velvet rope, plush throne,
# rack of empty fursuit heads staring at the door.
# Petal trail from the south door.
# Save point at the entrance.
# Floofy on his plush throne before the encounter; gone after surviving (ruins.floofy.lair >= 1).
# Exits: south to ruins_basement (R12), north to ruins_exit (R13).
# =============================================================================
def build_ruins_viplounge():
    width = 16
    height = 18

    ground = [
        "################",
        "###[==]  [==]###",
        "###{-v}  {-k}###",
        "###.;.7889.,.###",
        "###.,.4556.;.###",
        "###;.,4556.,.###",
        "###.,*4556.;.###",
        "###;.,4556.w.###",
        "###.,.4556.,.###",
        "###;.,4556.;.###",
        "###.,.4556.,.###",
        "###;.,4556.,.###",
        "###.,.4556.;.###",
        "###;.,4556...###",
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
        {"x": 0, "y": 0, "w": 120, "h": 720, "name": "left_void"},
        {"x": 520, "y": 0, "w": 120, "h": 720, "name": "right_void"},
        {"x": 0, "y": 640, "w": 280, "h": 80, "name": "bottom_wall_left"},
        {"x": 360, "y": 640, "w": 280, "h": 80, "name": "bottom_wall_right"},
        # Solid props collision
        {"x": 120, "y": 180, "w": 80, "h": 52, "name": "col_headrack"},
        {"x": 120, "y": 340, "w": 64, "h": 28, "name": "col_rope"},
        {"x": 480, "y": 340, "w": 40, "h": 32, "name": "col_beanbag"},
        {"x": 300, "y": 240, "w": 40, "h": 46, "name": "col_throne"},
    ]

    objects = [
        # Spawns: from_basement (south), from_exit (north), default spawn
        {
            "name": "from_basement",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 620,
            "properties": {"facing": "up"},
        },
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 620,
            "properties": {"facing": "up"},
        },
        {
            "name": "from_exit",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 140,
            "properties": {"facing": "down"},
        },
        # Door south back to ruins_basement (R12)
        {
            "name": "door_to_basement",
            "type": "",
            "class": "door",
            "x": 280,
            "y": 680,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_basement",
                "targetSpawn": "from_viplounge",
            },
        },
        # Door north forward to ruins_exit (R13)
        {
            "name": "door_to_exit",
            "type": "",
            "class": "door",
            "x": 280,
            "y": 40,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_exit",
                "targetSpawn": "from_viplounge",
            },
        },
        # Save point at the entrance
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 200,
            "y": 580,
            "properties": {
                "lines": "* A bass line thumps through the wall. It fills you with DETERMINATION. Against your will.",
            },
        },
        # Entry / center trigger for Floofy lair encounter
        {
            "name": "trigger_floofy_lair",
            "type": "trigger",
            "x": 160,
            "y": 380,
            "w": 320,
            "h": 40,
            "properties": {
                "cutscene": "r12a_lair_opener",
                "hideIf": "ruins.floofy.lair >= 1",
            },
        },
        # Floofy NPC waiting on/by his throne (hidden once lair survived)
        {
            "name": "floofy",
            "type": "npc",
            "x": 300,
            "y": 280,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/sprites/ow/floofy.png",
                "anim": "idle",
                "solid": True,
                "hideIf": "ruins.floofy.lair >= 1",
            },
        },
        # --- PROPS ---
        # Fairy lights along the upper wall
        {
            "name": "prop_lights_left",
            "type": "prop",
            "x": 160,
            "y": 70,
            "w": 80,
            "h": 14,
            "properties": {
                "sprite": "assets/props/ruins_lounge_lights.png",
                "solid": False,
            },
        },
        {
            "name": "prop_lights_right",
            "type": "prop",
            "x": 400,
            "y": 70,
            "w": 80,
            "h": 14,
            "properties": {
                "sprite": "assets/props/ruins_lounge_lights.png",
                "solid": False,
            },
        },
        # Flower walls on left and right walls
        {
            "name": "flowerwall_left",
            "type": "prop",
            "x": 120,
            "y": 40,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_lounge_flowerwall.png",
                "solid": True,
            },
        },
        {
            "name": "flowerwall_right",
            "type": "prop",
            "x": 440,
            "y": 40,
            "w": 80,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_lounge_flowerwall.png",
                "solid": True,
            },
        },
        # Rack of empty fursuit heads staring at the door
        {
            "name": "prop_headrack",
            "type": "prop",
            "x": 120,
            "y": 180,
            "w": 80,
            "h": 52,
            "properties": {
                "sprite": "assets/props/ruins_lounge_headrack.png",
                "solid": True,
            },
        },
        # Velvet rope
        {
            "name": "prop_rope",
            "type": "prop",
            "x": 120,
            "y": 340,
            "w": 64,
            "h": 28,
            "properties": {
                "sprite": "assets/props/ruins_lounge_rope.png",
                "solid": True,
            },
        },
        # Heart beanbag
        {
            "name": "prop_beanbag",
            "type": "prop",
            "x": 480,
            "y": 340,
            "w": 40,
            "h": 32,
            "properties": {
                "sprite": "assets/props/ruins_lounge_beanbag.png",
                "solid": True,
            },
        },
        # Plush throne
        {
            "name": "prop_throne",
            "type": "prop",
            "x": 300,
            "y": 240,
            "w": 40,
            "h": 46,
            "properties": {
                "sprite": "assets/props/ruins_lounge_throne.png",
                "solid": True,
            },
        },
        # --- EXAMINABLES ---
        {
            "name": "examine_headrack",
            "type": "examine",
            "x": 120,
            "y": 180,
            "w": 80,
            "h": 52,
            "properties": {
                "counter": "ruins.r12a.headrack",
                "lines": "* Fursuit heads. Dozens. All empty. All facing the door.",
            },
        },
        {
            "name": "examine_beanbag",
            "type": "examine",
            "x": 480,
            "y": 340,
            "w": 40,
            "h": 32,
            "properties": {
                "counter": "ruins.r12a.beanbag",
                "lines": "* Heart-shaped. Still warm.\n* You decide not to sit on it.",
            },
        },
        {
            "name": "examine_rope",
            "type": "examine",
            "x": 120,
            "y": 340,
            "w": 64,
            "h": 28,
            "properties": {
                "counter": "ruins.r12a.rope",
                "lines": "* VIP. Very Important Plushies.",
            },
        },
        {
            "name": "examine_throne",
            "type": "examine",
            "x": 300,
            "y": 240,
            "w": 40,
            "h": 46,
            "properties": {
                "counter": "ruins.r12a.throne",
                "lines": "* A throne of plushies. One of them wears a smaller crown.",
                "showIf": "ruins.floofy.lair >= 1",
            },
        },
        {
            "name": "examine_flowerwall",
            "type": "examine",
            "x": 120,
            "y": 40,
            "w": 80,
            "h": 80,
            "properties": {
                "counter": "ruins.r12a.flowerwall",
                "lines": "* A wall of flowers. One spot is still shaking.\n* (You leave it alone.)",
            },
        },
        {
            "name": "examine_lights",
            "type": "examine",
            "x": 160,
            "y": 70,
            "w": 80,
            "h": 14,
            "properties": {
                "counter": "ruins.r12a.lights",
                "lines": "* Fairy lights. They pulse in time with the bass.",
            },
        },
        # Petal trail from the south door toward the center
        *lay_petal_trail(
            [(8, r) for r in range(15, 8, -1)],
            scatter_tiles=[(7, 13), (9, 11), (7, 9)],
        ),
    ]

    tiled_map = make_tiled_map(
        room_id="ruins_viplounge",
        display_name="Ruins - VIP Lounge",
        width=width,
        height=height,
        ground_grid=ground,
        collision_boxes=collision,
        objects=objects,
        above_grid=above,
        music="floofy_lounge",
        save_place="* A bass line thumps through the wall. It fills you with DETERMINATION. Against your will.",
    )
    for layer in tiled_map["layers"]:
        if layer["type"] == "objectgroup":
            for obj in layer["objects"]:
                if obj.get("name") in ("door_to_basement", "door_to_exit"):
                    obj["type"] = ""
                    obj["class"] = "door"
    return tiled_map
