# Asset status log

Covers every image and audio file under `assets/` as of 2026-11-01 (after ART-185 round 8), grouped by folder. Fonts, `lang/` and text files are listed at the end for completeness. Replace a file by overwriting it with the same name. D-22: sprites and audio are never generated inside synergy runs; they are made interactively and only wired by runs.

**Status**
- **Final**: the user reviewed or accepted it, or it is the current ChatGPT art with nothing pending.
- **Placeholder**: stands in for art or audio that still has to be made, or has no license record.
- **Final (not checked in game)**: round 8 art, accepted on previews but not yet looked at in the running game at 2× (ART-185 acceptance).

**Source keys**
- **R7**: ChatGPT image gen, macro-pixel, round 7 (2026-10-29/30, ART-184; briefs in `../nundertale_sprite_audit/regen_r7/`).
- **R8**: ChatGPT image gen, macro-pixel, round 8 (2026-11-01, ART-185; briefs in `../nundertale_sprite_audit/regen_r8/`).
- **v3.1**: ChatGPT macro-pixel pass of 2026-10-18 (`../nundertale_sprite_audit/regen_v31/`, `macro_pixel.py`).
- **r1 (first pass)**: OpenAI image-generation guide reduced to 1× by `tools/art_prompts/reduce_*.py` (2026-10-17/18).
- **Synth**: deterministic synthesis by a script in `tools/` (`gen_placeholder_sfx.js`, `audio/generate_opening_sfx.py`, `audio/music/generate_opening_music.py`).
- **Legacy**: inherited from the 2023 project (git history from `Deleting useless files`, 2023-05-09); origin and license not recorded.

All ChatGPT/OpenAI art: generated/project work; check the image service's commercial-use terms before release. Pepe is © Matt Furie; the frog sheets are gone from use but `pepe_*` names may remain in old docs.

---

## bullets/ (`.png` + `.json` pairs)

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `can`, `colon_three`, `fluff_ball`, `glitter_hand`, `grass_blade`, `heart_pellet`, `paw_print`, `seen_eye`, `sound_ring`, `tear`, `uwu_letters` | Placeholder | r1 (first pass), ART-D01–D13 | Hand check at 2×; nothing urgent. |
| `fly`, `moth`, `gel_hammer`, `text_bug` | Placeholder | r1 (first pass) | Hand redraw at their size. |
| `turret`, `turret_line`, `companion_cube`, `portal`, `neurotoxin_cloud` | Placeholder | v3.1 (GLaDOwOS, R9a); not loaded yet | Review when R9a is wired. |
| `petal`, `pollen`, `rose_vine` | Final | v3.1, redone in R7 (ART-184, row 6/8/10: rose-shaped toy, thorned vine, cloud) | LOR-010 check noted in the item. |

## sprites/bullets/

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `petal.png`, `petal_bullet.png`, `rose_vine.png`, `thorn_petal.png`, `pollen.png` | Final | R7 (ART-184; ART-183 rows 6–10) | In-game 2× check. |
| `giant_petal.png`, `giant_petal.json` | Final (not checked in game) | R8, ART-185 row 9 (box-sized petal, D-20 amended; C1-002) | 2× check in R2's fake tutorial; LOR-010 review. The buzz sound is still to be made (see audio). |
| `fluff_tuft_big.png`, `fluff_tuft_small.png` | Placeholder | r1 (first pass) | Redraw at final size (Bass Shake bursts). |

## fonts/ (not images or audio)

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `8bitoperator.ttf`, `monster_friend.otf`, `monster_friend_fore.otf`, `gomarice_shmup_in_the_zone.ttf` | Placeholder (license) | Third-party fonts, Legacy | Record each font's license or replace. |

## img/

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `lore/card01.png`, `card03.png`–`card08.png`, `card10.png`, `card10_tall.png` | Final | v3.1 / R7 (ART-041, ART-170, ART-182, ART-184; card 6 typed per BUG-55) | Nothing. |
| `lore/card02.png`, `lore/card09.png` | Final (not checked in game) | R8, ART-185 rows 1–2 | 2× check in the intro. |
| `icon.png` | Placeholder | Legacy | Final window icon (needs license record). |
| `lore.jpg`, `lore1.bmp`–`lore9.bmp`, `lore10gif.gif`, `lore10/lore10__000000.bmp` … `lore10__000270.bmp` (271 frames) | Placeholder | Legacy intro cards, fallback only when `img/lore/` is incomplete | Delete when no longer a fallback. `lore4.bmp` uses a real face: do not ship. |
| `npode.bmp`, `pepe_char.bmp`, `test.bmp`, `whoru.bmp` | Placeholder | Legacy; `whoru.bmp` is the SKYRIM easter egg (origin unknown) | License record or removal. |
| `deletethis/75941.png` | Placeholder | Legacy, name says delete | Delete. |

