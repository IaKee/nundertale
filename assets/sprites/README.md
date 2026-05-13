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
