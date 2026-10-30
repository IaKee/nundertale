"""ruins_barky: the room builder for data/maps/ruins_barky.json (edit here, then run tools/maps/build_ruins_maps.py)."""

from ruins_common import *  # noqa: F401,F403 (shared tile tables and map helpers)


# =============================================================================
# Room R9: Barky's Arena (ruins_barky)
# Size: 16x12 tiles (640x480 px).
# C1-009/BUG-18: everything is room objects + cutscenes (no hard-coded scene code).
# Flow: intro trigger (r9_barky_intro: howl, Barky drops, fight) -> battle sets
# ruins.barky.state (1 spared, 2 killed) -> post-battle triggers (r9_barky_spared /
# r9_barky_killed); the north gate (to_bakesale) opens on ruins.barky.state >= 1.
# =============================================================================
def build_ruins_barky():
    # OVW-020: real tiles instead of the old test.bmp; the old objects keep their positions.
    width = 16
    height = 12

    ground = [
        "#[=====  =====]#",
        "#{--v-}  {k--o}#",
        "#.,;,..,;,..,;.#",
        "#;..,*...,;,..;#",
        "#;,I,..,;,..I,.#",
        "#.,B..,;,..,B;.#",
        "#;..,....,;,..;#",
        "#.,;,....,;,..;#",
        "#;,..,,.,;,..,.#",
        "#..;,..,;,..c,;#",
        "#;,..,.p..,;,..#",
        "#######..#######",
    ]

    above = [
        "................",
        ".......LR.......",
        "................",
        "...T........T...",
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
        {"x": 120, "y": 160, "w": 40, "h": 80, "name": "left_pillar"},
        {"x": 480, "y": 160, "w": 40, "h": 80, "name": "right_pillar"},
    ]

    objects = [
        {
            "name": "spawn",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 320,
        },
        # BUG-19: LORE R8 notice
        {
            "name": "sign",
            "type": "sign",
            "x": 100,
            "y": 70,
            "w": 40,
            "h": 30,
            "properties": {
                "speaker": "SIGN",
                "lines": "* NOTICE FROM RUINS ADMINISTRATION: Please do not shed in the ventilation shafts. Violators will be banned from the Minecraft server.",
            },
        },
        # North gate while Barky is unresolved (once state >= 1 it is the to_bakesale door)
        {
            "name": "door",
            "type": "examine",
            "x": 280,
            "y": 50,
            "w": 80,
            "h": 30,
            "properties": {
                "hideIf": "ruins.barky.state >= 1",
                "lines": "* (The northern gate is sealed.)\n* (Something with a very loud voice is guarding it.)",
            },
        },
        # Intro (ROTEIRO Scene 3, LORE R9): walking into the arena sets off the echoing howl;
        # r9_barky_intro drops Barky from the ceiling and starts the fight. The once-flag burns
        # when the battle starts. Covers the spawn and the whole southern band of the room.
        {
            "name": "trigger_barky_intro",
            "type": "trigger",
            "x": 40,
            "y": 300,
            "w": 560,
            "h": 100,
            "properties": {
                "cutscene": "r9_barky_intro",
                "once": "ruins.r9.barky_intro",
                "showIf": "ruins.barky.state == 0",
            },
        },
        # Post-battle: spared -> Barky recruits Grub (the same scene replays when you talk to him)
        {
            "name": "trigger_barky_spared",
            "type": "trigger",
            "x": 40,
            "y": 80,
            "w": 560,
            "h": 320,
            "properties": {
                "cutscene": "r9_barky_spared",
                "once": "ruins.r9.barky_spared",
                "showIf": "ruins.barky.state == 1",
            },
        },
        # Post-battle: killed -> the fur pile + flag
        {
            "name": "trigger_barky_killed",
            "type": "trigger",
            "x": 40,
            "y": 80,
            "w": 560,
            "h": 320,
            "properties": {
                "cutscene": "r9_barky_killed",
                "once": "ruins.r9.barky_killed",
                "showIf": "ruins.barky.state == 2",
            },
        },
        # Barky: invisible until he drops in (r9_barky_intro sets ruins.r9.barky_here), then fights;
        # talking to him before the fight resolves re-runs the entrance (barky_entrance) as a rematch.
        {
            "name": "npc_barky",
            "type": "npc",
            "x": 490,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "sprite": "assets/sprites/ow/barky.png",
                "anim": "stand_down",
                "showIf": "ruins.barky.state == 0 && ruins.r9.barky_here >= 1",
                "cutscene": "barky_entrance",
            },
        },
        # Barky spared: standing in his arena, recruiting
        {
            "name": "npc_barky_spared",
            "type": "npc",
            "x": 490,
            "y": 80,
            "w": 40,
            "h": 50,
            "properties": {
                "sprite": "assets/sprites/ow/barky.png",
                "anim": "stand_down",
                "showIf": "ruins.barky.state == 1",
                "cutscene": "r9_barky_spared",
            },
        },
        # Barky killed (OVW-010, BND-003): a pile of synthetic fur. No fur sprite exists yet, so the
        # wrapper pile stands in until the art lands.
        {
            "name": "fur_pile",
            "type": "prop",
            "x": 490,
            "y": 90,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/ruins_wrapper_pile.png",
                "anim": "pile",
                "showIf": "ruins.barky.state == 2",
                "lines": "* (A small mound of gray fur and gamer dust.)\n* (It smells faintly of regret and Doritos.)",
            },
        },
        # Snack bowl: up to 4 cans of Mountain Dew (counter ruins.dew.taken), the 4th knocks it over
        {
            "name": "bowl",
            "type": "prop",
            "x": 200,
            "y": 160,
            "w": 40,
            "h": 30,
            "properties": {
                "sprite": "assets/props/ruins_dorito_bowl.png",
                "anim": "full",
                "altAnim": "knocked",
                "altAnimIf": "ruins.dew.taken >= 4",
                "cutscene": "r9_dew_bowl",
                "counter": "ruins.dew.taken",
                "solid": True,
            },
        },
        {
            "name": "savepoint",
            "type": "savepoint",
            "point": True,
            "x": 320,
            "y": 260,
        },
        # y 340 (was 370): the old spawn put the player's collision box on the to_notices door,
        # so entering R9 bounced straight back to R8.
        {
            "name": "from_notices",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 340,
            "properties": {"facing": "up"},
        },
        {
            "name": "to_notices",
            "type": "door",
            "x": 280,
            "y": 410,
            "w": 80,
            "h": 30,
            "properties": {
                "targetRoom": "ruins_notices",
                "targetSpawn": "from_barky",
            },
        },
        # T2 addition: Lamp prop (Barky's natural enemy)
        {
            "name": "ruins_lamp",
            "type": "prop",
            "x": 530,
            "y": 140,
            "w": 40,
            "h": 80,
            "properties": {
                "sprite": "assets/props/ruins_lamp.png",
                "anim": "lit",
                "solid": True,
                "lines": "* Barky's natural enemy. Or his moon. He hasn't decided.",
            },
        },
        # Floor message (OVW-022, LORE §30.4)
        {
            "name": "floor_message",
            "type": "prop",
            "x": 220,
            "y": 280,
            "w": 40,
            "h": 40,
            "properties": {
                "sprite": "assets/props/floor_message.png",
                "solid": False,
                "cutscene": "floormsg_r9",
            },
        },
        # Door north forward to ruins_bakesale (revealed once Barky is resolved)
        {
            "name": "to_bakesale",
            "type": "door",
            "x": 280,
            "y": 70,
            "w": 80,
            "h": 20,
            "properties": {
                "targetRoom": "ruins_bakesale",
                "targetSpawn": "from_barky",
                "showIf": "ruins.barky.state >= 1",
            },
        },
        # Spawn returning from ruins_bakesale
        {
            "name": "from_bakesale",
            "type": "spawn",
            "point": True,
            "x": 300,
            "y": 100,
            "properties": {"facing": "down"},
        },
        # C1-014 (LORE §12 R9a): a spared Barky has a favorite booth. West wall; the first door is
        # for the first visit (Barky spared, booth untouched), the second one keeps the booth
        # reachable after the sacrifice turns ruins.barky.state into 2 (the host is powered down).
        {
            "name": "booth_arrow",
            "type": "examine",
            "x": 40,
            "y": 190,
            "w": 40,
            "h": 50,
            "properties": {
                "showIf": "ruins.barky.state == 1",
                "lines": "* A cardboard arrow is taped to the wall: \"APURRTURE FAN BOOTH ->\"||* Barky's handwriting. He drew a paw print as the dot.",
            },
        },
        {
            "name": "to_booth",
            "type": "door",
            "x": 40,
            "y": 240,
            "w": 40,
            "h": 80,
            "properties": {
                "targetRoom": "ruins_booth",
                "targetSpawn": "from_barky",
                "showIf": "ruins.barky.state == 1 && ruins.booth.state == 0",
            },
        },
        {
            "name": "to_booth_again",
            "type": "door",
            "x": 40,
            "y": 240,
            "w": 40,
            "h": 80,
            "properties": {
                "targetRoom": "ruins_booth",
                "targetSpawn": "from_barky",
                "showIf": "ruins.booth.state >= 1",
            },
        },
        # Spawn returning from ruins_booth
        {
            "name": "from_booth",
            "type": "spawn",
            "point": True,
            "x": 130,
            "y": 300,
            "properties": {"facing": "right"},
        },
        {
            "name": "pack_roster",
            "type": "examine",
            "x": 380,
            "y": 80,
            "w": 80,
            "h": 40,
            "properties": {
                "lines": "* SHADOW FANG PACK ROSTER: BARKY.||* Attendance: unanimous.",
            },
        },
    ]

    return make_tiled_map(
        room_id="ruins_barky",
        display_name="Ruins - Barky's Arena",
        width=width,
        height=height,
        ground_grid=ground,
        above_grid=above,
        collision_boxes=collision,
        objects=objects,
        tile_size=40,
    )


# =============================================================================
# Room R10: Bowl, Commission Bake Sale and Mod Rock (ruins_bakesale)
# Size: 20x12 tiles (800x480 px).
# Dorito bowl, Commission Bake Sale booth with sample donut, mod rock,
# Furiell forced call trigger, save point, and dark doorway north to R11.
# =============================================================================