## music/

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `Undertale - Once Upon A Time Remix (ft Yuri, zR).ogg` | Placeholder | Third-party remix of Undertale music, Legacy | **Replace before release** (original or licensed). |
| `main_menu_loop.ogg` | Placeholder | Legacy, origin unknown | License record or the "Start Menu" music box track. |
| `epic_background_loop.ogg` | Placeholder | Legacy, origin unknown | Split into an area track and a battle track (AUD-001). |
| `bass_shake.ogg` | Placeholder | The Bass Shake meme loop, 130 BPM, added 2026-10-19 (`c7d5b6d`); third-party meme audio | License or original replacement (LOR table, D-04). |
| `ruins_quiet.ogg`, `floofy_lounge.ogg`, `floofy_lounge_muffled.ogg`, `floofy_fight.ogg` | Placeholder | Synth/derived (`tools/audio/music/generate_opening_music.py`, T39, 2026-10-29: slowed and detuned) | Real tracks made interactively (D-22). |

## portraits/

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `furiell_{angry,flustered,happy,neutral,sad}.png` + `.json` | Final (not checked in game) | R8, D-24 look, 100×100 with `"density": 2` (ChatGPT neutral at 25× plus edits, `portrait_decode.py`) | BUG-63 loader must read `density`. |
| `furiell_{angry,flustered,happy,neutral,sad}_d1.png` + `.json` | Final (not checked in game) | R8, the 50×50 density-1 set kept beside the density-2 one | Pick one set when wired. |
| `furiell_silhouette.png` + `.json`, `furiell_silhouette_d1.png` + `.json` | Final (not checked in game) | R8, ART-185 row 10; outline rebuilt from the neutral portrait's shape | Not wired yet (C1-002). |
| `floofy_cute.png`, `floofy_pouncing.png`, `floofy_sinister.png`, `floofy_smug.png` | Final | v3.1 (ART-C02), 50×50 | Nothing. |
| `floofy_soaked.png` | Final | R7 (edit of `floofy_cute`, ART-184) | Nothing. |
| `barky_edgy.png`, `barky_happy.png` | Placeholder | r1 (first pass) | ART-183 row 24 pass. |
| `barky_embarrassed.png` | Placeholder | v3.1 (goblin ear tip, LORE v3) | Redo for D-16 (headset under the fursuit head, row 29). |
| `grub_pawsome_reflection.png`, `grub_pawsome_reflection_surprised.png` | Placeholder | v3.1, goblin-era | Redo as the mint wolf (ART-183 row 29). |

