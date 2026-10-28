"""ruins_house_hallway: the room builder for data/maps/ruins_house_hallway.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R11-H: Mirror Hallway (ruins_house_hallway)
# Size: 16x12 tiles (640x480 px).
# Short hallway between the living room and Grub's bedroom with the mirror on
# the north wall.
# =============================================================================
def build_ruins_house_hallway():
    width = 16
    height = 12

    ground = [
        "################",
        "#[============]#",
        "#{--h------o--}#",
        "#.,;..;,..;.,..#",
        "#;.,..*....*..;#",
        "#.,7888888889.,#",
        ".,;4555z55556;,.",
        ";.,455555p556.,;",
        "#.,1222222223.,#",
        "#;.,..;,..;.,.;#",
        "#______________#",
        "################",
    ]

    above = ["." * width] * height

    collision = [
        {"x": 0, "y": 0, "w": 640, "h": 120, "name": "top_wall"},
        {"x": 0, "y": 400, "w": 640, "h": 80, "name": "bottom_wall"},
        {"x": 0, "y": 0, "w": 40, "h": 240, "name": "left_wall_top"},
        {"x": 0, "y": 320, "w": 40, "h": 160, "name": "left_wall_bottom"},
        {"x": 600, "y": 0, "w": 40, "h": 240, "name": "right_wall_top"},
        {"x": 600, "y": 320, "w": 40, "h": 160, "name": "right_wall_bottom"},
    ]

    objects = [
        # Default spawn in front of the mirror
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 220,
            "properties": {"facing": "up"},
        },
        # West spawn from the living room
        {
            "name": "from_house",
            "type": "spawn",
            "point": True,
            "x": 80,
            "y": 240,
            "properties": {"facing": "right"},
        },
        # East spawn from Grub's room
        {
            "name": "from_pepe",
            "type": "spawn",
            "point": True,
            "x": 520,
            "y": 240,
            "properties": {"facing": "left"},
        },
        # Door west back to ruins_house
        {
            "name": "door_to_house",
            "type": "door",
            "x": 0,
            "y": 240,
            "w": 40,
            "h": 80,
            "properties": {
                "targetRoom": "ruins_house",
                "targetSpawn": "from_hallway",
            },
        },
        # Door east to ruins_house_pepe
        {
            "name": "door_to_pepe",
            "type": "door",
            "x": 600,
            "y": 240,
            "w": 40,
            "h": 80,
            "properties": {
                "targetRoom": "ruins_house_pepe",
                "targetSpawn": "from_hallway",
            },
        },
        # Hallway mirror (LORE v3.1: cat ears reflection at any FUR > 0)
        {
            "name": "mirror",
            "type": "prop",
            "x": 300,
            "y": 80,
            "w": 40,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_mirror.png",
                "anim": "normal",
                "altAnim": "fur",
                "altAnimIf": "fur > 0",
                "solid": True,
                "lines": "* It's you!",
                "furMin": 1,
                "furLines": "* It's you. ...With cat ears?||* (You touch your head. Nothing there.)",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_house_hallway",
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
# Room R11-B: Pepe's Room (ruins_house_pepe)
# Size: 16x11 tiles (640x440 px).
# Cozy bedroom with bed, bedside lamp, closet with fursuit head, and beanbag.
# =============================================================================
