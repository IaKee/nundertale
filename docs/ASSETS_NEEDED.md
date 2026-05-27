# nUndertale — Assets needed (shopping list)

Status as of 2026-10-17 (synergy round 5). This file consolidates the art, audio and font needs scattered across `docs/LORE.md` §25–§27 and the `ART`/`AUD` epics in `BACKLOG.md`, and checks them against what is in `assets/` today. Use it to decide what to look for, in what format, and how many variants each asset needs.

**Priorities** (LORE §26): **P0** = a playable Ruins (vertical slice) · **P1** = the rest of the Gamer route · **P2** = Furry route, Toxic route and optional content.

---

## 1. Ground rules for every asset

| Rule | Detail |
| :--- | :--- |
| Canvas | Virtual 640×480. Art is **native 1×** (Undertale scale, 320×240 for a full screen) and the game draws it at **2×**. All sizes in this file are native 1×. |
| Format | **PNG with alpha** for everything new. Spritesheets: one row per animation, fixed cell size, no padding needed. The legacy magenta `#FF00FF` key (`pepe_char.bmp`) is legacy only. |
| Music | **OGG Vorbis**, 44.1 kHz stereo, ~128–160 kbps. A seamless loop, or an intro plus loop (loop points go in `data/music.json`, AUD-005). |
| SFX | **OGG** (or WAV under 1 s), mono, 44.1 kHz, peak about −3 dB, no silence at the start (otherwise it feels laggy). |
| Fonts | TTF/OTF with an open license (OFL or similar). English only: ASCII plus `★ ♥ — “ ” ‘ ’ …`. |
| File names | Put a placeholder at its **final path** (the paths below), so swapping it later is just replacing the file. |
| Logging | Every non-final file goes in `assets/PLACEHOLDERS.md` (file, source URL, author, license). Started in round 5 (ART-000). |
| Undertale rips | Fine as a **size reference only**, in `assets/_ref/` (git-ignored). Never committed or shipped. |
| Content rules | No real people (photos, meme faces), nothing explicit (the ceiling is mosaic blocks, closed objects and stink lines), **adult proportions** for every character, and any text inside art in English. |
| Battle look | **White on black**, with color only for special moments (souls, FUR purple, yellow spare names). The overworld is colorful, with one limited palette per area (§3). |
| Silhouette | Each character needs one element that reads in solid black (Barky's ears, Sans's body pillow, Papyrus's cardboard ears, Sharkdyne's harpoon, Asgrr's lanyards, Mettapaw's ring light, Floofy's zipper head, the Moderator's chair-mech). |

> **Engine note:** today the battle sprites, souls, buttons, save star, FIGHT target and most effects are **drawn with primitives**. Dropping a PNG in place does nothing until its hook exists. **GFX-002** (round 5) added the sheet format and hooks for **Barky in battle** and **Pepe in the overworld**, so those two can be dropped in today; the rest need small per-asset hooks.

---

## 2. What is in `assets/` today