## props/

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `floofy_badge.png`, `ruins_reg_turnstile.png` + `.json`, `petal_trail_pieces.png` + `.json`, `strange_beacon.png` + `.json` | Final (not checked in game) | R8, ART-185 rows 14, 14b, 13, 18; `petal_trail_pieces` has 12 decals | Wire (C1-015, BUG-59, C1-029); `strange_beacon` is P3. |
| `ruins_hall_welcome_sign.png`, `ruins_hall_scanner.png`, `ruins_hall_schedule.png`, `ruins_hall_bench.png`, `ruins_hall_snacks.png`, `ruins_hall_light.png` | Final (not checked in game) | R8, ART-185 row 15 (R3 hall, C1-003) | Re-check once R3's layout exists. |
| `ruins_mech_keys.png` + `.json`, `ruins_mech_keyboard.png`, `ruins_floor_cables.png` + `.json`, `ruins_switch_door.png` + `.json` | Final (not checked in game) | R8, ART-185 row 16 (C1-004; new names so the old 18 px keys keep working) | Switch click and shutter sounds (audio, not made). |
| `ruins_banner_deodorant.png` | Final (not checked in game) | R8, text `DEODORANT IS / NOT OPTIONAL` stamped in 3×5 font (BUG-62) | Nothing. |
| `ruins_wrapper_pile.png` + `.json` | Placeholder | R8; still reads as triangles, not Doritos bags | Another pass (BUG-62). |
| `ruins_rgb_keyboard.png`, `ruins_rgb_keys.png` + `.json`, `ruins_rgb_switch.png` + `.json`, `ruins_cables.png` + `.json` | Placeholder (superseded) | v3.1, flat | Delete when C1-004 swaps in `ruins_mech_*`. |
| `ruins_landing_flowerbed.png` + `.json` | Final | R7 (replaces `ruins_plush_pile`), kept as is in R8 (BUG-58) | Nothing. |
| `ruins_con_doors.png`, `ruins_reg_badges.png`, `ruins_reg_desk.png`, `ruins_reg_escalator.png`, `ruins_reg_lostfound.png`, `ruins_reg_rules.png`, `petal_trail.png`, `ruins_panel_door.png`, `ruins_panel_door_1.png`, `ruins_panel_door_2.png`, `ruins_panel_door_3.png`, `ruins_panel_lounge_door.png` | Final | R7 (ART-184; panel-door plates restamped in 3×5 font) | `petal_trail.png` is the old single strip; superseded by `petal_trail_pieces`. |
| `ruins_lounge_beanbag.png`, `_flowerbed.png` + `.json`, `_flowerwall.png`, `_headrack.png`, `_lights.png`, `_neon.png`, `_rope.png`, `_throne.png`, `_vanity.png` | Final | R7 (ART-184, ART-183 row 18) | Nothing. |
| `ruins_mirror.png` + `.json` | Final | R7 (ART-184); shows the protagonist, check against D-16 (row 29) | Possible redo. |
| `ruins_package.png`, `ruins_aurora_glimpse.png` | Final | R7 (ART-184) | Nothing. |
| `save_point.png` + `.json` | Final | r1 (first pass) bottle, star rasterized by script (ART-E10) | Optional hand polish. |
| `ruins_dorito_bowl.png` + `.json`, `ruins_fireplace.png` + `.json`, `ruins_closet.png` + `.json`, `ruins_crt.png` + `.json`, `ruins_lamp.png` + `.json` | Placeholder | r1 (first pass); `ruins_lamp` needs a layout decision | Hand redo; `ruins_lamp` re-cut. |
| `ruins_armchair.png`, `ruins_bed.png`, `ruins_bookshelf.png`, `ruins_booth.png`, `ruins_banner_bad_dragon.png`, `ruins_banner_welcome.png`, `ruins_curtain_18.png`, `ruins_hall_*` (see above), `ruins_notice_board.png` | Placeholder | r1 (first pass) / v3.1 (`ruins_banner_bad_dragon`, stamped in a 5 px font) | Hand pass per room. |
| `ruins_apurrture_cube.png`, `_host.png`, `_incinerator.png`, `_panel.png` (+ `.json`), `ruins_apurrture_sign.png` | Placeholder | v3.1, not loaded yet (R9a) | Review when R9a is wired. |
| `floor_message.png`, `tide_pod.png`, `ruins_white_glue.png` | Placeholder | v3.1 | In-game check. |

## sfx/

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `menu_move.wav`, `menu_select.wav`, `heal.wav` | Placeholder | Synth, `tools/gen_placeholder_sfx.js` (AUD-003, BUG-32) | Designed sounds. |
| `spray.ogg`, `record_scratch.ogg`, `pounce.ogg`, `sparkle.ogg`, `petal_buzz.ogg`, `grunt_effort.ogg`, `grunt_hurt.ogg`, `grunt_death.ogg` | Placeholder | Synth, `tools/audio/generate_opening_sfx.py` (T38, 2026-10-29) | Real sounds made interactively (D-22). |
| `alert.ogg`, `bell.ogg`, `bullet_spawn.ogg`, `encounter_beep.ogg`, `encounter_fall.ogg`, `enemy_hit.ogg`, `fall_long.ogg`, `flee.ogg`, `glitter_spray.ogg`, `hitmarker.ogg`, `hurt.ogg`, `intro_noise.ogg`, `level_up.ogg`, `menu_error.ogg`, `name_swoosh.ogg`, `notify.ogg`, `phone_buzz.ogg`, `phone_ring.ogg`, `plush_squeak.ogg`, `pomf.ogg`, `save_can.ogg`, `slash.ogg`, `soul_crack.ogg`, `soul_shatter.ogg`, `spare_poof.ogg`, `spikes.ogg`, `step_crunch.ogg`, `switch_click.ogg`, `vaporize.ogg`, `voice_default.ogg`, `voice_floofy.ogg`, `voice_furiell.ogg`, `zap.ogg` | Placeholder | Added together 2026-10-19 (`c7d5b6d`, Round 10 "new SFX"); no generator script or source recorded | Record the source (or mark synthesized) and replace weak ones. |
| `areusure.ogg`, `cartoon_slip.ogg`, `pipipipopopo_backwards.ogg`, `type.ogg`, `type_long.ogg`, `n.ogg`, `u1.ogg`–`u12.ogg` | Placeholder | Legacy, origin unknown | License record or replace. |
| `sans_type.wav` | Placeholder | Legacy, likely an Undertale rip | **Replace before release.** |
| `selection_sound.wav` | Placeholder | Legacy, no longer used by the code | Delete. |
| `u_activate.wav` | Placeholder | Legacy, origin unknown | License record or replace. |

