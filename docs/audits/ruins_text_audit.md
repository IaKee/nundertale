# Ruins text audit (LORE v3.1)

Scope: player-visible English copy reachable in Ruins R1-R13, audited against `docs/LORE.md` sections 2, 6.5, 8.7, 10.2, 10.10, 21 and 23 and `docs/writing_style.md`. The authoritative map source is `tools/maps/build_ruins_maps.py`; generated `data/maps/*.json` duplicates are not counted as independent findings. This is a read-only audit: the proposed copy below has not been applied.

## Findings

### Content-rule violations

| Location | Current | Problem | Proposed text |
| --- | --- | --- | --- |
| `data/enemies/barky.json:53` | `...I was twelve.` | Direct reference to a child/child age, forbidden by LORE section 2.2 even when the character is now an adult. | `...That was years ago.` |
| `tools/maps/build_ruins_maps.py:2148` | `For a drawing of a "very tired raccoon".` | An unqualified animal can read as a real animal; LORE section 2.2 requires in-world animals to be people, fursonas or fursuits. | `For a drawing of a "very tired raccoon fursona".` |
| `tools/maps/build_ruins_maps.py:1644` | `The Minecraft server runs on this hallway.` | Unconditional named-game reference in R6, so a clean player can see it before the first kill. LORE section 2.3 rule 7 requires zero such references before the first kill. | `The registration server runs on this hallway.` |
| `tools/maps/build_ruins_maps.py:2087` | `Violators will be banned from the Minecraft server.` | Same unconditional pre-kill named-game violation in R8. | `Violators will lose the Wi-Fi password.` |

The two Minecraft lines should not merely be renamed if a later implementation wants to keep the Minecraft joke: another valid fix is to gate the existing copy behind `toxic.track >= 1` and supply the neutral alternatives above before the first kill.

### Floofy v3.1

The active R2 trigger points to `r2_floofy`; its opening copy is the pre-v3.1 flower/SHAME version. `r2_floofy_battle.json:7-8` and `data/enemies/floofy.json` already use the correct zesty recruiter voice and can be the copy reference.

| Location | Current | Problem | Proposed text |
| --- | --- | --- | --- |
| `data/cutscenes/r2_floofy.json:7` | `Howdy! I'm FLOOFY! Floofy the Flower!` | Old identity and restrained voice; v3.1 defines Floofy as a flamboyant furry recruiter, not a literal flower. | `Howdy~! I'm FLOOFY! Floofy the Floof! *pounces on you* OwO what's this? A new friend?` |
| `data/cutscenes/r2_floofy.json:8` | `"free hug pellets"` | Old tutorial terminology. | `Down here, we share these little... "friendship petals"! Catch them, cutie~` |
| `data/cutscenes/r2_floofy.json:15-16` | `In this world, it's SHAME or be SHAMED.` / `HUG.` | Conflicts with Floofy's canonical v3.1 motto and does not pitch conversion in his established voice. | `You idiot. In this world, it's YIFF or be YIFFED.` / `Shame? I donated mine to science. Want to see what that's like?` |

### Furiell v3.1

| Location | Current | Problem | Proposed text |
| --- | --- | --- | --- |
| `data/cutscenes/r2_floofy.json:71` | `Do not fear, my child... my glitter is hypoallergenic. It washes right off.` | NAR-035 explicitly forbids Furiell calling Grub `my child`; her terms are `pup` or `hun`. | `There, there. My glitter is hypoallergenic, pup. It'll wash the fluff right off.` |
| `tools/maps/build_ruins_maps.py:657` | `Welcome to your new home, my child. ...Do you like blankets? I have forty.` | Forbidden form of address and pre-v3.1 voice. | `Hewwo~! Welcome to your new home, pup! uwu ...Do you like bwankets? I have forty.` |
| `tools/maps/build_ruins_maps.py:880` | `The surface is full of cruel people with cameras...` / `You can always accept a hug. It is the fastest way to make friends.` | Fallback NPC copy retains Furiell's old neutral voice rather than the canonical R3 hand-grab/uwu copy. | `*grabs your hand* The surface is full of meanies with cameras. Down here, nobody will laugh at you. >w<||You can always accept a hug, hun. It's the fastest way to make fwiends~` |
| `tools/maps/build_ruins_maps.py:1400` | `A warm embrace is always the best choice, my child.` | Forbidden form of address and old neutral voice. | `A warm embwace is always the best choice, pup. uwu` |
| `data/enemies/furiell.json:63-70` | Flavor pools contain no insult/hit reaction. | The canonical battle response `* Furiell is enjoying this.` is missing, so the text does not communicate that shame/insults backfire. This is a missing-copy finding rather than a bad existing line. | Add a triggered flavor line: `* Furiell is enjoying this.` |

### Guild/raid terminology and clock copy

Code/flag identifiers beginning with `raid` may remain per NAR-033; only player-visible text and the menu dispatch key need to change.

| Location | Current | Problem | Proposed text |
| --- | --- | --- | --- |
| `include/core/Cell.hpp:12` | `Guild` | Player-visible pre-v3.1 contact name. | `Squad` |
| `src/scenes/OverworldScene.cpp:1011` | `selected == "Guild"` | Dispatch key would stop matching after the CELL label is corrected. | `selected == "Squad"` |
| `include/core/RaidClock.hpp:51-52` | `RAID: KICKED` / `RAID mm:ss` | Player-visible old clock label. | `SQUAD: QUEUED` / `SQUAD mm:ss` (or the canonical `SQUAD QUEUES IN mm:ss` where space permits) |
| `include/core/RaidClock.hpp:76` | `RAID STARTS IN ... BE THERE OR GET KICKED. bring dew` | Old guild-raid premise; canonical event is the squad's Fortnite queue. | `SQUAD QUEUES IN ... get on or we queue without u. and bring the dew -xX_Moderator_Xx` |
| `include/core/RaidClock.hpp:86` | `5 min. ur kicked if ur late. no cap.` | Still uses the raid/kick premise. | `5 min. we're queuing without u. no cap.` |
| `include/core/RaidClock.hpp:91` | `Raid started without you. Kicked.` | Old zero-time outcome. | `Queued without you.` |
| `include/scenes/FurryOver.hpp:14` | `Your guild is counting on you.` | Old group name in the Ruins-reachable game-over voice. | `Your squad is counting on you.` |