| File | Used for | Status | Action |
| :--- | :--- | :--- | :--- |
| `music/Undertale - Once Upon A Time Remix (ft Yuri, zR).ogg` | Intro (LoreScene) | **Third-party remix of Undertale music** | Replace before any release (AUD-020). Placeholder target: "Once Upon a Dew". |
| `music/main_menu_loop.ogg` | Title / name screen | Origin unknown | Log the license or replace. |
| `music/epic_background_loop.ogg` | Overworld **and** battle | Origin unknown | Split into an area track plus a battle track (AUD-001). |
| `sfx/sans_type.wav` | Sans voice / name easter egg | **Likely an Undertale rip** | Replace (AUD-020). |
| `sfx/selection_sound.wav` | Nothing anymore | Unused | Split into `menu_move`/`menu_select` placeholders (AUD-003, done); can be deleted. |
| `sfx/u_activate.wav`, `u1..u12.ogg`, `n.ogg`, `type*.ogg`, `areusure.ogg`, `cartoon_slip.ogg`, `pipipipopopo_backwards.ogg` | Misc. stand-ins (ding, voices, FURRY OVER, Barky's entrance) | Origin unknown | Log the license, then rename or replace to the Appendix B names. |
| `sfx/menu_move.wav`, `sfx/menu_select.wav`, `sfx/heal.wav` | Cursor, confirm/back, healing | **Original placeholders** from `tools/gen_placeholder_sfx.js` | Replace with final sounds (same names) when you have them. |
| `img/lore1..9.bmp`, `img/lore10/` (271 frames) | Intro cards | Hand-made, 398×214, BMP | Redraw at 400×220 PNG in a limited sepia palette (ART-041). |
| `img/lore4.bmp` | Intro card "BEGONE!" | **Uses a real face** | **P0** redraw with no recognizable face (ART-170). |
| `img/pepe_char.bmp` | Pepe in the overworld | Legacy, magenta key | Replace with `sprites/ow/pepe_frog.png` (ART-001). |
| `img/whoru.bmp` | SKYRIM name easter egg | Probably third-party | Check the license or redraw. |
| `img/icon.png` | Window icon | OK for now | Final icon set in ART-050. |
| `img/test.bmp`, `img/npode.bmp`, `img/lore.jpg`, `img/lore10gif.gif`, `img/deletethis` | — | Leftovers | Remove or move out of `assets/` (REPO-004). |
| `Untitled-1.psd`, `lore.psd` | Art sources | Not loaded at runtime | Move out of `assets/` (REPO-004); the test package already skips them. |
| `fonts/8bitoperator.ttf`, `monster_friend*.otf`, `gomarice_shmup_in_the_zone.ttf` | UI, titles, disclaimer | OK, but confirm the licenses | Log them in `assets/LICENSES.md`. |

---

## 3. Area palettes (for every overworld asset)

| Area | Base | Accent | Highlight | Notes |
| :--- | :--- | :--- | :--- | :--- |
| Ruins (P0) | `#3B1F5C` · `#5E3A87` | `#7CFF3A` | `#F4E3FF` | Banners `#C9A0DC`; black 18+ curtain `#111`; CRT `#1E3FBF` |
| Snowdinha | `#E8F0FF` · `#9BB3D9` | aurora `#FF4FD8 → #4FD8FF → #7CFF3A` | `#FFFFFF` | Wood `#8A5A3C`; LAN screen `#2EE86B` |
| Wetfall | `#0E1A3A` · `#1F3B73` | `#3FF2FF` · `#FF8FC7` | `#B9FBFF` | Drops, fursuit clotheslines |
| Hotland | `#5A1A0A` · `#C2410C` | `#39FF14` | `#FFB347` | Heat shimmer; cables `#222`/`#444` |
| Con Center | `#CFC4AE` · `#8F8775` | `#E6B422` | Multicolor stained glass | Deliberately desaturated |
| Barrier | `#1A1A0A` | fog `#B8FF4A` | white spinner | Animated stink lines |
| Surface / Pepe's room | `#2B2B3A` · sunrise `#FF9E5E` | RGB `#FF2E63` | monitor `#9FD3FF` | — |
| Fluff particles (everywhere) | `#FFB3E6` · `#FFD27F` | — | — | Already drawn in code (FUR-010) |

---

## 4. P0 — the Ruins vertical slice

### 4.1 Characters — overworld spritesheets

Standard: 4 directions (down, up, left, right). Down/up have standing + 4 walk frames; left/right have standing + 2 walk frames (Frisk-style). Mirror left/right only if the design is symmetric.

| File | What it is | Native size | Variants / frames | References & ideas | Where to look |
| :--- | :--- | :---: | :--- | :--- | :--- |
| `sprites/ow/pepe_frog.png` | Pepe, the protagonist: the classic Pepe meme frog (eye domes, heavy eyelids, wide two-part reddish-brown lips) in a blue T-shirt with nacho stains, shorts, flip-flops, eye bags, green hands | ~20×30 | 4 dirs × (1 stand + 2–4 walk) ≈ **16 frames** + "lying down" and "getting up" (R1, 2–3 frames) | Frisk's proportions and walk cycle; the classic Pepe look, as in the legacy `pepe_char.bmp`. Spec and anchor sprite: `docs/art_prompts/ruins/A01_ow_pepe_frog.md`. Pepe is Matt Furie's character: review before any commercial release | Commission, or draw it over a CC0 top-down base (Kenney "Tiny Dungeon" / "Roguelike Characters" for proportions) |
| `sprites/ow/furiell.png` | Furiell: a white goat in a paw-print apron, half-moon glasses, a zipper on her back (a fursuit) | ~26×40 | 4 dirs × 2–4 walk + talking 2f | Toriel's silhouette, but in a fursuit; the zipper should be visible from behind | Commission / OpenGameArt "goat" bases |
| `sprites/ow/barky.png` | Barky: an edgy wolf fursuiter in a studded jacket, plastic chains, a safety-pinned tail | ~22×32 | 4 dirs × 2 walk + crouch/drop (entrance, 2f) | Emo/scene kid in a cheap wolf suit; the ears are the silhouette | Commission |
| `sprites/ow/floofy.png` | Floofy: a flower wearing a fursuit head | ~20×20 | idle 2f, talking 2f, sinister 2f (the zipper open), sinking 3f | Flowey, with a cheap mascot head | Draw (small, easy) |

### 4.2 Characters — battle (white on black)

| File | What it is | Native size | Variants | Notes |
| :--- | :--- | :---: | :--- | :--- |
| `sprites/battle/barky.png` | Barky in battle, **in parts** (ears, jacket, chains, tail, visible zipper) | ~60×80 | idle 2f, hurt 1f, blush 1f (after Cringe Check), spared 1f | Parts let the code wobble ears/tail like Undertale's enemies |
| `sprites/battle/furiell.png` | Furiell, the Ruins boss | ~80×110 | idle, hurt, sad | — |
| `sprites/battle/floofy.png` | Floofy in battle | ~40×40 | idle, talking, sinister | — |
| `sprites/battle/fursuit_dummy.png` | A training dummy wearing a fursuit | ~40×60 | idle 2f, hurt | Undertale's Dummy |
| `sprites/battle/froggsona.png` · `shysona.png` · `modsmal.png` · `lurkeye.png` · `reply_guy.png` · `grasstoid.png` | Ruins common enemies (LORE §18.1) | 30–60 | idle 2f + hurt each → **6 × 3 = 18 frames** | Parodies of Froggit, Whimsun, Moldsmal, Loox, Vegetoid… Read §18.1 for each one's joke |

### 4.3 Dialogue portraits (~50×50)

| File | Variants | Notes |
| :--- | :--- | :--- |
| `portraits/furiell_*.png` | neutral, happy, sad, angry, flustered ("tab closed") = **5** | — |
| `portraits/floofy_*.png` | cute, smug, sinister (zipper open) = **3** | — |
| `portraits/barky_*.png` | edgy, embarrassed, happy = **3** | — |
| Pepe | **none** | Pepe never speaks (D-15) |

### 4.4 Bullets (white, 8–16 px, a hitbox in JSON for each)

| File | Who uses it | Variants |
| :--- | :--- | :--- |
| `bullets/heart_pellet.png` | Floofy ("friendliness pellets") | 1–2f spin |
| `bullets/uwu_letters.png` | Barky (today it's drawn text) | U, W (or the whole "UWU"), 1f |
| `bullets/can.png` | Barky's Dew Bounce | 4f rotation |
| `bullets/sound_ring.png` | Barky's howl wave | 2–3 sizes |
| `bullets/fluff_ball.png` · `bullets/glitter_hand.png` (~30) | Furiell | 1–2f each |
| `bullets/colon_three.png` · `fly.png` · `moth.png` · `gel_hammer.png` · `text_bug.png` · `grass_blade.png` (+ green) · `tear.png` | Ruins commons | 1–2f each, ~10 files |

### 4.5 UI

| File | What it is | Size | Variants |
| :--- | :--- | :---: | :--- |
| `ui/buttons.png` | FIGHT, ACT, ITEM, MERCY with chunky hand-drawn letters and icons | 55×21 each | 4 buttons × 2 states (orange / yellow selected) = **8**, + BONK × 2 (P2), + a grayed MERCY (P1) |
| `ui/souls.png` | Souls | 8×8 (heart) | red, blue, green, yellow, **pink plush with ears**, **frog** = **6**; + cracked/shard frames (P2), + the FURRY OVER sequence (ears and tail sprouting, inflating, ~6f) |
| `ui/target.png` · `ui/attack_bar.png` | FIGHT target (elliptical) and the moving bar | ~280×65 / 6×65 | bar: white + flashing 2f |
| `ui/slash.png` | Attack hit: an FPS hitmarker / mouse cursor | ~30×30 | ~6f (the same length as Undertale's slash) |
| `ui/hud.png` | HP bar pieces, the purple FUR overlay, the `FUR` label | — | 1 each (the FLUFF bar is P2) |
| `ui/dialog_box.png` · `ui/portrait_frame.png` | Dialogue box (white border on black) | 289×76 (box) | 1 each (can stay drawn in code) |
| `ui/exclaim.png` | The "!" over Pepe on encounter | ~10×12 | 1 |
| `ui/furry_over.png` | The "FURRY OVER" logo (laid out like GAME OVER) | ~210×90 | 1 |
| `ui/logo.png` | "nUNDERTALE" with the trembling lowercase red `n` | ~200×30 | 1 (the `n` as a separate layer so it can shake) |
| `props/save_point.png` | A golden 4-point star spinning on top of a soda bottle | ~20×28 | **4–8 frames** of spin |

### 4.6 Tiles and props — Ruins (20×20 tiles)

| File | Content |
| :--- | :--- |
| `tiles/ruins.png` | Purple stone floor, carpet, walls (top/side/corners), stairs, doors, pillars: about 40–60 tiles |
| `props/ruins_*.png` | Con '14 banner, a booth, the black 18+ curtain, a CRT, the notice board, an RGB switch, the Dorito bowl (full → empty → knocked over: **3 states**), a wrapper pile (crunchy floor), cables (the "spikes" puzzle, on/off), a fireplace, an armchair, a bed, a closet, a **mirror** (with a FUR variant), a bookshelf, the lamp ("Barky's moon") |

Ideas: think of "an abandoned furry convention from 2014 inside purple Undertale ruins". Banners and signs must be in English.

### 4.7 Intro cards

| File | What | Size | Notes |
| :--- | :--- | :---: | :--- |
| `img/lore4` | "BEGONE!" card | 400×220 | **Redraw without the real face** (ART-170), same composition |
| `img/lore1..9`, `lore10` | The other 9 cards + the tall falling image | 400×220; `lore10` tall for vertical scrolling | Optional for P0: the current ones work. A limited sepia palette, like Undertale's intro |

### 4.8 Audio — music (P0)

| Track (final name) | Where | Parody of | Direction | Today |
| :--- | :--- | :--- | :--- | :--- |
| `music/once_upon_a_dew.ogg` | Intro, title | Once Upon a Time | The main theme, with **Pepe's leitmotif** (4 descending notes) | The Undertale remix (**replace**) |
| `music/start_menu.ogg` | Name screen | Start Menu | Music box | `main_menu_loop.ogg` |
| `music/your_best_floof.ogg` | Floofy | Your Best Friend | Cute and fake, a pitch bend at the end | — |
| `music/fluffy_mother.ogg` | Furiell (overworld/house) | Fallen Down / Home | Lullaby | — |
| `music/ruins.ogg` | Ruins | Ruins | Lo-fi chiptune with vinyl crackle ("beats to get lost to") | `epic_background_loop.ogg` |
| `music/enemy_approaching.ogg` | Common battles | Enemy Approaching | Upbeat, with 8-bit "uwu"s | the same overworld track |
| `music/alpha_wolf.ogg` | Barky | — | Emo chiptune with an off-key howl | the same overworld track |
| `music/heartache_fluffier.ogg` | Furiell (fight) | Heartache | Dramatic, with glitter bells | — |
| `music/furry_over.ogg` | FURRY OVER screen | Determination | Short and soft | — |

Placeholder sources: OpenGameArt "chiptune" (CC0/CC-BY), Incompetech (CC-BY), itch.io free music packs, FreePD. Final versions should be original compositions "in the style of" (D-04).

### 4.9 Audio — SFX (P0)

Make them with **sfxr / jsfxr / Bfxr / ChipTone** (free, and the output is yours), or take CC0 sounds from Freesound/Kenney "Interface Sounds" / "Impact Sounds".

| File | What it sounds like | Used by |
| :--- | :--- | :--- |
| `sfx/menu_move.wav` | A short squeak | every menu (AUD-003; placeholder in place) |
| `sfx/menu_select.ogg` | "Select" | every menu |
| `sfx/heal.wav` | A rising chime | items, save (placeholder in place) |
| `sfx/save_can.ogg` | A soda can opening, *tssss* | save point |
| `sfx/hurt.ogg` | Soul hit | battle |
| `sfx/enemy_hit.ogg` · `sfx/slash.ogg` · `sfx/hitmarker.ogg` | Attack, and an FPS hitmarker on a crit | FIGHT |
| `sfx/encounter_beep.ogg` · `sfx/encounter_fall.ogg` · `sfx/alert.ogg` | Encounter start (3 beeps, the soul drop, the "!") | ENC-001/002 |
| `sfx/spare_poof.ogg` · `sfx/notify.ogg` | Spare poof, notification *ding* | MERCY, CELL |
| `sfx/vaporize.ogg` | Plush stuffing dust | kills |
| `sfx/level_up.ogg` · `sfx/flee.ogg` | Level up, running away | battle end |
| `sfx/pomf.ogg` | A soft fluffy "pomf" | FUR stage up, FURRY OVER |
| `sfx/plush_squeak.ogg` | A plush toy squeak | Hug / Accept Hug |
| `sfx/zipper.ogg` | A zipper | Cringe Check |
| `sfx/howl.ogg` | An off-key howl | Barky |
| `sfx/glitter_spray.ogg` | An aerosol spray with sparkles | Furiell scaring Floofy off |
| `sfx/phone_buzz.ogg` | Phone vibrating | guild messages (RAD-003, done in code, uses a stand-in) |
| `sfx/voice_default.ogg` + `voice_furiell` · `voice_barky` · `voice_floofy` | One short beep per character (Furiell soft and low, Barky a high little bark, Floofy the default) | dialogue (AUD-012) |

---

## 5. P1 — the rest of the Gamer route

Same formats as P0. The counts are the minimum to get each area playable.

| Category | Files (LORE §26) | Count / variants |
| :--- | :--- | :--- |
| Overworld cast | `sans` (short skeleton, blue hoodie, pink slippers, body pillow) ~22×30 · `papyrus` (tall skeleton in a cardboard fursuit with LEDs, ring light, selfie stick) ~24×44 · `sharkdyne` + `_overheated` ~26×40 · `alphys` (lizard, lab coat, MOD badge) ~22×28 · `mettapaw` (box robot, ring-light face) ~24×36 · `asgrr` (giant goat, lanyard cape, trophy crown) ~32×48 · NPCs `grillby` · `bunny` · `innkeeper` · `washua` · `uwumie` · `townsfolk_01..04` ~20×30 | ~15 characters × ~16 frames |
| Pepe FUR stages | `pepe_ears` · `pepe_paws` · `pepe_tail` (animated shiny tail) · `pepe_snout` (mint-green patches) ~20×30 | **4 full spritesheets**, the same frames as `pepe_frog` (FUR-007) |
| Battle commons | Snowdinha: `doge` · `lesser_doge` (extendable neck) · `punsona` · `jerry` · `dogsonas`. Wetfall: `vtuber_en` · `musk_ron` · `gatekeeper` · `washua` · `uwumie` · `mad_daki`. Hotland: `overclockin` · `tsundere_bot` | 13 enemies × (idle 2f + hurt) |
| Bosses | `papyrus` (~60×120: idle, pose, detached head) · `sharkdyne` (~90×120: idle, attack, hurt) · `mettapaw` (~70×110 host) · `asgrr` (~120×140: idle, kneeling) · `the_mascot` (~300×240, in parts) | 5 bosses, 2–3 poses each |
| Portraits | `sans_*` · `papyrus_*` · `sharkdyne_*` · `alphys_*` · `asgrr_*` | 3 each = **15** |
| Bullets | `word_blue` · `cardboard_sword` · `snowflake_letter` · `axe` · `bone` (sizes, blue/white) · `dm_arrow` (+ yellow) · `harpoon` · `shield` · `note` · `pixel_glitch` · `sweat` · `green_cloud` · `gate_bar` · `soap_bubble` · `daki_pellet` · `heat_cloud` · `error_popup` · `answer_letters` · `trident` (blue/orange) · `tea_drop` · `trophy` · `mini_fursuiter` · `plush_tentacle` · `owo_missile` · `plastic_eye` | ~25 files |
| Tilesets | `tiles/snowdinha` (styrofoam snow, plastic pines with string lights, sentry station, CAPTCHA tiles, LAN House, the ceiling aurora) · `tiles/wetfall` (blue cave, water, mushrooms, crystals, fursuit clotheslines, Uwu-mie statue) · `tiles/hotland` (server racks, cables, fans, water cooler, lab, quiz stage, `DO NOT TURN OFF` / `SERVER 7: 18+` signs) · `tiles/concenter` (beige carpet, booths under sheets, lanyards, stained glass of the 7 Moderators + Azzy, trophies, jars, throne, Barrier) | 4 tilesets, ~40–60 tiles each, + props |
| UI | `ui/cell.png` (a flip phone screen with paw stickers; the code already draws the CELL list and the raid clock) · grayed MERCY · `ui/dust_plush.png` · the stink-line effect | ~4 |
| Music | Senpai Sans (lazy swing) · Nyeh Heh Stream (goofy march) · Snowdinha Town (8-bit bossa nova) · Bonestream · Wetfall (city pop with drizzle) · DMs of Justice (heroic, with "rawr"s) · Hotland (funk + case fans) · Death by Engagement (disco) · Con Center (Furiell's theme with echo) · ASGRR (epic and sad) · Your Best Nightmare (Mascot Mix) + a shop theme | **12 tracks** |
| SFX | `tab_close` · `airhorn` · `notify` · `modem` (R3 save) · `step_crunch` · `phone_ring` / `phone_dial` · `crowd` · `door` · `switch` · `zap` · `intro_noise` · `name_swoosh` · `ribbit` | ~14 |
| Voices | Senpai Sans (bony beep) · Papyrus · Sharkdyne (metallic) · Alphys (keyboard clicks, `...is typing`) · Mettapaw (electronic) · Asgrr (deep) · Uwu-mie (chaotic) | 7 |
| Fonts | A free **Comic Sans-style** font for Sans (e.g. **Comic Neue**, OFL) and a free **Papyrus-style** one for Papyrus (search Google Fonts / Font Squirrel for "papyrus alternative"). Don't ship the originals. | 2 |
| Screens | Ending cards `bg/ending_gamer.png` · `_clean.png` (320×240) · `bg/pepe_room.png` (320×240: basement bedroom, monitor, chair, bottles) | 3 |

---

## 6. P2 — Furry route, Toxic route, optional

| Category | Files | Count |
| :--- | :--- | :--- |
| Characters | `pepe_pawsome` (green frog-wolf with a scarf, fluffy ears, tail, eye bags still there) ~22×32 · `moderator_mecha` (RGB gaming-chair mech + green cloud) ~40×40 · `raider_01..03` (silhouettes in headsets) · `napstablurk` | 6 |
| Battle | `sans` (idle, asleep) · `moderator` (~140×140, phases) · gamers: `discord_simp` · `gacha_whale` · `bottle_guy` · `apology_streamer` · `dragon_excuse_guy` (just the guy, no dragon art) · `napstablurk` | 8 |
| Portraits | `moderator_*` · `pepe_pawsome_reflection` (2 each) | 4 |
| Bullets | `air_horn_skull` (~40) · `beam` · `hey_text` · `credit_card` · `bottle_yellow` · `text_line` · `ban_hammer` · `mosaic_block` · `video_frame` | 9 |
| UI / effects | `ui/bonk.png` + BONK buttons · `ui/gamer_over.png` · the FLUFF bar · `ui/horny_jail.png` (a small pink cage) · `ui/mosaic.png` (8×8 blocks) · `ui/fluff_burst.png` · soul crack/shard frames | ~8 |
| Screens | `bg/mirror.png` (pulsing frame + reflection) · `bg/ending_toxic.png` · `bg/ending_furry_*.png` · `bg/title_*.png` (5 title variations, MET-005) · `tiles/surface.png` (the top of Mt. Dew at sunrise) · `img/lore5b` (the Seven Moderators in silhouette, optional) · Continue-menu backgrounds (one per area, ART-042) | ~14 |
| Music | Megalomania.exe (aggressive, air horns) · Reflection (the leitmotif on a pink music box) · Raid Night · RGB Overdrive · Pawsome · uwu-mie village · GAMER OVER · credits · Last Lobby (wind) | 9 |
| SFX | `bonk` · `jail_door` · `user_left` (voice chat "user left") · `bottle_bounce` · `tape_rip` · `soul_crack` · `soul_shatter` | 7 |
| Voices | xX_Moderator_Xx (headset crackle) | 1 |

---

## 7. Totals (approximate)

| Category | P0 | P1 | P2 | Total |
| :--- | :---: | :---: | :---: | :---: |
| Overworld characters (spritesheets) | 4 | ~19 (incl. 4 Pepe FUR stages) | 6 | ~29 |
| Battle sprites (enemies + bosses) | 9 | 18 | 8 | ~35 |
| Portrait images | 11 | 15 | 4 | 30 |
| Bullet files | ~16 | ~25 | 9 | ~50 |
| UI sheets / effects | ~11 | ~4 | ~8 | ~23 |
| Tilesets (+ prop sets) | 1 | 4 | 1 | 6 |
| Full-screen images / cards | 1 (lore4) | 3 | ~14 | ~18 |
| Music tracks | 9 | 12 | 9 | 30 |
| SFX | ~22 | ~14 | 7 | ~43 |
| Character voices | 4 | 7 | 1 | 12 |
| Fonts | 0 | 2 | 0 | 2 |

**The P0 core that unblocks a real vertical slice:** Pepe's spritesheet, Barky battle + overworld, the 8 battle buttons, the 6 souls, the save point, the Ruins tileset, `heal` / `menu_move` / `menu_select` / `save_can` / `pomf` / `zipper` / `plush_squeak` / `howl`, and 3 music tracks (Ruins, a common battle, Alpha Wolf). Everything else in P0 can follow.

---

## 8. Where to look (alternatives)

| Need | Free sources | Notes |
| :--- | :--- | :--- |
| Pixel-art bases (characters, tiles) | **Kenney.nl** (CC0: "Tiny Dungeon", "1-Bit Pack", "Pixel Platformer"), **OpenGameArt.org** (filter by CC0/CC-BY), **itch.io** (free + pixel art; check each license), LPC (Liberated Pixel Cup) characters | Undertale's battle style is 1-bit (white on black), so Kenney's **1-Bit Pack** fits bullets and UI well |
| Custom pixel art | Aseprite (paid; LibreSprite is a free fork), Pixelorama (free) | Draw at 1×, export PNG, let the game scale |
| Commissions | itch.io / Fiverr / r/HungryArtists | Brief the artist with this file + LORE §10 (cast) and §25 (visual bible) |
| SFX | **jsfxr** / **Bfxr** / **ChipTone** (you own the output), **Freesound.org** (check CC0 vs CC-BY), Kenney "Interface Sounds" / "Impact Sounds" | Keep them short and dry; mono |
| Music placeholders | **OpenGameArt** chiptune, **Incompetech** (CC-BY, needs credit), **FreePD** (CC0), itch.io music packs | Log CC-BY credits for the end credits |
| Music, final | Original compositions: FamiTracker / Furnace / BeepBox (free, browser) | BeepBox is the fastest way to sketch the 4-note leitmotif |
| Fonts | Google Fonts (OFL), Font Squirrel (check "commercial use"), dafont (check each license) | Comic Neue for Sans |

---

## 9. Next code tasks that consume these assets

| Code item | Unlocks |
| :--- | :--- |
| **GFX-002** sprite/spritesheet pipeline | **Done for Barky (battle) and Pepe (overworld)**: drop `png`+`json` in `assets/sprites/` (format in `assets/sprites/README.md`). Other sprites need their own hooks. |
| **AUD-001** `data/music.json` + **AUD-005** loop points | **Done**: edit `data/music.json` to swap a track or set loop points, no recompile |
| **AUD-002 / AUD-003** SFX names | `menu_move` / `menu_select`, `heal` and the rest of Appendix B |
| **OVW-003** Tiled map loader | Tilesets and props |
| **ART-000** `assets/PLACEHOLDERS.md` + **AUD-020** `assets/LICENSES.md` | License tracking before any public build |