**Audio still to make (ART-185, D-22, none exists):** floor-message chime (OVW-022), R1 landing thud, muffled thud (C1-015), switch click and shutter (C1-004; `switch_click.ogg` is only the 10-19 stand-in), giant petal buzz (`petal_buzz.ogg` is the synth stand-in).

## sprites/battle/ (`.png` + `.json`)

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `floofy` | Final (not checked in game) | R8, 60×80, density 1 (ART-185 row 6; the R7 density-2 sheet is gone); sinister and soaked rows acceptable after one retouch | Retouch of the sinister and soaked rows. |
| `floofy_close` | Final (not checked in game) | R8, Bass Shake close-up, 4 expressions × 3 frames (ART-185 row 7; C1-028) | In-game check in R12a. |
| `floofy_wide` | Final (not checked in game) | R8, 60×80 (row 8); the hurt frame touches the cell edge | Fix the edge. |
| `furiell` | Final (not checked in game) | R8, D-24 look (row 11) | In-game check. |
| `barky` | Placeholder | v2 macro-pixel (2026-10-18), `blush` cell v3.1 | Review against D-16 (row 29). |
| `fursuit_dummy`, `fursuit_dummy_close` | Final | v3.1; `fursuit_dummy` redone in R7 (idle sway + hurt). `_close` is retired as the Bass Shake carrier | Nothing. |
| `froggsona`, `shysona`, `modsmal`, `lurkeye`, `reply_guy`, `grasstoid`, `napstablurk` | Placeholder | r1 (first pass); Shysona and Modsmal lose detail at their cell size | Hand pass (ART-B04–B10). |
| `gladowos` | Placeholder | v3.1, not loaded yet (R9a) | Review when wired. |

## sprites/ow/ (`.png` + `.json`)

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `floofy` | Final (not checked in game) | R7 base; R8 redrew row 6 `vanity_back` and added rows 11 `landing_idle`, 12 `landing_impact` (ART-185 rows 3–4) | Walk cycle if the lair needs it. |
| `grub` | Final (not checked in game) | R7 (ART-180/184); R8 added row 6 `fall`, row 7 `stunned` (row 5) | In-game check in the R1 landing. |
| `grub_fur` | Final | R7, four full FUR stage sheets (ears 20, paws 40, tail 60, snout 80; `row_offset` per stage) | Loader picks the sheet by `furry.value` (D-25). |
| `grub_pawsome` | Final (not checked in game) | R8, mint-green wolf with the blanket cape and headset (ART-185 row 17, BUG-61) | In-game check. |
| `furiell` | Final (not checked in game) | R8, D-24 look, spritz and hand-holding walk kept (row 11) | In-game check. |
| `barky` | Placeholder | v2 macro-pixel, reduced by `reduce_ruins.py`; needs hand touch-up (face, back safety pin) | Touch-up. |
| `fursuit_dummy` | Final | v3.1, kept in the review | Nothing. |
| `pepe_frog`, `pepe_wakeup` | Placeholder (legacy) | r1 (first pass); frog-era, Pepe © Matt Furie | Delete once nothing refers to them (BUG-61, D-16). |

## sprites/ (other)

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `player.png` | Placeholder | Legacy (2023 MVC test) | Delete if unused. |
| `dummy/ex.txt` | n/a | Legacy text | None. |
| `README.md` | n/a | Doc | None. |

## tiles/

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `landing.png` | Final | R7 (ART-184; ART-183 row 13) | Nothing. |
| `landing_bg.png` | Final (not checked in game) | R8 desaturated Landing background (ART-185 row 12, BUG-58) | In-game check. |
| `ruins.png` | Final | r1 (first pass) tileset; doorway tiles redone in R7 (ART-184) | Hand polish only. |
| `ruins_hall.png` | Placeholder | R8 R3 hall tileset (row 15); usable material, not a clean tile grid | Redo when R3's layout exists. |

## ui/ (`.png` + `.json` where listed)

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `buttons` (+ json), `slash` (+ json), `souls` (+ json), `attack_bar` (+ json), `target`, `exclaim`, `hud`, `dialog_box`, `portrait_frame`, `logo`, `logo_n` | Final | r1 (first pass) with hand parts via `reduce_ruins.py`; `souls` got the headset soul in R7 | Optional hand polish. |
| `water_burst` (+ json) | Final | R7 (ART-184) | Nothing. |
| `furry_over.png` | Placeholder | r1 (first pass); the code still loads it | Replaced by `welcome_home.png` when wired (BND-012). |
| `welcome_home.png`, `you_died.png`, `wasted.png` | Placeholder | v3.1, not loaded yet | Review when wired. |

## lang/ and other

| File | Status | Source | A final version needs |
| :--- | :--- | :--- | :--- |
| `lang/en.json`, `lang/master.json` | n/a (text) | Project, legacy | None. |
