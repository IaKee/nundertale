# ART-185 In-Game 2× Verification Report (Round 8 Assets)

**Date:** 2026-11-01  
**Task:** T11 (synergy-2 session `s2-20261101-042934-c43db5`, attempt `T11-a1`)  
**Scope:** In-game verification at 2× integer scale of all assets replaced in-place in round 8 (`ART-185`).

---

## 1. Overview & Verification Summary

All round-8 replaced-in-place sheets were tested and inspected in the running game at native 2× integer resolution (`640×480` virtual canvas):

| Sheet / Asset | Resolution & Spec | In-Game Location / Context | Status | Captured Shot |
| :--- | :--- | :--- | :---: | :--- |
| `assets/sprites/battle/floofy.png` | 120×560 (cell 60×80, density 1) | `--battle floofy` (normal & intro) | **PASS** | `floofy_battle_row[0-6]_*.png` |
| `assets/sprites/battle/furiell.png` | 160×330 (cell 80×110, density 1) | `--battle furiell` | **PASS** | `furiell_battle_row[0-2]_*.png` |
| `assets/sprites/ow/furiell.png` | 128×484 (cell 32×44, density 1) | `--room ruins_floofy` | **PASS** | `furiell_ow_*.png` |
| `assets/sprites/ow/floofy.png` (row 6) | 120×468 (cell 40×36, row 6 `vanity_back`) | `--room ruins_floofy` (R2 vanity) | **PASS** | `floofy_ow_r2_vanity_back.png` |
| `assets/tiles/landing_bg.png` | 640×480 background tile | `--room ruins_fall` (R1 Landing) | **PASS** | `landing_bg_r1.png` |
| `assets/props/ruins_banner_deodorant.png` | 60×20 wall prop | `--room ruins_entrance` (R3 Entrance) | **PASS** | `ruins_r3_deodorant_chippile.png` |
| `assets/props/ruins_wrapper_pile.png` | 55×40 prop (cell 55×20) | `--room ruins_entrance`, `--room ruins_barky` | **PASS** | `ruins_r3_deodorant_chippile.png`, `ruins_barky_chippile.png` |
| `assets/img/lore/card02.png` | 200×110 intro lore panel | Intro sequence (Card 2) | **PASS** | `lore_card02.png` |
| `assets/img/lore/card09.png` | 200×110 intro lore panel | Intro sequence (Card 9, slip) | **PASS** | `lore_card09.png` |

---

## 2. Detailed Findings by Asset

### 2.1 Battle Floofy (`assets/sprites/battle/floofy.png` + `floofy.json`)
- **Format:** Redone at 60×80 density 1 (superseding the density-2 100×120 sheet). Sidecar JSON defines cell `[60, 80]` and origin `[30, 80]`.
- **Placement:** Positioned at `(enemyX=320, ENEMY_BASELINE_Y=241)` with 2× scale. The bottom edge aligns precisely with `y=241`, resting 9 pixels above the battle text box outer border (`y=250`), matching Barky and other battle enemies.
- **Rows Verified at 2×:**
  - `floofy_battle_row0_idle.png`: Row 0 `idle` (2 frames, 2 fps) — standard neutral battle pose.
  - `floofy_battle_row1_talking.png`: Row 1 `talking` (2 frames, 4 fps) — mouth movement during spoken intro beats.
  - `floofy_battle_row2_sinister.png`: Row 2 `sinister` (2 frames, 3 fps) — evil grin during petal ambush.
  - `floofy_battle_row3_soaked.png`: Row 3 `soaked` (2 frames, 3 fps) — wilted/soaked state after spritz.
  - `floofy_battle_row4_attack_petals.png`: Row 4 `attack_petals` (2 frames, 5 fps) — casting gesture.
  - `floofy_battle_row5_attack_vines.png`: Row 5 `attack_vines` (2 frames, 5 fps) — vine summon gesture.
  - `floofy_battle_row6_attack_pounce.png`: Row 6 `attack_pounce` (2 frames, 7 fps) — airborne attack frames (cell bbox `y=[0, 77]`, naturally floating 3 px above baseline).

