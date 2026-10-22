# Real-brand and game-reference register

This register tracks real commercial names and other games that are intentionally referenced by nUndertale. Per LORE §2.1 and decision D-14, names may remain in both text and art; this is a tracking register, not a substitution list. A location marked **planned** is specified by the canonical LORE or backlog but is not necessarily wired into the current build yet.

Real people, real events, sites, and conventions are outside this register and are prohibited by LORE §2.2. Parody names are recorded separately so that they are not mistaken for brands.

## Food and drink

| Brand or product | Appearance | Where | Backlog / LORE reference |
|---|---|---|---|
| Mountain Dew / Mt. Dew | Text, art, sound | World name and intro card 6 (`src/scenes/LoreScene.cpp`, `lore5`); starting inventory and item use (`include/core/GameContext.hpp`, `src/combat/BattleSystem.cpp`, `src/scenes/OverworldScene.cpp`, `mtn_dew_warm`); R10 bowl and Barky dialogue; planned can art and can-opening sound | LORE §2.1, §9, §10.1, §12 R1/R10, §20.1, §27.2, §30; ITM-002, ITM-005, C1-018, ART-182 |
| Doritos | Text, art, sound | Starting inventory and item use (`include/core/GameContext.hpp`, `src/combat/BattleSystem.cpp`, `src/scenes/OverworldScene.cpp`, `stale_doritos`); R1 save text (`include/core/SaveText.hpp`); R10 bowl (`assets/props/ruins_dorito_bowl.png`); planned crunch sound | LORE §2.1, §10.1, §12 R1/R10, §20.1, §27.2; ITM-002, C1-018, AUD-012 |
| Monster Energy (as “White Monster”) | Text, planned item art | `data/items/items.json` item `white_monster`; Grillby's LAN House and Care Package pool | LORE §20.1 and §8.9; ITM-007, TOX-007 |
| Cheetos | Text, planned character art | xX_Moderator_Xx has “Cheetos-orange fingers” | LORE §10.5; NAR-033 / character art specifications |
| Tide Pods (Tide) | Text and art | Intro card 9 and R1 key item `tide_pod`; `assets/props/tide_pod.png`; Furiell reaction in R11 | LORE §9, §12 R1/R11, §20.4, §30.3; C1-026, ART-182 |

## Platforms and apps

| Brand or product | Appearance | Where | Backlog / LORE reference |
|---|---|---|---|
| Discord | Text and planned UI/sound styling | Barky's server line (`src/combat/Enemy.cpp`); Discord Simp encounter and `discord_kitten_request`; dialogue speaker styling (`src/ui/DialogBox.cpp`) | LORE §18.5 and Discord Simp encounter; BHL-006, C6-020 |
| Reddit | Historical text reference only | Old intro-card wording documented as removed in `BACKLOG.md` BUG-29; no canonical shipping scene | LORE §2.2; BUG-29 / FRT-002 |

## Games referenced