### Game-over name

| Location | Current | Problem | Proposed text |
| --- | --- | --- | --- |
| `src/scenes/GameOverScene.cpp:311-320` | Loads `assets/ui/furry_over.png`, with `FURRY` / `OVER` fallback text. | `FURRY OVER` was removed in v3. Main/Toxic HP 0 must be `WELCOME HOME`; Furry-route FLUFF 0 must be `GAMER OVER`. | Select `assets/ui/welcome_home.png` and fallback `WELCOME` / `HOME` for Gamer/Toxic routes; select a `GAMER OVER` asset/fallback only for the Furry route. |
| `assets/ui/furry_over.png` (loaded at `src/scenes/GameOverScene.cpp:314-318`) | Image reads `FURRY OVER`. | Visible legacy title even if hardcoded fallback text is fixed. | Stop loading this asset; use the existing `assets/ui/welcome_home.png` on Gamer/Toxic routes and a new/approved `GAMER OVER` asset on the Furry route. |

## Non-player-visible `Pepe` migration residue

No player-visible Ruins line calls Grub `Pepe`. The following source residues are paths, variable names, comments or room IDs, not copy. They do not violate LORE section 2 at runtime, but they contradict the full NAR-032/ART-180 migration and should be renamed atomically with assets and map destinations.

| Location | Current | Problem | Proposed replacement |
| --- | --- | --- | --- |
| `data/cutscenes/r1_wake.json:5,11` | `assets/sprites/ow/pepe_wakeup.png` | Legacy asset path. | `assets/sprites/ow/grub_wakeup.png` after ART-180 supplies it. |
| `src/scenes/OverworldScene.cpp:1806-1824` | `pepeSheet`, `pepeTexture`, `pepe_frog.*`, `pepe_char.bmp` | Legacy identifiers and asset paths. | `grubSheet`, `grubTexture`, and Grub asset paths, changed together. |
| `tools/maps/build_ruins_maps.py:332,1505,1775` | Comments name Pepe. | Stale developer-facing terminology. | Replace `Pepe` with `Grub`. |
| `tools/maps/build_ruins_maps.py:2787,2978,2998-3007,3046-3050,3115,3179,3597` | `from_pepe`, `door_to_pepe`, `ruins_house_pepe`, `build_ruins_house_pepe` | Legacy internal map/spawn/function IDs. | Rename consistently to `from_grub`, `door_to_grub`, `ruins_house_grub`, and `build_ruins_house_grub`, then regenerate maps. |

The `PEPE` name-entry easter egg (`* Not that frog.`) is the sole canonical player-visible exception and was not treated as a violation.

## Source coverage

- Cutscenes audited: all 32 JSON files in `data/cutscenes/`. Findings are limited to `r2_floofy.json`; `r1_wake.json` has only the non-visible asset-path residue listed above. The other 30 files have no LORE section 2, writing-style, or v3.1 naming violation.
- Enemies audited: all 11 JSON files in `data/enemies/`. `barky.json` has the child-age finding; `furiell.json` has the missing canonical response. `floofy.json`, `froggsona.json`, `fursuit_dummy.json`, `grasstoid.json`, `lurkeye.json`, `modsmal.json`, `napstablurk.json`, `reply_guy.json`, and `shysona.json` have no scoped violation. CHECK formatting is assembled correctly by `Enemy::getCheckText()` as `* NAME - ATK ... DEF ...` plus a prefixed description.
- Map copy audited: every `lines` property in `tools/maps/build_ruins_maps.py`, including R1-R13 and the generated house/shop subrooms. Findings are listed above. Generated `data/maps/*.json` contains the same bad copy at generated locations and must be refreshed by the builder rather than edited directly.
- Hardcoded overworld copy audited: player-facing literals in `src/scenes/OverworldScene.cpp`; only the `Guild` dispatch dependency is a v3.1 issue. Its Barky fallback copy and item/save/CELL narration contain no scoped violation.
- CELL audited: every label and response in `include/core/Cell.hpp`; only `Guild` violates v3.1. Furiell's five options and voicemail match LORE section 21 verbatim.
- Cross-cutting Ruins UI audited because it is visible during this area: `include/core/RaidClock.hpp`, `include/scenes/FurryOver.hpp`, and `src/scenes/GameOverScene.cpp`. These contain the clock and game-over findings above.

No scoped copy contains a slur, coded abuse slang, a real named person, explicit sexual anatomy/acts, song lyrics, Portuguese player-facing text, or an unclosed adult-brand product. The remaining jokes use implication/closed objects, narrator lines use the required `* ` prefix, Grub's thoughts use `* (...)`, and prose shown as narration is in second person where applicable.
