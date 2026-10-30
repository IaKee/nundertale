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
        # South spawn (from VIP Lounge / Basement) & default spawn
        {
            "name": "from_viplounge",
            "type": "spawn",
            "point": True,
            "x": 320,
            "y": 380,
            "properties": {"facing": "up"},
        },
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
        # Door south back to ruins_viplounge (R12a)
        {
            "name": "door_to_viplounge",
            "type": "",
            "class": "door",
            "x": 280,
            "y": 440,
            "w": 80,
            "h": 40,
            "properties": {
                "targetRoom": "ruins_viplounge",
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
    ]

    tiled_map = make_tiled_map(
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
    for layer in tiled_map["layers"]:
        if layer["type"] == "objectgroup":
            for obj in layer["objects"]:
                if obj.get("name") == "door_to_viplounge":
                    obj["type"] = ""
                    obj["class"] = "door"
    return tiled_map