| Game or franchise | Appearance | Where | Backlog / LORE reference |
|---|---|---|---|
| Fortnite | Text and planned art | Squad clock/messages, character dialogue, endings, and X1 “The Drop” with Battle Bus and storm imagery | LORE §1, §8.7, §17, §21.3, §22; NAR-033, RAD-003, ART-182 |
| Minecraft | Text | R8 Administration notice (`src/scenes/OverworldScene.cpp`, room `ruins_notices`) | LORE §12 R8; C1-008, BUG-19 |
| League of Legends | Text, planned UI and sound | Toxic kill announcer (`FIRST BLOOD` through `ACE`); items `dorans_blade` and `mejais` in `data/items/items.json` | LORE §8.9, §20.2–20.3, §27.2; TOX-002, ITM-007, AUD-014 |
| Old School RuneScape / RuneScape | Text, planned art, UI and sound | Grub's visual/voice reference; Toxic hitsplats, XP drops and PK skull; `dragon_scimitar` item and rare-drop effects | LORE §4.1, §8.9, §10.1, §20.2, §27.2; TOX-003, TOX-004, ITM-007, AUD-014 |
| Call of Duty / Modern Warfare 2 | Planned UI and sound | Toxic hitmarker; Care Package, Predator Missile, Tactical Nuke, killstreak icons and voice lines | LORE §8.9 and §27.2; TOX-005, TOX-007, AUD-014 |
| Counter-Strike 1.6 / Counter-Strike | Planned UI and text | Toxic killfeed; `karambit` item (`★ Karambit | Fade (Factory New)`) | LORE §8.9 and §20.2; TOX-006, ITM-007 |
| Portal | Text, art, planned battle patterns and sound | R9a fan booth, GLaDOwOS, companion-cube display and portal bullets (`assets/bullets/portal.png`); see parody table for renamed elements | LORE §12 R9a, §19.10, §30.3; C1-014, C1-025 |
| Five Nights at Freddy's (FNAF) | Text, planned art and sound | H1a Fuzzbear's Night Shift fan booth, empty animatronics and night-shift call | LORE §12 H1a and §30.3; C4-016 |
| Dark Souls | Text, planned prop art and UI | Floor-message and bloodstain format; Toxic `YOU DIED` title at 16 kills | LORE §22.4, §26.7, §30.4; OVW-019, MET-005 |
| Grand Theft Auto (GTA) | Planned UI | Toxic `WASTED` title at 22 kills | LORE §22.4; MET-005 |
| Undertale | Game-wide structure and parody, with original assets excluded from release | nUndertale's battle/menu/story mappings; Undertale rips are reference-only under ignored `assets/_ref/` | LORE §2, §25–§27, §29; D-04, D-05 |

## Hardware, software, and other brands

| Brand or product | Appearance | Where | Backlog / LORE reference |
|---|---|---|---|
| Bad Dragon | Text and art | Closed-object/logo treatment at the R8 Art Show (`assets/props/ruins_banner_bad_dragon.png`); planned discreet package in R11 | LORE §2.1, §12 R8/R11, §25.3; D-14, D-19, ART-181 |
| Gamer Girl Bath Water | Text and planned art | Registered in `data/items/items.json` (`gamer_girl_bath_water`); key item left by Discord Simp in X2 (Furry route); referenced by W3 waterfall jar (`FURRY BATH WATER`); bullet pattern `bath_water_jars` in X2. Permitted as a product name only (closed object/jar label); no seller is named, drawn, described, or depicted | LORE §2.2, §18.5, §20.4, §30.1; D-21, NAR-037, ART-183 (rows 25-26) |
| Microsoft Windows XP | Legacy placeholder art only (removed from the room) | `assets/img/test.bmp` was documented as the Windows XP wallpaper and is no longer used by the rebuilt Ruins rooms | BACKLOG OVW-020; LORE §26 placeholder policy |
| Comic Sans (Microsoft typeface) | Text reference only; original font must not ship | R8 CRT error-message gag; canonical font plan uses the free Comic Neue substitute for rendering | LORE §12 R8 and §26.9; C1-008, GFX-004 |

## Parody names (not brands)

| Parody | Based on | Where | Reference |
|---|---|---|---|
| Apurrture Science Enrichment Center | Aperture Science / Portal | R9a fan booth and item `apurrture_cake` | LORE §12 R9a, §19.10, §20.1; C1-014 |
| GLaDOwOS | GLaDOS / Portal | R9a animatronic host | LORE §19.10 and §30.3; C1-025 |
| Fuzzbear's Night Shift | Five Nights at Freddy's | H1a fan booth | LORE §12 H1a and §30.3; C4-016 |
| Mt. Dew | Mountain Dew used as a literal mountain name | Surface world, intro and endings | LORE §9, §22 and §30 |

## Maintenance notes

- “Text” includes dialogue, item names, descriptions, UI copy, and canonical specifications.
- “Art” includes an existing asset or an explicit final-art requirement. Placeholder and planned assets are identified as such.
- “Sound” includes an explicit sound or voice-line reference, even when the final licensed/original recording is still pending.
- When adding a real commercial name or a game reference, add it here in the same change and cite the scene, room, encounter, or item id.
