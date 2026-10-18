# Sprite sheets

nUndertale sprite sheets use a PNG with alpha and a JSON definition beside it. Both files share the same base name. Artwork stays at its native pixel size and is drawn at 2x scale with point filtering.

```json
{
  "image": "barky.png",
  "cell": [60, 80],
  "origin": [30, 80],
  "anims": {
    "idle": { "row": 0, "frames": 2, "fps": 2, "loop": true },
    "hurt": { "row": 1, "frames": 1 },
    "blush": { "row": 2, "frames": 1 },
    "spared": { "row": 3, "frames": 1 }
  }
}
```

`cell` is one frame's width and height. Frames for an animation run left to right on its `row`. `origin` is the pivot inside each cell in native pixels; use the character's feet or bottom center. `frames` defaults to 1, `fps` defaults to 4, and `loop` defaults to `true`. A non-looping animation holds its final frame.

Expected assets and animation names:

- `battle/barky.png` and `battle/barky.json`: `idle`, `hurt`, `blush`, `spared`. A cell near 60 x 80 pixels is appropriate.
- `ow/pepe_frog.png` and `ow/pepe_frog.json`: `walk_down`, `walk_up`, `walk_left`, `walk_right`, plus `stand_down`, `stand_up`, `stand_left`, `stand_right`. A cell near 20 x 30 pixels is appropriate.

Keep every frame for a sheet the same size, preserve transparent pixels around the character, and place the pivot consistently across all rows. Until both the PNG and JSON exist and the JSON is valid, the game keeps using its current fallback art.

## What the game loads today

Each of these falls back to the old primitive drawing when its file is missing or invalid.

| Where | Files |
| :--- | :--- |
| Battle buttons, SOUL, FIGHT target, attack bar, hit effect | `ui/buttons.*`, `ui/souls.*`, `ui/target.png`, `ui/attack_bar.*`, `ui/slash.*` |
| Barky's bullets (`uwu_rain`, `dew_bounce`, `howl_wave`) | `bullets/uwu_letters.*`, `bullets/can.*`, `bullets/sound_ring.*` |
| Barky in battle and in the overworld | `sprites/battle/barky.*`, `sprites/ow/barky.*` (`stand_down`) |
| Pepe in the overworld and the encounter "!" | `sprites/ow/pepe_frog.*`, `ui/exclaim.png` |
| Save point, snack bowl | `props/save_point.*` (`spin`), `props/ruins_dorito_bowl.*` (`full`, `knocked`) |
| Title logo, FURRY OVER | `ui/logo.png` + `ui/logo_n.png` (the "n" trembles), `ui/furry_over.png` |

The other sheets in `assets/` (Furiell, Floofy, the other enemies, portraits, the remaining bullets and props, the Ruins tileset) wait for the rooms, encounters or loaders that use them. `docs/art_prompts/ruins/*.md` says where each one goes.
