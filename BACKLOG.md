# nUndertale — Backlog: 1:1 Undertale fidelity (visuals, gameplay and feel) + LORE v2 content

> **Generated on** 2026-10-16 from a full read of the C++20/Raylib code (`src/`, `include/`), `ROTEIRO.md`, `README.md`, and a comparison between a current screenshot and an Undertale battle screenshot (Loox, Vegetoid and Migosp).
>
> **Revised on** 2026-10-16 against [`docs/LORE.md`](docs/LORE.md) **v2 (canonical)**. When this backlog conflicts with the LORE, **the LORE wins** — and it also overrides `ROTEIRO.md`, `notes.txt` and `LoreScene.cpp`. `LORE §N` references point to its sections.
>
> **Language:** this backlog and the LORE are written in **English**: they are development references, the game ships English-only (D-03), and the jokes are native English internet culture. Quoted in-game text is final copy unless marked *draft*.
>
> **Goal:** the parody should be practically indistinguishable from Undertale in layout, proportions, timings, key sounds and "feel of control," swapping only the **content** (characters, text, jokes, music and original sprites) for nUndertale's script.
>
> **Content scope (LORE §11):** a **~25–30 min** Gamer route (speedrun 15–18 min), a ~28–32 min Furry route, a ~30–35 min Toxic route. Five areas (Ruins of Con '14, Snowdinha, Wetfall, Hotland Data Center, Con Center) + the Furry route's **The Raid** act.
>
> **Status as of 2026-10-16:** the foundation (M0) and a good chunk of the battle vertical slice (M1) are implemented (see the `[x]` items). **All LORE v2 narrative content is still to do.** Each item is self-contained: what to do, why, where to change things and how to know it's done.

### Revision changelog (LORE v2)

| Type | What changed |
| :--- | :--- |
| **Language** | The game is **100% English**, including all on-screen lore; PT-BR localization (`L10N-001`, `L10N-002`) is dropped. LORE and backlog were rewritten in English; the remaining copy pass is `NAR-031`. |
| **Decisions closed by the LORE** | `D-01` (red soul + cosmetic frog soul), `D-03` (English only), `D-05` (persistent FUR; 100% opens the Mirror), `D-06` (Barky mini-boss, Furiell boss), `D-07` (Gamer / Toxic / Furry routes). New: `D-11` (Toxic cap) and `D-13` (mosaic censorship) closed; `D-12` (raid clock tuning), `D-14` (real brands) and `D-15` (FUR outside the overworld) open. |
| **New epics** | `FLF` (Furry route: Mirror, FLUFF, SHAME, BONK), `RAD` (raid clock), `SHP` (shops and economy), `END` (endings, epilogues, credits), `MET` (meta-narrative and title screens), `LOR` (content-rule compliance). |
| **Rewritten epics** | `FUR`, `NAR`, `ART`, `AUD`, roadmap and appendices. The old `C3+` was replaced by `C3` Wetfall, `C4` Hotland, `C5` Con Center, `C6` The Raid act and `C7` Toxic route; `C1` and `C2` were rebuilt room by room from LORE §12–13. |
| **Done items needing rework** | `ACT-002`, `BHL-014`, `BND-004`, `BND-010`, `FUR-003`, `MNU-003`, `PRG-020`, `PRG-030` went back to `[~]` with the note **"LORE rework."** Code-level divergences became `BUG-18`..`BUG-30` (§4.1). |
| **Dropped by the LORE** (`[-]`) | Weebfall and weebs as a faction, Undyne-chan, Mettatwitch, Spider Discord Server/Muffet and the purple soul (`BHL-013`), Core Overclock, Trollowey, the NFT Bake Sale, Frogposter, Soytoid, Hat Kid, Greater Doge, the "raid leader" game-over voice, the Wholesome reconciliation ending, Firewall/Gamertags/WASD keycaps, the "Midwest FurFest" banners (a real convention), the `GABEN` easter egg (a real person, LORE §2.2), the dimensional box, PT-BR localization. |

---

## Contents

0. [How to use this backlog](#0-how-to-use-this-backlog)
1. [Diagnosis: current screenshot vs Undertale](#1-diagnosis-current-screenshot-vs-undertale)
2. [Reference spec sheet (Undertale measurements)](#2-reference-spec-sheet-undertale-measurements)
3. [Decisions (open and closed)](#3-decisions-open-and-closed)
4. [Bugs found in the current code](#4-bugs-found-in-the-current-code) · [4.1 Divergences from the LORE](#41-divergences-between-the-current-code-and-lore-v2)
5. [Epics and items](#5-epics-and-items)
   - **Engine and 1:1 systems**
     - [REPO — Repository hygiene and build](#repo--repository-hygiene-and-build)
     - [CORE — Engine, loop and scenes](#core--engine-loop-and-scenes)
     - [GFX — Rendering, fonts and visual style](#gfx--rendering-fonts-and-visual-style)
     - [TXT — Text and dialogue system](#txt--text-and-dialogue-system)
     - [BUI — Battle: layout and UI](#bui--battle-layout-and-ui)
     - [BFL — Battle: flow and state machine](#bfl--battle-flow-and-state-machine)
     - [FGT — Battle: FIGHT](#fgt--battle-fight)
     - [ACT — Battle: ACT](#act--battle-act)
     - [ITM — Battle: ITEM](#itm--battle-item)
     - [MRC — Battle: MERCY / Flee](#mrc--battle-mercy--flee)
     - [BHL — Battle: enemy turn and bullet hell](#bhl--battle-enemy-turn-and-bullet-hell)
     - [BND — Battle: endings (victory, dust, level up, game over)](#bnd--battle-endings-victory-dust-level-up-game-over)
     - [ENC — Encounters and battle transition](#enc--encounters-and-battle-transition)
     - [OVW — Overworld](#ovw--overworld)
     - [MNU — Overworld menu (ITEM / STAT / CELL)](#mnu--overworld-menu-item--stat--cell)
     - [SAV — Save point, save/load and meta-persistence](#sav--save-point-saveload-and-meta-persistence)
     - [PRG — Progression, equipment and routes](#prg--progression-equipment-and-routes)
     - [FRT — Front-end (disclaimer, intro, title, name, settings)](#frt--front-end-disclaimer-intro-title-name-settings)
     - [AUD — Audio](#aud--audio)
   - **nUndertale-only mechanics (LORE §6–8)**
     - [FUR — Furrification (FUR meter and stages)](#fur--furrification-fur-meter-and-stages)
     - [FLF — Furry route: Mirror, FLUFF, SHAME and BONK](#flf--furry-route-mirror-fluff-shame-and-bonk)
     - [RAD — The Raid Clock](#rad--the-raid-clock)
     - [SHP — Shops and economy](#shp--shops-and-economy)
     - [END — Endings, epilogues and credits](#end--endings-epilogues-and-credits)
     - [MET — Meta-narrative and title screens](#met--meta-narrative-and-title-screens)
   - **Narrative and content**
     - [LOR — Content-rule compliance](#lor--content-rule-compliance)
     - [NAR — Script and writing](#nar--script-and-writing)
     - [C1 — Area 1: The Ruins of Con '14](#c1--area-1-the-ruins-of-con-14)
     - [C2 — Area 2: Snowdinha](#c2--area-2-snowdinha)
     - [C3 — Area 3: Wetfall](#c3--area-3-wetfall)
     - [C4 — Area 4: Hotland Data Center](#c4--area-4-hotland-data-center)
     - [C5 — Area 5: The Con Center](#c5--area-5-the-con-center)
     - [C6 — Furry route and "The Raid" act](#c6--furry-route-and-the-raid-act)
     - [C7 — Toxic Gamer route](#c7--toxic-gamer-route)
   - **Production**
     - [ART — Art (placeholders and final assets)](#art--art-placeholders-and-final-assets)
     - [L10N — Language (English only)](#l10n--language-english-only)
     - [QA — Debug tools, tests and CI](#qa--debug-tools-tests-and-ci)
     - [DST — Distribution](#dst--distribution)
6. [Roadmap by milestone](#6-roadmap-by-milestone)
7. [Appendices](#7-appendices) (enemy schema, SFX, affected files, **LORE → items traceability**)

---

## 0. How to use this backlog

### Item format

```
### <ID> — <title>
Priority · Size · Depends on
What / Why / Details / Where / Acceptance criteria
```

| Field | Values |
| :--- | :--- |
| **Priority** | **P0** = without it, it doesn't feel like Undertale (blocks the vertical slice) · **P1** = important fidelity · **P2** = polish/fine feel · **P3** = nice to have/long term |
| **Size** | **S** ≤ 2h · **M** ≈ half a day to 1 day · **L** 2–4 days · **XL** ≥ 1 week (break it down before starting) |
| **Status** | `[ ]` to do · `[~]` in progress, or done but with **LORE rework** pending · `[x]` done · `[!]` blocked (note why) · `[-]` dropped/merged (note where) |
| **Content priority** | In art/audio items, `P0→P2` means the item mixes priorities; each row's priority is in the item's table (following LORE §26: P0 = playable Ruins, P1 = rest of the Gamer route, P2 = Furry/Toxic routes and optional content). |

### General rules for implementers

1. **Pixel measurements are on the 640×480 virtual canvas** (`VIRTUAL_WIDTH/HEIGHT` in `include/core/Constants.hpp`), which is Undertale's real resolution.
2. **All pixel art is drawn at an integer 2× scale** (a 20×20 native sprite becomes 40×40 on screen). No character, bullet or icon should be drawn with primitives (`DrawCircle`, `DrawTriangle`) in the final version — primitives are debug placeholders only.
3. **Undertale assets can't be copied** (sprites, music, sounds and proprietary fonts). "1:1" means **same layout, proportion, timing and feel**, with original art and sound made in the same style. See `D-04`.
4. Values marked **(confirm)** come from memory/community documentation and must be checked against reference gameplay (30 fps recording, frame by frame) before becoming constants.
5. When an item is done, mark it `[x]`, note the commit/PR and update `README.md` if the feature is player-visible.
6. **Content follows [`docs/LORE.md`](docs/LORE.md)**: text, names, enemy and item numbers come from there. If something must change, change the LORE first (or log it in `NAR-030`), then the data. All text and art goes through the content checklist (`LOR-001`).
7. **All player-visible text is English** (D-03) — dialogue, signs, examinables, menus, UI, on-screen lore, epilogues and credits. Text inside art (drawn signs, banners, PC screens) is English too. Final strings live in `data/dialogue/en/`.

---

## 1. Diagnosis: current screenshot vs Undertale

A direct comparison of the two screenshots (the current one is the Barky fight; the reference is "Loox and co. decided to pick on you!").

| # | Element | Undertale (reference) | nUndertale today | Items |
| :-- | :--- | :--- | :--- | :--- |
| 1 | **Battle background** | Green grid of 1 px lines (#27AF4B) covering the top area | Plain black | `BUI-002` |
| 2 | **Enemy sprite** | White monochrome pixel art on black, 2×, feet at y≈241, with an idle animation | Gray/bluish primitives (circle, triangles, rectangle), static | `ART-010`, `BUI-003` |
| 3 | **Enemy name/HP** | Not shown permanently; name only in the target list (with an HP bar in FIGHT) | "Barky HP: 30 / 30" fixed under the enemy | `BUI-004` |
| 4 | **Text box** | 575×140 px, from (32,250) to (606,389), 5 px white border, full width | 200×130 centered, 4 px border | `BUI-001` |
| 5 | **Box text** | Large pixel mono font (~32 px line), "* " with a hanging indent on line 2, typed out with sound | Small font, text overflows the border (no wrapping) | `TXT-001`, `TXT-002`, `GFX-003` |
| 6 | **HUD** | `NAME  LV n  HP [bar] x / y` in distinct fonts, tiny "HP" label, bar width = HPmax×1.2 | All one font, "FURRY: 0%" in the middle, fixed 60 px bar at x=320 that can overlap the text | `BUI-005`, `FUR-003` |
| 7 | **Buttons** | 4 sprites 110×42 with icon + text (sword, heart, bag, X), orange #FF7F27; selected turns yellow #FFFF00 and the icon becomes the red soul | Rectangles with a border and text, orange #FFA500 (too light), cursor is a green dot inside the button | `BUI-006`, `ART-020` |
| 8 | **Button positions** | x = 32, 185, 345, 500; y = 432 | x = 60, 200, 340, 480; y = 418 | `BUI-006` |
| 9 | **Soul** | 16×16 red heart sprite | Green heart built from circles + a triangle | `BUI-007`, `D-01` |
| 10 | **Frame rate / feel** | Logic and render at 30 fps, movement in px/frame | 60 fps with variable `dt` | `CORE-001` |
| 11 | **Multiple encounters** | Up to 3 enemies ("X and co.") | 1 fixed enemy | `BFL-004` |
| 12 | **Turn flow** | Text → choice → enemy speech bubble → box shrinks → attack → box returns → "flavor" text | Message → enemy turn directly, with an overlapping bubble, the box never resizes | `BFL-001`..`BFL-003`, `BHL-001` |
| 13 | **Overworld** | No HUD, following camera, rooms bigger than the screen, C menu | Fixed HUD at the bottom with key hints, a single screen-sized room | `OVW-001`, `OVW-002`, `MNU-*` |

> **Status as of 2026-10-16:** rows 1, 3–10 and the HUD/C-menu part of 13 are resolved (commits `b4401be`..`35e6b01`). Still open: **2** (sprites still primitives → `ART-010`, `GFX-002`), **11** (multi-enemy → `BFL-004`), **12** (full state machine → `BFL-001`) and the rooms/camera part of **13** (`OVW-002`, `GFX-010`).

---

## 2. Reference spec sheet (Undertale measurements)

> Measured pixel by pixel on the 640×480 reference screenshot (Loox/Vegetoid/Migosp). Values without "(confirm)" were **measured**.

### 2.1 Battle — geometry

| Element | Value |
| :--- | :--- |
| Box (text/menu state), outer border | x 32→606, y 250→389 (575×140) |
| Box border thickness | 5 px, white (#FFFFFF) |
| Usable box interior | x 37→601, y 255→384 |
| Text: start of line 1 | x = 52 (the "*"), glyph top at y ≈ 276 |
| Line height | 32 px (lines at y≈276 and y≈308) |
| Line 2 indent | aligned with the first character after "* " (hanging indent) |
| Background grid: horizontal lines | y = 9, 126, 244 (1 px) |
| Background grid: vertical lines | x = 15, 115, 218, 317, 420, 523, 623 (1 px) |
| Grid color | #27AF4B (39,175,75) |
| Enemy sprites | feet at y ≈ 241, top of an average enemy at y ≈ 126 (≈115 px tall at 2×) |
| HUD: baseline | glyphs from y ≈ 400 to 419 |
| HUD: name | x = 30 |
| HUD: "LV" | x = 132 (number at x ≈ 171) |
| HUD: "HP" label | x = 244→265 (small font, sprite) |
| HUD: HP bar | x = 275, y = 400, 20 px tall, width = `HPmax × 1.2` (24 HP → 29 px) |
| HUD: bar colors | red background #FF0000, yellow fill #FFFF00 proportional to current HP |
| HUD: "16 / 24" numbers | start ≈ 18 px after the end of the bar (x ≈ 322 for HPmax 24) |
| Buttons (4) | x = 32, 185, 345, 500 · y = 432 · 110×42 · 2 px border |
| Buttons — colors | normal #FF7F27 (measured #FF8124) · selected #FFFF00 |
| Selected button | the icon (sword/heart/bag/X) is replaced by the red soul |

### 2.2 Battle — behavior (confirm)

| Element | Reference value |
| :--- | :--- |
| Frame rate | 30 fps (logic and drawing) |
| Soul | 16×16, red #FF0000, speed 4 px/frame (120 px/s) (confirm), hitbox smaller than the sprite (~8×8 center) (confirm) |
| Invincibility after damage | ~30 frames (1 s) with flashing (confirm), extended by some armor |
| Default attack box | ~165×140 centered on x, bottom aligned with the big box's bottom (confirm per attack pattern) |
| Box resize | animated, ~6–10 frames, linear interpolation of the 4 edges (confirm) |
| Player damage formula (community) | `damage = round((AT + weapon_AT − enemy_DF + rand(0..2)) × mult)`; `mult = 2.2` if `|distance from center| ≤ 12`, else `(1 − |distance|/273) × 2` (confirm) |
| Max HP by LV | `HPmax = 16 + 4·LV` (LV1 = 20 … LV19 = 92, LV20 = 99) |
| EXP table to level up | 10, 30, 70, 120, 200, 300, 500, 800, 1200, 1700, 2500, 3500, 5000, 7000, 10000, 15000, 25000, 50000, 99999 |
| Battle text | 1 character/frame (30 cps) with a per-character "voice" sound (confirm) |

### 2.3 Fonts used by Undertale (to pick equivalents)

| Use | Original font | Suggested equivalent |
| :--- | :--- | :--- |
| Main text (boxes, menus) | DTM-Mono ("Determination Mono") | **Determination Mono** (free fan font; check the license) or keep `8bitoperator.ttf` as a fallback |
| HUD (name, LV, numbers) | "Mars Needs Cunnilingus" (pixel) | check the license; alternative: our own 2× bitmap font |
| Damage numbers | "Hachicro" | our own bitmap font (digits 0–9 + "MISS") |
| "HP"/"KR" labels, tiny text | sprites / "Crypt of Tomorrow" | our own sprite |
| Enemy speech bubble | small DotumChe-style font (confirm) | small pixel font, ~12 px native |
| Title | Monster Friend | `monster_friend.otf` / `monster_friend_fore.otf` (already in the repo) |
| Senpai Sans | Comic Sans | a free "comic" font (e.g. Comic Neue) in lowercase |
| Papyrus Streamer | Papyrus | a free papyrus-style font, always UPPERCASE |

---

## 3. Decisions (open and closed)

Each decision has a **recommendation**. The ones closed by the LORE are kept here for traceability; open ones need an answer before their dependent items.

| ID | Topic | State | Resolution / recommendation | Blocks |
| :--- | :--- | :---: | :--- | :--- |
| D-01 | Soul color | ✅ closed (LORE §3) | Default red; name `PEPE` or `FROG` → frog-shaped soul (cosmetic). | `BUI-007`, `BHL-010`+ |
| D-02 | Logic frame rate | ✅ implemented | Fixed 30 Hz (`CORE-001`). | — |
| D-03 | Language | ✅ closed (LORE header; reaffirmed by the user on 2026-10-16) | **The whole game is English**, including all on-screen lore (intro cards, Hall of Posts, Librarby books, stained glass, jar labels, CELL, epilogues, credits). **No localization planned** (PT-BR dropped). The design docs (LORE, backlog) are in English too. | `TXT-010`, `NAR-031`, `L10N-*` |
| D-04 | Asset licensing | ⏳ open | See below. | `AUD-020`, `ART-000`, `DST-003` |
| D-05 | FUR semantics | ✅ closed (LORE §8.4–8.5) | Persistent, 0–100, with 5 visual stages; **100% opens the Mirror** (not a game over); HP 0 = FURRY OVER and back to the save with the saved FUR. | `FUR-*` |
| D-06 | Ruins boss | ✅ closed (LORE §3) | Barky = mini-boss (R9); Furiell = boss (R12). | `C1-*` |
| D-07 | Routes and LV/EXP | ✅ closed (LORE §3, §22.0) | LV = **Level of Vitriol**, EXP = **eXcessive Posting**. Routes: **Gamer** (with the no-kill **Clean Run** variant), **Toxic Gamer**, **Furry**. "Wholesome"/"Normie" retired. | `PRG-031`, `END-*` |
| D-08 | Map format | ⏳ open | Tiled (`.tmx` → `.json`). | `OVW-003` |
| D-09 | Pattern/cutscene scripting | ⏳ open | See below (the LORE names ~70 patterns). | `BHL-004`, `CORE-020` |
| D-10 | Running in the overworld | ⏳ open | Remove it (Undertale has no running); accessibility option off by default. | `OVW-006` |
| D-11 | Toxic route kill cap | ✅ closed (LORE §3, §22.0) | Ruins **6**, Snowdinha **6**, Wetfall **5**, Hotland **3** (random encounters only) **and** kill Barky, Furiell, Papyrus and Sharkdyne. Replaces the cap of 20 in `flags.md`. | `ENC-004`, `PRG-031`, `C7-*` |
| D-12 | Raid clock tuning | ⏳ open | See below. | `RAD-*`, `END-006` |
| D-13 | What "explicit" looks like | ✅ closed (LORE §2.2, §25.4) | The **only** allowed visual is mosaic censorship (8×8 blocks of random colors) and closed objects (tab, folder, curtain). | `GFX-012`, `LOR-*` |
| D-14 | Real brands in the text | ⏳ open | See below. | `DST-001` |
| D-15 | FUR stages outside the overworld | ⏳ open | See below. | `FUR-007` |

### D-04 — Asset licensing (open)
- **Context:** the repository is public (IaKee account) and contains `assets/music/Undertale - Once Upon A Time Remix (ft Yuri, zR).ogg` and sounds of unknown origin (`u1..u12.ogg`, `sans_type.wav`).
- **The LORE adds (§26.1, §27):** placeholders use **the final file name** and are logged in `assets/PLACEHOLDERS.md` (file, source, license); Undertale rips only as reference in an ignored folder (`assets/_ref/`); final compositions are **original** "in the style of"; never ship the original Comic Sans/Papyrus.
- **Recommendation:** `assets/LICENSES.md` + `assets/PLACEHOLDERS.md`; replace everything ripped before any public release (`AUD-020`, `ART-000`, `DST-003`).

### D-09 — Attack pattern and cutscene scripting (open)
- **Context:** the original recommendation was "C++ classes now, reconsider Lua above 30 patterns." LORE §18–19 already names **~70 patterns** (`colon_three_hop`, `sorry_moths`, `doge_words`, `musk_cloud`, `file_load`…).
- **Recommendation:** stay in C++, but with **~12 parameterized generic patterns** (rain, bounce, ring, wave, sweep, text-as-bullet, persistent zone, homing, shield arrows, blue/orange bones, blue-soul platforms, yellow-soul cursor) that cover most of them through JSON; dedicated classes only for the unique ones (`file_load`, `mad_daki_reflect`, `homework_folder`). Reconsider Lua (`CORE-020`) only if more than 15 patterns need their own class.

### D-12 — Raid Clock tuning (open)
- **Context:** LORE §8.7 says the clock starts at **30:00** in R1, counts real play time, and is "tuned so the average player runs out of time around the Con Center." But per the LORE §11.2 table, the player reaches R1 at ~2:30 and the Con Center at ~24:30: only **~22 min** of clock. At 30:00, the average player finishes with ~2 min to spare and never sees "Kicked" in the Con Center.
- **Options:** (a) start at **22:00** (text `RAID STARTS IN 22:00`), keeping the "runs out at the Con Center" rule; (b) keep 30:00 and accept that only completionists get kicked; (c) keep 30:00 and count faster (not diegetic).
- **Recommendation:** **(a)**, with the starting value in data (`data/raid.json: startSeconds`) tuned in playtest (`QA-020`) and the threshold messages (15:00, 5:00) recalculated. The **speedrun** threshold (> 10:00 left) still applies: a 15–18 min speedrun leaves ~8–10 min with (a) → reconsider "> 6:00".

### D-14 — Real brands in the text (open)
- **Context:** LORE §2.2 forbids real people, events, sites and conventions (hence FurNation Con '14), but the text uses real brands (Mountain Dew, Doritos, Discord, Minecraft, Comic Sans).
- **Recommendation:** keep them as parody during development; list occurrences in `docs/brands.md` (`LOR-006`) and decide before `DST-001` whether the public build swaps them for variations ("Mtn. Doo", "Nachitos").

### D-15 — Where FUR stages show up outside the overworld (open)
- **Context:** LORE §8.4 says the stages show up in "overworld sprites, portrait and battle," but Pepe **doesn't speak** (no portrait, LORE §10.1) and, like in Undertale, has no battle sprite.
- **Recommendation:** overworld (5 spritesheets) + **an accessory on the red soul** in battle (little ears ≥ 20, tiny tail ≥ 60; cosmetic only, the hitbox doesn't change) + the stage written in STAT. A Pepe portrait only for the Mirror reflection (`pepe_pawsome_reflection`).

---

## 4. Bugs found in the current code

> Fix these before or alongside the fidelity items, since several of them change the battle flow.

| ID | File | Bug | Suggested fix |
| :--- | :--- | :--- | :--- |
| **BUG-01** | `src/core/Game.cpp:43` / `SceneManager::pushScene` | The first scene enters through `pushScene`, which **doesn't call `onEnter`**, so `DisclaimerScene::onEnter` (sound + reset) never runs. | **[x] Fixed:** `pushScene` and `popScene` now call `onEnter`, `onPause`, `onResume` and `onExit` with full lifecycle management. |
| **BUG-02** | `src/combat/BattleSystem.cpp` (`DialogMessage` state) | If the enemy is spareable after an ACT, pressing Z **wins the fight by itself**, without using MERCY → Spare. | **[x] Fixed:** `DialogMessage` only ends the battle if the enemy died or was actually spared via MERCY (`m_isSpared`). |
| **BUG-03** | `src/scenes/BattleScene.cpp` | Defeat (`Defeat`) goes back to `OverworldScene` with 0 HP; there's no game over. | **[x] Fixed:** direct transition to `GameOverScene` with soul break, determination and save reload (`BND-010`). |
| **BUG-04** | `BattleScene.cpp` / `OverworldScene` | Returning from battle **recreates `OverworldScene`**: Pepe goes back to the starting position, Barky is still there and the fight can repeat. | **[x] Fixed:** the battle is pushed with `pushScene` and popped with `popScene`, preserving overworld state and position. |
| **BUG-05** | `BattleSystem::handleItemInput` | Always consumes the **last** item, with no selection list; any item other than "Mt. Dew" falls into the Doritos branch. | **[x] Fixed:** 2-column × 2-row ITEM menu with paging (`PAGE 1/2`), grid navigation and per-item effects (`ITM-001`, `ITM-002`). |
| **BUG-06** | `BattleSystem::handleActInput` | The hardcoded Check text ("ATK 5 DEF 2") doesn't match `Enemy("Barky", 30, 4, 2)`; ACTs are compared by string literal. | **[x] Fixed:** data-driven ACTs (`ActDef`), Check with the instance's real stats (`ACT-001`, `ACT-002`). |
| **BUG-07** | `BattleSystem::updateEnemyTurn` | Fixed damage of 3 and +8% furry; the enemy's `m_atk`/`m_def` are never used. | **[x] Fixed:** per-bullet damage + enemy AT − player DF (`BHL-005`). |
| **BUG-08** | `BattleSystem::handleFightTargetInput` | Uses `GetFrameTime()` instead of the `dt` it receives. | **[x] Fixed:** receives the fixed `dt` from the 30 Hz logic (`CORE-001`). |
| **BUG-09** | `BattleSystem::render` | Narrator text doesn't wrap and spills out of the box (visible in the screenshot). | **[x] Fixed:** automatic wrapping with a hanging indent for "* " (`TXT-002`). |
| **BUG-10** | `include/combat/Bullet.hpp` | Collision uses an AABB from `radius` even for the "UWU" text bullet (which is much bigger); bullets aren't removed when they leave the screen and are drawn outside the box. | **[x] Fixed:** per-type hitboxes (UwU, SodaCan, Bone) + scissor clipping to the box interior + off-screen culling (`BHL-002`, `BHL-003`). |
| **BUG-11** | `src/ui/DialogBox.cpp` | **Z completes the text** while typing. In Undertale, Z only advances after the page finishes; **X** is the skip. | **[x] Fixed:** only X (Cancel) skips typing; Z advances completed pages (`TXT-004`). |
| **BUG-12** | `BattleSystem::render` (HUD) | The HP bar is fixed at x=320 and overlaps the text if the name is long. | **[x] Fixed:** 1:1 layout at the measured positions (section 2.1) with a proportional bar (`BUI-005`). |
| **BUG-13** | `src/services/AudioService.cpp` | Master volume is applied twice (`SetMasterVolume` + multiplication in `playSound`). | **[x] Fixed:** removed the double multiplication in `playSound`. |
| **BUG-14** | `OverworldScene::update` | ESC goes back to the title instantly, without confirmation, losing progress. | **[x] Fixed:** removed the instant return; holding ESC for 1 s quits the game globally with visual feedback (`CORE-006`). |
| **BUG-15** | `ResourceService::getFont` | Fonts are loaded at one size and drawn at another (e.g. load 22 and draw 20), which makes pixels uneven with POINT filtering. | **[x] Fixed:** drawing at the native loaded size (`baseSize`), eliminating distortion with POINT filtering (`GFX-003`). |
| **BUG-16** | `BattleSystem` | EXP is gained but LV never goes up; the spare (+10) and kill (+15) gold is hardcoded. | **[x] Fixed:** official LV/EXP table added; VITRIOL increase, recalculated HP and heal on level up (`PRG-001`). |
| **BUG-17** | `BattleSystem::handleFightTargetInput` | The attack cursor always starts on the left. In Undertale, the side is random. | **[x] Fixed:** random entry from the left or right with matching directional speed (`FGT-002`). |

### 4.1 Divergences between the current code and LORE v2

> Not engine bugs: these are texts, names and rules implemented before LORE v2. Fix them along with the listed item.

| ID | File | Divergence | Fix (item) |
| :--- | :--- | :--- | :--- |
| **BUG-18** | `src/scenes/OverworldScene.cpp:164` | Hardcoded, non-canon encounter text `* Barky the Alpha Furry attacks!` (Barky is "the Alpha Wolf"; his entrance is ROTEIRO Scene 3 / LORE R9). | Move to data and use the LORE scene (`C1-009`, `TXT-010`). **[~] Partly fixed (2026-10-17):** Barky's ROTEIRO/LORE §19.1 entrance lines (speaker BARKY) replace the old text; still hardcoded until C1-009/TXT-010. |
| **BUG-19** | `src/scenes/OverworldScene.cpp:43` and `data/dialogue/en/ruins.json` (`ruins.sign.notice`) | Two different versions of the Ruins Administration notice, neither matching the canon (LORE R8: `Please do not shed in the ventilation shafts. Violators will be banned from the Minecraft server.`). | One key only, with the LORE text (`C1-008`). **[~] Partly fixed (2026-10-17):** The overworld sign uses the LORE R8 notice verbatim; `data/dialogue/en/ruins.json` still to align (TXT-010). |
| **BUG-20** | `src/scenes/GameOverScene.cpp:163-170` | `GAME OVER` logo and a shattering soul. LORE §23.1 calls for **FURRY OVER**: the soul grows ears and a tail, inflates and pops into pink pom-poms; Asgrr's voice. | `BND-010` (rework). |
| **BUG-21** | `src/combat/BattleSystem.cpp:531` | The narrator writes `* GAME OVER! You lost all HP and became a furry!` inside the box — Undertale has no such line; the sequence is the game over screen. | Remove; go straight to the sequence (`BND-010`). **[x] Fixed (2026-10-17):** The narrator line is gone; defeat goes straight to the game over scene. |
| **BUG-22** | `src/combat/BattleSystem.cpp:127` | Level-up says `* Your VITRIOL increased.`; LORE §8.2 fixes it as `* Your LEVEL OF VITRIOL increased.` | `BND-004` (rework). **[x] Fixed (2026-10-17):** `* Your LEVEL OF VITRIOL increased.` |
| **BUG-23** | `src/scenes/OverworldScene.cpp:403` | STAT shows `FURRY: n%`; the LORE uses the label **`FUR`** (and `FLF` on the Furry route). | `MNU-003`, `FUR-006`. **[x] Fixed (2026-10-17):** STAT shows `FUR: n` (FLF label pending with the Furry route). |
| **BUG-24** | `data/items/items.json` | Names `Frayed USB Optical Mouse` / `One-Sided Broken Gaming Headset` (LORE: `Frayed USB Mouse` / `One-Sided Broken Headset`); Duct Tape says "Furry meter … 15%" (LORE: `FUR −15`). The starting `Bottle` item is missing. | `ITM-006`, `ITM-007`. **[x] Fixed (2026-10-17):** Item names already matched; the default armor in `PlayerData` is now `One-Sided Broken Headset`. |
| **BUG-25** | `include/core/GameContext.hpp:64-75` | `RouteType { Wholesome, Normie, … }` — names retired by D-07; the Furry route and the Clean Run variant are missing. | `PRG-031`. |
| **BUG-26** | `data/flags.md` | `kills.ruins = 20` cap, `route.wholesome`/`route.normie` flags, and `meta.ever_transformed` described as "times it reached 100%" (LORE §23.1 uses it as the FURRY OVER count). | `PRG-021`. |
| **BUG-27** | `include/core/GameContext.hpp:29,44` + `src/services/SaveService.cpp:160-172` | Two sources of truth for FUR: `PlayerData::furryMeter` (float) and `FurryState::meter`, both saved. The LORE uses an integer `fur.value` 0–100. | `FUR-002`. **[x] Fixed (2026-10-17):** FUR-002: single integer `furry.value` with save migration. |
| **BUG-28** | `data/dialogue/en/ruins.json` (`ruins.intro.fall`) | Merges Pepe's wake-up thought and the save text into one line; in LORE R1 they're two things (a `* (...)` thought and the save point text). | `C1-001`. |
| **BUG-29** | `src/scenes/LoreScene.cpp:18-43` | Old intro cards ("FURRIES and GAMERS", "yiffing the GAMERS", "reddit posts"), 6 cards instead of LORE §9's 10. | `FRT-002`, `NAR-001`. **[x] Fixed (2026-10-17):** FRT-002: the 10 canonical cards. |
| **BUG-30** | `data/enemies/barky.json` | Missing the `Laser` (with Toy Laser Pointer) and `Accept Hug` ACTs, the 2nd `Cringe Check` response and the `howl_wave` pattern; the Check isn't the LORE §19.1 text. | `ACT-002`, `BHL-014`. **[~] Partly fixed (2026-10-17):** `Laser` (Toy Laser Pointer), `Accept Hug`, the 2nd `Cringe Check` and the LORE Check text are in; `howl_wave` pending (BHL-014). Also fixed: JSON strings are unescaped, so the Check no longer prints a literal backslash-n and a doubled header. |

---

## 5. Epics and items

### REPO — Repository hygiene and build

#### REPO-000 — Version the C++20 rewrite (urgent) `[x]`
P0 · S · REPO-001
- **Situation on 2026-10-16:** `git status` showed **the entire modern version outside Git**: `src/`, `include/`, `CMakeLists.txt`, `build.bat` and `ROTEIRO.md` were untracked; the assets had moved from `old/assets/` (pending deletions) to `assets/` (untracked); `README.md` was modified. The only copy of that code lived in the OneDrive folder.
- **What:** REPO-001 first (so `vendor/` at 743 MB and the `.exe`/`.o` files don't get versioned), then a "C++20 + Raylib rewrite" commit with `src/`, `include/`, the build scripts, the docs and the reorganized `assets/` (deleting `old/assets/` + adding `assets/` shows up as a rename), pushed with the personal account (`/iakee_push`).
- **Acceptance:** a fresh clone + vendor setup builds `nundertale_modern.exe`.

#### REPO-001 — Complete `.gitignore` `[x]`
P0 · S · —
- **What:** ignore `vendor/` (743 MB, not versioned but not ignored either), `*.exe`, `*.o`, `*.dll` (unless we decide to ship them), `build/`, `saves/`, `**/__pycache__`, `crash_reports/`, `*.psd` (or move to LFS, see REPO-004), and `assets/_ref/` (LORE §26.1).
- **Acceptance:** `git status` is clean after a full build.

#### REPO-002 — Remove the Python legacy from the root `[x]`
P1 · S · —
- **What:** `main.py`, `CONSTANTS.py`, `controller/`, `model/`, `view/`, `log/`, `test/*.py`, `crash_reports/` and the `__pycache__` folders belong to the old Python version. Move them to `old/legacy_python/` (following `old/legacy_c/`) or delete them.
- **Note:** the `old/assets/` → `assets/` reorganization wasn't committed yet (REPO-000). Do REPO-000 first and don't mix the two in one commit. `nundertale.c`, `nundertale.exe`, `nundertale.o` and `allegro-5.0.10-monolith-mt.dll` also sit loose in the root, duplicating `old/legacy_c/`.
- **Acceptance:** the root contains only `src/`, `include/`, `assets/`, `data/`, `config/`, `tools/`, the docs and the build scripts.

#### REPO-003 — A single build system `[x]`
P1 · M · —
- **Problem:** `build.bat` lists the `.cpp` files by hand; `CMakeLists.txt` uses `GLOB_RECURSE`. A new file forgotten in the `.bat` = a link error.
- **What:** make `build.bat` call CMake ("MinGW Makefiles" generator with `w64devkit`) **or** generate the list with `for /r src %%f in (*.cpp)`. Add `debug` (`-O0 -g -DNUNDERTALE_DEBUG`) and `release` (`-O2 -s -mwindows`) targets.
- **Acceptance:** `build.bat`, `build.bat debug` and `cmake --build` produce the same executable; adding a new `.cpp` doesn't require editing any script.

#### REPO-004 — Art sources out of `assets/` `[ ]`
P2 · S · —
- **What:** move `assets/Untitled-1.psd`, `assets/lore.psd` and `assets/img/deletethis/` to `art_src/` (with Git LFS) or delete them. `assets/` should only hold what the game loads.

#### REPO-005 — `vendor/` setup script `[ ]`
P1 · M · REPO-001
- **What:** `tools/setup_vendor.ps1`, which downloads w64devkit and raylib 6.2 (win64 mingw-w64) with checksums and extracts them into `vendor/`. Document it in the README.
- **Acceptance:** fresh clone + `tools/setup_vendor.ps1` + `build.bat` = the game runs.

#### REPO-006 — Zero warnings `[x]`
P2 · S · REPO-003
- **What:** compile with `-Wall -Wextra -Wpedantic -Wshadow -Wconversion` and fix what comes up (e.g. the unused `context` parameters in `OverworldScene`'s lambdas).
- **Done (2026-10-16):** all three `build.bat` targets now use `-Wall -Wextra -Wpedantic -Wshadow -Wconversion` (dropped `-Wno-unused-parameter`) and print zero warnings; so does `tests/test_runner.cpp` under the same flags. Fixes: unused parameters commented out, `(Color){…}` compound literals → `Color{…}`, explicit int→float casts.

#### REPO-007 — A `data/` folder for content `[x]`
P0 · S · —
- **What:** create `data/{enemies,encounters,items,rooms,dialogue/en,patterns,shops}` and a `data/README.md` explaining the schemas. Base for ACT-001, ITM-001, OVW-003 and TXT-010.

---

### CORE — Engine, loop and scenes

#### CORE-001 — Fixed 30 Hz logic loop `[x]`
P0 · M · D-02
- **Why:** it's what changes the "feel" the most. Undertale runs at 30 fps; speeds, i-frame durations, flashing and typing are all per frame.
- **What:** in `Game::run`, use an accumulator `while (acc >= 1/30) { update(1/30); acc -= 1/30; }`; `SetTargetFPS(30)` or render at 60 repeating the frame. Swap `dt` in seconds for `frames` where it makes sense (`constexpr int SOUL_SPEED = 4;`).
- **Where:** `src/core/Game.cpp`, `include/core/Constants.hpp`, every scene.
- **Acceptance:** with the window minimized or lagging, logic doesn't speed up or skip collisions; movement is identical at 30, 60 and 144 Hz.

#### CORE-002 — A real scene stack (push/pop with state) `[x]`
P0 · M · BUG-01, BUG-04
- **What:** `SceneManager` with `push(scene, transition)`, `pop(transition)`, `replace(...)`. The battle is **pushed** on top of the overworld (which stays paused and preserved) and popped at the end. `onPause/onResume` in `IScene`.
- **Acceptance:** after a battle, Pepe is exactly where he was and the defeated/spared NPC already reflects the result.

#### CORE-003 — Pluggable transition system `[~]`
P1 · M · CORE-002
- **What:** an `ITransition` interface with `FadeBlack(frames)`, `FadeWhite(frames)` (used on name confirmation), `Cut`, `EncounterFlash` (ENC-002) and `SoulBreak` (BND-010) implementations. `SceneManager` stops having the black fade built in. Add `PinkMirror` (FLF-001) and `FurryOverPomf` (BND-010).
- **Partly done (2026-10-17):** `ITransition` (`include/core/Transition.hpp`) with `FadeTransition` (`fadeBlack`, `fadeWhite`) and `CutTransition` (`cut`); `SceneManager::changeScene(scene, transition)` replaces the built-in black fade (the old `changeScene(scene, seconds)` still works and builds a black fade). Name confirmation fades to white. Still to do: `EncounterFlash`, `SoulBreak`, `PinkMirror`, `FurryOverPomf`.

#### CORE-004 — Serializable global `GameState` `[x]`
P0 · M · —
- **What:** split from `PlayerData` (in `GameContext.hpp`) a `GameState { PlayerStats, Inventory, Equipment, Flags (map<string,int>), RoomId, Position, PlayTime, KillsByArea, FurryState }`. Everything that goes into the save lives here. LORE v2 adds `fur`, `fluff`, `raid` and the route (SAV-007).
- **Acceptance:** `GameState` round-trips JSON → struct → JSON without loss (test in QA-010).

#### CORE-005 — F4 fullscreen and optional integer scaling `[x]`
P1 · S · —
- **What:** F4 toggles fullscreen (as in Undertale). "Integer scale" option (letterbox in multiples of 640×480) for always-square pixels; decorative border option (P3, like Undertale's PS4/Switch borders).

#### CORE-006 — "Hold ESC to quit" `[x]`
P1 · S · BUG-14
- **What:** holding ESC for ~1 s shows "QUITTING..." in the top-left corner with progressing dots; releasing cancels; completing closes the game. ESC no longer goes back to the title.

#### CORE-007 — User configuration `[x]`
P1 · S · —
- **What:** expand `config/user.json` with `musicVolume`, `sfxVolume`, `fullscreen`, `integerScale`, `keybinds`, `border`, saved in `%LOCALAPPDATA%/nUndertale/config.json` (not in the game folder). The `lang` field stays fixed at `en` or is removed (D-03).

#### CORE-008 — Key remapping and gamepad `[x]`
P2 · M · CORE-007
- **What:** `InputService` with abstract actions (`Confirm`, `Cancel`, `Menu`, directions) mapped to keys and gamepad (raylib `IsGamepadButtonPressed`). Defaults match Undertale: Z/Enter, X/Shift, C/Ctrl. Revisit using **Space** as confirm (Undertale doesn't) and **X as backspace** on the name screen.

#### CORE-009 — Per-frame input buffer `[x]`
P1 · S · CORE-001
- **What:** with logic at 30 Hz, an `IsKeyPressed` read between ticks can get lost. Accumulate "pressed" events between ticks and consume them on the next update.

#### CORE-010 — Events/flags system `[x]`
P0 · M · CORE-004
- **What:** `Flags::get/set/inc` with string keys (`"ruins.barky.spared"`, `"ruins.candy.taken"`), used by conditional dialogue, doors, NPCs and routes.

#### CORE-011 — Step-scripted cutscenes `[ ]`
P1 · L · CORE-010, TXT-001
- **What:** a sequence runner (`Cutscene { steps: [MoveNpc, Wait, Dialog, PlaySound, SetFlag, CameraPan, FadeOut, StartBattle] }`) loaded from JSON. Blocks player input while it runs. The LORE needs it in R2, R6, R7, R9, S1, S4, W1, H1, N3, the Mirror, the Furry-route scenes and every ending.
- **Acceptance:** Barky's entrance (the dramatic drop + line, ROTEIRO Scene 3) is done purely with data.

#### CORE-012 — Play time clock `[x]`
P1 · S · CORE-004
- **What:** count play time (shown on the save and Continue screens as `0:00`). The raid clock (RAD-001) runs on top of this.

#### CORE-020 — Evaluate Lua for patterns and cutscenes `[ ]`
P3 · L · D-09

---

### GFX — Rendering, fonts and visual style

#### GFX-001 — Official palette in `Constants.hpp` `[x]`
P0 · S · —
- **What:** fix/add: `UI_ORANGE = #FF7F27` (was `{255,165,0}`), `UI_YELLOW = #FFFF00`, `SOUL_RED = #FF0000`, `HP_RED = #FF0000`, `HP_YELLOW = #FFFF00`, `ENEMY_HP_GREEN = #00FF00`, `GRID_GREEN = #27AF4B`, `MISS_GRAY = #C0C0C0` (confirm), `SOUL_BLUE = #0026FF`, `SOUL_GREEN = #00C000`, `SOUL_YELLOW = #FFFF00`, `KR_PURPLE = #FF00FF`. LORE v2 adds `FLUFF_PINK = #FF77C8`, `FLUFF_BG = #3A1030`, `FLUFF_NUM = #FFB3E6`, `GRID_RED` (the gamer battles).

#### GFX-002 — Sprite and spritesheet pipeline `[ ]`
P0 · M · —
- **What:** `SpriteSheet` loaded from PNG + JSON (Aseprite format: frames, tags, per-frame duration). `AnimatedSprite` plays the tags. PNG with alpha replaces BMP + magenta (the current `pepe_char.bmp`).
- **Rule:** always draw at an integer scale (2× by default) at a position rounded to an integer.
- **Acceptance:** Pepe, the soul, the bullets and Barky come from spritesheets; no primitive draws a character.

#### GFX-003 — Fonts at native size and integer scale `[x]`
P0 · M · BUG-15
- **What:** load each font at its native pixel size (e.g. DTM Mono 16 px), render at 2× (32 px) with no filtering. Even better: bitmap fonts (BMFont `.fnt` or a PNG atlas) for the HUD, damage and buttons. The `ResourceService::drawText` API takes a `FontId` + an integer scale, never an arbitrary size.
- **Acceptance:** the text "* Barky blocked your path!" has glyphs whose pixels are all the same size (2×2), like the reference screenshot.

#### GFX-004 — Switch the main font to Determination Mono `[ ]`
P0 · S · GFX-003, D-04
- **What:** add `assets/fonts/DeterminationMono.ttf` (check the license), register it as `FontId::Main`. `8bitoperator` stays as the fallback. English-only (D-03) means ASCII plus a few symbols (`★`, `♥`, `—`, curly quotes) is enough.

#### GFX-005 — Character fonts `[ ]`
P1 · S · GFX-003
- **What:** `FontId::Sans` (comic style, lowercase) for Senpai Sans and `FontId::Papyrus` (UPPERCASE) for Papyrus Streamer, picked automatically by the dialogue "voice" (TXT-006).
- **LORE §26.9:** use **Comic Neue** (OFL) for Sans and a free "Papyrus-style" font; **don't** ship the original Comic Sans/Papyrus. Sharkdyne uses mixed case with lots of "!"; Floofy uses the default font and, in sinister mode, a thicker, shaky variant (GFX-009).

#### GFX-006 — Screen shake `[x]`
P1 · S · —
- **What:** `Camera::shake(intensity, frames)` applied to the virtual canvas. Used for: big soul damage, explosions, falls, Barky's drop and boss attacks.

#### GFX-007 — Screen flash and tint `[x]`
P2 · S · —
- **What:** a white/red overlay for N frames (encounter flash, special attacks, the Mirror's pink screen).

#### GFX-008 — Dissolve effect ("dust") `[x]`
P0 · M · GFX-002
- **What:** shader or CPU: the enemy sprite turns into dust line by line from top to bottom, with particles rising/scattering. Used for enemy deaths (BND-003). LORE v2 restyles it as plush stuffing + glitter (GFX-013).

#### GFX-009 — Shaky/wavy text `[x]`
P2 · S · TXT-001
- **What:** a mode where each glyph jitters ±1 px randomly per frame (name screen, scared lines, Floofy's sinister mode, the FUR bar's first appearance).

#### GFX-010 — 2D overworld camera `[ ]`
P0 · M · OVW-003
- **What:** the camera follows the player, centered and clamped to the room bounds; supports scripted pans (cutscenes) and rooms smaller than the screen (centered with black borders).

#### GFX-011 — Per-area palettes `[x]`
P1 · S · GFX-001
- **What:** LORE §25.2's palettes as constants/a data table (Ruins, Snowdinha, Wetfall, Hotland, desaturated Con Center, Barrier, Surface, Furry HUD), used by tilesets, background effects and the Continue screen.
- **Done (2026-10-16):** `include/core/AreaPalettes.hpp` — `Area` enum, `paletteFor(area)` / `areaFromId("ruins")`, every hex from LORE §25.2 (base, accent stops, highlight, and the hexes from the Notes column as `extra`). Unit-tested; not wired into rendering yet (tilesets/backgrounds pick it up with OVW/ART items).

#### GFX-012 — Mosaic censorship `[x]`
P1 · S · D-13
- **What:** a reusable component that draws 8×8 blocks of random colors (reshuffled every N frames) over a rectangle; used as a bullet (`mosaic_block` from `homework_folder`), as a joke overlay and in `excuse_text`. It's the **only** allowed visual for anything "explicit" (LORE §2.2).
- **Done (2026-10-16):** `Mosaic` (`include/ui/Mosaic.hpp`, `src/ui/Mosaic.cpp`): tiles a rectangle in N×N blocks (edge blocks clipped), reshuffles every N logic frames at 30 Hz, and takes a seed for deterministic colors (tests). Only `render()` touches raylib. Not used yet — `mosaic_block`/`excuse_text` come with BHL-004/BHL-018.

#### GFX-013 — LORE particle effects `[ ]`
P1 · M · GFX-008, ART-025
- **What (LORE §25.4):** **pomf** (a burst of fluff: FUR stage, FURRY OVER), **furry dust** (polyester + glitter instead of gray, on top of GFX-008; ex-gamers release a **red spark** that flies toward the castle), **horny jail** (a little pink cage that drops with a `BONK` and vanishes), **rage-quit** (the sprite flickers and becomes the text `[name] has left the game.`), ambient fluff particles (FUR-010).

#### GFX-014 — Visible stench and the Barrier `[ ]`
P2 · M · GFX-002
- **What:** wavy green (cartoon) lines coming off fursuits, Bottle Guy, the Moderator and the Barrier (LORE §25.1); the Barrier as an animated greenish-yellow `#B8FF4A` fog with the "no signal" spinner in the middle; heat distortion in Hotland.

---

### TXT — Text and dialogue system

#### TXT-001 — Text engine with markup `[ ]`
P0 · L · GFX-003
- **What:** a parser that turns a string into tokens (character, pause, color, voice, face, speed, line break, wait for input, end). Suggested syntax, close to Undertale's to make transcription easy:
  - `&` line break · `/` wait for Z and open a new page · `%` end
  - `^1`..`^9` pause of N×(≈ 3 frames)
  - `\R` `\Y` `\B` `\W` `\X` colors (red, yellow, blue, white, reset)
  - `\E0`..`\E9` change the portrait expression · `\V[name]` change the voice · `\S[n]` speed
  - `{shake}`/`{/shake}` shaky text · `{strike}`/`{/strike}` struck-through text (the narrator crossing out `excuse_text`, BHL-018)
- **Acceptance:** the same renderer is used in the overworld, in battle, in bubbles and on the save screen.

#### TXT-002 — Automatic wrapping with a hanging indent `[x]`
P0 · M · TXT-001
- **What:** wrap by pixel width of the usable area; when the line starts with "* ", the following lines indent to the column after "* " (see the screenshot: "pick on you!" aligned with "Loox").
- **Acceptance:** no text crosses the box border; the indent matches the reference screenshot.

#### TXT-003 — Typing speed and automatic pauses `[x]`
P0 · S · TXT-001, CORE-001
- **What:** default 1 character per frame (30 cps) (confirm), with extra automatic pauses after `.`, `!`, `?` and `,` (configurable per voice). Voice sounds play on every non-space character (some voices, every 2nd).

#### TXT-004 — Undertale-style text controls `[x]`
P0 · S · BUG-11
- **What:** **X** (cancel) completes the current page instantly; **Z** only advances once the page is done. No blinking indicator (Undertale has no triangle; remove the yellow triangle from `DialogBox`).

#### TXT-005 — Overworld dialogue box (position and portrait) `[x]`
P0 · M · TXT-001
- **What:** the box sits at the bottom by default and **at the top when the player is in the lower half of the screen**. Optional portrait on the left (2× sprite, e.g. Furiell, Senpai Sans) with the text shifted right. Measurements from an Undertale overworld screenshot (≈ x 32→608, y 320→470 at the bottom; y 10→160 at the top) (confirm).
- **Remove:** the yellow "* SPEAKER:" line (Undertale never shows who's talking; that's conveyed by voice/portrait/font).
- **LORE:** Pepe **doesn't speak** (faithful to Frisk) — no portrait or voice for him; his thoughts use TXT-011.

#### TXT-006 — Per-character voices `[x]`
P0 · M · TXT-001, AUD-002
- **What:** a `voices.json` registry: `{ "narrator": "snd_txt1", "furiell": "...", "sans": "...", "barky": "..." }` with optional pitch/variation. The narrator (`* ...`) uses the default beep. Full cast in AUD-012.

#### TXT-007 — Dialogue choices `[x]`
P0 · M · TXT-001
- **What:** a `{choice:Yes|No}` token shows the options with the red soul as the cursor (left/right); the result goes to a flag/callback. Used for: the save point, NPCs, Furiell ("Do you want to... ?"), shops, the **Mirror** (`Accept` / `Refuse`, FLF-001), the Repost Flower (whisper), the mod rock (ask permission).
- **Done (2026-10-16), ahead of TXT-001:** `DialogBox` strips a trailing `{choice:A|B[|C]}` token from a page (`parseChoiceToken`, unit-tested; malformed tokens stay plain text). Once the page is typed, the options show under the text with the red soul; left/right move, Z confirms and calls `setOnChoice(cb)` (set it *after* `startDialog`), X does nothing. `lastChoice()` keeps the result. No call site uses it yet; TXT-001's parser should absorb this token when it lands.

#### TXT-008 — Enemy speech bubble `[x]`
P0 · M · TXT-001
- **What:** a white bubble sprite with a tail (9-slice), black text in a small font, placed to the right of the speaking enemy, auto-sized to the text, typed with the enemy's voice; Z advances and closes it.
- **Where:** replaces the rectangle drawn in `BattleSystem::render`.

#### TXT-009 — Undertale-style "Check" text `[x]`
P1 · S · ACT-001
- **What:** format `* BARKY - ATK 4 DEF 2&* Description...`, name in uppercase and ATK/DEF read from data.

#### TXT-010 — All text comes from files `[ ]`
P0 · M · D-03, REPO-007
- **What:** zero game strings in the C++. Keys in `data/dialogue/en/*.json` (`"ruins.sign.notice": "..."`). Reuse and clean up `assets/lang/en.json` (inherited from Python).
- **LORE:** one file per area (`ruins`, `snowdinha`, `wetfall`, `hotland`, `concenter`, `raid`) + `system`, `cell`, `gameover`, `endings`, `meta`, `enemies/<id>`. Source text comes from the LORE (fixes BUG-18, BUG-19, BUG-21, BUG-22, BUG-28). English is the only language (D-03).

#### TXT-011 — Pepe's thoughts `[ ]`
P1 · S · TXT-001
- **What:** the `* (...)` format of Pepe's thoughts (LORE §10.1): narrator voice, no portrait; at FUR ≥ 80 they end with a "squeak" (FUR-008). Tag them in data as `speaker: "pepe_thought"` for the lint.

---

### BUI — Battle: layout and UI

#### BUI-001 — Battle box at official measurements `[x]`
P0 · S · GFX-001
- **What:** default `m_arenaRect` = outer border (32,250)–(606,389), 5 px white border, black interior. Represent the box by its **inner edges** (left/top/right/bottom) so it can animate resizing (BHL-001).
- **Where:** `include/combat/BattleSystem.hpp` (`m_arenaRect{220,240,200,130}` → a new `BattleBox`).
- **Acceptance:** overlaid on the reference screenshot (50% opacity), it lines up pixel for pixel.

#### BUI-002 — Green grid background `[x]`
P0 · S · GFX-001
- **What:** 1 px lines in #27AF4B: horizontal at y = 9, 126, 244 (from x 15 to 623) and vertical at x = 15, 115, 218, 317, 420, 523, 623 (from y 9 to 244). Draw it before the enemies. Configurable per encounter (`background: "grid" | "none" | "<sprite>"`): bosses use their own background or black (BUI-013).

#### BUI-003 — Positioning 1 to 3 enemies `[x]`
P0 · S · BFL-004
- **What:** enemies aligned by their **feet** at y ≈ 241; 1 enemy = center (x 320); 2 = ~x 213 and 427; 3 = ~x 110, 320, 530 (measured on the screenshot: Loox, Vegetoid, Migosp). Per-enemy offset in the data.

#### BUI-004 — Remove the permanent enemy HP `[x]`
P0 · S · —
- **What:** delete the fixed "Barky HP: 30 / 30". HP only shows up (a) in the FIGHT target list (green/red bar next to the name) and (b) after a hit (FGT-005).

#### BUI-005 — 1:1 HUD `[x]`
P0 · M · GFX-003
- **What:** draw each element at its measured position (section 2.1): name at x=30, "LV n" at x=132, the "HP" label (small sprite) at x=244, the bar at x=275 with width `HPmax×1.2` and height 20, the `hp / max` numbers after the bar. Name capped at 6 characters (as in Undertale).
- **Furry meter:** see FUR-003 (it goes in as Sans's "KR", not as loose text in the middle of the HUD). On the Furry route the whole HUD flips to FLUFF (FLF-004).
- **Acceptance:** with the name "CHARA", LV 2 and HP 16/24, overlaid on the reference screenshot, the elements line up (± font).

#### BUI-006 — Sprite buttons with icons `[x]`
P0 · M · ART-020
- **What:** 4 sprites (FIGHT sword, ACT "mouth"/heart, ITEM bag, MERCY X) in 2 states (orange/yellow), 110×42, at x = 32, 185, 345, 500, y = 432. When selected, the icon disappears and the red soul is drawn in its place.
- **Where:** `BattleSystem::render` (was `DrawRectangleLinesEx` + a green `DrawCircle`).
- **Optional parody:** themed icons (FIGHT = USB mouse, ITEM = a Dew can) **keeping the silhouette and position** — decide in ART-020.
- **LORE:** state variants — FIGHT becomes **BONK** on the Furry route (FLF-008) and MERCY is **grayed out/unavailable** at the start of Asgrr's fight (MRC-004).

#### BUI-007 — Soul sprite `[x]`
P0 · S · D-01, ART-021
- **What:** a red 16×16 sprite (native 8×8 at 2× or native 16×16, confirm); color variants per mode; "cracked" and "shards" frames for the game over.
- **LORE §8.1:** red (normal), blue (Papyrus), green (Sharkdyne), yellow (Mettapaw), **pink plush with ears** (Furry route, FLF-013), **frog** (names `PEPE`/`FROG`, cosmetic); FUR-stage accessories per D-15.

#### BUI-008 — Menus inside the box with the soul as cursor `[x]`
P0 · M · TXT-001
- **What:** every list (targets, ACTs, items, mercy) drawn **inside the big box**, in a 2-column grid, with the red soul to the left of the selected option (Undertale doesn't highlight the ACT text in yellow; the cursor is just the soul). Positions: column 1 with the "*" at x ≈ 100, column 2 at x ≈ 340; rows at y ≈ 270 and 302 (confirm).
- **Remove:** the current yellow text + "* " only on the selected item.

#### BUI-009 — Spareable enemy names in yellow `[x]`
P0 · S · BUI-008
- **What:** in the target list (FIGHT/ACT/MERCY), a spareable enemy's name shows in yellow; in MERCY, "* Spare" turns yellow if **any** enemy can be spared.

#### BUI-010 — Enemy idle animation `[x]`
P1 · M · GFX-002
- **What:** every enemy has an idle (sway, breathing, separately moving parts). E.g. Barky wags his tail and moves his ears; the idle stops when he takes damage and when he's spared.

#### BUI-011 — Spared/fled enemy turns translucent `[x]`
P1 · S · BND-002
- **What:** once spared, the sprite goes ~50% transparent and stops animating until the battle ends (in multi-enemy fights, the others keep going).

#### BUI-012 — Yellow name and notification sound on spare `[~]`
P2 · S · BUI-009, AUD-011
- **What (LORE §8.8):** spare stays 1:1 (yellow name), with an optional notification *ding* (configurable) alongside `spare_poof`.
- **Done (2026-10-16), pending a real SFX:** the ding plays once, on the ACT that turns the name yellow; toggle `"spareDing"` in `config.json` (default `true`, no in-game options menu yet). It reuses `u_activate.wav` as a placeholder until AUD-011 ships the ding/`spare_poof`.

#### BUI-013 — Per-encounter battle backgrounds `[~]`
P1 · M · BUI-002, GFX-002
- **What (LORE §25.3):** commons = green grid · Barky = grid + a fluorescent tube at the top · Furiell = plain black · Papyrus = grid + a scrolling chat panel (3 names) · Sharkdyne = speed lines at the edges + drops · Mettapaw = stage, spotlights, an audience of silhouettes · Asgrr = black + a border of trophies · The Mascot = an endless wall of blinking plastic eyes · Senpai Sans = black with the Last Lobby's gold · gamers (X2) = **red** grid · Moderator = a dark bedroom with pulsing RGB + a green cloud. A `background` field on the encounter.
- **Partly done (2026-10-16):** `BattleBackground` (`grid`, `grid_tube`, `red_grid`, `black`, `speed_lines`, `trophies`, `lobby_gold`) drawn with primitives; `BattleScene` reads `background` from `data/encounters/barky.json` (Barky now `grid_tube`). Still to do: Papyrus's chat panel, Mettapaw's stage, The Mascot's eyes, the Moderator's bedroom (they need art/content), and loading the encounter by id instead of the hardcoded Barky file (BFL-004).

---

### BFL — Battle: flow and state machine

#### BFL-001 — Rewrite the battle state machine `[ ]`
P0 · L · BUG-02, BUI-008
- **What:** explicit states, in Undertale's real order:
  1. `Intro` — encounter text ("* Barky blocked your path!" / "* X and co. decided to pick on you!"), buttons already active.
  2. `ChooseButton` — left/right across the 4 buttons; the soul sits on the button.
  3. `ChooseTarget` — the enemy list (FIGHT shows HP bars; ACT and MERCY don't).
  4. `ChooseAct` / `ChooseItem` / `ChooseMercy`.
  5. `FightBar` (FGT) → `FightAnim` (slash + damage number).
  6. `ActionText` — the result text (ACT/ITEM/MERCY); Z advances.
  7. `EnemyDialogue` — bubbles (TXT-008), one per talking enemy.
  8. `BoxToArena` — animate the box to the attack size; the soul appears in the center.
  9. `EnemyAttack` — the pattern runs for its set duration.
  10. `ArenaToBox` — the box goes back; the soul disappears.
  11. `FlavorText` — the turn's "mood" text → back to 2.
  12. `Victory` / `Spared` / `Fled` / `GameOver`.
- **X** goes back one level in any submenu.
- **Acceptance:** a flow diagram in `docs/battle_flow.md`; no state skips steps; BUG-02 no longer happens.

#### BFL-002 — Per-turn "flavor" text `[x]`
P0 · M · BFL-001, ACT-001
- **What:** every enemy has flavor text pools: `default`, `lowHp` (HP < 30%), `spareable`, and flag-conditional ones (e.g. after "Cringe Check": "* Barky is still blushing."). Random pick without repeating the last one.

#### BFL-003 — Trigger-based enemy dialogue `[x]`
P0 · M · BFL-001
- **What:** the enemy bubble depends on what the player did that turn (a response to a specific ACT, to damage, to the item used) with a random fallback. E.g. ROTEIRO Scene 3: "H-hey! Stop looking at the zipper!..." after "Cringe Check."

#### BFL-004 — Multi-enemy encounters `[ ]`
P0 · L · BFL-001
- **What:** `Encounter { enemies[1..3], introText, background, music, canFlee, routeOverrides }`. The attacks of every living/unspared enemy add up in the same turn (each with its own pattern); victory comes when all are dead, spared or fled.
- **LORE:** company-dependent conditions — **Reply Guy** only chills out **alone**; **Jerry** with `Ditch` makes the others spareable; Dogsona & Dogsonissa are a pair (ACT-004).

#### BFL-005 — Encounter and enemy data in JSON `[x]`
P0 · M · REPO-007, BFL-004
- **What:** `data/enemies/barky.json` with `name, hp, atk, def, exp, gold, sprite, voice, check, acts[], flavor{}, dialogue{}, patterns[], spareCondition, fleeable` (full schema in Appendix A). `data/encounters/*.json` references the enemies.
- **Where:** replaces the hardcoded constructor in `BattleScene::BattleScene`.

#### BFL-006 — Declarative spare conditions `[x]`
P0 · M · BFL-005
- **What:** `mercy` as a counter (0–100) or a set of flags: `spareWhen: { all: ["act.cringe_check", "act.offer_dew"] }` or `{ mercy: 100 }`. "HP below X%" and "turns survived ≥ N" also count.

#### BFL-007 — Per-encounter battle music `[x]`
P1 · S · AUD-001
- **What:** common enemies use the default battle theme; bosses use their own; the overworld music **stops** on the transition (ENC-002) and comes back on exit. Track list in AUD-010.
- **Done (2026-10-17):** the encounter JSON `music` id resolves to `assets/music/<id>.ogg` (`include/services/MusicTracks.hpp`); missing tracks (e.g. `boss_barky`, until AUD-010) fall back to the default battle theme. The overworld music already stops on the encounter transition and restarts on resume.

---

### FGT — Battle: FIGHT

#### FGT-001 — Elliptical attack target sprite `[~]`
P0 · M · ART-022
- **What:** a "target" sprite (an ellipse with vertical lines and bands, Undertale-style) the size of the box interior (565×130). Replaces the current gray/yellow rectangles.
- **Partly done (2026-10-17):** Undertale-style target drawn with primitives (white ellipse, green vertical bands narrowing to a white center line) filling the box interior. Pending: the real sprite (ART-022).

#### FGT-002 — Attack bar `[x]`
P0 · M · FGT-001
- **What:** a white vertical bar with a black outline enters from a **random** side (BUG-17) and crosses the box in ~1.5–2 s (confirm); Z stops it. After the hit, the bar flashes white/black for a few frames. If the bar exits the other side without Z: "MISS."
- **Special weapons** (P2): multiple bars (Real Knife/Burnt Pan style); weapon data defines `bars`, `speed`.
- **Done (2026-10-17):** white bar with a black outline from a random side, crossing in 1.6 s; after Z it blinks white/black for 0.55 s while the slash plays, then the target squishes out; leaving the far edge = MISS (`include/combat/AttackTarget.hpp`). Multi-bar weapons (P2) pending.

#### FGT-003 — Damage formula from data `[x]`
P0 · S · PRG-001
- **What:** apply the section 2.2 formula with player AT + weapon AT − enemy DF. Minimum 1 (or "MISS" if the bar wasn't stopped).
- **Acceptance:** a unit test with edge cases (dead center, edge, outside) (QA-011).

#### FGT-004 — Slash animation on the enemy `[x]`
P0 · M · ART-023
- **What:** a ~6-frame slash over the enemy (ROTEIRO parody: a mouse cursor/FPS "hitmarker," keeping the original slash's duration and position), with a slash sound and then the damage sound. On the Furry route the slash becomes BONK (FLF-008).

#### FGT-005 — Damage number and enemy HP bar `[x]`
P0 · M · GFX-003
- **What:** the damage number (custom damage font, red) appears above the enemy, bounces up and falls/settles; under it the HP bar shows up (red/gray background, green #00FF00) and **shrinks with animation**; it disappears after ~1 s. "MISS" in gray with no bar.

#### FGT-006 — Enemy shakes when hit `[x]`
P0 · S · —
- **What:** the sprite oscillates horizontally with decreasing amplitude (e.g. ±8, −6, +4, −2, 0 px over ~10 frames).

#### FGT-007 — Death by hit → vaporization `[x]`
P0 · S · GFX-008, BND-003

#### FGT-008 — Parody hitmarker `[x]`
P2 · S · FGT-004
- **What:** the ROTEIRO calls for an "FPS hitmarker sound" on crits. Play the hitmarker **instead of** the slash sound when `|distance| ≤ 12`, keeping the rest of the sequence identical.

---

### ACT — Battle: ACT

#### ACT-001 — Per-enemy ACTs defined in data `[x]`
P0 · M · BFL-005
- **What:** `acts: [{ id, label, text, effects: { mercy:+N, setFlag, spareable, heal, fur:-N }, requires?, responseDialogue? }]`. "Check" is always first and automatic.
- **Acceptance:** adding a new ACT to Barky doesn't require recompiling.

#### ACT-002 — Barky's ACTs `[~]` (LORE rework)
P0 · S · ACT-001, NAR-002
- **Done:** `Check`, `Cringe Check`, `Offer Dew`, `Debate Lore` in `barky.json`.
- **Missing (LORE §19.1, BUG-30):** the 2nd Cringe Check response, `Laser` (requires the Toy Laser Pointer, ACT-006) and `Accept Hug` (ACT-005); Check and responses matching the LORE text. Full detail in `C1-021`.

#### ACT-003 — ACTs that change the enemy attack `[ ]`
P1 · M · ACT-001, BHL-004
- **What:** effects like "the next attack is easier/faster/shorter" or "the enemy skips its turn." LORE cases: Grasstoid `Dinner` (green bullets heal next turn), Musk-ron `Hold Breath` (skips `musk_cloud`), Gatekeeper `Ask Favorite Species` (2 turns without attacking), Barky `Debate Lore`/`Laser`, Papyrus `Rate Fursona`, Sharkdyne `Reply to a DM`, Lurkeye `Pick On` (hits harder once), Gacha Whale `Ask About Pity`, Discord Simp `Reply "hey"`.

#### ACT-004 — Group and companion ACTs `[ ]`
P1 · M · BFL-004
- **What:** ACTs that affect everyone or depend on the encounter's makeup: Dogsona & Dogsonissa (`Roll Around` + `Re-sniff` → `Pet`), Reply Guy (`Talk` only works alone), Jerry (`Ditch`). Bumped from P3 to P1 because the Ruins and Snowdinha depend on it.

#### ACT-005 — Universal `Accept Hug` `[~]`
P0 · M · ACT-001, FUR-002
- **What:** an ACT present on every furry the LORE marks (Fursuit Dummy, Froggsona, Shysona, Doge, Barky, Furiell…): **spares on the spot** and adds FUR (+10 to +15; +30 on Furiell, which ends the fight). Squeak SFX; per-enemy text. It's the love-bombing mechanic (LORE §8.4) — it never appears on gamers.
- **Partly done (2026-10-17):** `ActDef::fur` + `spareNow`; Barky's `Accept Hug` spares on the spot with FUR +15 (stage-up feedback included) and shows his LORE line. Pending: the other LORE furries, the squeak SFX, Furiell's +30 ending.

#### ACT-006 — Conditional ACTs `[x]`
P1 · S · ACT-001
- **What:** `requires` by carried item (Laser ↔ Toy Laser Pointer), by FUR (Dogsona & Dogsonissa spare you outright at FUR ≥ 40), by flag/route (furry Pepe's ACTs, FLF-008) and by repetition (Cringe Check's 2nd time; Asgrr's `Talk` ×3; Napstablurk's `Cheer` ×3 in sequence).
- **Done (2026-10-16), mechanism only:** `ActDef` gained `requiresItem`, `requiresFurMin`, `requiresFlag` (unmet → the ACT is hidden from the menu, like Undertale) and `minUses` (effects only from the Nth use; per-battle counter). Pure helpers `actAvailable` / `actEffectApplies` are unit-tested. The per-enemy content (Laser, Dogsona, Asgrr `Talk` ×3…) comes with each enemy.

#### ACT-007 — ACTs that change stats `[x]`
P1 · S · ACT-001
- **What:** `atk`/`def` effects on the enemy: Asgrr `Talk` (on the 3rd, AT and DF −1), Moderator `Open a Window` (AT −2) and `Ask When He Last Showered` (DF −2); Moderator `Mention His Mom` (freezes him 1 turn).
- **Done (2026-10-16), mechanism only:** `atkDelta` / `defDelta` (clamped ≥ 0, via `Enemy::modifyAtk/modifyDef`) and `freezeTurns` (the enemy skips that many attacks and play goes back to the menu with flavor text). Combined with `minUses` this covers Asgrr's 3rd `Talk`.

---

### ITM — Battle: ITEM

#### ITM-001 — Data-driven item database `[x]`
P0 · M · REPO-007
- **What:** `data/items.json`: `{ id, name, shortName (≤ 9 characters, the battle name), type: consumable|weapon|armor|key, heal, at, df, furry, speedBuff, info, useText, sellPrice, buyPrice }`.
- **ROTEIRO items:** `Mountain Dew (Warm)` (+10 HP and a temporary speed boost), `Stale Doritos` (+6 HP), `Duct Tape` (−15 FUR), starting weapon `Frayed USB Optical Mouse` (≈ Stick), starting armor `One-Sided Broken Gaming Headset` (≈ Bandage).
- **New fields the LORE needs (pending):** `fur` (rename `furry`), `furMult`, `furOnEquip` (FUR-009), `fluff` (healing on the Furry route), `usable: false` / `droppable` (the `Bottle`), `key: true` (CELL, the jar), `routeOnly`. Full catalog in ITM-007.

#### ITM-002 — Battle item menu `[x]`
P0 · M · ITM-001, BUI-008
- **What:** 2 columns × 2 rows per page, "PAGE 1"/"PAGE 2" in the box's bottom-right corner, left/right switches column and page, Z uses, X goes back.

#### ITM-003 — Undertale-style use text `[x]`
P0 · S · ITM-001
- **What:** "* You drank the Mountain Dew.&* You recovered 10 HP!" / "* Your HP was maxed out." with the heal sound. Using an item **spends the turn** (the enemy attacks next).

#### ITM-004 — 8-slot inventory `[x]`
P0 · S · CORE-004
- **What:** an 8-item cap as in Undertale; picking up an item with a full inventory gives "* You're carrying too much." (or similar).

#### ITM-005 — Mountain Dew speed effect `[x]`
P2 · S · ITM-001, BHL-006
- **What:** a soul speed buff for N turns (a direct ROTEIRO parody), with a subtle visual cue (a trail).

#### ITM-006 — Starting `Bottle` item `[x]`
P1 · S · ITM-001, MNU-002
- **What (LORE §10.1, §20.4):** the inventory starts with `Bottle`: INFO `* A bottle. It's not Mountain Dew.` / `* (You don't want to talk about it.)`; USE does nothing (a refusal line); DROP: `* You throw it away. The Underground will remember.` Paid off by Bottle Guy (C6-020). It takes 1 of the 8 slots on purpose.
- **Done (2026-10-16):** `bottle` in `data/items/items.json`; first slot of the starting inventory; USE (overworld and battle) shows the refusal and keeps the item (the battle turn still passes). The overworld item menu gained a minimal `USE / INFO / DROP` row (soul cursor, left/right) for every item. The Bottle strings are still hardcoded in the scenes, matching the rest of the item code — TXT-010/ITM-007 move them to data.

#### ITM-007 — LORE item catalog `[~]`
P1 · M · ITM-001, FUR-009
- **What:** `data/items/items.json` with every item from LORE §20 (and the starting names aligned, BUG-24):
  - **Consumables:** `mtn_dew_warm`, `stale_doritos`, `duct_tape`, `glitter_poptart` (full HP), `commission_donut` (+12), `commission_cider` (+24), `snow_pao` (+14), `gamer_fuel` (+30, FUR +3), `uwumie_flakes` (+2), `cold_water` (+16; the key to Sharkdyne's friendship), `instant_noodles` (+4), `herbal_tea` (+99), `hug_coupon` (+10 FLUFF, Furry route only).
  - **Weapons:** `usb_mouse` (2), `laser_pointer` (3, unlocks `Laser`), `fingerless_gloves` (5), `phone_camera` (7), `real_keyboard` (10), `paws` (10, Furry route, FIGHT → BONK).
  - **Armor:** `broken_headset` (1), `faux_fur_bandana` (3, ×1.25), `fursuit_head` (4, +40 / ×1.5), `tryhard_headband` (5), `pillowcase_cape` (6, ×1.25), `uwumie_armor` (20, ×2), `blue_light_glasses` (4, ×0.75).
  - **Key:** `bottle`, `cell`, `old_soul_jar`.
  - INFO/USE text exactly as in the LORE.
- **Data done (2026-10-16):** all 29 ids are in `data/items/items.json` (types, HP, AT/DF, FUR, `furMult`, `route`, `unlocks`, LORE text). Lines/prices the LORE doesn't define are flagged `textTodo`/`priceTodo` for a writing pass. Still open: the code doesn't read `items.json` yet (items are still matched by name in the scenes) — that's ITM-001's loader + BUG-24.

---

### MRC — Battle: MERCY / Flee

#### MRC-001 — Spare `[x]`
P0 · S · BFL-006
- **What:** only works if the target is spareable; otherwise "* You spared Barky.&* But Barky wasn't yellow..." (parody) and the turn passes. Spared enemy → dust/cloud + translucency (BUI-011) + spare sound.

#### MRC-002 — Flee `[x]`
P0 · M · ART-024
- **What:** a flee chance (base ~50%, higher with more turns, confirm); success: the soul becomes the "running" sprite (with little legs) and exits left, with a random line ("* I'm outta here." / parody: "* Pepe alt-F4'd out of the fight."); failure: the turn passes. Bosses: `canFlee=false`.
- **ROTEIRO:** "Pepe flees backwards with little dust clouds" → use the dust clouds in the soul's flee animation.

#### MRC-003 — Spare/flee rewards `[x]`
P0 · S · PRG-001
- **What:** spare: 0 EXP and the enemy's gold. Flee: "* Escaped..." with no reward (in Undertale, fleeing after dealing damage gives partial EXP/gold, confirm).

#### MRC-004 — Unavailable MERCY (Asgrr) `[x]`
P1 · S · BUI-006
- **What (LORE §10.9, §19.6):** at the start of Asgrr's fight, the MERCY button is grayed out and, if chosen, shows `This feature is not available in your region.`; it goes back to normal when he kneels (low HP). A 1:1 parody of Asgore's destroyed MERCY, without breaking the button.
- **Done (2026-10-16):** optional enemy JSON field `mercyLockedAbovePct` (0 = off). While HP% is above it, MERCY is drawn gray; picking it shows the region line and goes back to the menu **without** passing the turn. Asgrr's data (with e.g. `"mercyLockedAbovePct": 30`) comes with his enemy item.

#### MRC-005 — Scripted Flee (Sharkdyne) `[ ]`
P1 · M · MRC-002, BHL-011, CORE-011
- **What (LORE §19.4):** after ~8 turns the soul turns red again and the fight gains a guaranteed **Flee** option; fleeing ends the battle and chains into an overworld chase to H1 (no victory screen). `wet.sharkdyne.state = 1`.

---

### BHL — Battle: enemy turn and bullet hell

#### BHL-001 — Animated box resize `[x]`
P0 · M · BUI-001
- **What:** `BattleBox::resizeTo(rect, frames)` interpolating the 4 edges. Each attack pattern declares its box size (default ~165×140 centered, bottom at y 389) (confirm). The soul appears in the center once the resize finishes.
- **Acceptance:** the text → attack → text transition matches a frame-by-frame reference recording.

#### BHL-002 — Bullets clipped to the box interior `[x]`
P0 · S · BUG-10
- **What:** `BeginScissorMode` on the box interior when drawing bullets (patterns that need to leave the box set `clip:false`). Bullets outside the bounds + a margin are destroyed.

#### BHL-003 — Precise hitboxes `[x]`
P0 · M · GFX-002
- **What:** the soul has a small central hitbox (confirm ~8×8); bullets have per-type hitboxes (circle, rotated rectangle, optional per-pixel mask for irregular shapes). The debug overlay (QA-002) draws them all.

#### BHL-004 — Attack pattern framework `[ ]`
P0 · L · D-09, BFL-005
- **What:** `IAttackPattern { boxSize(); duration(); onStart(); update(frame); }` + a `PatternRegistry` mapping the JSON name → class. Parameters (speed, density, damage, `fur`, `shame`) in the enemy JSON. Per-turn selection: sequential, random, or conditioned on flags/HP.

#### BHL-005 — Per-bullet damage and player DF `[x]`
P0 · S · BUG-07, PRG-002
- **What:** every bullet has `damage`; final damage = `max(1, damage − floor(DF/5))` (confirm). Damage sound, i-frames (BHL-007), +FUR (FUR-002).

#### BHL-006 — Faithful soul movement `[x]`
P0 · S · CORE-001
- **What:** 4 px/frame in 8 directions (no diagonal normalization, as in Undertale) (confirm); holding X halves the speed (confirm whether that's Undertale or only Deltarune; if it isn't, leave it out). Clamped to the box interior, accounting for sprite size.

#### BHL-007 — I-frames and damage feedback `[x]`
P0 · S · BUI-007
- **What:** after damage, the soul flashes (visible/invisible every 2 frames) for ~30 frames; "hurt" sound; light screen shake (GFX-006). Armor can extend the duration.

#### BHL-008 — White/blue/orange bullets `[x]`
P1 · M · BHL-004
- **What:** besides normal white ones: **blue** bullets only hurt if the soul is moving; **orange** only if it's still.
- **LORE uses:** Doge's blue words (`doge_words`: stand still), Papyrus's blue bones (`blue_bones`), Asgrr's blue/orange lanyard trident (`lanyard_trident_sweep`).
- **Done (2026-10-16):** `AttackColor` + `Bullet::hurtsSoul(soulMoving)` in `Bullet.hpp`; "moving" = the soul's position changed this frame (holding into a wall counts as still). Unit-tested. No pattern uses it yet — the LORE patterns above wire it up with BHL-004.

#### BHL-009 — Green healing bullets `[x]`
P1 · S · BHL-004
- **What:** a bullet that heals on contact (Vegetoid-style). **LORE uses:** Grasstoid's `grass_blades` (Ruins, which is why it went up to P1) and Washua's `soap_bubbles`.
- **Done (2026-10-16):** `AttackColor::Green` + `Bullet::heal`; touching it heals (clamped to max HP), plays `heal.ogg`, removes the bullet and grants no i-frames.

#### BHL-010 — Blue soul mode (gravity) `[ ]`
P1 · L · D-01, BHL-004
- **What:** gravity, jumping with up (height proportional to how long it's held), platforms, a "slam" that throws the soul against the wall. Fiction (LORE §8.1): "streamer mode," the weight of lag. Used by Papyrus (from turn 1) and Senpai Sans (Toxic route).

#### BHL-011 — Green soul mode (shield) `[ ]`
P1 · L · BHL-004
- **What:** the soul is locked in the center and a shield rotates across 4 directions to block arrows. Fiction: **Do Not Disturb** mode (`* Your SOUL is now on DO NOT DISTURB. Block the DMs!`). The arrows are **DMs with text** (`hey :3`, `u up?`, `rp?`…) and the **yellow** ones (`reverse_dms`) loop around and come from the opposite side, like Undyne's. Sharkdyne's fight (C3-021); the soul turns red again after ~8 turns (MRC-005).

#### BHL-012 — Yellow soul mode (cursor) `[ ]`
P1 · L · BHL-004
- **What:** the soul faces up and shoots with Z. Fiction: "a cursor to answer the quiz" — in Mettapaw's quiz (C4-021) the yellow soul **clicks** one of 4 answers (the `answer_letters` are targets). Bumped from P3 to P1 because Hotland depends on it.

#### BHL-013 — Purple soul mode (strings) `[-]`
- **Dropped:** LORE v2 has no Muffet parody and no purple soul (LORE §8.1).

#### BHL-014 — Barky's patterns `[~]` (LORE rework)
P0 · M · BHL-004, ART-012
- **LORE §19.1 rework:** the LORE fixes **three** patterns — `uwu_rain` (+6 FUR), `dew_bounce` (+4) and `howl_wave` (only **after Cringe Check**). The "Tail Swipe" below isn't in the LORE: remove it, or keep it only as a low-HP variation if playtesting asks for it. Per-bullet FUR values now come from data (FUR-002).
- **What (original version):** 3–4 patterns:
  1. **UwU Rain** — "UWU" letters (sprite, not `DrawText`) falling in columns with a slight zigzag.
  2. **Dew Bounce** — cans bouncing off the box walls with gravity.
  3. **Awoo Howl** — sound waves (arcs) coming from the sides with a gap to slip through.
  4. **Tail Swipe** (when HP < 50%) — a tail sweeps the box horizontally; jump over it with timing (no blue mode, just dodging up/down).
- **Acceptance:** "first Undertale boss" difficulty: beatable without taking damage after 2–3 tries.

#### BHL-015 — "Thinking" text over the soul `[x]`
P3 · S · —
- **What:** an easter egg (e.g. a tiny "ribbit" bubble when the soul stays still for a long time).
- **Done (2026-10-16):** after 3 s with the soul still during an enemy turn, a tiny `ribbit` bubble shows for 1.5 s with a soft click (at most once per turn; moving cancels it). The logic is `updateRibbit`/`RibbitState` in `BattleSystem.hpp` (unit-tested).

#### BHL-016 — Persistent damage zones `[x]`
P1 · M · BHL-004
- **What:** an area (cloud) that stays in the box for N frames and deals damage over time while the soul is inside, with its own i-frames: `musk_cloud` (Musk-ron), `musk_wave` (Sharkdyne, covers the box in green for 1 s), `cologne_cloud` (`Eau de Grr`, Asgrr), `musk_cloud_gamer` (Moderator, with residual SHAME).
- **Done (2026-10-16), mechanism only:** `DamageZone` (`include/combat/DamageZone.hpp`: area, lifetime, damage per tick, own tick interval, FUR per tick, color) + `BattleSystem::addDamageZone`. Zones tick independently of the bullet i-frames, render as translucent rectangles and are cleared every enemy turn; bullets and zones share `applySoulDamage`. The LORE clouds above get wired up with their patterns (BHL-004); SHAME is still to do.

#### BHL-017 — KR-style residual damage `[x]`
P1 · M · BUI-005, BHL-005
- **What:** the KARMA mechanic: a hit leaves an overlaid portion on the bar that drains slowly. Uses: **residual SHAME** on the Furry route (a gray overlay on the FLUFF bar, FLF-005) and Senpai Sans's `karma_fur` on the Toxic route (uses the FUR bar as temporary KR, reset at the end of the fight, C7-004).
- **Done (2026-10-16), mechanism only:** `ResidualMeter` (`include/combat/ResidualDamage.hpp`, ~1 HP/s drain with fractional carry) + `BattleSystem::addResidualDamage`. It drains without i-frames or feedback, never takes HP below 1 by itself (confirm against Undertale's KR), shows as magenta on the right end of the yellow fill, and clears at the end of the battle. SHAME on the FLUFF bar and `karma_fur` reuse it later.

#### BHL-018 — Text as bullets `[ ]`
P1 · M · BHL-004, GFX-003
- **What:** bullets that are words rendered with a sprite font (hitbox = the text rectangle): `doge_words` (`wow`, `such`, `very`, in blue), Sharkdyne's DMs, `hey_spam`, `well_actually`, `self_reflection`, `copypasta_wall` (a wall with a gap), `skill_issue_text`, and Dragon Excuse Guy's `excuse_text`, which the narrator **crosses out on screen** (strike animation + vanish).

#### BHL-019 — Mad Daki's reflection `[ ]`
P2 · M · BHL-004
- **What (1:1 with Mad Dummy):** FIGHT doesn't work (`* It's too soft. Your attack sinks in.`); Mad Daki's bullets hit it when the soul dodges close to it; its HP drops from its own bullets.

#### BHL-020 — Bullets that swerve at low HP (Furiell) `[ ]`
P0 · S · BHL-004
- **What (1:1 with Toriel):** with Pepe at HP ≤ 4, Furiell's bullets change course so they don't hit.

#### BHL-021 — `File Load` (The Mascot) `[ ]`
P1 · L · BFL-001, CORE-004
- **What:** a parody of Flowey's save/load: the screen flashes `File Loaded` and **the player's last turn is undone** (HP, items used and position revert to the snapshot from the start of the turn). Requires a per-turn battle-state snapshot.

---

### BND — Battle: endings (victory, dust, level up, game over)

#### BND-001 — Victory screen `[x]`
P0 · S · PRG-001
- **What:** "* YOU WON!&* You earned X EXP and Y gold." in the big box, with the music cut. Z → back to the overworld with a fade.

#### BND-002 — Spare sequence `[x]`
P0 · S · MRC-001
- **What:** spare sound + little clouds coming off the enemy + translucency; if it was the last enemy, go to BND-001 with 0 EXP.

#### BND-003 — Vaporization `[x]`
P0 · S · GFX-008
- **What:** the dead enemy turns to dust (GFX-008) with the "vaporized" sound; gold/EXP is tallied at the end.
- **LORE §8.8:** the dust is **plush stuffing + glitter** (shame kills furries); `exGamer` enemies (Barky, Napstablurk, Jerry, Musk-ron, Vtuber-en, Mettapaw) also release a **red spark** that flies toward the castle (GFX-013). Asgrr turns into lanyard dust.

#### BND-004 — Level up `[~]` (LORE rework)
P0 · S · PRG-001
- **What:** when EXP crosses the threshold: the extra line + level-up sound; HPmax/AT/DF updated.
- **Rework (BUG-22):** the canonical text is `* Your LEVEL OF VITRIOL increased.` (LORE §8.2); today it says `* Your VITRIOL increased.`

#### BND-010 — 1:1 game over + furrification `[~]` (LORE rework)
P0 · L · CORE-003, FUR-004
- **LORE §23.1 rework (BUG-20, BUG-21):** the implemented sequence (the soul cracks and shatters, `GAME OVER` logo) becomes the **FURRY OVER**: the soul **doesn't break** — it grows little ears and a tail, **inflates and pops in a "pomf" of pink pom-poms**; the **`FURRY OVER`** logo laid out like "GAME OVER"; **King Asgrr's** voice (1:1) with one of 6 lines using `[NAME]`; on continue, back to the save with the saved FUR. Exception: HP 0 against Papyrus = capture (C2-022). The original crack/shatter sequence is reused for the **GAMER OVER** (FLF-007). The "raid leader voice" below is dropped.
- **Original version (timing reference; steps 2, 3 and 5 are superseded by the LORE):**
  1. Everything freezes; the screen goes black except for the soul, which stays where it was.
  2. ~0.5 s later the soul **cracks** ("break1" sound); ~1 s later it **splits into 4–6 shards** that fly off with gravity ("break2" sound). Parody: the shards become **tufts of fur**.
  3. The "GAME OVER" logo fades in slowly (parody: "GAME OVER" with cat ears on the "O", or "FURRY OVER" — decided in NAR-011).
  4. Game over music (original, with a "Determination" vibe).
  5. Text typed in a deep voice (Asgore's in Undertale). Suggested parody: the **Discord raid leader's** voice: "You cannot ragequit just yet... / PEPE! / Stay determined..." (uses the chosen name).
  6. Z → fade to the last save, with full HP.
- **Acceptance:** recorded and compared against Undertale's game over, every step's timing matches within ±3 frames.

#### BND-011 — FURRY OVER variants `[ ]`
P2 · M · BND-010, SAV-005
- **What (LORE §23.1):** against Barky: `...welcome to the pack? no? ok.` · with 3+ FURRY OVERs (`meta.ever_transformed ≥ 3`): `...Again? Keep the tail, [NAME].` · Toxic route: **no voice**. Every FURRY OVER also takes 100G off the Uwu-mie Armor (SHP-005).

---

### ENC — Encounters and battle transition

#### ENC-001 — "!" alert `[x]`
P1 · S · ART-030
- **What:** on scripted encounters, a "!" bubble appears over Pepe's head for ~15 frames before the transition (with a sound).

#### ENC-002 — 1:1 encounter transition `[x]`
P0 · M · CORE-003, BUI-007
- **What:** the exact sequence:
  1. The overworld music stops.
  2. The screen goes black except for the **red soul** drawn over Pepe's chest.
  3. The soul blinks 3 times (visible/invisible), with a "beep" on each blink.
  4. The soul **slides** to the FIGHT button position (x ≈ 40, y ≈ 446) over ~15–20 frames, with a "falling" sound.
  5. The battle appears (the soul is already on the FIGHT button).
- **Where:** replaces the current black fade from `changeScene` in `OverworldScene` and the B shortcut.
- **Acceptance:** identical to a reference recording; zero frames of gray fade.

#### ENC-003 — Step-based random encounters `[ ]`
P1 · M · OVW-003
- **What:** each room defines an `encounterTable` + `stepsMin/stepsMax`; a step counter (distance walked) fires a weighted encounter. No encounter during the first N steps after a battle or after entering the room.

#### ENC-004 — Area exhaustion (for the Toxic Gamer route) `[ ]`
P2 · M · ENC-003, PRG-030
- **What:** a kill counter per area; when the cap is hit, encounters stop and "* But nobody logged in." shows up.
- **LORE (D-11):** caps **Ruins 6, Snowdinha 6, Wetfall 5, Hotland 3** (random encounters only), instead of Undertale's 20. Stays P2 (Toxic route), but the `kills.<area>` counters land in M4 because Sans's verdict and the endings read them.

#### ENC-005 — Remove the B debug shortcut `[x]`
P0 · S · QA-001
- **What:** the B key only works with `NUNDERTALE_DEBUG`; it becomes an entry in the debug menu (QA-003).

#### ENC-006 — Per-area encounter budget `[ ]`
P1 · S · ENC-003
- **What (LORE §11.1, §18.6):** outside the Toxic route, **2–3 random encounters per area**, spread across rooms with a budget (R8: 1 · R10: 1 · S5: 2 · W3: 2 · H1→H2: 1). Once a room's budget is spent, the step counter stops firing. On the Toxic route the budget is ignored until the cap (ENC-004); on the Furry route encounters become hugs (FLF-009).

---

### OVW — Overworld

#### OVW-001 — Remove the overworld HUD `[x]`
P0 · S · MNU-001
- **What:** Undertale has no HUD while exploring. Delete the bottom bar with NAME/LV/HP/G/FURRY and the "[Z: Interact | B: Battle | ESC: Menu]" hints. That info moves to the C menu (MNU-001).

#### OVW-002 — Rooms bigger than the screen + camera `[ ]`
P0 · M · GFX-010, OVW-003
- **What:** a room is a map of any size; the camera follows Pepe. Replaces the 640×480 `test.bmp` with pillars drawn by `DrawRectangle`.

#### OVW-003 — Tiled map loader `[ ]`
P0 · L · D-08
- **What:** read Tiled JSON: tilesets, tile layers (with draw order), a collision layer (rectangles **and polygons**, for the Ruins' diagonal walls), objects (`door`, `sign`, `npc`, `savepoint`, `trigger`, `spawn`, `encounterZone`) with custom properties.
- **Acceptance:** the current test room is rebuilt in Tiled and works the same, with no coordinates in the C++.

#### OVW-004 — Depth sorting (Y-sort) `[ ]`
P0 · S · OVW-003
- **What:** Pepe, NPCs and tall objects drawn by the Y of their base; an "above" layer for things always on top (arches, pillar tops).

#### OVW-005 — Sliding collision and diagonal walls `[ ]`
P0 · M · OVW-003
- **What:** per-axis collision (already exists) + sliding along diagonal walls (like walking diagonally against a 45° wall in Undertale). Pepe's collision box only at his feet (adjust the current `{x+5, y+38, 28, 18}` to the new sprite).

#### OVW-006 — Faithful Pepe movement `[x]`
P0 · S · CORE-001
- **What:** 3 room px per frame, which at the 2× view is 6 screen px per frame = 180 px/s at 30 fps (confirm). **No diagonal normalization** (diagonally, Frisk moves 3 px on each axis). The sprite facing follows Undertale's rule: on a diagonal, keep the direction that was pressed first. No running (D-10).
- **Before:** `PLAYER_SPEED = 180 px/s` already matched in straight lines, but the diagonal was normalized and facing used the dominant axis → changed.

#### OVW-007 — Undertale-style interaction `[ ]`
P0 · S · OVW-003
- **What:** Z interacts with the object **in front of** Pepe (a short interaction box in the facing direction); NPCs turn toward the player while talking and go back to their original facing afterward.

#### OVW-008 — Doors and room transitions `[ ]`
P0 · M · OVW-003, CORE-003
- **What:** a `door { targetRoom, targetSpawn, facing }` object; on touch, a quick black fade (~8 frames, confirm), room swap, Pepe appears at the spawn facing the right way. The music keeps playing if it's the same area track.

#### OVW-009 — NPCs with routines `[ ]`
P1 · M · OVW-003, CORE-011
- **What:** idle NPCs, NPCs walking a route (waypoints), NPCs following the player (Furiell guiding through the Ruins), with dialogue conditioned on flags and a conversation counter ("talkCount").

#### OVW-010 — Room state persistence `[x]`
P0 · M · CORE-010, BUG-04
- **What:** objects taken/used, defeated NPCs, solved puzzles and open doors are read from flags on room entry.

#### OVW-011 — Generic interactive objects `[ ]`
P0 · M · OVW-003, TXT-001
- **What:** data types: `sign` (text), `examine` (text with talkCount variations), `itemPickup` (gives an item once), `switch` (sets a flag), `container` (e.g. the "Take one" candy bowl → parody "Take one Dorito"), `bed`, `mirror` ("* It's you!" → "* It's you, Pepe. Feels bad man.").
- **LORE cases:** `examine` with a counter (the Art Show curtain 3×, `ruins.curtain.looks`) and with a FUR condition (the R11 mirror `* It's... still you. With ears.`); `container` that breaks (bowl: take 4 → knocked over, `ruins.bowl.broken`); `bed` with an effect (full HP **+5 FUR**); hidden `itemPickup` (Toy Laser Pointer behind the CRT, Fursuit Head in the closet, Phone Camera in the W3 trash can, Real Mechanical Keyboard in the basement); `npcPush` with permission (the mod rock: pushing without asking → `* You have been muted for 5 seconds.` and controls **lock for 5 s**); `locked` (Furiell's room, closed objects — LOR-007); `waterCooler` (gives `Cold Water`).

#### OVW-012 — Base puzzles `[ ]`
P1 · L · OVW-011
- **What:** reusable building blocks: floor/wall switches, spikes that retract with a flag, grid-pushable rocks, a floor that cracks and drops you to the room below, ordered levers, ice floor (Snowdinha).
- **LORE puzzles (all < 45 s, with a funny hint):** RGB switches marked with glitter (R4), a shocking cable maze with a scripted guide (R6), the 3×3 **CAPTCHA Floor** with a traffic light straddling two tiles (S3), the stream-alert bridge (S4, cutscene). Cracking floors, grid rocks and ice **don't appear in the LORE** — build them only if a room needs them.

#### OVW-013 — Footstep sounds `[ ]`
P3 · S · AUD-002
- **What:** Undertale has **no** footstep sound on regular floors; only on special surfaces (crunchy dry leaves, snow, water). Implement per tile.

#### OVW-014 — Leaf piles and special tiles `[ ]`
P2 · S · OVW-003
- **What:** tiles that play a sound when stepped on (the Ruins' leaves → parody: crumpled snack bags, as in ROTEIRO Scene 1; the Dorito pile in R3).

#### OVW-015 — Overworld projectiles and chases `[ ]`
P1 · M · OVW-003, BHL-003
- **What:** projectiles that cross the room and take HP/add FUR outside battle (a parody of Undyne's spears): Sharkdyne's DM harpoon in W1 (just misses, scripted) and the **DMs in a line** during the W5 chase; the post-fight chase to H1 (MRC-005).

#### OVW-016 — Scripted hiding spots `[ ]`
P1 · S · CORE-011
- **What:** short cutscenes where Pepe hides behind an object while an NPC walks by: the **Pepe-shaped lamp** (S1, Papyrus) and the **fursuit clothesline** (W1, Sharkdyne).

---

### MNU — Overworld menu (ITEM / STAT / CELL)

#### MNU-001 — C menu `[x]`
P0 · M · TXT-001
- **What:** C/Ctrl opens a small box in the top-left corner with NAME, "LV 1", "HP 20/20", "G 10" and, below it, another box with `ITEM`, `STAT`, `CELL` and the soul as cursor. When Pepe is in the top half of the screen, the boxes move to the bottom. Measurements from reference (confirm).
- **Acceptance:** opens/closes with C and X; the game is paused while it's open.

#### MNU-002 — ITEM `[x]`
P0 · M · MNU-001, ITM-001
- **What:** the list of up to 8 items in a big box; picking an item opens `USE / INFO / DROP`. INFO shows the description ("* \"Mountain Dew (Warm)\" - Heals 10 HP&* Tastes like regret and citrus."). DROP: "* The Duct Tape was thrown away." Equipping a weapon/armor swaps it with the current one. The `Bottle` has its own DROP line (ITM-006).

#### MNU-003 — STAT `[~]` (LORE rework)
P0 · M · MNU-001, PRG-001
- **What:** a big box in Undertale's format:
  ```
  "PEPE"
  LV  1
  HP  20 / 20

  AT  0 (0)     EXP: 0
  DF  0 (0)     NEXT: 10

  WEAPON: Frayed USB Mouse
  ARMOR:  Broken Headset
  GOLD: 10
  ```
  Parody: an extra `FUR` line (FUR-003) and a "KILLS" line that only shows on the Toxic Gamer route.
- **LORE rework:** label **`FUR n`** + the stage name (BUG-23, FUR-006); a **`RAID mm:ss`** line (RAD-002); on the Furry route `HP` becomes `FLF` and FUR disappears; weapon/armor with the LORE names (BUG-24).

#### MNU-004 — CELL (phone) `[ ]`
P1 · M · MNU-001, NAR-004
- **What (LORE §21):** a flip phone with paw stickers (received in R7); the **raid clock** at the top (RAD-002); a visible `Homework` folder (400 GB) that never opens (`* Not here.`). Contacts unlocked by flag:

| Contact | Added | Behavior |
| :--- | :--- | :--- |
| Furiell | R7 | `Say Hello`, `Puzzle Help`, `About Yourself`, `Call Her "Mod"`, `Send a Meme`; per-room replies in the Ruins, then voicemail |
| Papyrus | S6 | calls with "content tips" |
| Alphys | H2 | quiz hints (`...is typing`) |
| Guild | R1 | receive-only (MNU-006) |
| Mom | R1 | receive-only (MNU-006) |

#### MNU-005 — Dimensional box (storage) `[-]`
- **Not in LORE v2:** no dimensional box in a ~30-minute scope. Reopen only if the 8-slot inventory becomes a problem in playtest (suggested parody: `CLOUD STORAGE - 3 GB FREE`).

#### MNU-006 — Automatic messages and forced calls `[ ]`
P1 · M · MNU-004, CORE-010
- **What:** a queue of messages that arrive in the overworld (buzz + `phone_buzz`; never mid-battle/cutscene): the **guild** by clock threshold (RAD-003); **Mom** on entering Snowdinha (`are you on the mountain again`), Wetfall (`bring the bottles down when you come back`) and the Con Center (`dinner's cold`); **Papyrus**'s content tips, one per area after S6 (LORE §21.3); Furiell's **forced calls** after R7 and twice in R10. Message history viewable on the CELL.

---

### SAV — Save point, save/load and meta-persistence

#### SAV-001 — Save point sprite and animation `[~]`
P0 · S · ART-031
- **What:** the yellow 4-point star as a sprite, with Undertale's 2-frame animation. ROTEIRO parody: the star shining **on top of a vintage soda bottle**. Currently it's 3 `DrawRectangle`s.
- **Partly done (2026-10-17):** the yellow 4-point star with a 2-frame spin (0.25 s) and a 1 px bob, hovering over a vintage green soda bottle, drawn pixel by pixel with primitives. Pending: the real sprite (ART-031) and the *tssss* SFX.

#### SAV-002 — Save point flow `[x]`
P0 · M · TXT-007, SAV-003
- **What:**
  1. The room's determination text ("* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.") + full HP + heal sound.
  2. The centered save box: `NAME   LV 1   0:42` / `Ruins - Entrance` / `Save   Return`, with the soul as cursor.
  3. Save: the lines turn **yellow**, save sound, "File saved." replaces the options; Z closes.
- **Per-save-point text:** every save has its own determination text (data), which can change with flags (Toxic Gamer route: "* Determination.").

#### SAV-003 — Save file `[x]`
P0 · M · CORE-004
- **What:** `%LOCALAPPDATA%/nUndertale/file0.json` with the full `GameState` + a schema version; atomic writes (temp file + rename). One slot, as in Undertale.

#### SAV-004 — Continue/Reset `[x]`
P0 · M · SAV-003, FRT-005
- **What:** with an existing save, the title shows the Continue screen (FRT-005). Reset deletes the save **but not the meta-persistence** (SAV-005).

#### SAV-005 — Meta-persistence (memory across resets) `[x]`
P2 · M · SAV-003, NAR-012
- **What:** a separate `meta.json` (the `undertale.ini` equivalent): number of resets, whether you ever killed anyone, whether you ever finished a route, number of full furrifications (D-05). Used by lines that "remember" (Senpai Sans breaking the fourth wall, the Flowey parody).
- **New LORE keys (pending, MET-*):** `meta.last_ending` (title screen), `meta.ever_furry`, `meta.last_name`; `meta.ever_transformed` now counts FURRY OVERs (PRG-021).

#### SAV-006 — Corrupted save recovery `[x]`
P2 · S · SAV-003
- **What:** if the JSON fails, try `file0.bak`; if that fails too, warn and start fresh without crashing.

#### SAV-007 — LORE save texts and rules `[~]`
P1 · S · SAV-002, FUR-004
- **What (LORE §8.8):** the formula `* [something about the place] fills you with DETERMINATION.` for all ~14 LORE saves (R1, R3, R13, Snowdinha entrance/town/S6, W1, W6, H3, N3, X2…); can SFX (*tssss*); heals HP (or FLUFF on the Furry route) **without reducing FUR** — with FUR > 0, an extra line `* DETERMINATION can't fix fluff.`; on the Toxic route, `* Determination.`
- **Save schema (v2):** `fur.value`, `fur.stage_max`, `fur.mirror_refused`, `route.furry`, `fluff.value`, `raid.time_left`, `kills.<area>`, a per-playthrough FURRY OVER counter (SHP-005), with a v1 migration (BUG-27).
- **Partly done (2026-10-17):** the LORE save line per room (`include/core/SaveText.hpp`, 11 rooms), `* DETERMINATION can't fix fluff.` when FUR > 0, `* Determination.` on the Toxic route; saving heals HP and never touches FUR. Pending: save schema v2 fields beyond FUR, FLUFF heal on the Furry route.

---

### PRG — Progression, equipment and routes

#### PRG-001 — LV, EXP and stats `[x]`
P0 · M · CORE-004
- **What:** `data/lv_table.json` with cumulative EXP per LV (section 2.2), `HPmax = 16 + 4·LV`, AT and DF per LV (wiki table, confirm). EXP/gold come from the enemy data.

#### PRG-002 — Equipment `[x]`
P0 · S · ITM-001
- **What:** weapon (AT, attack bars, bar speed) and armor (DF, extra i-frames). Starting gear: Frayed USB Optical Mouse and One-Sided Broken Gaming Headset (names aligned with the LORE in ITM-007).

#### PRG-003 — Gold and shops `[-]`
- **Moved to `SHP-001`** (shops epic).

#### PRG-004 — Spider sales / NPC shops `[-]`
- **Replaced by `SHP-002`** (Commission Bake Sale; the "NFT Bake Sale" was dropped by the LORE).

#### PRG-020 — Chapter progression flags `[~]` (LORE rework)
P0 · S · CORE-010
- **Done:** `data/flags.md` with the `<domain>.<entity>.<property>` taxonomy.
- **Rework:** see PRG-021.

#### PRG-021 — Migrate `flags.md` to the LORE taxonomy `[ ]`
P0 · S · PRG-020, D-11
- **What (LORE §28, BUG-26):** add the new flags (`ruins.floofy.met`, `ruins.dummy.result`, `ruins.cell.given`, `ruins.laser.taken`, `ruins.curtain.looks`, `ruins.bowl.broken`, `ruins.napstablurk.state`, `ruins.fursuit_head.taken`, `ruins.furiell.hugged`, `snow.*`, `wet.*`, `hot.*`, `con.*`, `fur.*`, `route.furry`, `fluff.value`, `raid.*`, `ending.type`, `kills.snow/wet/hot`, `meta.last_ending`, `meta.ever_furry`); retire `route.wholesome`/`route.normie`; `kills.ruins` cap 20 → 6; redefine `meta.ever_transformed` as the number of FURRY OVERs. Update `OVW-010` and the reactive NPCs that read the old flags.
- **Acceptance:** `data/flags.md` = LORE §28 + the flags already used by the code; no flag read in C++ is undocumented.

#### PRG-030 — Route tracking `[~]` (LORE rework)
P1 · M · PRG-020, D-07
- **Done:** kill, spare and flee counters and a `RouteType` with Wholesome/Normie/Toxic.
- **Rework:** see PRG-031.

#### PRG-031 — LORE routes `[ ]`
P1 · M · PRG-030, D-07, D-11
- **What (LORE §22.0, BUG-25):** `RouteType { Gamer, GamerCleanRun, Toxic, Furry }`. **Furry** = accepted the Mirror (`route.furry`); **Toxic** = exhausted every area's cap (6/6/5/3, random encounters only) **and** killed Barky, Furiell, Papyrus and Sharkdyne; **Gamer** = everything else, with **Clean Run** when `kills.total == 0`. Also expose the in-progress predicate `route.toxic_active` (C7-001). Read by Sans's verdict, the ending router (END-001) and the FUR lock (FUR-011).
- **Acceptance:** unit tests for each route and the edge cases (killed only bosses; killed every random but spared one boss; converted after killing).

---

### FRT — Front-end (disclaimer, intro, title, name, settings)

#### FRT-001 — Undertale's boot order `[x]`
P1 · S · SAV-004
- **What:** first boot: Disclaimer (nUndertale-only, fine) → Intro → "nUNDERTALE" title → **instructions screen with "Begin Game / Settings"** → name → white fade → the fall room. With a save: Intro (if idle) → title → Continue/Reset.
- **Before:** a title with "New Game / Instructions / Exit" and the instructions on a separate screen.
- **LORE §11.2:** the whole front-end (disclaimer → intro → title → instructions → name) fits in **~2:30** on first boot.
- **Done (2026-10-17):** Disclaimer → intro → press-Z title → start menu (no save) or Continue card (save) → name → white fade. ~20 s idle on the title replays the intro (`include/scenes/FrontEndFlow.hpp`).

#### FRT-002 — 1:1 intro (lore) `[~]`
P1 · M · TXT-001
- **What:** 400×220 images (the current ones are 398×214 → fix the art canvas) at x = 120, with the text typed below (main font, 2×), a black fade between cards, the intro music, and Z speeds it up. **The last image is tall and scrolls vertically** (the fall) before the title — today it's a 187-BMP animation (`assets/img/lore10/`); replace it with one tall image + scroll, or a PNG spritesheet.
- **Text (LORE §9, BUG-29):** the **10** canonical cards — the intro is **gamer propaganda** (a biased narrator; the furry version comes in the Hall of Posts): 1 `Long ago, two races ruled / over the internet: / GAMERS and FURRIES.` · 2 `One day, the FURRIES tried / to turn every GAMER / into a FURRY.` · 3 `The GAMERS fought back with / copypastas, raids, and a / total lack of shame.` · 4 `After a decisive ratio, / the GAMERS were victorious.` (`lore4` art **redrawn without a real face**, ART-170) · 5 `Seven GAMERS sealed the / degenerates underground / with a magic BARRIER.` · 6 `Many years later...` / `MT. DEW, 202X` · 7 `Legend says that those who / keep on yiffin' / may never game again...` · 8–10 no text (Pepe looking at the Rift; a foot slipping on a Dorito next to a tail; the fall + cut to the title).
- **Partly done (2026-10-17):** the 10 canonical LORE §9 cards with verbatim text (cards 4 and 5 share `lore4` until ART-170); skip hints use the main font. Pending: 400×220 art canvas, the tall scrolling fall image, Z speed-up instead of skip.

#### FRT-003 — 1:1 title `[~]`
P1 · S · GFX-004
- **What:** the "nUNDERTALE" logo (Monster Friend font, white) centered, a small gray "[PRESS Z OR ENTER]" that appears after ~1 s, an "intronoise" sound when it shows up. The random lines (`RANDOM_QUOTES`) can stay as a gray parody splash underneath.
- **LORE §9, §24:** after card 10, black screen → the logo with the **lowercase `n` in red, trembling slightly**. The floating logo is the **LORE's style choice** for the "no ending yet" state (floating logo on black); the other 4 variants come from the last ending (MET-005).
- **Idle:** ~20 s idle on the title → the intro plays again.
- **Partly done (2026-10-17):** "nUNDERTALE" logo with a red trembling `n`, `[PRESS Z OR ENTER]` after 1 s with the `n.ogg` sting, quotes as a gray splash, menu music. Pending: Monster Friend-style font (GFX-004) and the ending-based variants (MET-005).

#### FRT-004 — Instructions/start menu screen `[x]`
P1 · S · FRT-001
- **What:** Undertale's layout: `--- Instruction ---` in gray, the list `[Z or ENTER] - Confirm`, `[X or SHIFT] - Cancel`, `[C or CTRL] - Menu (In-game)`, `[F4] - Fullscreen`, `[Hold ESC] - Quit`, `When HP is 0, you become a furry.` (already there, keep it), and below `Begin Game` / `Settings` with a yellow selection.
- **Done (2026-10-17):** Undertale layout with `Begin Game` / `Settings` (Settings opens the FRT-009 screen).

#### FRT-005 — Continue screen `[~]`
P1 · M · SAV-004
- **What:** NAME, LV, time and save location; `Continue` / `Reset`; `Settings` underneath. **Background art from the last area** and music that changes with progress (like Undertale's menu).
- **Partly done (2026-10-17):** Undertale layout (NAME, LV, time, room; Continue / Reset; Settings). Pending: background art from the last area and progress-based music.

#### FRT-006 — 1:1 name screen `[x]`
P0 · M · GFX-009
- **What:** "Name the fallen gamer." (already there); a grid of **uppercase A–Z (7 columns) and lowercase a–z (7 columns)** as in Undertale; `Quit`, `Backspace`, `Done` buttons at the bottom; slightly trembling letters; a 6-character limit.
- **Parody:** keep the special `[PRO]` and `[GAMER]` buttons only if they fit without breaking the layout (suggestion: make them name easter eggs, not buttons).
- **Done (2026-10-17):** A–Z / a–z 7-column grids with trembling letters, Quit / Backspace / Done, 6-letter limit; the PRO/GAMER buttons became name easter eggs.

#### FRT-007 — 1:1 name confirmation `[x]`
P0 · M · FRT-006, CORE-003
- **What:** "Is this name correct?" (current parody: "Are you sure about that?", fine), the name **slowly grows** from small to large in the center while trembling, `No` / `Yes` underneath; Yes → a **long white fade (~3 s)** with a rising sound (cymbal/"swoosh") straight into the fall room. Forbidden names only show `Go back`.
- **Done (2026-10-17):** the name grows and trembles toward the center over ~4 s, No / Yes (forbidden names: Go back only), Yes → 3 s white fade (CORE-003 `fadeWhite(out, in)`). The rising cymbal SFX is still missing (AUD-011).

#### FRT-008 — Name easter eggs: review `[~]`
P2 · S · FRT-007
- **What:** keep the ROTEIRO §6 table + LORE §30.2 (`FURIEL`/`FURIELL` `* Furiell is flattered. But no.` · `BARKY` `* AWOO! ...wait. That's MY name.` · `FLOOFY` `* No shame. Respect. ...Not allowed, though.` · `AZZY` screen darkens, `* ...`, allowed · `FROG`/`PEPE` frog soul · `YIFF` `* The narrator refuses.` (not allowed) · `OWO` `* What's this?` · `CHARA` the first-fallen easter egg). Forbidden name `GASTER` → restarts the game (as in Undertale). Move the list to `data/names.json` (NAR-028).
- **Dropped:** the previously proposed `GABEN` (a real person, LORE §2.2). Replace "Midwest FurFest" with FurNation Con '14 wherever it shows up.
- **Partly done (2026-10-17):** ROTEIRO §6 + LORE §30.2 table in `include/core/NameRules.hpp` (FURIEL/BARKY/FLOOFY/YIFF forbidden, AZZY darkens, FROG/PEPE frog color, GASTER restarts the game). Pending: move it to `data/names.json` (NAR-028).

#### FRT-009 — Settings `[~]`
P2 · M · CORE-007
- **What:** the "SETTINGS" screen with `EXIT`, music/SFX volume, fullscreen, integer scaling and controls (no language option, D-03). Undertale's background/style (big text, selected option in yellow).
- **Partly done (2026-10-17):** `SettingsScene` pushed from the start menu and the Continue card: EXIT, music volume, master volume (live, 10% steps), fullscreen, spare ding, a static controls block; saved to the config on exit. Pending: SFX volume and integer scaling (need engine support) and a remapping UI.

#### FRT-010 — Disclaimer `[ ]`
P3 · S · BUG-01
- **What:** already exists; review the opening fade (depends on BUG-01) and make sure Z skips it.

---

### AUD — Audio

> Final compositions are **original** "in the style of" (D-04, LORE §27). Placeholders: any CC0/CC-BY chiptune with the indicated vibe, logged in `assets/PLACEHOLDERS.md`.

#### AUD-001 — Music by context `[ ]`
P0 · M · D-04
- **What:** `data/music.json` mapping a track per room/area/encounter/scene, with loop points (`loopStart`, `loopEnd`). Today `epic_background_loop.ogg` plays on the title, in the overworld **and** in battle.
- **Acceptance:** every LORE area, boss and scene plays its own track (AUD-010); swapping a track doesn't require recompiling.

#### AUD-002 — Equivalent SFX library `[ ]`
P0 · M · D-04
- **What:** **original** sounds (sfxr/ChipTone/jsfxr/Bfxr or recorded) covering every Undertale function **and** the LORE §27.2 SFX. Full list in Appendix B. Standardized names `assets/sfx/<function>.ogg`.

#### AUD-003 — Correct navigation sounds `[ ]`
P0 · S · AUD-002
- **What:** **moving the cursor** = a short "squeak"; **confirming** = "select." Today both use `selection_sound.wav` at different volumes.

#### AUD-004 — Music cuts and resumes `[ ]`
P1 · S · AUD-001
- **What:** the music stops on encounter (ENC-002); the battle theme plays; on victory the music stops; back in the overworld, the area track restarts (confirm against Undertale). Special cases: the Ruins music in slow motion on the Toxic route (C7-002); the Con Center **without music** until the throne.

#### AUD-005 — Loop points `[ ]`
P1 · S · AUD-001
- **What:** intro + loop (raylib `Music` with a manual seek to `loopStart` after passing `loopEnd`).

#### AUD-006 — Music fades `[ ]`
P2 · S · AUD-001
- **What:** `fadeOut(frames)` / `fadeIn(frames)` for cutscenes (Furiell leaving, Senpai Sans showing up, the Mirror).

#### AUD-007 — Channels and priority `[ ]`
P2 · S · —
- **What:** the same sound overlapping (multi-instance with `LoadSoundAlias`), e.g. fast voices, bullets, back-to-back plush squeaks.

#### AUD-008 — Music speed/pitch variation `[ ]`
P2 · S · AUD-001
- **What:** play a track in slow motion (lower pitch and tempo) for the Toxic route's empty areas (LORE §22.2) and the pitch bend at the end of "Your Best Floof" (LORE §27.1).

#### AUD-010 — The LORE soundtrack (25 tracks) `[ ]`
P0→P2 · XL · AUD-001
- **What:** produce (placeholder → final) every track in LORE §27.1. **Pepe's leitmotif** (4 descending notes) appears in "Once Upon a Dew," "Reflection" and "Pawsome."

| Track | Where | Parody of | Direction | Prio. |
| :--- | :--- | :--- | :--- | :---: |
| Once Upon a Dew | Intro, title | Once Upon a Time | Main theme; Pepe's leitmotif | P0 |
| Start Menu | Name screen | Start Menu | Music box | P0 |
| Your Best Floof | Floofy | Your Best Friend | Cute and fake, pitch bend at the end | P0 |
| Fluffy Mother | Furiell (overworld/house) | Fallen Down / Home | Lullaby | P0 |
| Ruins (lo-fi beats to get lost to) | Ruins | Ruins | Lo-fi chiptune with crackle | P0 |
| Enemy Approaching (Fluffier) | Common battles | Enemy Approaching | Upbeat, with 8-bit "uwu"s | P0 |
| Alpha Wolf (Not a Phase) | Barky | — | Emo chiptune, off-key howl | P0 |
| Heartache (but Fluffier) | Furiell (fight) | Heartache | Dramatic, glitter bells | P0 |
| Senpai Sans. | Senpai Sans | sans. | Lazy swing | P1 |
| Nyeh Heh Stream | Papyrus | Nyeh Heh Heh! | Goofy march | P1 |
| Snowdinha Town | Snowdinha | Snowdin Town | 8-bit bossa nova | P1 |
| Bonestream | Papyrus (fight) | Bonetrousle | High energy | P1 |
| Wetfall | Wetfall | Waterfall | City pop with drizzle | P1 |
| DMs of Justice | Sharkdyne | Spear of Justice | Heroic, fast, with "rawr"s | P1 |
| uwu-mie village | Uwu-mie Village | Temmie Village | Chaotic, out of tune | P2 |
| Hotland (fan noise & funk) | Hotland | Hotland | Funk + case fans | P1 |
| Death by Engagement | Mettapaw | Death by Glamour | Disco | P1 |
| Con Center | Con Center | Home (Music Box) | Furiell's theme with echo | P1 |
| ASGRR | Asgrr | ASGORE | Epic and sad | P1 |
| Your Best Nightmare (Mascot Mix) | The Mascot | Your Best Nightmare | Distorted, plush squeaks | P1 |
| Megalomania.exe | Senpai Sans (Toxic) | MEGALOVANIA | Aggressive, air horns | P2 |
| Reflection | Mirror | — | Slow leitmotif on a pink music box | P2 |
| Raid Night | The Raid act | — | Generic "gamer" rock with a dubstep kick | P2 |
| RGB Overdrive | xX_Moderator_Xx | — | Chiptune metal + air horn + case fan | P2 |
| Pawsome | Furry ending | His Theme | Heroic, fluffy leitmotif | P2 |

- **Also needed (not listed in the LORE):** game over/FURRY OVER (a "Determination" vibe), GAMER OVER (a sad version of the same theme), shop (Bunny Merch / Uwu-mie), credits, the fall room ambience, the Last Lobby theme (silence + wind?). Log as soundtrack decisions in `docs/audio.md`.

#### AUD-011 — LORE SFX `[ ]`
P0→P2 · M · AUD-002
- **What:** beyond the 1:1 functions, LORE §27.2's SFX: can opening (*tssss*, save) P0 · FPS hitmarker (crit) P0 · **zipper** (Cringe Check) P0 · plush squeak (Hug/Accept Hug) P0 · *pomf* (FUR stage, FURRY OVER) P0 · off-key howl (Barky) P0 · tab closing (fast clicks) P1 · MLG air horn P1 · notification *ding* (CELL, spare) P1 · dial-up modem (R3 save) P1 · *crunch* (Doritos) P1 · **BONK** P2 · cell door (horny jail) P2 · voice chat "user left" (rage-quit) P2 · bottle bouncing (Bottle Guy) P2 · glitter spray (Furiell scaring Floofy off) P0 · tape *RIIIIIP* (refusing the Mirror) P2 · phone buzz (guild messages) P1 · audience (Mettapaw's quiz) P1.

#### AUD-012 — LORE character voices `[ ]`
P1 · M · TXT-006
- **What:** one beep per character (LORE §10): Furiell (soft and low), Barky (high little bark), Senpai Sans (bony beep), Papyrus, Sharkdyne (metallic), Floofy (default; thicker in sinister mode), Alphys (keyboard — `...is typing`), Mettapaw (electronic), Asgrr (deep), the Moderator (headset crackle), Uwu-mie (chaotic). Pepe **has no voice** (he only thinks).

#### AUD-020 — Audio license audit `[ ]`
P0 · S · D-04
- **What:** log the origin of every file in `assets/music/` and `assets/sfx/` in `assets/LICENSES.md`; flag what must be replaced before release (the "Once Upon A Time" remix and `sans_type.wav` first).

---

### FUR — Furrification (FUR meter and stages)

> Source: LORE §6.2, §8.4, §8.5 (Mirror trigger, see `FLF`). The core tension: **the easiest way to spare is to accept the hug** — going pacifist fast costs fluff.

#### FUR-001 — Meter design `[x]`
P0 · M · D-05
- **Resolved by LORE §8.4** (gain, loss, stages, HUD) and §8.5 (100% = Mirror). The proposed `docs/furry_meter.md` is no longer needed: the LORE is the reference. Any number tweaks go to `data/fur.json` (FUR-002), not the LORE.

#### FUR-002 — Single state and per-source gain `[~]`
P0 · M · FUR-001, BHL-005, BUG-27
- **Today:** each bullet adds `b.furry` (fixed 6/7/5 in `BattleSystem.cpp`) to a `float furryMeter`, and a second `FurryState::meter` is saved in parallel.
- **What:** a single **integer `fur.value` 0–100** in `GameState`, with `addFur(n, source)` applying the armor multiplier (FUR-009) and triggering stage changes/the Mirror. Base table in `data/fur.json`:

| Source | FUR |
| :--- | :---: |
| "Neutral" bullet (cans, bones, cables) | +2 to +4 |
| "Cute" bullet (hearts, UwU, glitter, plush) | +6 to +8 |
| "Very cute" bullet (hugs, petals, `:3` DMs) | +10 |
| Floofy's "free hug pellets" (R2 only) | +20 |
| ACT `Accept Hug` | +10 to +15 (spares on the spot) |
| `Accept Hug` on Furiell (ends the fight) | +30 |
| Furiell's bed | +5 (and full HP) |
| Equipping the `Fursuit Head` | +40 at once |
| Wrong answer in Mettapaw's quiz | +5 |

- **Acceptance:** the per-bullet value comes from the pattern JSON (`params.fur`); unit tests cover the multiplier, the 0–100 clamp and the Mirror trigger; an old save with both fields migrates without loss.
- **Progress (2026-10-17):** single integer `GameState::furry.value` 0–100 with `stageMax`, `applyFur(value, stageMax, delta, armorMult)` in `include/core/Fur.hpp`; `PlayerData::furryMeter` removed; old saves migrate (max of the two old fields, rounded). Per-bullet values unchanged (6/7/5). Pending: `data/fur.json` and per-pattern `params.fur`.

#### FUR-003 — KR-style HUD display `[~]` (LORE rework)
P0 · S · BUI-005, FUR-002
- **Today:** there's a `KR_PURPLE` overlay proportional to the meter and pinkish numbers (`BattleSystem.cpp:627-637`).
- **Missing (LORE §8.4):** a small **`FUR`** label after the HP bar (a sprite, like "KR"); hidden at 0%; on its **first appearance (R2)** the bar comes in trembling (`GFX-009`). On the Furry route the whole HUD switches to FLUFF (`FLF-004`).

#### FUR-004 — Loss sources `[ ]`
P0 · S · FUR-002
- **What (LORE §8.4):** `Duct Tape` −15 · ACT `Touch` on Grasstoid −10 · ACT `Shower` on Washua −10 · Shed Inn (10G) −25 · refusing the Mirror → back to **60** · save points **don't reduce it** (with FUR > 0 the save text gets the extra line `* DETERMINATION can't fix fluff.`) · Furiell zeroes FUR after Floofy's tutorial ("my glitter is hypoallergenic").

#### FUR-005 — Stages and transitions `[~]`
P0 · M · FUR-002
- **What:** 0–19 **Frog** · 20–39 **Ears** · 40–59 **Paws** · 60–79 **Tail** · 80–99 **Snout** · 100 **Mirror**. On **going up** a stage: a "pomf" of fluff (`GFX-013`) + *pomf* SFX + `* You feel... fluffier.` (in the overworld, or in battle's `FlavorText`). Going down (Duct Tape, Shed Inn) swaps the sprite back with no ceremony. Track `fur.stage_max`.
- **Acceptance:** jumping two stages in one hit shows a single transition; the text doesn't interrupt the enemy turn.
- **Partly done (2026-10-17):** stages Frog/Ears/Paws/Tail/Snout/Mirror; going up shows a pink fluff burst + flash and `* You feel... fluffier.` as the next menu flavor text (one transition per hit); reaching 100 sets `fur_mirror_pending`. Pending: the *pomf* SFX and per-stage sprites (FUR-007).

#### FUR-006 — FUR in STAT `[~]` (LORE rework)
P1 · S · MNU-003, BUG-23
- **What:** a `FUR  n` line (the LORE label, not `FURRY: n%`) + the stage name (`Ears`, `Paws`…); hidden on the Furry route (it becomes `FLF`).

#### FUR-007 — Pepe's look per stage `[ ]`
P1 · L · ART-101, FUR-005, D-15
- **What:** 5 overworld spritesheets (`pepe_frog`, `pepe_ears`, `pepe_paws`, `pepe_tail` with a tail that wags as he walks, `pepe_snout` with mint-green patches) swapped by stage; an accessory on the soul in battle per D-15.

#### FUR-008 — Stage reactions `[ ]`
P2 · M · FUR-005, NAR-027
- **What (LORE §8.4):** `* Nice ears, newcomer!` (≥ 20, generic NPCs) · Barky `Dude. Your paws are SO much better than mine.` (≥ 40) · the shops' "fandom discount" (≥ 60, SHP-008) · Senpai Sans `heh. you're one bad day away from a con badge.` (≥ 80) · a "squeak" at the end of Pepe's thoughts (≥ 80) · the mirror in Furiell's house `* It's... still you. With ears.` (≥ 20) · Dogsona & Dogsonissa spare you outright (≥ 40). Conditions on `fur.value` in the conditional dialogue system (CORE-010).

#### FUR-009 — Armor multiplier and FUR on equip `[ ]`
P1 · S · ITM-001, FUR-002
- **What:** `furMult` and `furOnEquip` fields on items: Faux-Fur Bandana ×1.25 · Fursuit Head +40 on equip and ×1.5 · Pillowcase Cape ×1.25 · Uwu-mie Armor ×2 · Blue-Light Glasses ×0.75. `furOnEquip` applies once per item instance (unequip/re-equip can't farm FUR).

#### FUR-010 — Fluff particles in the air `[ ]`
P2 · S · GFX-002
- **What (LORE §25.1):** 2–4 particles per screen in every area (`#FFB3E6`, `#FFD27F`), density proportional to FUR; off on the Toxic route.

#### FUR-011 — Cap at 99 on the Toxic route `[ ]`
P1 · S · PRG-031
- **What:** while `route.toxic_active` is true (C7-001), FUR stops at 99 and the Mirror doesn't fire: `* Your vitriol burns the fluff right off.` (once per area).

#### FUR-012 — FUR balancing `[ ]`
P1 · M · FUR-002, QA-020
- **What:** playtest so that (a) a careful player finishes the Gamer route under 100 without needing Duct Tape; (b) someone who accepts every hug reaches the Mirror around Snowdinha/Wetfall; (c) the deliberate path (Fursuit Head + bed + hugs, LORE §8.5) reaches the Mirror while still in the Ruins.

---

### FLF — Furry route: Mirror, FLUFF, SHAME and BONK

> Source: LORE §8.5, §8.6, §17, §23.2. Room and scene content lives in the `C6` epic.

#### FLF-001 — The Mirror scene `[ ]`
P1 · L · FUR-005, TXT-007, CORE-011
- **Trigger:** FUR reaches 100 **from R3 on** (Floofy's tutorial in R2 doesn't count). It fires **at the end** of the battle, cutscene or menu that caused the 100 (never mid-enemy-turn).
- **Scene:** pink screen, pulsing mirror frame, the "Reflection" track, the reflection (Pepe the Pawsome) says `* (Hey.)` / `* (You've been fighting this the whole way down.)` / `* (...You don't have to.)` → choice `Accept` / `Refuse` (soul as cursor).
- **Lock:** never on the Toxic route (FUR-011). Define the final cutoff (suggestion: it doesn't fire after entering N4) — see NAR-030.

#### FLF-002 — Refuse `[ ]`
P1 · S · FLF-001
- **What:** the Duct Tape cutscene (`* RIIIIIP.` `* You are a gamer. You are a GAMER.`, tape SFX), FUR = 60, `fur.mirror_refused += 1`. On later refusals the reflection opens with `* (...Again?)` (2nd) and `* (I'll be here.)` (3rd onward).

#### FLF-003 — Accept: the conversion `[ ]`
P1 · M · FLF-001
- **What:** the red soul leaves the chest and drops into a jar (key item `Your Old SOUL (Jar)`, can't be dropped); the **pink plush soul** takes its place (FLF-013); thought `* (You always kind of wondered.)`; `route.furry = 1`, `meta.ever_furry = 1`; the weapon automatically becomes `Your Paws` (AT 10); HP → FLUFF (FLF-004); FUR stops existing.

#### FLF-004 — FLUFF HUD (flipped bar) `[ ]`
P1 · M · BUI-005, FLF-003
- **What:** `HP` label → `FLF`; a **pink `#FF77C8` bar over `#3A1030`** that **fills right to left** (mirrored); `#FFB3E6` numbers; max = HPmax + 10 ("fluff is thick"). Applies in battle, in the C menu, in STAT and in the save box.

#### FLF-005 — SHAME damage and residual SHAME `[ ]`
P1 · M · FLF-004, BHL-017
- **What:** gamer bullets take FLUFF. Patterns marked `residualShame` leave a **gray** overlay that drains slowly (the same mechanic as KR, BHL-017).

#### FLF-006 — Healing FLUFF `[ ]`
P1 · S · FLF-004
- **What:** NPC hugs (hug encounters, FLF-009; the furries between X2's gamers), the `Hug Yourself` ACT, Pop-Tarts, plush items, `Hug Coupon` (+10 FLUFF), save points (heal FLUFF the way they'd heal HP).

#### FLF-007 — GAMER OVER `[ ]`
P1 · M · BND-010, FLF-004
- **What (LORE §23.2):** FLUFF 0 → the plush soul **loses its fur** all at once, turns back into the original red soul and **cracks** — Undertale's **original** game over, 1:1 (the joke: on this route it's the worst fate). The **`GAMER OVER`** logo; **Furiell's** voice with one of 4 lines (with `[NAME]`); back to the last save.

#### FLF-008 — Furry Pepe's battle: BONK and his own ACTs `[ ]`
P1 · M · BUI-006, ACT-001
- **What:** the FIGHT button becomes **BONK** (its own sprite); the hit is a little pink cage that drops with a `BONK` (it doesn't kill: zeroing a gamer's HP → horny jail, FLF-011). Pepe gets global ACTs added to every enemy's list: `Hug`, `OwO`, `Show Fursona`, `Wag`, `Hug Yourself` (heals). Text in NAR-025.

#### FLF-009 — Hug encounters `[ ]`
P1 · S · ENC-003, FLF-003
- **What:** on the Furry route, random encounters don't open a battle: the "!" shows up, a furry from the area hugs Pepe (`* Shysona hugs you. FLUFF restored.`), healing 3–5 FLUFF.

#### FLF-010 — Bosses become scenes per route `[ ]`
P1 · M · BFL-005, CORE-011
- **What:** a `routeOverrides: { "furry": "cutscene:<id>" }` field on encounters; Furiell, Barky, Papyrus and Sharkdyne (if not fought yet) become short scenes (C6-001). Mettapaw's quiz stays, with wrong answers costing FLUFF.

#### FLF-011 — Beating and sparing gamers `[ ]`
P2 · S · FLF-008, GFX-013
- **What:** HP 0 by BONK → **horny jail** (a pink cage drops, `BONK`, cell door, it vanishes); spare → **rage-quit** (the sprite flickers and becomes the text `* [name] has left the game.`, "user left" SFX). Nobody turns to dust; **no EXP** (G to be defined in NAR-030).

#### FLF-012 — Gamer battle background `[ ]`
P2 · S · BUI-002
- **What:** a **red** grid (the green one inverted) for the X2 fights; a dark bedroom with pulsing RGB + a green cloud for the Moderator (LORE §25.3).

#### FLF-013 — Pink plush soul `[ ]`
P1 · S · BUI-007
- **What:** a pink sprite with little ears, same movement and hitbox as the red one; `* Your SOUL is soft now.` on the first battle after the conversion.

---

### RAD — The Raid Clock

> Source: LORE §8.7, §21.3, §22.4. **Never a game over.**

#### RAD-001 — State and counting `[ ]`
P1 · M · CORE-012, D-12
- **What:** `raid.time_left` (seconds) starts when Pepe wakes up in R1 (value from `data/raid.json`, D-12); counts real play time; **pauses** in the C menu, the CELL, Settings and while the window is unfocused; keeps running in battles and cutscenes. Goes into the save; a FURRY OVER restores the saved value.
- **Acceptance:** a pause/resume unit test; the saved time and the one shown in STAT match.

#### RAD-002 — Display `[ ]`
P1 · S · RAD-001, MNU-003, MNU-004
- **What:** `RAID mm:ss` in STAT and at the top of the CELL; once it runs out, `RAID: KICKED`.

#### RAD-003 — Guild messages by threshold `[ ]`
P1 · S · RAD-001, MNU-006
- **What:** R1 `RAID STARTS IN 30:00. BE THERE OR GET KICKED. bring dew -xX_Moderator_Xx` (the phone buzzes) · 15:00 `15 MIN WHERE ARE U` · 5:00 `5 min. ur kicked if ur late. no cap.` · 0:00 `* Your phone buzzes.` / `* "Raid started without you. Kicked."` / `* ...You feel strangely okay with that.` **Deferred** if the threshold lands in a battle/cutscene (it shows up back in the overworld).

#### RAD-004 — Queries for the endings `[ ]`
P1 · S · RAD-001
- **What:** `raid.expired` and `raid.time_left` read by `END-006` (per-ending lines and the extra speedrun line).

#### RAD-005 — The Furry route twist `[ ]`
P2 · S · RAD-001, C6-002
- **What:** when the Barrier falls, the clock **stops** and `RAID RELOCATED. MT. DEW. FURRY OUTBREAK. EVERYONE ON VOICE. -xX_Moderator_Xx` arrives.

---

### SHP — Shops and economy

> Source: LORE §20.5. Target economy: **~150–200G** on a no-kill Gamer run.

#### SHP-001 — 1:1 shop screen `[ ]`
P1 · L · TXT-007, ITM-001
- **What:** Undertale's screen: the shopkeeper's scene/portrait on top; a `Buy / Sell / Talk / Exit` menu at the bottom right; a priced list; the shopkeeper's speech box; G in the corner. **Sell only works at the Uwu-mie Shop** (1:1 with Temmie); elsewhere, the shopkeeper refuses with a joke (data).
- **Replaces:** the old `PRG-003`.

#### SHP-002 — Commission Bake Sale (R10) `[ ]`
P2 · M · SHP-001
- **What:** a spider bake sale parody, with no visible seller: `Commissioned Donut` (7G, +12 HP) and `Commissioned Cider` (18G, +24 HP); sign `* "All proceeds go to spider artists. Slots open. ETA: yes."`. **Replaces:** the old `PRG-004` (NFT Bake Sale, dropped).

#### SHP-003 — Bunny Merch (S5) `[ ]`
P1 · M · SHP-001, SHP-008
- **What:** a bunny fursuiter; sells `Snow Cheese Bread` 12G, `Gamer Fuel 9000` 30G, `Duct Tape` 25G, `Fingerless Gloves` 50G, `Tryhard Headband` 50G. A 10% "fandom discount" at FUR ≥ 60, with Pepe's thoughts `* (Fandom discount? You don't need that.)` / `* (You take the fandom discount.)`.

#### SHP-004 — Shed Inn (S5) `[ ]`
P1 · S · SHP-001, FUR-004
- **What:** a night + a trim for 10G: full HP, **FUR −25**, a black fade and `* You wake up smooth. Unsettlingly smooth.`; the innkeeper offers "deshedding" for gamers.

#### SHP-005 — Uwu-mie Shop (W4) `[ ]`
P2 · M · SHP-001
- **What:** `Uwu-mie Flakes` 2G; `Uwu-mie Armor` 1000G, **−100G per FURRY OVER** in the current playthrough (a counter in the save); accepts Sell; the joke `uwu-mie go to colleg!!`.

#### SHP-006 — Grillby's LAN House (S5a) `[ ]`
P2 · S · SHP-001
- **What:** 1G/hour of PC time (joke: `* You play for an hour. Your rank drops. Grillby doesn't judge.`) and fries (an item with no entry in LORE §20.1 — define it in NAR-030). Grillby doesn't talk; he points at the `NO RAGE QUITTING` sign.

#### SHP-007 — Economy balancing `[ ]`
P1 · S · SHP-003, QA-020
- **What:** check in playtest that a no-kill Gamer run collects ~150–200G (enemy G from LORE §18), that it buys 1 weapon **or** 1 armor at Bunny Merch, and that the Uwu-mie Armor is out of reach without repeated FURRY OVERs.

#### SHP-008 — Price modifiers `[ ]`
P2 · S · SHP-001, FUR-002
- **What:** final price = base × conditional modifiers (FUR ≥ 60 → −10%; Furry route → optional welcome discount).

---

### END — Endings, epilogues and credits

> Source: LORE §22. Routing via `PRG-031`.

#### END-001 — Ending router `[ ]`
P1 · M · PRG-031
- **What:** on reaching N5 (or the end of X3 on the Furry route), decide `ending.type` (1 Gamer · 2 Gamer Clean Run · 3 Toxic · 4 Furry) and run the matching sequence. Write `meta.last_ending` (MET-005).

#### END-002 — Gamer ending `[ ]`
P1 · L · C5-021, C5-022, CORE-011
- **What:** Asgrr spared → Floofy **bans** him (`* ASGRR has been banned from the server.`, `con.asgrr.state = 3`) and steals the jars → the **Mascot** fight; afterward, Pepe crosses the Barrier **alone**, with his own soul; the Barrier stays up. (The "Asgrr killed" case isn't in the LORE → NAR-030.)

#### END-003 — Clean Run variant `[ ]`
P1 · M · END-002
- **What:** no kills: once the Mascot is beaten, the 6 souls go back to their owners; **Barky goes up with Pepe**; at the raid, the guild exchange from LORE §22.1 (`who's the wolf` / `bro brought a wolf main??` / Barky: `I'm... I'm the DPS. I think I always was.`) → `* (He's actually good.)` `* (He tops the damage meter. You don't mention the zipper.)`

#### END-004 — Epilogue phone call `[ ]`
P1 · M · END-002
- **What:** who calls depends on who's alive (LORE §22.1 table, with full lines): Furiell → **Queen Furiell** · otherwise Papyrus → **King Papyrus** · otherwise only Sharkdyne · otherwise only Alphys (`...is typing` → `nvm`) · nobody → `* But nobody logged in.`

#### END-005 — "Pepe at home" scene `[ ]`
P1 · M · END-002, ART-160
- **What:** the basement bedroom, monitor glow, bottles; he plays the raid or got kicked (RAD-004); his mom knocks: `dinner`; he opens the gallery, scrolls to a photo from the Underground and closes it fast: `* (...Whatever.)`

#### END-006 — Raid clock lines per ending `[ ]`
P1 · S · RAD-004
- **What (LORE §22.4):** Gamer (plays silently / kicked `* (Whatever.)`) · Clean Run (brings Barky / kicked, and Barky starts the two-member "Shadow Fang Pack" guild) · Toxic (top DPS) · Furry (the raid came to him) · **speedrun** (D-12 threshold, any ending): `* Your guild asks how you got back so fast.` `* (Skill issue.)`.

#### END-007 — Toxic ending `[ ]`
P2 · M · C7-005
- **What:** Pepe takes the 6 souls, crosses, makes it in time, **top DPS**, posts `gg ez`; `xX_Moderator_Xx` invites him to be co-leader and he accepts; final scene in the chair, a green cloud, bottles up to the ceiling: `* You haven't showered in a while.` `* It's a ritual now.`; an empty title with no music.

#### END-008 — Furry ending ("Pepe the Pawsome") `[ ]`
P2 · L · C6-021, END-009
- **What:** the furries climb up for the first time in 12 years (they hug everything, then complain about fursuit heat); the 4 epilogue cards from LORE §22.3; final scene: the Moderator, alone, quietly **follows** a clip of the mascot, `* (Even the Seven have secrets.)`

#### END-009 — Epilogue card engine `[ ]`
P2 · S · FRT-002
- **What:** reuse the intro engine (400×220 image + typed text + fade) for the epilogues (`bg/ending_*.png`).

#### END-010 — Credits `[ ]`
P2 · M · DST-003
- **What:** a crawl with the authors, third-party fonts, sounds and music, and the unaffiliated fan-parody notice; it ends with `* Leave a review?` (paying off Furiell's running gag).

---

### MET — Meta-narrative and title screens

> Source: LORE §24. Depends on meta-persistence (`SAV-005`, already implemented).

#### MET-001 — Reload detection `[ ]`
P2 · M · SAV-005
- **What:** session flags `session.reloaded_after_furry_over` and `session.reloaded_after_kill` (a kill happened since the last save and the player loaded). The basis for Floofy's lines.

#### MET-002 — Reload lines `[ ]`
P2 · S · MET-001, NAR-012
- **What:** Floofy (if in the Ruins) after reloading post-FURRY OVER: `Did you just reload? I saw that. No shame, though. I'd do it too.` · kill and reload: `You killed them and loaded your save. Deleting the evidence? Gamer behavior.`

#### MET-003 — Reset lines by last ending `[ ]`
P2 · S · SAV-005
- **What:** any ending → Senpai Sans in S1 `heh. you look like you've seen this episode before.` · after Toxic → Floofy in R2 `You again? You know what you did. I have screenshots. I have NO shame about showing them.` · after Furry → Furiell in R3 `...Have we met? You smell familiar.` · `meta.resets ≥ 3` → Sans at Grillby's `you ever feel stuck in a loop? like a gacha banner that never ends?`

#### MET-004 — Same name as the previous game `[ ]`
P3 · S · FRT-007
- **What:** `meta.last_name`; confirming the same name: `* Again? You must really love this name.`

#### MET-005 — Title by last ending `[ ]`
P2 · M · FRT-003, END-001
- **What:** none → floating logo on black · Gamer → Pepe in the basement room · Clean Run → Pepe and Barky in headsets · Furry → Pepe the Pawsome waving · Toxic → just the logo, **no music**.

---

### LOR — Content-rule compliance

> Source: LORE §2. The rules apply to text, final art **and placeholders**. No content item (C1–C7, ART) is done until it passes them.

#### LOR-001 — Content checklist `[ ]`
P0 · S · —
- **What:** `docs/content_rules.md` (or a new section of `writing_style.md`) turning LORE §2.2's fixed rules into a review checklist: nothing visually explicit (ceiling: mosaic and closed objects); **no children**, references to children or childlike looks, and no fandom vocabulary tied to minors; no real animals; no real people (including photo-meme faces, YouTubers, streamers) and no real events/sites/conventions; the "dragon excuse" only as the narrator cutting someone off — the character never appears or is described; Pepe is the "classic Pepe," no political iconography.
- **Acceptance:** the checklist is linked from `README.md` and cited in the acceptance of every C*/ART item.

#### LOR-002 — Audit existing text `[ ]`
P0 · M · LOR-001
- **What:** review against LORE §2.2 all text already written: `src/**` (hardcoded strings), `data/dialogue/**`, `assets/lang/en.json`, `ROTEIRO.md`, `notes.txt`, the name table in `NameSelectScene.cpp`. Already known: "Midwest FurFest 2014" (a real convention → FurNation Con '14), the intro's "yiffing the GAMERS" (BUG-29), the `GABEN` proposed in the old FRT-008 (a real person).

#### LOR-003 — Audit existing art `[ ]`
P0 · S · LOR-001
- **What:** `assets/img/lore4.bmp` has a **real face** ("BEGONE!") → redraw (ART-170, P0). Review the other intro cards, `lore10/` and any placeholder with a photo-meme face.

#### LOR-004 — Adult proportions in every design `[ ]`
P0 · S · LOR-001
- **What:** an acceptance criterion for every sprite/portrait: adult proportions (LORE §25.1). Monster Kid **has no parody**; no "small" NPC with a childlike look (Uwu-mie and Napstablurk included: stylized adults).

#### LOR-005 — Automatic text lint `[ ]`
P2 · M · TXT-010, QA-012
- **What:** `tools/content_lint` scans `data/dialogue/**` against a list of blocked terms (kept out of the public repo if needed) and fails the validation build when it finds one.
- **Language (D-03):** the same lint flags text that isn't English — accented characters (`ã`, `ç`, `é`…) and a short list of common Portuguese words — and player-visible strings hardcoded in C++.

#### LOR-006 — Real brand register `[ ]`
P3 · S · D-14
- **What:** `docs/brands.md` with every real brand used (Mountain Dew, Doritos, Discord, Minecraft…), where it shows up and the release alternative.

#### LOR-007 — "Closed object" rules `[ ]`
P1 · S · OVW-011
- **What:** every "suspicious" object (the 18+ curtain, the laptop tab, the `Homework` folder, the `study materials` box, Furiell's locked room, the 18+ server) **never opens**: examining again changes the reaction, never the contents (LORE §2.3). Test: none of these objects has an "open" state in the data.

#### LOR-008 — Per-room density and tone review `[ ]`
P1 · S · NAR-005
- **What:** acceptance for every room: ≥ 1 funny examinable; a pace of ~1 laugh every 20 s; ~70% of the gross-out jokes on furries and ~30% on gamers (the gamers concentrated on the surface side: messages, Pepe's room, The Raid); every main character gets a moment of dignity (LORE §2.4). Jokes written in the native meme format (LORE §2.3, rule 6).

---

### NAR — Script and writing

> **LORE v2** already settles most of the narrative "what." These items cover what still needs **writing** (complete lines, variations, flavor text) and the move into `data/dialogue/en/*.json`. All in English, in the `docs/writing_style.md` format.

#### NAR-001 — Unify the lore `[~]`
P0 · M · —
- **Done:** `docs/LORE.md` v2 is the canonical source, now written in English; `README.md` and `ROTEIRO.md` already point to it (changes not committed yet).
- **Missing:** rewrite, or mark as "superseded by LORE §N," the `ROTEIRO.md` sections that conflict (Weeaboos as a faction and the Flame Wars in §2, Barky as "Ruins Boss" in §3, the name table in §6); mark `notes.txt` as obsolete; swap the `LoreScene.cpp` cards (FRT-002).

#### NAR-002 — Barky fight script `[~]`
P0 · M · D-06
- **In LORE §19.1:** intro, Check, 6 ACTs with responses, patterns, spare, death.
- **Still to write:** the encounter text (`* Barky blocked your path!` or a scene equivalent), flavor text (`default`, `lowHp`, `spareable`, `blushed`), 10–15 speech bubbles (including the gamer flashbacks `Fear the Shadow Fang Pa— one more match, mom— ...what?`), a response to each item used, spared-NPC lines in R9/R10 (recruiting Pepe into the "Shadow Fang Pack").

#### NAR-003 — Furiell's scenes (Ruins) `[~]`
P0 · L · D-06
- **In LORE §10.2, §12, §19.2:** key lines, the R3 introduction, the Dummy, the R6 hand-holding, the R7 hallway, the R11 house, the R12 basement, the 13 spare lines, death, the final hug.
- **Still to write:** the living-room conversation (the equivalent of Toriel's "when can I go home?", with choices), the basement-walk lines (3–4 stops, like Toriel's), the sleeping scene and the Glitter Pop-Tart on the floor, the reactions to each Dummy result, and FUR-based variants (she gets "cozier" the fluffier Pepe gets).

#### NAR-004 — CELL: calls and per-room replies `[~]`
P1 · M · NAR-003
- **In LORE §21:** Furiell's 5 options (one line each), her voicemail, the forced calls, the automatic messages and Papyrus's 4 content tips.
- **Missing:** per-room variants of `Say Hello` / `Puzzle Help` / `About Yourself` (R7–R12) and Alphys's hints outside the quiz.

#### NAR-005 — Examinables for every room `[ ]`
P1 · L · C1..C5
- **What:** complete the examinable tables in LORE §12–16 so that **every room** has ≥ 1 funny object (LOR-008), with 2nd/3rd-look variants where it makes sense (rule of three), and every save point text that's still missing.

#### NAR-006 — Bestiary: complete text `[ ]`
P0 · L · C1-020, C2-020, C3-020, C4-020, C6-020
- **In LORE §18:** stats, Check, ACTs, patterns, bubbles.
- **Missing, per enemy:** encounter text (alone and "and co."), flavor text (`default`, `lowHp`, `spareable`), result text for ACTs that don't have it yet, spare/death lines, the response to FIGHT/MISS.

#### NAR-007 — Floofy's script `[~]`
P1 · M · —
- **Resolved:** "Trollowey" was dropped; the Flowey parody is **Floofy** (LORE §6.5, §10.10).
- **Still to write:** the full R2 tutorial (every box up to the `HUG.` ring), the R13 variants (spared / killed / hugged Furiell), S7, the conversion exit line (`Huh. You did it yourself. ...No shame. I like that.`), the N5 finale and the C6 stream.

#### NAR-008 — Ruins exit (R13) `[~]`
P0 · S · D-06
- **In LORE R13:** Floofy's line for players who spared her, and the transition to the forest.
- **Missing:** variants for a dead Furiell and a hugged Furiell (`ruins.furiell.hugged`).

#### NAR-009 — Boss opening lines `[ ]`
P1 · S · —
- **What:** a box encounter line for every boss and event (Barky, Furiell, Papyrus, Sharkdyne, Mettapaw, Asgrr, Mascot, Senpai Sans, Moderator) and each one's victory/end text.

#### NAR-010 — System names and jargon `[x]`
P1 · S · D-07
- **Resolved by LORE §8:** `LV` = Level of Vitriol, `EXP` = eXcessive Posting, `G` = can tabs, level-up `* Your LEVEL OF VITRIOL increased.`, save `* [something about the place] fills you with DETERMINATION.`, emptied area `* But nobody logged in.`, menu `ITEM / STAT / CELL`.

#### NAR-011 — Game over text `[x]`
P0 · S · BND-010
- **Resolved by LORE §23:** FURRY OVER (Asgrr's voice, 6 lines + the Barky, 3+ FURRY OVERs and Toxic variants) and GAMER OVER (Furiell's voice, 4 lines). Implemented in `BND-010`, `BND-011`, `FLF-007`.

#### NAR-012 — Meta lines `[x]`
P2 · M · SAV-005
- **Resolved by LORE §24.** Implemented in `MET-*`.

#### NAR-013 — Endings `[x]`
P2 · L · PRG-031
- **Resolved by LORE §22** (Gamer, Clean Run, Toxic, Furry; the phone call with full lines; the clock; the Furry epilogue cards). Implemented in `END-*`; gaps in `NAR-030`.

#### NAR-014 — Writing style guide `[x]`
P1 · S · —
- **Done:** `docs/writing_style.md`. **Keep it in sync** with LORE §2 (the file has uncommitted local changes, and it's still written in Portuguese — translating it is a separate call).

#### NAR-015 — Hall of Posts (layer 6) `[x]`
P1 · M · C5-001
- **Done in LORE §16:** 8 printed posts from `King_Asgrr_Official` (the con's past, the Free Hugs era, the cameras, Chara, Azzy's compilation, the grief, the pinned Decree with Furiell's reply, the "very large jar") with residents' comments. Implementation in C5-001.

#### NAR-016 — Librarby: the gamer version `[~]`
P2 · S · C2-007
- **Done in LORE §13:** "A History of the Great Yiff War," "The Seven Moderators: A Biography," "Mt. Dew: A Geological Survey."
- **Missing:** 1–3 more small-town library gags (target: 4–6 books).

#### NAR-017 — Napstablurk `[ ]`
P2 · S · C1-023
- **What:** encounter dialogue and overworld lines (a lurker from the NSFW channel, lo-fi beats nobody listens to, formerly `xX_AFK_Xx`).

#### NAR-018 — Senpai Sans and Papyrus (Snowdinha) `[ ]`
P1 · L · C2-*
- **What:** S1 (the branch, the footsteps, the air horn, Papyrus's introduction), S2 (the station), S3/S4 (the puzzle lines and the fake chat; S4's give-up line is done in the LORE), S5 (townsfolk), **S5a** (Grillby's: the layer 4 reveal), S5c (the house), S6 (pre-fight, fight, post-fight, the 3 Streaming Shed captures), S7. Sans always lowercase with `-desu`/`ne`/`nani`; Papyrus always ALL CAPS.

#### NAR-019 — Sharkdyne `[ ]`
P1 · M · C3-*
- **What:** W1 (the DM harpoon), W5 (the chase), W6 (the speech `For twelve years we've been stuck down here, smelling each other!`), fight lines, the escape to Hotland, the faint in H1 and the **friendship scene** after `Cold Water` (not written in the LORE).

#### NAR-020 — Alphys and Mettapaw `[ ]`
P1 · M · C4-*
- **What:** arriving at the lab (`* You received 400 notifications.`, the 400 rules), the experiment log, the quiz hints over the CELL (`...is typing` → a letter), Mettapaw's host lines per question, right answer and wrong answer.

#### NAR-021 — Asgrr `[ ]`
P1 · M · C5-021
- **What:** tea in N3, the throne-room conversation, the walk to the Barrier, the responses to `Talk` ×3 and to `Refuse` (changes the final line), spare/kill, the ban scene.

#### NAR-022 — The Mascot `[~]`
P1 · M · C5-022
- **Done in LORE §19.7:** the text for all 6 soul ACTs and the ending (spare `...why? you're so cringe.` / kill `...gg.`).
- **Missing:** the per-phase lines.

#### NAR-023 — The Last Lobby: the verdict `[ ]`
P1 · M · PRG-031
- **What:** Senpai Sans's LV/EXP explanation (LORE §8.2) and a performance-based verdict: 0 kills, few kills, many kills, with a nod to the number of FURRY OVERs (`meta`).

#### NAR-024 — Automatic messages `[x]`
P1 · S · —
- **Written in LORE §21.3** (the guild by threshold, Mom by area, Papyrus's content tips). Implemented in `RAD-003`, `MNU-006`.

#### NAR-025 — Furry route: all the changed text `[ ]`
P2 · L · C6-*
- **What:** NPC welcome lines (`you've got good fluff`), the 4 scenes replacing fights (Furiell, Barky, Papyrus's collab stream — its key lines are in the LORE —, Sharkdyne recruiting), the quiz with FLUFF, the Con Center (the King's tears, the jar, the Barrier falling, Floofy's stream), X1–X3, furry Pepe's ACTs (`Hug`, `OwO`, `Show Fursona`, `Wag`, `Hug Yourself`) against each gamer.

#### NAR-026 — Toxic route: all the changed text `[ ]`
P2 · M · C7-*
- **What:** emptied-area text, NPCs who fled (locked doors, notes), the `* Determination.` save texts, Senpai Sans's fight, Floofy's offer and the co-leader ending.

#### NAR-027 — FUR reactions `[ ]`
P2 · M · FUR-008
- **What:** beyond LORE §8.4's fixed lines, 1–2 reactions per area for each stage (Snowdinha townsfolk, Uwu-mies, Alphys), keeping the rule: nobody comments explicitly on Pepe's secret.

#### NAR-028 — Name easter eggs `[ ]`
P2 · S · FRT-008
- **What:** merge the ROTEIRO §6 table with LORE §30.2 (`FURIEL`/`FURIELL`, `BARKY`, `FLOOFY`, `AZZY` darkens and allows, `FROG`/`PEPE` frog soul, `YIFF` refused, `OWO`, `CHARA`) into `data/names.json`; `TORIEL` → `Furiell has already trademarked this name.` stays.

#### NAR-029 — Pepe's thoughts and the secret `[ ]`
P1 · S · —
- **What:** a register of every `* (...)` line from Pepe (he **doesn't speak**), including the 5 subtle hints about his secret (digitigrade legs, "Good stitching.", the curtain 3×, the fandom discount, the Mirror) and the body pillow in Sans's room. Criterion: no hint is explicit; the game never says he likes it.

#### NAR-030 — LORE gaps to resolve `[ ]`
P1 · S · —
- **What:** answer (and feed back into the LORE) the points v2 doesn't define:
  1. **Asgrr killed:** does Floofy still absorb the souls and become the Mascot? (LORE §22.1 only describes the spared case.)
  2. **Alphys as "interim Queen"** (LORE §10.7) vs the phone call table (§22.1): which condition applies when.
  3. **Fries** at Grillby's: an item with no entry in the consumables table.
  4. **Gamer route variants** "by kill count": besides the phone call and the Clean Run, what changes (Sans's verdict? the final scene?).
  5. **Sharkdyne's friendship** after `Cold Water`: scene, reward and flag.
  6. **Hug encounters** on the Furry route: which furries per area.
  7. **The Mirror's cutoff:** how late the conversion can still happen (suggestion: before N4).
  8. **EXP/G for the gamers** in The Raid.
  9. **Faux-Fur Bandana:** which room of Furiell's house.
  10. **Raid clock tuning** (D-12).
  - *Resolved during the English rewrite:* the Phone Camera is in the W3 trash can.

#### NAR-031 — English copy pass `[~]`
P0 · M · D-03, TXT-010
- **Why:** the game is 100% English (D-03) and the jokes land best in their native format. Every line has to exist as final English copy before it goes into data.
- **Done in the English LORE:** Papyrus giving up in S4, the chat making Papyrus an honorary furry, Mettapaw deactivating their account, the Clean Run raid scene and the Clean Run "kicked" line, the full epilogue phone calls, the 4 Furry epilogue cards, the King's Citrus Blast can, Furiell's voicemail, Papyrus's 4 content tips, the stained glass of the Seven Moderators, all 6 jar labels, the 8 Hall of Posts posts, 3 Librarby books, the Mascot's 6 soul-ACT lines, all temperatures in °F.
- **Still to write:** everything listed as "still to write" in NAR-002..NAR-027, plus system text (STAT, CELL, shop, save, Settings and instruction screens, the FUR lock line per area, rage-quit and horny jail messages).
- **Acceptance:** `data/dialogue/en/**` covers every line the game shows; the lint (`LOR-005`) finds no accented characters or Portuguese words in the dialogue files; a full playthrough shows no non-English text. Feed new lines back into the LORE.

---

### C1 — Area 1: The Ruins of Con '14

> **Source:** LORE §12 (rooms), §18.1 (bestiary), §19.1–19.2 (bosses). **Target time:** 8.5 min · **Music:** "Ruins (lo-fi beats to get lost to)" · **Palette:** purple `#3B1F5C`/`#5E3A87` + neon green `#7CFF3A` (LORE §25.2).
> **Concept:** FurNation Con '14's original halls, sunk 12 years ago: cracked purple stone + convention carpet, faded banners, canvas booths, the Art Show with its 18+ section behind a black curtain. The smell gets described in almost every room.
> **Acceptance shared by every room:** built in Tiled (OVW-003), text in `data/dialogue/en/ruins.json`, passes LOR-008, LORE §28 flags written.

| Room | Time | Item | Room | Time | Item |
| :--- | :---: | :--- | :--- | :---: | :--- |
| R1 The Fall Room | 0:20 | C1-001 | R8 Notices, CRT and Art Show | 0:40 | C1-008 |
| R2 Floofy | 1:00 | C1-002 | R9 Barky's Arena | 1:30 | C1-009 |
| R3 Entrance | 0:30 | C1-003 | R10 Bowl, Bake Sale, Napstablurk | 1:00 | C1-010 |
| R4 RGB Switches | 0:30 | C1-004 | R11 Furiell's House | 1:00 | C1-011 |
| R5 Fursuit Dummy | 0:45 | C1-005 | R12 Basement + Furiell's fight | 2:00 | C1-012 |
| R6 Cable Maze | 0:20 | C1-006 | R13 Exit | 0:30 | C1-013 |
| R7 Long Hallway | 0:30 | C1-007 | | | |

#### C1-001 — R1: The Fall Room `[ ]`
P0 · M · OVW-003, ART-120, RAD-001
- **What:** Pepe wakes up on a bed of snack wrappers under a beam of light (a 3–4 frame getting-up animation); thought `* (Ugh... where's my wireless mouse? And why does it smell like body spray and wet dog?)`; the phone buzzes with the raid message and the clock starts (RAD-003).
- **Save:** `* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.` (a save in the fall room is a script decision, unlike Undertale). **Examinable:** can `* Citrus Blast, 2014 edition. A vintage.` (sets up the Citrus Blast running gag).

#### C1-002 — R2: Floofy and the fake tutorial `[ ]`
P0 · L · BHL-004, BFL-001, ART-102
- **What:** the first "battle": Floofy introduces the "free hug pellets" (little hearts that deal damage and **+20 FUR** each); the **FUR bar shows up for the first time, trembling** (FUR-003); he surrounds Pepe with a ring of pellets: `HUG.`; Furiell shows up and drives him off with a **glitter spray** (SFX), heals Pepe and zeroes FUR ("my glitter is hypoallergenic"). Teaches soul movement. `ruins.floofy.met = 1`. The Mirror does **not** fire here.
- **Art:** Floofy idle, talking, sinister (zipper open showing only darkness and two white eyes) and sinking into the ground.

#### C1-003 — R3: Ruins Entrance `[ ]`
P0 · M · OVW-003, ART-120
- **What:** Furiell introduces herself (NAR-003); double staircase; banners `FurNation Con '14 — Welcome!` and, underneath, `Deodorant is NOT optional`; a pile of Dorito bags with a *crunch* (OVW-014). From here on, the Mirror can fire.
- **Save:** `* The distant hum of dial-up internet fills you with DETERMINATION.` (modem SFX).

#### C1-004 — R4: RGB Switches `[ ]`
P1 · M · OVW-012
- **What:** a tutorial puzzle; the right switches are marked with glitter; note `Not this one. — Ruins Administration (not Furiell)`. **Sign:** `* "Only the fearless may proceed. Brave ones, foolish ones. Both walk not the path of the noob."`

#### C1-005 — R5: Fursuit Dummy `[ ]`
P0 · M · BFL-001, ACT-005, NAR-003
- **What:** the battle tutorial; Furiell teaches **Talk** and **Accept Hug** (`You can always accept a hug, my child. It is the fastest way to make friends.` — plants the love-bombing). Reactions to each outcome: Talk → happy; Accept Hug → beaming; FIGHT → `Ah... the dummy is for hugging!`; flee → `...that is also a choice.` `ruins.dummy.result` (0 none · 1 talk · 2 fight · 3 flee · 4 spare · 5 hug).

#### C1-006 — R6: Cable Maze `[ ]`
P1 · M · CORE-011, OVW-012
- **What:** network cables that shock you (the spikes parody); Furiell holds Pepe's "hand" and guides him with scripted movement: `* Her paw is very soft. Suspiciously soft.` `* (Good stitching.)` (a hint at his secret, NAR-029).

#### C1-007 — R7: Long Hallway `[ ]`
P1 · M · CORE-011, MNU-004
- **What:** the independence test; Pepe gets the **CELL** (a flip phone with paw stickers, `ruins.cell.given`); behind the pillar, Furiell's tail sticks out: `* (You can see her tail. She can see you seeing her tail.)`. After the room, a forced call: `...You don't have allergies to faux-fur, do you?`

#### C1-008 — R8: Notices, CRT and Art Show `[ ]`
P0 · M · OVW-011, ENC-003, LOR-007
- **What:** notice `* NOTICE FROM RUINS ADMINISTRATION: Please do not shed in the ventilation shafts. Violators will be banned from the Minecraft server.` (fixes BUG-19) · CRT `* A blue screen of death. The error message is in Comic Sans.` · the **Toy Laser Pointer** behind the CRT (`ruins.laser.taken`) · the Art Show's 18+ curtain with 3 reactions (`* You decide to respect the curtain.` → `* You respect the curtain. For a while.` → `* (...No.)`, `ruins.curtain.looks`) · the Art Show table `* A bid sheet. Highest bid: 400G. For a drawing of a "very tired raccoon". You don't ask.` · random encounters (budget 1: Froggsona, Shysona, Modsmal).

#### C1-009 — R9: Barky's Arena `[ ]`
P0 · M · NAR-002, CORE-011, C1-021
- **What:** ROTEIRO Scene 3: an echoing howl, Barky drops from the ceiling into a crouch, his introduction line, the fight (C1-021). **Examinable:** lamp `* Barky's natural enemy. Or his moon. He hasn't decided.` Afterward: spared → an NPC recruiting Pepe into the Shadow Fang Pack; killed → a pile of synthetic fur + a flag. A test version exists in `OverworldScene` today (BUG-18): move it to data.

#### C1-010 — R10: Bowl, Commission Bake Sale and Napstablurk `[ ]`
P1 · L · OVW-011, SHP-002, C1-023
- **What:** the bowl `* "Take one Dorito."` (take 4 → it tips over, `ruins.bowl.broken`) · the Commission Bake Sale (SHP-002) · Napstablurk in the hallway (optional, C1-023) · the **mod rock**: `Hey. No pushing without permission. ...Permission granted.`; pushing without asking → `* You have been muted for 5 seconds.` and controls lock for 5 s · random encounters (budget 1: Lurkeye, Reply Guy, Grasstoid) · two forced calls (`Do you prefer glitter or MORE glitter? ...No reason.` and `I put something in your closet. Just in case. No pressure!`).

#### C1-011 — R11: Furiell's House `[ ]`
P0 · L · NAR-003, ART-120, OVW-011
- **Rooms:** living room, hallway with a mirror, Pepe's room, her room (locked: `* It's locked. You hear a printer.`), stairs to the basement.
- **Examinables:** mirror `* It's you!` → FUR ≥ 20 `* It's... still you. With ears.` · book `* "Fanfics of Questionable Quality, Vol. 7". You read one sentence. You put it down very carefully.` · laptop `* She closes a tab very fast.` (sets up the running gag) · letterhead `* "RUINS ADMINISTRATION". In her handwriting. Hm.` · Pepe's closet: the **`Fursuit Head`** (`* A spare fursuit head. It's your size. Why is it your size?`, `ruins.fursuit_head.taken`) · bed `* You nap under the faux-fur blanket. HP fully restored. FUR +5.`
- **House items:** `Glitter Pop-Tart` (full HP), `Faux-Fur Bandana` (room to be decided, NAR-030). Line: `Do you prefer glitter or MORE glitter?`.

#### C1-012 — R12: Basement and Furiell's fight `[ ]`
P0 · XL · NAR-003, C1-022
- **What:** the basement hallway with her lines (`Go upstairs. Your room has a beanbag. And a fursuit head. No pressure.`) and the fight (C1-022). Break this item down before starting (XL).

#### C1-013 — R13: Exit `[ ]`
P0 · M · NAR-008
- **What:** Floofy comments (`You spared her? Wow. No shame. ...I respect that.` + variants); the iron door opens onto the frozen forest under a neon aurora; Senpai Sans at the sentry station in the distance.
- **Save:** `* The cold air and the faint smell of ozone fill you with DETERMINATION.`

#### C1-020 — Ruins bestiary `[ ]`
P0 · L · NAR-006, ART-110, BHL-004
- **What:** data (`data/enemies/*.json`), sprites and patterns for every common enemy (LORE §18.1). Replaces the old proposal (Frogposter/Soytoid dropped).

| Enemy (parody) | HP/AT/DF · EXP/G · FUR | ACTs → spare | Patterns | Note |
| :--- | :--- | :--- | :--- | :--- |
| Fursuit Dummy (Training Dummy) | 15/0/−5 · 0/0 | Talk · Accept Hug (+10) | — | R5 tutorial |
| Froggsona (Froggit) | 30/4/2 · 3/2 · +3 | Compliment · Threaten (flees) · Accept Hug | `colon_three_hop`, `fly_swarm` | thought `* (It looks like you. A little. Don't think about it.)` |
| Shysona (Whimsun) | 10/5/0 · 2/0 · +6 | Console · Terrorize (flees) · Accept Hug | `sorry_moths` | — |
| Modsmal (Moldsmal) | 50/1/0 · 3/3 · +2 | Imitate · Flirt | `ban_gel` | — |
| Lurkeye (Loox) | 50/6/4 · 5/5 · +2 | Don't Pick On · Pick On (hits harder once) | `seen_rings` | — |
| Reply Guy (Migosp) | 40/6/4 · 5/3 · +2 | Talk (only **alone**) | `well_actually` | needs BFL-004 + the "alone" condition (ACT-004) |
| Grasstoid (Vegetoid) | 72/6/6 · 6/4 · +2 | Touch (**FUR −10**) · Dinner (greens heal) | `grass_blades` (greens heal) | BHL-009 |
| Napstablurk (Napstablook) | 88/10/10 · 0/0 · +1 | Cheer ×3 · Flirt | `seen_tears` (1 damage) | optional, C1-023 |

#### C1-021 — Barky's fight (LORE §19.1) `[~]`
P0 · M · ACT-002, BHL-014, ART-111
- **Done:** the base fight with Check, Cringe Check, Offer Dew, Debate Lore, `uwu_rain` and `dew_bounce` (data in `barky.json`).
- **Missing (LORE rework):** Check `* BARKY - ATK 4 DEF 2` / `* Alpha Wolf of the Ruins. The zipper says otherwise.`; Cringe Check +50 mercy with a 2nd response (`...I KNOW, okay? I KNOW.`) and the zipper SFX; Offer Dew with the gamer flashback (`Citrus Blast 2012?! ...wait, how do I know that flavor?`); **Laser** (requires the Toy Laser Pointer: spareable + skips an attack); **Accept Hug** (spare + FUR +15); the `howl_wave` pattern after Cringe Check; spare `You can tell no one about the zipper. Pack secret.`; death `* Only a pile of synthetic wolf fur remains.` + a **red spark** flying off (BND-003); the FURRY OVER variant `...welcome to the pack? no? ok.`; grid + fluorescent tube background; "Alpha Wolf (Not a Phase)" music.

#### C1-022 — Furiell's fight (LORE §19.2) `[ ]`
P0 · XL · BHL-004, BHL-020, ART-111
- **What:** HP 250 · AT 6 · DF 1 · EXP 150. Before: `Every gamer who leaves comes back worse. Angrier. Lonelier. Filmed. ...Prove to me you can survive up there.` ACTs: Check, Talk (`* You talk about video games. She nods. She is not listening.`), **Accept Hug** (ends the fight immediately, FUR +30, `ruins.furiell.hugged`, `...You'll stay a little longer, then?`).
- **Repeated spare:** the 13 lines in order up to `...No. You'd be unhappy down here. Even with the fursuit head.`; the spared ending with a hug (`* It's warm. It's VERY fluffy. It smells like lavender and fursuit.`, `...But please. Leave a review.`).
- **Patterns:** `fluff_balls` (+6), `glitter_hands` (+6), `hug_ring` (+10). **1:1 rule:** with Pepe at HP ≤ 4 the bullets swerve around him (BHL-020).
- **Death:** `You... really think I'm that cringe...?` → `...so you really are... just a gamer...` → plush dust. Plain black background; "Heartache (but Fluffier)" music.

#### C1-023 — Napstablurk `[ ]`
P2 · M · NAR-017, ART-111
- **What:** an optional encounter in the R10 hallway; `Cheer` ×3 in sequence (`Like their post` → `Leave a comment` → `Share`) → spare; `Flirt` (`* You slide into their DMs. They leave you on read. Out of nervousness.`); `seen_tears` dealing 1 damage; ending `oh... i usually come here because nobody's online... but today i met somebody nice...`; `ruins.napstablurk.state`. Formerly `xX_AFK_Xx` (jar #2).

#### C1-024 — Ruins random encounters `[ ]`
P1 · S · ENC-003, C1-020
- **What:** `data/encounters/ruins_*.json`: R8 (budget 1: Froggsona · Shysona · Modsmal), R10 (budget 1: Lurkeye · Reply Guy · Grasstoid), with "X and co." combos and weights. Toxic route: encounters until the cap of 6 (C7-002).

#### C1-030 — The Ruins playable start to finish `[ ]`
P0 · XL · C1-001..C1-024
- **Acceptance:** from a new game to R13 without debug tools, in ~8.5 min at average pace; save/load at every save point; the in-area variations work (spare everything, Accept Hug on Furiell, kill everything with "nobody logged in," the Mirror triggered and refused); FUR, the raid clock and flags persist in the save.

---

### C2 — Area 2: Snowdinha

> **Source:** LORE §13, §18.2, §19.3. **Target time:** 6.5 min · **Music:** "Snowdinha Town (8-bit bossa nova)" · **Palette:** white `#E8F0FF` + RGB aurora `#FF4FD8 → #4FD8FF → #7CFF3A`.
> **Concept:** a village under the convention's industrial AC (60 °F for 12 years). Plastic pine trees with string lights, styrofoam snow, an LED aurora on the ceiling. Small-town energy: everybody's in the same group chat. **New gameplay:** the blue soul.

#### C2-001 — S1: Forest and bridge `[ ]`
P1 · L · CORE-011, GFX-005, NAR-018
- **What:** a branch snaps, footsteps, `don't you know how to greet a new pal? turn around and shake my hand.` → an **MLG air horn** (instead of the whoopee cushion); Papyrus arrives and Pepe hides behind a **Pepe-shaped lamp** (`* A lamp shaped exactly like you. Suspicious. Convenient. Cringe.`). `snow.sans.met`.
- **Save (entrance):** `* The crunch of styrofoam snow fills you with DETERMINATION.`

#### C2-002 — S2: Sans's station + Doge `[ ]`
P1 · M · C2-020
- **What:** sign `SENPAI SANS — WAIFU SENTRY DUTY — 4 NAPS/SHIFT`; examinable `* There's a body pillow under the counter. And another one. Is it body pillows all the way down?`; a fixed encounter with **Doge** (`snow.doge.state`).

#### C2-003 — S3: CAPTCHA Floor `[ ]`
P1 · M · OVW-012
- **What:** `SELECT ALL TILES WITH TRAFFIC LIGHTS!` — a 3×3 grid on the floor; one traffic light straddles the border between two tiles (you have to step on both); `I HAVE FAILED IT FORTY TIMES! AM I... A ROBOT?`; the stream's floating chat (`SansUndertaker: lol` · `PapyrusAlt2: GREAT PUZZLE!!` · `bot_4471: free followers at`). Solvable in < 45 s, with a funny hint (LORE §11.1).

#### C2-004 — S4: Gauntlet of Deadly Stream Alerts `[ ]`
P1 · M · CORE-011
- **What:** a bridge hung with stream alerts (`NEW FOLLOWER!`, `DONATION!`, air horn, a plush dog); Papyrus calls it off: `WAIT. THIS IS TOO MUCH CONTENT. I CANNOT IN GOOD CONSCIENCE OVERSTIMULATE MY THREE VIEWERS.`; examinable alert `* "NEW FOLLOWER!" It's PapyrusAlt2. Again.` Optional detour with **Dogsona & Dogsonissa**.
- **Replaces:** the ROTEIRO's laser grids and typing test (not in the LORE).

#### C2-005 — S5: Town `[ ]`
P1 · XL · OVW-009, SHP-003, SHP-004
- **What:** a hub with Bunny Merch (SHP-003), Shed Inn (SHP-004), Grillby's LAN House (C2-006), the Librarby (C2-007), the skeletons' house (C2-008); sign `* WELCOME TO SNOWDINHA. Population: 47. Showered: 3.`; 4 generic townsfolk (`townsfolk_01..04`); random encounters (budget 2: Punsona, Lesser Doge, Jerry); Mom's message on entering the area (`are you on the mountain again`).
- **Save (town):** `* The sight of 47 people sharing one Wi-Fi password on a fridge fills you with DETERMINATION.`

#### C2-006 — S5a: Grillby's LAN House (layer 4) `[ ]`
P1 · M · SHP-006, NAR-018
- **What:** Sans buys a soda and delivers **layer 4**: `a gamer soul only comes out if you let it out. that's the rule. so... why do you think everyone's been so nice to you? heh.` (`snow.grillbys.reveal`); PC `* An ancient PC. The wallpaper is a green hill. The hill has seen things.`; sign `* NO RAGE QUITTING. NO CAPS LOCK. NO CAPS LOCK ABOUT RAGE QUITTING.`; the `meta.resets ≥ 3` meta line (MET-003).

#### C2-007 — S5b: Librarby `[ ]`
P2 · S · NAR-016
- **What:** the "official" (gamer) history of the war and the Seven Moderators — the 3 books from LORE §13 (`A History of the Great Yiff War`, `The Seven Moderators: A Biography`, `Mt. Dew: A Geological Survey`).

#### C2-008 — S5c: The skeletons' house `[ ]`
P2 · M · LOR-007
- **What:** Papyrus's room: a rack with **12 cardboard fursuit prototypes** (`* Fursona v1 through v12. All skeletons. All cardboard. All rejected.`), PC `* Followers: 3. Following: 1,204.`; Sans's room: a swirl of body pillows, a box `* "Study materials". Taped shut. Twice.` (never opens); the secret: one of the body pillows is Pepe's (`* (You don't remember buying this.)` `* (You do.)`).

#### C2-009 — S6: Fog and Papyrus's fight `[ ]`
P1 · XL · C2-021
- **What:** the fight (C2-021) and the aftermath: `* Papyrus gives you his phone number. And his stream key. By accident.` (CELL contact).
- **Save:** `* The hum of the town's single shared router fills you with DETERMINATION.`

#### C2-010 — S7: Exit `[ ]`
P1 · S · NAR-007
- **What:** Floofy in the snow: `Six SOULs in the castle, idiot. Guess who's lucky number seven?`

#### C2-020 — Snowdinha bestiary `[ ]`
P1 · L · NAR-006, ART-110, BHL-008
- **What:** replaces the old proposal (Hat Kid and Greater Doge dropped).

| Enemy (parody) | HP/AT/DF · EXP/G · FUR | ACTs → spare | Patterns | Note |
| :--- | :--- | :--- | :--- | :--- |
| Doge (Doggo) — fixed in S2 | 70/13/7 · 30/30 · +3 | Pet (`* wow. such pat. very spare.`) · Accept Hug (+12) | `doge_words` (**blue** words: stand still) | BHL-008, BHL-018 |
| Lesser Doge (Lesser Dog) | 18/6/0 · 18/20 · +6 | Pet (repeatable; spare after 2; the neck leaves the screen) | `cardboard_slash`, `excited_doge` | `* Lesser Doge is beyond the stratosphere.` |
| Punsona (Snowdrake) | 74/12/7 · 22/15 · +5 | Laugh · Heckle (leaves sad) | `pun_flakes` | — |
| Jerry (Jerry) | 80/2/0 · 0/0 · +1 | Ditch (leaves; the others become spareable) | `brb` (AFK on 50% of turns) | formerly `BrbJerry` (jar #3) |
| Dogsona & Dogsonissa (Dogamy & Dogaressa) — optional | 98 each/14/5 · 40/50 · +5 | Roll Around + Re-sniff → Pet | `axe_cross`, `pup_heart` | **at FUR ≥ 40 they spare you outright**; a pair ACT (ACT-004) |

#### C2-021 — Papyrus's fight (LORE §19.3) `[ ]`
P1 · XL · BHL-010, BHL-008, ART-111
- **What:** HP 340 · AT 8 · DF 2. Intro `HUMAN! TODAY, I CAPTURE MY FIRST GAMER! LIVE! THE PAWTROL WILL HAVE TO ACCEPT ME!`; **blue soul from turn 1** (`YOUR SOUL IS BLUE NOW! THAT'S THE COLOR OF MY SUBSCRIBER BADGE! NYEH!`). ACTs: Check, Flirt (`FLIRTING?! ON STREAM?! CHAT, CLIP THAT!`), Insult, **Rate Fursona** (skips 1 attack). Patterns `bone_stream`, `blue_bones`, `hop_bones`; special: a plush dog steals the network cable (`STREAM OFFLINE`) → a single tiny bone `...THAT WAS MY SPECIAL ATTACK.` End: he spares you (`I WILL JUST BE YOUR FRIEND! THAT'S ALSO CONTENT!`). Death: `I STILL BELIEVE IN YOU... DON'T FORGET TO... FOLLOW...` and the stream keeps running at 0 viewers. Background: grid + scrolling chat panel (3 names). "Bonestream" music. `snow.papyrus.state`.

#### C2-022 — Streaming Shed capture `[ ]`
P1 · M · C2-021
- **What:** HP 0 against Papyrus is **not** a FURRY OVER (LORE §8.3): Pepe wakes up in the Streaming Shed (ring light, dog bed, the door doesn't lock); `snow.papyrus.captures` 0–3; on the 3rd he gives up and lets you through.

#### C2-023 — Senpai Sans (NPC) `[ ]`
P1 · M · GFX-005, AUD-012
- **What:** Sans as a recurring NPC (S1, S2, S5a, N2): a Comic-style font (Comic Neue), all lowercase, a bony beep, always with the "Skele-chan" body pillow; naps at his post; the only one who tells Pepe the truth.

#### C2-024 — Toxic route in Snowdinha `[ ]`
P2 · M · C7-002
- **What:** cap 6 (random encounters only), Papyrus is killable, the town emptied out (shops without a seller, "closed" signs), an empty Grillby's.

#### C2-030 — Snowdinha playable `[ ]`
P1 · XL · C2-001..C2-024
- **Acceptance:** from R13 to S7 in ~6.5 min; puzzles < 45 s; the Papyrus capture and fight in all 3 variants; layer 4 delivered.

---

### C3 — Area 3: Wetfall

> **Source:** LORE §14, §18.3, §19.4. **Target time:** 5 min · **Music:** "Wetfall (city pop drizzle)" · **Palette:** dark blue `#0E1A3A` + cyan `#3FF2FF` + pink `#FF8FC7`.
> **Concept:** caverns where 12 years of fursuit steam condensed into a waterfall. Everything drips. Glowing mushrooms, blue crystals, **clotheslines of drying fursuits**. It smells like "wet dog and vanilla perfume." **New:** the green soul. **Replaces** the old "Weebfall" (dropped).

#### C3-001 — W1: Entrance + Sharkdyne `[ ]`
P1 · M · OVW-015, OVW-016
- **What:** Sharkdyne, on a high rock, fires a **DM harpoon** (`hey :3`) that just misses; Pepe hides in the "tall grass" — **a clothesline of drying fursuits** (`* Fursuits, drying. One of them is still warm. You move on.`).
- **Save:** `* The sound of dripping and a distant "rawr" fill you with DETERMINATION.`

#### C3-002 — W2: Repost Flowers (layer 5) `[ ]`
P1 · M · OVW-011
- **What:** flowers that repeat the last thing said near them (the Echo Flowers parody). One has Barky's voice: `"...one more match, mom, I'm almost at prestige..."`; another: `"...brb..."` → `* (Why would a wolf say that?)` (`wet.flowers.barky`). Interaction: whisper → `* (You whisper "cringe" into the flower.) * It reposts "cringe". 0 likes.`

#### C3-003 — W3: Commission Queue Room `[ ]`
P1 · M · ENC-003, C3-022
- **What:** the Wishing Room parody: the ceiling crystals are **open commissions** "waiting on the artist since 2016" (`* "Long ago, people would whisper their commissions to the ceiling... and the ceiling would say 'ETA 6-8 weeks'."`); random encounters (budget 2); the lonely body pillow (examine 2× → Mad Daki, C3-022); the trash can with the **Phone Camera** (C3-007).

#### C3-004 — W4: Uwu-mie Village `[ ]`
P2 · M · SHP-005
- **What:** the shop (SHP-005), Uwu-mie chatter (`hOI!!! i'm uwu-mie!!!`), statue `* An uwu-mie statue. Its face says "uwu". Its eyes say "help".`; "uwu-mie village" music.

#### C3-005 — W5: Chase bridge `[ ]`
P1 · M · OVW-015
- **What:** Sharkdyne gives chase, firing DMs in lines; overworld dodging (real HP damage and FUR).

#### C3-006 — W6: Sharkdyne's fight `[ ]`
P1 · XL · C3-021
- **What:** the speech `For twelve years we've been stuck down here, smelling each other!` and the fight.
- **Save:** `* The humidity of a thousand fursuits fills you with DETERMINATION.`

#### C3-007 — Phone Camera in the trash can `[ ]`
P2 · S · OVW-011
- **What:** the `Phone Camera` weapon (AT 7, `* Filming is the deadliest weapon on the surface.`) in W3's trash can (`* Someone threw away a phone. The camera still works. Dangerous.`).

#### C3-020 — Wetfall bestiary `[ ]`
P1 · L · NAR-006, ART-110, BHL-016

| Enemy (parody) | HP/AT/DF · EXP/G · FUR | ACTs → spare | Patterns | Note |
| :--- | :--- | :--- | :--- | :--- |
| Vtuber-en (Shyren) | 70/10/0 · 32/26 · +6 | Hum ×3 · Superchat | `note_bullets`, `avatar_glitch` | formerly `quiet_healer` (jar #5) |
| Musk-ron (Aaron) | 98/12/4 · 36/30 · +4 | Flex ×3 · Hold Breath (skips `musk_cloud`) | `sweat_drops`, `musk_cloud` (persistent zone) | formerly `GymRat_Gaming` (jar #4) |
| Gatekeeper (original) | 60/10/8 · 28/20 · +3 | Admit · Ask Favorite Species (2 turns with no attack) | `gate_bars`, `species_chart` | — |
| Washua (Woshua) | 45/8/3 · 10/8 · +2 | Shower (**FUR −10**) · Clean Yourself | `soap_bubbles` (greens heal) | the only furry who showers |
| Uwu-mie (Temmie) | 5/1/1 · 1/1 · +8 | Pet · Pander · Pet Again | — | — |

#### C3-021 — Sharkdyne's fight (LORE §19.4) `[ ]`
P1 · XL · BHL-011, MRC-005, ART-111
- **What:** HP 600 · AT 10 · DF 4. **Green soul (Do Not Disturb)**: arrows from 4 directions are **DMs** (`hey :3`, `u up?`, `rp?`, `OwO what's this?`, `nice fursona`, `check DMs`, `commission?`, `*nuzzles*`); yellow `reverse_dms` arrows loop around. Other patterns: `harpoon_rain`, `musk_wave` (+10 FUR, covers the box in green for 1 s). ACTs: Check, Plead (`THAT'S WHAT THEY ALL SAY! RAAAWR!`), Reply to a DM (she skips a turn).
- **1:1 pacifist resolution:** she can't be spared; after ~8 turns the soul turns red again and **Flee** appears (MRC-005); Pepe runs left to Hotland with her chasing → H1.
- **Death:** `...this isn't... how the fanfic... was supposed to end...` → dust and the last DM on screen: `...seen.` Background: speed lines + drops. "DMs of Justice" music. `wet.sharkdyne.state` (1 fled · 2 killed · 3 friend).

#### C3-022 — Mad Daki (optional mini-boss) `[ ]`
P2 · M · BHL-019
- **What:** HP 200 · AT 7 · DF 0 · FUR +6. FIGHT doesn't work (`* It's too soft. Your attack sinks in.`); its bullets hit it when you dodge close (1:1 with Mad Dummy). Bubbles `NINE YEARS! AS A BEANBAG!` etc. Reward: `Pillowcase Cape`. `wet.maddaki.state`.

#### C3-023 — CELL in Wetfall `[ ]`
P2 · S · MNU-006
- **What:** Mom `bring the bottles down when you come back`; Papyrus's two Wetfall tips (LORE §21.3).

#### C3-024 — Toxic route in Wetfall `[ ]`
P2 · S · C7-002
- **What:** cap 5; Sharkdyne has to die; the Uwu-mie village emptied out.

#### C3-030 — Wetfall playable `[ ]`
P1 · XL · C3-001..C3-024
- **Acceptance:** from S7 to the Hotland exit in ~5 min; Sharkdyne's chase and escape work; layer 5 delivered.

---

### C4 — Area 4: Hotland Data Center

> **Source:** LORE §15, §18.4, §19.5. **Target time:** 2 min · **Music:** "Hotland (fan noise & funk)" · **Palette:** orange-red `#5A1A0A`/`#C2410C` + green LEDs `#39FF14`, heat distortion.
> **Concept:** the Archive's server room. Racks, cable spaghetti, fans at full blast, 113 °F. Signs `DO NOT TURN OFF` and `SERVER 7: 18+ (DO NOT LOOK DIRECTLY)`. **New:** the yellow soul.

#### C4-001 — H1: Entrance + overheated Sharkdyne `[ ]`
P1 · M · NAR-019
- **What:** if spared/fled, Sharkdyne faints from the heat **inside her fursuit** (`* Fursuits are not rated for 113 degrees.`); the water cooler gives **`Cold Water`** (`* Cold water. Actual water. Unlike some bottles you know.`); handing it to her starts the friendship (`wet.sharkdyne.state = 3`, scene in NAR-019). The 18+ server: `* You do not look directly.` (LOR-007).

#### C4-002 — H1→H2 encounters `[ ]`
P1 · S · ENC-003, C4-020
- **What:** budget 1 (Overclockin, Tsundere-Bot).

#### C4-003 — H2: Alphys's Lab (layer 7) `[ ]`
P1 · M · NAR-020, MNU-004
- **What:** Alphys adds Pepe to the server (`* You received 400 notifications.`) and becomes a CELL contact; the **experiment log** (`* "Exp. 7: DETERMINATION + golden flower + the Prince's old fursuit head. Result: it talks. It has no shame. Oh no. Oh no no no."`, `hot.alphys.log`); the moderation queue (`* Approve? Deny? ...She clicks approve without looking.`); the fridge (`Instant Noodles`); `Blue-Light Glasses`; she also closes a tab in a hurry (running gag).

#### C4-004 — H3: Mettapaw's Quiz `[ ]`
P1 · L · C4-021
- **Save:** `* The sound of 400 server fans fills you with DETERMINATION.`

#### C4-020 — Hotland bestiary `[ ]`
P1 · M · NAR-006, ART-110

| Enemy (parody) | HP/AT/DF · EXP/G · FUR | ACTs → spare | Patterns |
| :--- | :--- | :--- | :--- |
| Overclockin (Vulkin) | 20/10/0 · 15/10 · +2 | Encourage | `heat_clouds`, `fan_spin` |
| Tsundere-Bot (Tsunderplane) | 85/10/10 · 20/15 · +4 | Approach ×2 | `error_popups` |

#### C4-021 — "Who Wants To Be Verified?" (LORE §19.5) `[ ]`
P1 · L · BHL-012, MNU-004, ART-111
- **What:** a quiz event with the **yellow soul** as a cursor (`OH YES! YOUR SOUL IS A CURSOR NOW, DARLING! CLICK!`); the LORE's 5 questions (with questions 4 and 5's "all correct" answers); wrong = 3 damage + **FUR +5** (`* The audience hugged you. Aggressively.`), `hot.quiz.wrong`; Alphys whispers answers over the CELL (`...is typing` → `D`); end `You're a natural, darling! ... All 1.2K of them.` `* They're all named "bot_".` Background: a stage, spotlights, an audience of identical silhouettes. "Death by Engagement" music.
- **Variations:** Furry route — wrong answers cost FLUFF; Toxic route — Mettapaw deactivates their account and flees (C7-003).

#### C4-030 — Hotland playable `[ ]`
P1 · L · C4-001..C4-021
- **Acceptance:** ~2 min; the optional Sharkdyne friendship works; layer 7 delivered.

---

### C5 — Area 5: The Con Center

> **Source:** LORE §16, §19.6–19.7, §22. **Target time:** 4 min · **Music:** "Con Center (empty hall)" → "ASGRR" · **Palette:** beige `#CFC4AE`/`#8F8775` + gold `#E6B422`, **deliberately desaturated**.
> **Concept:** FurNation Con '14's main hall turned capital (like New Home): beige carpet, booths under bedsheets, hanging lanyards, panel posters from 2014, stained glass of the Seven Moderators **and** of Azzy.

#### C5-001 — N1: Hall of Posts (layer 6) `[ ]`
P1 · M · NAR-015, ART-120
- **What:** the 8 printed `King_Asgrr_Official` posts from LORE §16 hung along the hallway (the furry version of the war; Chara and Azzy), each with residents' comments on a second page; the stained glass of the Seven Moderators and Azzy; Mom's message `dinner's cold` on entering.

#### C5-002 — N2: The Last Lobby `[ ]`
P1 · M · NAR-023
- **What:** a golden hallway; Senpai Sans explains LV/EXP (`lv is level of vitriol. exp? excessive posting. ...`) and delivers the verdict. On the Toxic route, this is his fight (C7-004).

#### C5-003 — N3: Throne and basement `[ ]`
P1 · L · OVW-011, NAR-021
- **What:** the garden of gold **participation trophies**; the King offers tea (`Questionable Herbal Tea`, +99 HP); in the basement, the **6 jars** with the King's labels (`con.jars.seen`; the 6 label texts from LORE §6.3) and the thought `* (Barky... was a gamer?)`; the `Real Mechanical Keyboard` (AT 10; it fell with Pepe and someone kept it); the King's unopened Citrus Blast can (`* An unopened can of Citrus Blast, 2014 edition. The note says "for a special occasion." * It has been twelve years.`); an empty booth with another Pepe lamp (`* A lamp shaped like you. Another one. Who is making these?`).
- **Examinables:** trophy `* "PARTICIPANT". Even the trophy has one.`; the King's cape `* There's a zipper on the King's back. You decide not to mention it.`
- **Save:** `* The smell of herbal tea and old carpet fills you with DETERMINATION.`

#### C5-004 — N4: The Barrier `[ ]`
P1 · M · GFX-014
- **What:** the greenish-yellow fog (animated stink lines, a "no signal" spinner in the middle); `* The air smells like a closed bedroom. Your eyes water. You feel... at home?`; Asgrr's fight (C5-021).

#### C5-005 — N5: Finale `[ ]`
P1 · L · END-001
- **What:** depends on the route (END-002/003/007, C6-002).

#### C5-021 — King Asgrr's fight (LORE §19.6) `[ ]`
P1 · XL · MRC-004, BHL-008, BHL-016, ART-111
- **What:** HP 1500 · AT 12 · DF 6 · EXP 2000. Before: `Howdy. ...I have not been outside in twelve years. Would you like some tea? ...No? Then I must ask you for your SOUL.` **Grayed-out MERCY** (`This feature is not available in your region.`, MRC-004). ACTs: Check, Talk ×3 (on the 3rd, AT and DF −1), **Refuse** (`...You are the first one who ever said it.`, skips 1 attack, changes the final line). Patterns: `lanyard_trident_sweep` (blue/orange), `tea_splash` (+2), `trophy_rain` (+3), `convention_crowd` (a wave of tiny fursuiters, +10), `cologne_cloud` (`Eau de Grr`, a green zone with damage over time). Low HP → he kneels, MERCY returns: Spare `...You would spare me? After I tried to make you one of us?` · Kill `...Ah. So that is how it is.` (lanyard dust). Background: black + a border of trophies. `con.asgrr.state`.

#### C5-022 — The Mascot (LORE §19.7) `[ ]`
P1 · XL · BHL-021, ART-111
- **What:** Floofy absorbs the 6 souls and becomes a giant convention mascot made of hundreds of fursuit heads, plushies and tails stitched together, with a CRT TV in its chest showing the zipper face (cute horror, **nothing explicit**: the horror is the number of plastic eyes). Patterns: **`File Load`** (undoes the player's last turn, the screen flashes `File Loaded`, BHL-021), `plush_tentacles`, `glitter_cannon`, `owo_missiles`, `zipper_mouth`. **The 6 souls rebel:** each phase adds an ACT named after an ex-gamer (`Barky`, `Napstablurk`, `Jerry`, `Musk-ron`, `Vtuber-en`, `Mettapaw`) with its LORE line, which heals Pepe and weakens the Mascot. End: Floofy begs; Spare `...why? you're so cringe.` · Kill `...gg.` Background: an endless wall of blinking plastic eyes. "Your Best Nightmare (Mascot Mix)" music.

#### C5-023 — Stained glass and lore objects `[ ]`
P2 · S · NAR-016
- **What:** one examinable window per Moderator with the LORE §5.3 stained-glass text (`xX_N00bSl4yer_Xx`, `CTRL_ALT_DEFEAT`, `TheRatioKing`, `MLG_Prophet`, `Sk1llIssue`, `CapsLockCarl`, `xX_Moderator_Xx`) and Azzy's.

#### C5-030 — Con Center playable + finale `[ ]`
P1 · XL · C5-001..C5-023, END-002
- **Acceptance:** ~4 min to the end of the fight + finale; the 4 endings route correctly; the N3 save lets you retry Asgrr.

---

### C6 — Furry route and "The Raid" act

> **Source:** LORE §17, §18.5, §19.9, §22.3. Systems in `FLF`. **Runtime:** the same path until the conversion; boss fights become scenes (−5 to −6 min) and The Raid act is added (+4 to +5 min) → ~28–32 min.

#### C6-001 — The rest of the path after the conversion `[ ]`
P2 · L · FLF-010, NAR-025
- **What (LORE §17.2):** Furiell (if not fought) cries with joy `My child... you came home.`, hands over Pop-Tarts and lets you go · Barky `DUDE. Welcome to the pack. You're... honestly a better wolf than me.` · Papyrus does a **collab stream** (`A FURRY?! ON MY STREAM?!`, chat spams `HONORARY FURRY`, `CHAT HAS SPOKEN! I AM AN HONORARY FURRY! THIS IS THE BEST DAY OF MY CAREER!`, he gives you his number) · Sharkdyne recruits you (`I KNEW you had it in you, recruit!`) · Mettapaw keeps the quiz with FLUFF · NPCs give welcomes and discounts · Floofy disappears (`Huh. You did it yourself. ...No shame. I like that.`) · NPCs hand out `Hug Coupon`s.

#### C6-002 — The Con Center on the Furry route `[ ]`
P2 · L · CORE-011, RAD-005
- **What:** the King welcomes Pepe in tears (`You came... willingly?`); Pepe hands over the jar → **seven souls**; the Barrier falls in a "pomf" of glitter and stench, sunlight; **Floofy** turns on a camera and streams to the surface (`FURRY OUTBREAK AT MT. DEW (NOT CLICKBAIT)`, `What? I don't have shame. I have VIEWS.`); the phone buzzes with `RAID RELOCATED…`.

#### C6-003 — X1: The Barrier breach `[ ]`
P2 · M · CORE-011, ART-103
- **What:** the guild comes down Mt. Dew (silhouettes with headsets, bottles, ring lights); the furries hide behind Pepe; `* (Oh no. These are your friends.)` `* (Were.)`. "Raid Night" music.

#### C6-004 — X2: Gauntlet in the Con Center hall `[ ]`
P2 · L · BFL-004, C6-020
- **What:** 4 gamer encounters in a row, one per booth (Discord Simp, Gacha Whale, Bottle Guy, Apology Streamer); the **Dragon Excuse Guy** is optional, in a dark corner booth; between them, furries hug Pepe (FLUFF heal); a save in the middle: `* The terrified hugging of a hundred furries fills you with DETERMINATION.`; `raid.gauntlet` (0–5). Battle background: red grid.

#### C6-005 — X3: The top of the Rift (sunrise) `[ ]`
P2 · L · C6-021, ART-121
- **What:** boss **xX_Moderator_Xx** in the chair-mech; the furries watch; Papyrus streams it. Surface palette: `#2B2B3A` + sunrise `#FF9E5E`.

#### C6-020 — Gamer bestiary `[ ]`
P2 · L · FLF-008, FLF-011, LOR-001
- **What (LORE §18.5):** bullets deal **SHAME**; BONK leads to horny jail; spare = rage-quit.

| Gamer | HP/AT/DF | ACTs → spare | Patterns | Note |
| :--- | :--- | :--- | :--- | :--- |
| Discord Simp | 40/8/2 | Leave on Read ×2 · Reply "hey" (skips the attack) | `hey_spam`, `discord_kitten_request` | — |
| Gacha Whale | 60/10/4 | Show Bank Statement · Ask About Pity (talks for a turn) | `card_rain`, `banner_tickets` | — |
| Bottle Guy | 50/10/6 | Open a Window · Ask About the Bottles | `bottle_barrage` | Pepe recognizes the brand of his `Bottle` item (running gag) |
| Apology Streamer | 70/12/4 | Ask What He Did · Turn Comments Back On | `self_reflection` | spare: `* Comments have been disabled.` |
| Dragon Excuse Guy — optional | 30/6/0 | Ask Her Age (the narrator mutes him) · Bonk | `excuse_text` (crossed-out text only, BHL-018) | **the character never appears and is never described** (LOR-001) |
| Tryhard (`Sk1llIssue Jr.`) — reserve | — | Ask for Help | `skill_issue_text` | only if the gauntlet needs a 5th |

#### C6-021 — xX_Moderator_Xx's fight (LORE §19.9) `[ ]`
P2 · XL · FLF-005, BHL-016, BHL-017, BHL-018, GFX-012, ART-111
- **What:** HP 800 · AT 14 · DF 6; pink plush soul, FLUFF bar. Opening `Pepe?? bro. BRO. you're a FURRY now?? ...`. Patterns (all SHAME): `ban_hammer`, `copypasta_wall` (a wall of text with a gap), `bottle_barrage`, `musk_cloud_gamer` (residual SHAME), **`homework_folder`** (a folder spits **mosaic censor blocks** as bullets — the folder's only "visual"), `cringe_compilation` (video frames of furry Pepe, high SHAME). ACTs: Open a Window (AT −2), Ask When He Last Showered (DF −2), Mention His Mom (freezes 1 turn), Hug (spareable after 3 ACTs), BONK. Final phase: the furries you helped show up behind Pepe and heal FLUFF. End: spare `...ok this is actually so cringe. i'm logging off.` + `* xX_Moderator_Xx has left the game.` · final BONK → horny jail. "RGB Overdrive" music.

#### C6-030 — Furry route playable `[ ]`
P2 · XL · C6-001..C6-021, END-008
- **Acceptance:** converting in at least three different areas (Ruins, Snowdinha, Hotland) leads to the same The Raid act; ~28–32 min at average pace; GAMER OVER returns to the right save.

---

### C7 — Toxic Gamer route

> **Source:** LORE §3 (cap), §19.8, §22.2. ~30–35 min (grind + Senpai Sans's fight).

#### C7-001 — Toxic route in progress `[ ]`
P2 · M · PRG-031, D-11
- **What:** the predicate `route.toxic_active` = up to the current area, every cap has been exhausted and every killable boss has died. Used by the FUR lock (FUR-011), the save texts and the emptied areas.

#### C7-002 — Emptied areas `[ ]`
P2 · M · ENC-004, AUD-008
- **What:** once the cap (6/6/5/3) is exhausted, encounters stop with `* But nobody logged in.`; NPCs vanish (locked houses, shops without a seller), the music plays in slow motion, save texts become `* Determination.`, the fluff particles disappear.

#### C7-003 — Mettapaw flees `[ ]`
P2 · S · C4-021
- **What:** in Hotland, Mettapaw deactivates their account and flees: `OH NO. NOT LIKE THIS, DARLING. NOT ON CAMERA.` → `* Mettapaw has deactivated their account.`; no quiz.

#### C7-004 — Senpai Sans: "a really bad stream" (LORE §19.8) `[ ]`
P2 · XL · BHL-010, BHL-017, ART-111
- **What:** in the Last Lobby; short and hard (3 phases of ~40 s); HP 1 · AT 1 · DF 1, dodges everything until he falls asleep in the final phase. Opening `it's a beautiful day outside. the wi-fi is down. on days like these, gamers like you... should be getting a really bad stream.` Patterns `bone_rows`, `air_horn_blasters` (MLG air-horn skulls), `karma_fur` (each hit adds KR that eats HP, using the FUR bar as temporary KR, reset at the end). End `...welp. i'm going to grillby's. papyrus, want anything?` Background: black with the Last Lobby's gold. "Megalomania.exe" music.

#### C7-005 — The throne on the Toxic route `[ ]`
P2 · M · END-007
- **What:** Floofy bans the King and offers himself (`I'll be your follower! Your biggest fan! I have no shame, we're the SAME—`) → Pepe kills him: `* gg.` → the Toxic ending (END-007).

#### C7-030 — Toxic route playable `[ ]`
P2 · XL · C7-001..C7-005, C1-024, C2-024, C3-024
- **Acceptance:** exhausting every area and killing the bosses leads to Sans's fight and the co-leader ending; the title is empty and silent on the next boot.

---

### ART — Art (placeholders and final assets)

> **Source:** LORE §25 (visual bible) and §26 (placeholder list). Native 1× pixel art drawn at **2×** on the 640×480 canvas; a **colorful overworld** with a limited palette per area; **battles in white on black**, color only for special moments; **every character with adult proportions** (LOR-004); a silhouette that reads in solid black (Barky's ears, Sans's body pillow, Papyrus's cardboard ears, Sharkdyne's harpoon, Asgrr's lanyards, Mettapaw's ring light, Floofy's zipper head, the Moderator's chair-mech). **Any text drawn into art is English** (D-03).
> **LORE §26 priorities:** **P0** = playable Ruins (vertical slice) · **P1** = rest of the Gamer route · **P2** = Furry route, Toxic route and optional content. Sizes below are native (1×).

#### ART-000 — Placeholder pipeline `[ ]`
P0 · S · D-04, LOR-001
- **What:** every placeholder goes in with the **final file name** (swapping = replacing the file) and is logged in `assets/PLACEHOLDERS.md` (file, source, license). Good sources: Kenney.nl (CC0), OpenGameArt, itch.io (free + pixel art), Freesound (SFX), Incompetech/OpenGameArt (CC-BY music). Undertale rips only in `assets/_ref/` (add it to `.gitignore`). Spritesheets as PNG with alpha (the `pepe_char.bmp` magenta `#FF00FF` stays as legacy only).
- **Acceptance:** `assets/PLACEHOLDERS.md` exists and lists 100% of the non-final files; `assets/_ref/` is ignored.

#### ART-001 — Pepe spritesheet (overworld) `[ ]`
P0 · M · GFX-002
- **What:** `sprites/ow/pepe_frog.png`, ~20×30 (gray nacho-stained hoodie, shorts, flip-flops, eye bags, orange fingertips); 4 directions × (standing + 4 frames up/down, 2 sideways), like Frisk; extras: lying down and getting up (R1).

#### ART-002 — Dialogue portraits `[ ]`
P0→P2 · L · TXT-005
- **What (LORE §26.5, ~50×50):** `furiell_*` (neutral, happy, sad, angry, flustered/tab closed) P0 · `floofy_*` (cute, smug, sinister with the zipper open) P0 · `barky_*` (edgy, embarrassed, happy) P0 · `sans_*` · `papyrus_*` P1 · `sharkdyne_*` · `alphys_*` · `asgrr_*` (3 each) P1 · `moderator_*` · `pepe_pawsome_reflection` (2 each) P2. **No Pepe portrait** (he doesn't speak, D-15).

#### ART-010 — Barky battle sprite `[ ]`
P0 · M · GFX-002
- **What:** `sprites/battle/barky.png` ~60×80, in parts (ears, studded jacket, plastic chains, tail with a safety pin, **visible zipper**): idle 2f, hurt, blush (after Cringe Check), spared. A placeholder already exists.

#### ART-011 — Ruins bestiary sprites `[-]`
- **Merged into ART-110.**

#### ART-012 — Bullets `[ ]`
P0→P2 · L · BHL-004
- **What (LORE §26.4, white, 8–16 px):** each with its hitbox in JSON.

| File | Use | Prio. |
| :--- | :--- | :---: |
| `heart_pellet` · `uwu_letters` · `can` · `sound_ring` · `fluff_ball` · `glitter_hand` (~30) | Floofy, Barky, Furiell | P0 |
| `colon_three` · `fly` · `moth` · `gel_hammer` · `text_bug` · `grass_blade` (+ green) · `tear` | Ruins commons | P0 |
| `word_blue` · `cardboard_sword` · `snowflake_letter` · `axe` · `bone` (sizes, blue/white) | Snowdinha, Papyrus, Sans | P1 |
| `dm_arrow` (+ yellow) · `harpoon` · `shield` | Sharkdyne | P1 |
| `note` · `pixel_glitch` · `sweat` · `green_cloud` · `gate_bar` · `soap_bubble` · `daki_pellet` | Wetfall | P1 |
| `heat_cloud` · `error_popup` · `answer_letters` | Hotland | P1 |
| `trident` (blue/orange) · `tea_drop` · `trophy` · `mini_fursuiter` | Asgrr | P1 |
| `plush_tentacle` · `owo_missile` · `plastic_eye` | The Mascot | P1 |
| `air_horn_skull` (~40) · `beam` | Senpai Sans (Toxic) | P2 |
| `hey_text` · `credit_card` · `bottle_yellow` · `text_line` · `ban_hammer` · `mosaic_block` · `video_frame` | Gamers and Moderator | P2 |

#### ART-020 — Battle buttons `[ ]`
P0 · M · BUI-006
- **What:** FIGHT, ACT, ITEM, MERCY × 2 states (orange/yellow), 55×21 native, chunky hand-drawn letters; **+ BONK** (Furry route) and a **grayed-out** MERCY (Asgrr, MRC-004). File `ui/buttons.png`.

#### ART-021 — Souls `[ ]`
P0 · S · BUI-007
- **What:** `ui/souls.png`: red, blue, green, yellow, **pink plush with ears**, **frog** (easter egg); cracked/shard frames (GAMER OVER), the FURRY OVER soul (grows ears and a tail, inflates, pops into pom-poms), the running soul (flee), FUR-stage accessories (D-15).

#### ART-022 — FIGHT target and bar `[ ]`
P0 · S · FGT-001
- **What:** `ui/target.png`, `ui/attack_bar.png`.

#### ART-023 — Slash and BONK `[ ]`
P0 · S · FGT-004
- **What:** `ui/slash.png` (hitmarker/mouse cursor, same duration as the original slash) and `ui/bonk.png` (Furry route).

#### ART-024 — Dust clouds and spare particles `[ ]`
P1 · S · MRC-002

#### ART-025 — Special effects `[ ]`
P1→P2 · M · GFX-012, GFX-013
- **What (LORE §25.4):** `ui/dust_plush.png` (polyester stuffing + glitter; the ex-gamers' **red spark**) · `ui/fluff_burst.png` (pomf) · `ui/horny_jail.png` (little pink cage) · `ui/mosaic.png` (8×8 blocks) · green (cartoon) stink lines · fluff particles.

#### ART-030 — "!" bubble `[ ]`
P1 · S · ENC-001
- **What:** `ui/exclaim.png`.

#### ART-031 — Save point `[ ]`
P0 · S · SAV-001
- **What:** `props/save_point.png`: a golden 4-point star spinning (4–8 frames) **on top of a soda bottle**.

#### ART-040 — Ruins tileset `[-]`
- **Merged into ART-120** ("Ruins" row).

#### ART-041 — Intro cards `[ ]`
P1 · M · FRT-002
- **What:** LORE §9's 10 cards at 400×220 (currently 398×214) with Undertale's limited sepia palette: `lore1`..`lore9` + the tall fall image (`lore10`, currently 187 BMPs) to scroll vertically. Includes the `lore4` redraw (ART-170) and the optional Seven Moderators silhouette card (`img/lore5b`, P2).

#### ART-042 — Continue menu backgrounds `[ ]`
P2 · M · FRT-005
- **What:** one background per area (the art of the last saved area).

#### ART-050 — Icon and logo `[ ]`
P2 · S · —
- **What:** `ui/logo.png` "nUNDERTALE" with the **lowercase red `n`** (it trembles slightly in-game, FRT-003) in the Monster Friend font; `icon.png` at 16/32/48/256 (a multi-size `.ico` for the executable).

#### ART-060 — General UI `[ ]`
P0→P2 · M · BUI-005, MNU-004
- **What:** `ui/hud.png` (HP bar, FUR overlay, `FUR` label, **mirrored FLUFF bar** and `FLF` label) P0 · `ui/dialog_box.png` · `ui/portrait_frame.png` P0 · `ui/furry_over.png` P0 · `ui/gamer_over.png` P2 · `ui/cell.png` (the flip phone screen with paw stickers, the raid clock at the top, the `Homework` folder) P1.

#### ART-101 — Pepe's FUR stages `[ ]`
P1 · L · FUR-007
- **What:** `sprites/ow/pepe_ears.png` · `_paws` · `_tail` (an animated shiny tail) · `_snout` (mint-green patches), ~20×30; `pepe_pawsome.png` ~22×32 (a green frog-wolf with a scarf, fluffy ears, a tail, the eye bags still there) P2.

#### ART-102 — The cast in the overworld `[ ]`
P0→P2 · XL · GFX-002
- **What (LORE §26.2, 4 directions × 2–4 frames):**

| File | What | Frame | Prio. |
| :--- | :--- | :---: | :---: |
| `furiell.png` | white goat in a paw-print apron, half-moon glasses, a zipper on her back | ~26×40 | P0 |
| `barky.png` | edgy wolf in a jacket | ~22×32 | P0 |
| `floofy.png` | flower with a fursuit head: idle, talking, sinister, sinking | ~20×20 | P0 |
| `sans.png` | short skeleton, blue hoodie, pink slippers, body pillow | ~22×30 | P1 |
| `papyrus.png` | tall skeleton in a cardboard fursuit, LEDs, ring light, selfie stick | ~24×44 | P1 |
| `sharkdyne.png` · `_overheated` | armored shark with a harpoon; passed out | ~26×40 | P1 |
| `alphys.png` | little lizard in a lab coat, MOD badge | ~22×28 | P1 |
| `mettapaw.png` | box robot with a ring light for a face | ~24×36 | P1 |
| `asgrr.png` | giant goat, lanyard cape, trophy crown | ~32×48 | P1 |
| `grillby.png` · `bunny.png` · `innkeeper.png` · `washua.png` · `uwumie.png` | NPCs | ~20×30 | P1–P2 |
| `townsfolk_01..04.png` | generic Snowdinha furries | ~20×30 | P1 |
| `napstablurk.png` | little ghost | ~20×20 | P2 |

#### ART-103 — Furry route overworld `[ ]`
P2 · M · C6-003
- **What:** `moderator_mecha.png` (~40×40, an RGB gaming-chair mech + a green cloud) and `raider_01..03.png` (silhouettes in headsets).

#### ART-110 — Common enemy battle sprites `[ ]`
P0→P2 · XL · C1-020, C2-020, C3-020, C4-020, C6-020
- **What (LORE §26.3, white on black, idle 2f + hurt):** `fursuit_dummy` (~40×60) P0 · `froggsona` · `shysona` · `modsmal` · `lurkeye` · `reply_guy` · `grasstoid` (30–60) P0 · `doge` · `lesser_doge` (extendable neck) · `punsona` · `jerry` · `dogsonas` (40–70) P1 · `vtuber_en` · `musk_ron` · `gatekeeper` · `washua` · `uwumie` · `mad_daki` (40–80) P1 · `overclockin` · `tsundere_bot` (40–60) P1 · `napstablurk` (~50×50) P2 · `discord_simp` · `gacha_whale` · `bottle_guy` · `apology_streamer` · `dragon_excuse_guy` (40–70, **just the guy**, LOR-001) P2.

#### ART-111 — Boss battle sprites `[ ]`
P0→P2 · XL · C1-022, C2-021, C3-021, C4-021, C5-021, C5-022, C6-021, C7-004
- **What:** `furiell` (~80×110: idle, hurt, sad) P0 · `papyrus` (~60×120: idle, pose, detached head) P1 · `sharkdyne` (~90×120: idle, attack, hurt) P1 · `mettapaw` (~70×110: host) P1 · `asgrr` (~120×140: idle, kneeling) P1 · `floofy` (40) and `the_mascot` (~300×240 in parts) P1 · `sans` (~50×60: idle, asleep) P2 · `moderator` (~140×140, phases) P2.

#### ART-120 — Per-area tilesets and props `[ ]`
P0→P2 · XL · OVW-003
- **What (LORE §26.6, 20×20 tiles):**

| File | Content | Prio. |
| :--- | :--- | :---: |
| `tiles/ruins.png` + `props/ruins_*.png` | purple stone, carpet, walls, stairs, doors; Con banner, booth, 18+ curtain, CRT, notice board, RGB switch, Dorito bowl, wrapper pile, cables (spikes), pillar, fireplace, armchair, bed, closet, mirror, bookshelf | P0 |
| `tiles/snowdinha.png` + `props/snow_*.png` | styrofoam snow, plastic pine with string lights, sentry station, Pepe lamp, CAPTCHA tiles, stream alerts, houses, LAN House, signs, the ceiling aurora | P1 |
| `tiles/wetfall.png` + `props/wet_*.png` | blue cave, water, mushrooms, crystals, fursuit clothesline, Repost Flower, Uwu-mie statue, bridge, trash can | P1 |
| `tiles/hotland.png` + `props/hot_*.png` | racks, cables, fans, water cooler, lab, quiz stage, `DO NOT TURN OFF` / `SERVER 7: 18+` signs | P1 |
| `tiles/concenter.png` + `props/con_*.png` | beige carpet, sheet-covered booth, lanyards, post boards, stained glass (7 Moderators + Azzy), trophies, jars, throne, Barrier | P1 |

#### ART-121 — The Surface `[ ]`
P2 · M · C6-005
- **What:** `tiles/surface.png`: the top of Mt. Dew at sunrise (cans, the "nacho stratum," headsets, the giant cap at the peak), the Rift with its `FREE HUGS ↓` sign.

#### ART-150 — Cutscenes and screens `[ ]`
P1→P2 · L · FLF-001, END-009, MET-005
- **What:** `bg/mirror.png` (pulsing frame + reflection) P2 · `bg/ending_gamer.png` · `_clean.png` · `_toxic.png` · `_furry_*.png` (ending cards) P1–P2 · `bg/title_*.png` (5 variations) P2.

#### ART-160 — Pepe's room `[ ]`
P1 · M · END-005
- **What:** `bg/pepe_room.png` (320×240): the basement bedroom, monitor, chair, bottles (palette `#2B2B3A` + monitor light `#9FD3FF`).

#### ART-170 — Redraw `lore4` ("BEGONE!") `[ ]`
P0 · S · LOR-003
- **What:** the current card uses a **real face**; redraw it with no recognizable face, keeping the composition.

---

### L10N — Language (English only)

> **D-03:** the game is English only. There's no language switch; `data/dialogue/en/` is the only folder. The IDs below stay for traceability.

#### L10N-001 — Language loader `[-]`
- **Dropped (D-03):** no runtime language switch. The text loader (`TXT-010`) reads only `data/dialogue/en/`; the `config/user.json` `lang` field stays fixed at `en` (or is removed).

#### L10N-002 — Full PT-BR `[-]`
- **Dropped (D-03):** the game won't be translated into PT-BR. Upside: the main font only needs ASCII + the few symbols in use (`★`, `♥`, `—`, curly quotes), which simplifies `GFX-004`.

#### L10N-003 — Overflow measurement `[ ]`
P2 · S · TXT-002
- **What:** a script/test that renders all of `data/dialogue/en/` and flags anything over 3 lines in the box (or past the width of the bubble, the CELL and the save screen).

---

### QA — Debug tools, tests and CI

#### QA-001 — Debug mode `[x]`
P0 · S · REPO-003
- **What:** `-DNUNDERTALE_DEBUG` enables: F1 overlay (FPS, room, position, scene state, flags), F2 hitboxes, F3 debug menu. No debug shortcut exists in release.

#### QA-002 — Hitbox overlay `[x]`
P0 · S · QA-001
- **What:** draws the room's collisions, Pepe's interaction box, the soul's hitbox and every bullet's.

#### QA-003 — Debug menu `[ ]`
P1 · M · QA-001
- **What:** warp to any room, start any encounter, edit HP/LV/gold/FUR/flags, god mode (no damage), skip the enemy turn, run logic at 0.25×/0.5×/2× speed.
- **LORE:** set FUR (and trigger the Mirror), switch route (Gamer/Toxic/Furry) and FLUFF, edit the raid clock, force each ending and each title variant, reset/edit `meta.json`.

#### QA-004 — Reference comparison screen `[ ]`
P1 · S · QA-001
- **What:** F6 toggles an overlay of `docs/reference/*.png` (Undertale screenshots) at 50% opacity over the canvas, to check the layout pixel for pixel.

#### QA-005 — Battle sandbox `[ ]`
P1 · M · BHL-004
- **What:** a scene just for testing attack patterns on loop, with pattern selection and live parameter tweaking.

#### QA-010 — Unit tests `[x]`
P1 · M · REPO-003
- **What:** vendor `doctest` (single header); `tests/` with: `GameState` round-trip, the LV table, the damage formula, wrapping/indent, the markup parser, spare conditions, loading every JSON in `data/` (schema).

#### QA-011 — Damage formula tests `[x]`
P1 · S · FGT-003, QA-010

#### QA-012 — Data validation in the build `[ ]`
P2 · M · QA-010
- **What:** a step that validates every JSON (referenced item/enemy/room/sound IDs exist, text keys exist in `en`) and runs the content lint (LOR-005).

#### QA-013 — GitHub Actions CI `[ ]`
P2 · M · REPO-005
- **What:** a Windows workflow that runs `tools/setup_vendor.ps1`, builds the release, runs the tests and publishes the `.zip` as an artifact.

#### QA-014 — Crash handler `[ ]`
P3 · S · —
- **What:** catch exceptions/signals, write `crash_reports/<date>.txt` (the idea existed in the Python version) with the scene, the room and the stack.

#### QA-015 — "Feel" checklist `[x]`
P1 · S · —
- **What:** `docs/feel_checklist.md` with the items you can only verify by playing side by side with Undertale: soul speed, text pacing, encounter transition timing, the weight of the game over, menu responsiveness (no input lag), the sound of every action. Run it at every milestone.

#### QA-020 — Timing and pacing playtest `[ ]`
P1 · M · QA-015
- **What:** time each area against LORE §11.2 (front-end 2:30 · Ruins 8.5 · Snowdinha 6.5 · Wetfall 5 · Hotland 2 · Con Center 4 · ending 1.5 min) and the §11.1 rules (cutscenes ≤ ~6 boxes, puzzles < 45 s, 2–3 minute bosses / 4–6 turns with the right ACT, 2–3 encounters per area). Log it in `docs/playtest_log.md` and use it to tune the raid clock (D-12), the economy (SHP-007) and FUR (FUR-012).

#### QA-021 — LORE mechanics tests `[ ]`
P1 · M · QA-010
- **What:** unit tests for `addFur` (multipliers, clamp, stage, the Mirror trigger), FLUFF (damage, residual SHAME, max = HPmax + 10), the raid clock (pause, thresholds, deferral), route and ending routing (PRG-031, END-001) and the v1 → v2 save migration.

---

### DST — Distribution

#### DST-001 — Release build and output folder `[ ]`
P2 · S · REPO-003
- **What:** `build.bat release` produces `dist/nUndertale/` with `nUndertale.exe` (`-mwindows`, no console, with an icon via `.rc`), `assets/`, `data/` and the licenses. Following the project convention, the final artifact goes in `C:\Users\delux\OneDrive\Documentos\Programas\C\` (the C/C++ stack).

#### DST-002 — Packaging `[ ]`
P3 · S · DST-001
- **What:** a versioned `.zip` (`nUndertale-v0.x.zip`); optional: itch.io (butler).

#### DST-003 — Credits and notices `[ ]`
P1 · S · D-04
- **What:** the credits screen + `CREDITS.md` with the third-party fonts, sounds and music, and the notice that this is a fan parody not affiliated with Toby Fox/Undertale.

---

## 6. Roadmap by milestone

> Revised for LORE v2. M0–M3 are the 1:1 engine fidelity; M4 onward is content, area by area, in game order. Items already `[x]` are left out of the lists.

### M0 — Foundation `(almost done)`
**Remaining:** `REPO-004`, `REPO-005`, `REPO-006`, `GFX-002`, `GFX-004`, `D-04`, `ART-000`.
**Done when:** a fresh clone + vendor setup builds; PNG spritesheets load; placeholders are logged.

### M1 — 1:1 battle vertical slice (Barky)
`TXT-001`, `BFL-001`, `BFL-004`, `FGT-001`, `FGT-002`, `BHL-004`, `ACT-002`/`C1-021` (rework), `ACT-005`, `BHL-014` (rework), `BND-004` (rework), `BND-010` (FURRY OVER), `FUR-002`, `FUR-003`, `FUR-005`, `ART-010`, `ART-012` (P0), `ART-020`..`ART-023`, `ART-060` (P0), `AUD-002`, `AUD-003`, `AUD-011` (P0), `QA-004`, `LOR-001`.
**Done when:** overlaying a battle screenshot on Undertale's, the layout matches; Barky's fight follows all of LORE §19.1 (6 ACTs, 3 patterns, spare, death with a spark, FURRY OVER).

### M2 — 1:1 overworld
`OVW-002`..`OVW-005`, `OVW-007`, `OVW-008`, `OVW-009`, `OVW-011`, `GFX-010`, `TXT-007`, `TXT-010`, `TXT-011`, `SAV-001`, `SAV-007`, `ENC-003`, `CORE-003`, `CORE-011`, `ART-001`, `ART-031`, `ART-120` (Ruins), `AUD-001`, `PRG-021`, `PRG-031`.
**Done when:** you can walk through 3 real Tiled rooms, examine things, save, open the C menu, hit a random encounter and come back with everything preserved.

### M3 — 1:1 front-end
`FRT-001`..`FRT-009`, `NAR-001`, `ART-041`, `ART-170`, `ART-050`, `LOR-002`, `LOR-003`.
**Done when:** boot to R1 in ~2:30 with LORE §9's 10 cards, a title with the red `n`, and a name screen with the LORE easter eggs.

### M4 — Area 1: The Ruins of Con '14 (content vertical slice)
`C1-*`, `NAR-002`..`NAR-008`, `NAR-017`, `NAR-029`, `MNU-004`, `MNU-006`, `RAD-001`..`RAD-003`, `FUR-004`..`FUR-009`, `ACT-006`, `BHL-009`, `BHL-020`, `SHP-001`, `SHP-002`, `ITM-006`, `ITM-007`, `ENC-004`, `ENC-006`, `GFX-012`, `GFX-013`, `ART-002` (P0), `ART-102` (P0), `ART-110`/`ART-111` (P0), `AUD-010` (P0), `AUD-012`, `AUD-020`, `LOR-007`, `LOR-008`, `QA-020`, `D-12`.
**Done when:** `C1-030` passes (~8.5 min, the Mirror can be triggered and refused).

### M5 — Area 2: Snowdinha
`C2-*`, `BHL-008`, `BHL-010`, `BHL-018`, `GFX-005`, `SHP-003`, `SHP-004`, `SHP-006`, `SHP-008`, `NAR-016`, `NAR-018`, `ACT-004`, `ART-102`/`ART-110`/`ART-111`/`ART-120` (Snowdinha), `AUD-010` (Snowdinha tracks).
**Done when:** `C2-030` passes.

### M6 — Areas 3 and 4: Wetfall and Hotland
`C3-*`, `C4-*`, `BHL-011`, `BHL-012`, `BHL-016`, `BHL-019`, `MRC-005`, `OVW-015`, `OVW-016`, `SHP-005`, `SHP-007`, `NAR-019`, `NAR-020`, `FUR-012`, art and music for both areas.
**Done when:** `C3-030` and `C4-030` pass.

### M7 — Area 5 and the Gamer route endings
`C5-*`, `END-001`..`END-006`, `END-010`, `MRC-004`, `ACT-007`, `BHL-021`, `GFX-014`, `NAR-009`, `NAR-021`..`NAR-023`, `NAR-030`, `MET-005`, `ART-150` (Gamer), `ART-160`, `DST-003`.
**Done when:** the **full Gamer route** (with and without kills) runs in 25–30 min and both endings (Gamer, Clean Run) roll into the credits.

### M8 — Furry route and The Raid act
`FLF-*`, `C6-*`, `END-008`, `END-009`, `RAD-005`, `BHL-017`, `NAR-025`, `ART-101`, `ART-103`, `ART-121`, `ART-150` (Furry), `AUD-010` (Reflection, Raid Night, RGB Overdrive, Pawsome).
**Done when:** `C6-030` passes.

### M9 — Toxic route and meta-narrative
`C7-*`, `END-007`, `FUR-010`, `FUR-011`, `AUD-008`, `MET-*`, `NAR-012`, `NAR-026`, `NAR-027`, `NAR-028`, `BND-011`.
**Done when:** `C7-030` passes and the reset/reload lines react to the last ending.

### M10 — Release
`NAR-031` (final English copy pass), `L10N-003`, `LOR-005`, `LOR-006`, `D-14`, `QA-012`, `QA-013`, `QA-014`, `DST-*`, `CORE-005` (decorative border).

---

## 7. Appendices

### Appendix A — Suggested enemy schema (`data/enemies/barky.json`, aligned with LORE §19.1)

```json
{
  "id": "barky",
  "name": "Barky",
  "hp": 30, "atk": 4, "def": 2,
  "exp": 20, "gold": 10,
  "exGamer": true,
  "sprite": "sprites/battle/barky.json",
  "offset": [0, 0],
  "voice": "barky",
  "background": "grid_lamp",
  "check": "enemy.barky.check",
  "acts": [
    { "id": "cringe_check", "label": "Cringe Check", "text": "enemy.barky.act.cringe",
      "repeatText": "enemy.barky.act.cringe2", "sfx": "zipper",
      "effects": { "mercy": 50, "setFlag": "battle.barky.blushed", "unlockPattern": "howl_wave" },
      "response": "enemy.barky.reply.cringe", "repeatResponse": "enemy.barky.reply.cringe2" },
    { "id": "offer_dew", "label": "Offer Dew", "text": "enemy.barky.act.dew",
      "effects": { "spareable": true }, "response": "enemy.barky.reply.dew" },
    { "id": "debate_lore", "label": "Debate Lore", "text": "enemy.barky.act.lore",
      "effects": { "skipNextAttack": true }, "response": "enemy.barky.reply.lore" },
    { "id": "laser", "label": "Laser", "text": "enemy.barky.act.laser",
      "requires": { "item": "laser_pointer", "equippedOrCarried": true },
      "effects": { "spareable": true, "skipNextAttack": true }, "response": "enemy.barky.reply.laser" },
    { "id": "accept_hug", "label": "Accept Hug", "text": "enemy.barky.act.hug",
      "effects": { "spareNow": true, "fur": 15 }, "response": "enemy.barky.reply.hug" }
  ],
  "spareWhen": { "any": [{ "mercy": 100 }, { "flag": "battle.barky.spareable" }] },
  "flavor": {
    "default": ["enemy.barky.flavor.1", "enemy.barky.flavor.2"],
    "lowHp": ["enemy.barky.flavor.lowhp"],
    "spareable": ["enemy.barky.flavor.spareable"],
    "flag:battle.barky.blushed": ["enemy.barky.flavor.blushing"]
  },
  "dialogue": {
    "default": ["enemy.barky.bubble.1", "enemy.barky.bubble.flash"],
    "afterHit": ["enemy.barky.bubble.hit"]
  },
  "patterns": [
    { "id": "uwu_rain", "weight": 2, "params": { "speed": 3, "density": 0.4, "damage": 3, "fur": 6 } },
    { "id": "dew_bounce", "weight": 1, "params": { "cans": 3, "damage": 4, "fur": 4 } },
    { "id": "howl_wave", "when": "flag:battle.barky.blushed", "params": { "damage": 4, "fur": 4 } }
  ],
  "spareText": "enemy.barky.spare",
  "deathText": "enemy.barky.death",
  "furryOverLine": "gameover.barky",
  "routeOverrides": { "furry": "cutscene:barky_welcome_pack" },
  "canFlee": false,
  "music": "music/alpha_wolf.ogg"
}
```

New fields compared to the previous version: `exGamer` (red spark on death, BND-003), `background` (BUI-013), `repeatText`/`repeatResponse` (the ACT's 2nd use), `requires.item` (ACT-006), `effects.fur`/`spareNow` (ACT-005), `unlockPattern`, `routeOverrides` (FLF-010), `furryOverLine` (BND-011), `params.fur` instead of `params.furry`.

### Appendix B — Required SFX (original equivalents)

| Function | Suggested name | Where |
| :--- | :--- | :--- |
| Move cursor | `menu_move` | every menu |
| Confirm | `menu_select` | every menu |
| Narrator voice | `voice_default` | "* " boxes |
| Character voices | `voice_<npc>` | dialogue (AUD-012) |
| Encounter beep | `encounter_beep` | ENC-002 (3×) |
| Soul dropping to the button | `encounter_fall` | ENC-002 |
| "!" | `alert` | ENC-001 |
| Slash (attack) | `slash` | FGT-004 |
| Hitmarker (crit) | `hitmarker` | FGT-008 |
| BONK (Furry route) | `bonk` | FLF-008 |
| Enemy hit | `enemy_hit` | FGT-005 |
| Soul hit | `hurt` | BHL-007 |
| Heal | `heal` | items, save |
| Save (can opening, *tssss*) | `save_can` | SAV-002 |
| Spare | `spare_poof` | MRC-001 |
| Notification (*ding*) | `notify` | spare (optional), CELL |
| Vaporize (plush dust) | `vaporize` | BND-003 |
| Level up | `level_up` | BND-004 |
| Flee | `flee` | MRC-002 |
| Soul cracking | `soul_crack` | FLF-007 (GAMER OVER) |
| Soul shattering | `soul_shatter` | FLF-007 |
| *Pomf* | `pomf` | FUR-005, FURRY OVER |
| Plush squeak | `plush_squeak` | Hug, Accept Hug |
| Zipper | `zipper` | Cringe Check |
| Off-key howl | `howl` | Barky |
| Glitter spray | `glitter_spray` | C1-002 |
| Tab closing (fast clicks) | `tab_close` | Furiell, Alphys |
| MLG air horn | `airhorn` | Senpai Sans |
| Dial-up modem | `modem` | R3 save |
| *Crunch* | `step_crunch` | Dorito piles (OVW-014) |
| Phone buzz | `phone_buzz` | RAD-003, MNU-006 |
| Phone | `phone_ring`, `phone_dial` | MNU-004 |
| Tape ripping (*RIIIIIP*) | `tape_rip` | FLF-002 |
| Cell door | `jail_door` | horny jail |
| Voice chat "user left" | `user_left` | rage-quit |
| Bottle bouncing | `bottle_bounce` | Bottle Guy, Moderator |
| Audience | `crowd` | Mettapaw's quiz |
| Door/transition | `door` | OVW-008 (if any) |
| Switch | `switch` | puzzles |
| Cable shock | `zap` | C1-006 |
| Title appearing | `intro_noise` | FRT-003 |
| Name white fade | `name_swoosh` | FRT-007 |
| Ribbit | `ribbit` | easter eggs, frog soul |

### Appendix C — Most affected files in the current code

| File | What changes |
| :--- | :--- |
| `src/combat/BattleSystem.cpp/.hpp` | an almost total rewrite (BFL-001); split into `BattleBox`, `BattleHud` (HP/FUR/FLF), `BattleMenu` (FIGHT/BONK), `FightBar`, `EnemyTurn`, `BattleText`; remove the hardcoded text (BUG-21, BUG-22) |
| `include/combat/Enemy.hpp` | becomes `EnemyDef` (data) + `EnemyInstance` (in-battle state) |
| `include/combat/Bullet.hpp` | a bullet system with sprites, per-type hitboxes, pooling, and per-bullet `fur`/`shame`/`residual` |
| `src/scenes/BattleScene.cpp` | receives the `Encounter` as a parameter (with `routeOverrides`) |
| `src/scenes/OverworldScene.cpp` | becomes a generic `RoomScene` loaded from Tiled (BUG-18, BUG-19) |
| `src/scenes/GameOverScene.cpp` | FURRY OVER and GAMER OVER (BND-010, FLF-007, BUG-20) |
| `src/ui/DialogBox.cpp/.hpp` | becomes the `TextBox` on top of the text engine (TXT-001) |
| `src/core/Game.cpp` | fixed 30 Hz loop (done), the raid clock (RAD-001) |
| `src/core/SceneManager.cpp/.hpp` | pluggable transitions (CORE-003) |
| `include/core/GameContext.hpp` | a single `fur.value` (BUG-27), the new `RouteType` (BUG-25), `fluff`, `raid` |
| `include/core/Constants.hpp` | the official palette + per-area palettes (LORE §25.2), FLUFF `#FF77C8`/`#3A1030` |
| `src/services/SaveService.cpp` | schema v2 (FUR, FLUFF, raid, route, meta `last_ending`/`ever_furry`/`last_name`) with migration |
| `src/services/ResourceService.cpp` | fonts by `FontId` (Main, Sans, Papyrus); spritesheets |
| `src/services/AudioService.cpp` | loop points, fades, multi-instance, pitch/tempo (AUD-008) |
| `src/scenes/TitleScene.cpp`, `NameSelectScene.cpp`, `LoreScene.cpp`, `InstructionsScene.cpp` | Undertale's flow (FRT-*), the LORE cards (BUG-29), easter eggs in `data/names.json` |
| `src/entities/Player.cpp` | a spritesheet per FUR stage (FUR-007) |
| `src/entities/Interactable.cpp` | a sprite save point, generic objects from Tiled |
| `data/` (new) | `fur.json`, `raid.json`, `music.json`, `names.json`, `shops/*.json`, `encounters/<area>_*.json`, `enemies/*.json` (~30 enemies), `dialogue/en/<area>.json`, a revised `flags.md` |

### Appendix D — Traceability: LORE → items

| LORE | Topic | Items |
| :--- | :--- | :--- |
| §1 | Pitch | — (guides everything) |
| §2 | Content and tone rules | `LOR-*`, `NAR-014`, `NAR-031`, `D-13`, `D-14` |
| §3 | Canonical decisions | `D-01`, `D-05`, `D-06`, `D-07`, `D-11` |
| §4 | Cosmology (areas, Barrier) | `C1`..`C5` (concepts), `C5-004`, `GFX-014` |
| §5 | History, timeline, the Seven Moderators | `FRT-002`, `NAR-015`, `NAR-016`, `C5-023` |
| §6 | World rules (soul, conversion, jars, SHAME, Floofy) | `FUR-*`, `FLF-*`, `C5-003`, `NAR-007` |
| §7 | Reveal layers | `C1-002` (2), `C1-011` (3), `C2-006` (4), `C3-002`/`C5-003` (5), `C5-001` (6), `C4-003` (7), `NAR-029`/`FLF-001` (8) |
| §8.1 | Soul modes | `BHL-010`..`BHL-012`, `FLF-013`, `BUI-007`, `ART-021` |
| §8.2 | LV and EXP | `NAR-010`, `BND-004`, `NAR-023` |
| §8.3 | HP and FURRY OVER | `BND-010`, `C2-022` |
| §8.4 | FUR | `FUR-002`..`FUR-012` |
| §8.5 | The Mirror | `FLF-001`..`FLF-003` |
| §8.6 | FLUFF | `FLF-004`..`FLF-007` |
| §8.7 | The Raid Clock | `RAD-*`, `D-12` |
| §8.8 | SAVE, G and death | `SAV-001`, `SAV-002`, `SAV-007`, `BND-003`, `FLF-011` |
| §9 | Intro (10 cards) | `FRT-002`, `ART-041`, `ART-170`, `BUG-29` |
| §10 | Cast | `NAR-002`..`NAR-022`, `AUD-012`, `ART-002`, `ART-102` |
| §11 | Structure and pacing | `QA-020`, `ENC-006`, roadmap |
| §12 | Ruins | `C1-*` |
| §13 | Snowdinha | `C2-*` |
| §14 | Wetfall | `C3-*` |
| §15 | Hotland | `C4-*` |
| §16 | Con Center | `C5-*` |
| §17 | Furry route and The Raid | `FLF-*`, `C6-*` |
| §18 | Bestiary | `C1-020`, `C2-020`, `C3-020`, `C4-020`, `C6-020`, `NAR-006`, `ART-110` |
| §19 | Bosses | `C1-021`, `C1-022`, `C2-021`, `C3-021`, `C4-021`, `C5-021`, `C5-022`, `C7-004`, `C6-021`, `ART-111` |
| §20 | Items, equipment and shops | `ITM-006`, `ITM-007`, `FUR-009`, `SHP-*` |
| §21 | CELL | `MNU-004`, `MNU-006`, `NAR-004` |
| §22 | Routes and endings | `PRG-031`, `END-*` |
| §23 | FURRY OVER / GAMER OVER | `BND-010`, `BND-011`, `FLF-007` |
| §24 | Meta-narrative and titles | `MET-*`, `FRT-003` |
| §25 | Visual bible | `GFX-011`..`GFX-014`, `BUI-013`, `ART-*` |
| §26 | Placeholders | `ART-000`, `ART-*`, `GFX-005` (fonts) |
| §27 | Audio bible | `AUD-010`, `AUD-011` |
| §28 | Flags | `PRG-021` |
| §29 | Undertale equivalence | reference for `C*` |
| §30 | Running gags and easter eggs | `NAR-028`, `FRT-008`, `C*` (setups and payoffs), `END-010` |
| §31 | Glossary | `NAR-010` |