### 2.2 Furiell Battle (`assets/sprites/battle/furiell.png` + `furiell.json`)
- **Format:** D-24 look at 80×110 density 1. Origin `[40, 110]`.
- **Placement:** Center `x=320`, baseline `y=241` (feet at `y=240`). Perfectly grounded on top of the battle box.
- **Rows Verified at 2×:**
  - `furiell_battle_row0_idle.png`: Row 0 `idle` (2 frames, 2 fps) — cardigan, glasses glint, lanyard, and holstered spray bottle clearly readable.
  - `furiell_battle_row1_hurt.png`: Row 1 `hurt` (1 frame) — flinch reaction.
  - `furiell_battle_row2_sad.png`: Row 2 `sad` (1 frame) — spare / remorse pose.

### 2.3 Furiell Overworld (`assets/sprites/ow/furiell.png` + `furiell.json`)
- **Format:** Cell `[32, 44]`, origin `[16, 43]`.
- **Animations Verified at 2×:**
  - Walk cycles: `furiell_ow_walk_down.png` (row 0), `furiell_ow_walk_up.png` (row 1), `furiell_ow_walk_left.png` (row 2). Walk animation is fluid and correctly proportioned against Grub.
  - Spritz actions: `furiell_ow_spritz_left.png` (row 5) and `furiell_ow_spritz_right.png` (row 6) — raised spray bottle with lavender spray nozzle clearly depicted.
  - Hand-holding: `furiell_ow_hold_walk_down.png` (row 7), `furiell_ow_hold_walk_up.png` (row 8), `furiell_ow_hold_walk_left.png` (row 9) — outstretched paw leading Grub forward.

### 2.4 Floofy Overworld `vanity_back` (`assets/sprites/ow/floofy.png`, row 6)
- **Check:** `floofy_ow_r2_vanity_back.png` captured in R2 (`ruins_floofy`) facing the vanity table.
- **Anatomy:** Redrawn fox anatomy (pointed fox ears, back of head with thorn crown, seated posture) completely resolves the former "pink rectangular cushion" defect.

### 2.5 Landing Background (`assets/tiles/landing_bg.png`)
- **Check:** `landing_bg_r1.png` captured in R1 (`ruins_fall`).
- **Contrast & Tone:** Walls and floor are desaturated, creating a moody, dim entrance cave. Background trash has subdued outlines, allowing the illuminated flower bed and Save point to stand out.

### 2.6 Deodorant Banner & Chip Pile (`assets/props/`)
- **Deodorant Banner:** `ruins_r3_deodorant_chippile.png` in R3 (`ruins_entrance`). Text `DEODORANT IS / NOT OPTIONAL` stamped cleanly in 3×5 font on a yellow background; 100% legible at 2×.
- **Chip Wrapper Pile:** `ruins_r3_deodorant_chippile.png` (R3) and `ruins_barky_chippile.png` (R8). Rendered as an examineable floor pile.

### 2.7 Lore Cards 2 & 9 (`assets/img/lore/`)
- **Card 02 (`lore_card02.png`):** Features the charismatic fursuit wolf with the bone and human gamer Grub in hoodie/headset with keyboard shield. Clean 2× rendering centered on screen during intro crawl.
- **Card 09 (`lore_card09.png`):** Shows Grub's pale leg and flip-flop slipping on the Tide Pod pickup, triggering the comic slip sound effect. Clean 2× rendering.

---

## 3. Notes & Recommendations for Art Backlog

1. **Chip Wrapper Pile (`ruins_wrapper_pile.png`):**
   - The current graphic reads mostly as loose triangular nachos/Doritos rather than crumpled commercial foil bags. While fully functional and acceptable in context, if higher fidelity is desired in a future art pass, a redraw emphasizing crushed foil bag wrinkles and logo fragments is recommended.
2. **Floofy Sinister & Soaked Battle Rows:**
   - Both rows are acceptable after the macro-pixel retouch. Outlines and proportions match density 1.
3. **No C++ Sprite Placement Constant Changes Needed:**
   - With `ENEMY_BASELINE_Y = 241` and sheet origins set to `[width / 2, height]`, all enemy sprites (Floofy 60×80, Furiell 80×110, Barky 60×80) naturally rest with feet at `y=240..241`, floating 9 px above the text box border (`y=250`).
