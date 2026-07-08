# nUndertale — Progression Flags Registry (PRG-020)

> **Standardized Flag Taxonomy**
> Format: `<domain>.<entity_or_area>.<property>`
> Storage: `GameState::flags` (`unordered_map<string, int>`), serialized to `file0.json`.

---

## 1. Chapter 1: The Descent into Mt. Dew (Ruins)

| Flag Key | Type | Default | Possible Values / Meaning |
| :--- | :---: | :---: | :--- |
| `ruins.save.visited` | bool | `0` | `1` when player uses the first save point |
| `ruins.snack.taken` | int | `0` | Number of times player took a can from the Dew snack bowl |
| `ruins.barky.state` | int | `0` | `0` = Not encountered, `1` = Spared, `2` = Killed, `3` = Fled |
| `ruins.barky.spared` | bool | `0` | `1` when Barky was successfully spared via MERCY |
| `ruins.barky.killed` | bool | `0` | `1` when Barky was vaporized in battle |
| `ruins.barky.blushed` | bool | `0` | `1` when player used "Cringe Check" on Barky |
| `ruins.furiell.spared` | bool | `0` | `1` when Furiell's fight ended alive (13-line spare or Accept Hug) |
| `ruins.furiell.hugged` | bool | `0` | `1` when Grub used Accept Hug on Furiell (FUR +30) |
| `ruins.furiell.killed` | bool | `0` | `1` when Furiell was killed |
| `ruins.furiell.met` | bool | `0` | `1` when player meets Furiell at the ruins gate |
| `ruins.furiell.state` | int | `0` | `0` = Not encountered, `1` = Spared, `2` = Killed |
| `ruins.exit.open` | bool | `0` | `1` when the heavy blast door to Snowdinha is unlocked |
| `ruins.cell.furiell` | bool | `0` | `1` when Furiell gives her Discord tag / Cell contact |
| `ruins.fursuit_head.taken` | bool | `0` | `1` when player takes the spare fursuit head from the closet in Grub's room |
| `ruins.r12.walk` | bool | `0` | `1` when Furiell's walk-ahead cutscene in the basement corridor has played |
| `ruins.r13.floofy` | bool | `0` | `1` when Floofy's post-basement exit cutscene has played |
| `ruins.r13.floofy_pop` | bool | `0` | `1` transient flag while Floofy has popped up in R13 |
| `ruins.dummy.talked` | bool | `0` | `1` when Talk was used on the Fursuit Dummy |
| `ruins.dummy.hugged` | bool | `0` | `1` when Accept Hug was used on the Fursuit Dummy |
| `ruins.dummy.shook` | bool | `0` | `1` when Shake to the Bass reached its 3rd use (Furiell joins; dummy becomes spareable) |
| `ruins.dummy.quiet` | bool | `0` | `1` when Turn It Down was used (dummy stops the beat, spareable, no more attack turns) |
| `ruins.dummy.reacted` | bool | `0` | `1` once Furiell's R5 reaction cutscene has played |
| `ruins.dummy.result` | int | `0` | R5 outcome: `1` = Talk, `2` = Fight (killed), `3` = Fled, `4` = Plain spare (Shake x3 or Turn It Down, `r5_dummy_spare`), `5` = Accept Hug |
| `ruins.napstablurk.state` | int | `0` | `0` = Not encountered, `1` = Spared, `2` = Killed, `3` = Fled |
| `ruins.napstablurk.met` | bool | `0` | `1` when player interacts with Napstablurk in R10 hallway to start battle |
| `ruins.napstablurk.spared` | bool | `0` | `1` when Napstablurk is spared via MERCY (Cheer x3) |
| `ruins.napstablurk.killed` | bool | `0` | `1` when Napstablurk is killed in battle |
| `item.tide_pod.taken` | bool | `0` | `1` when player takes the Tide Pod pickup in R1 |
| `item.tide_pod.shown` | bool | `0` | `1` when player shows the Tide Pod to Furiell in R11 |
| `ruins.floormsg.r1.read` | bool | `0` | `1` when player has read the R1 floor message (`hole ahead`) |
| `ruins.floormsg.r9.read` | bool | `0` | `1` when player has read the R9 floor message (`try zipper, but hole`) |

---

## 2. Global Progress & Route Tracking (PRG-030)

| Flag Key | Type | Default | Meaning |
| :--- | :---: | :---: | :--- |
| `kills.ruins` | int | `0` | Number of enemies slain in Ruins. **Cap: 6** (LORE §28; was 20; matches `TOXIC_ENCOUNTER_CAPS` in `include/core/RandomEncounters.hpp`) |
| `kills.total` | int | `0` | Total kills across the entire playthrough |
| `spares.total` | int | `0` | Total enemies spared across the entire playthrough |
| `flees.total` | int | `0` | Total battles escaped via Flee |
| `toxic.track` | int | `0` | Toxic track state: `0` never started, `1` running, `2` ended for good |
| `toxic.kills` | int | `0` | Counted monster kills while `toxic.track == 1` (including the kill that starts it) |
| `route.toxic_gamer` | bool | `0` | `1` if player exhausts ruins kill quota ("* But nobody logged in.") |
| `route.wholesome` | bool | `1` | **RETIRED (LORE §28).** Folded into the Gamer route; `ending.type` tells the Clean Run apart. The code does not read it. |
| `route.normie` | bool | `0` | **RETIRED (LORE §28).** Folded into the Gamer route. The code does not read it. |

---

## 3. Meta Persistence (SAV-005)

| Key | Type | Location | Meaning |
| :--- | :---: | :---: | :--- |
| `meta.resets` | int | `meta.json` | Total number of game resets performed |
| `meta.ever_killed` | bool | `meta.json` | Remembers if player ever killed anyone even after Reset |
| `meta.ever_transformed` | int | `meta.json` | Number of WELCOME HOMEs (Grub reached 100% FUR) across all timelines (LORE §28 redefinition; `timesTransformed` in `meta.json`) |
| `meta.last_ending` | int | `meta.json` | Last `ending.type` value (`0` none, `1` Gamer, `2` Clean Run, `3` Toxic, `4` Furry); fed by `endingTypeValue()` in `include/core/Endings.hpp`, read by `resetLines()` in `include/core/MetaLines.hpp` |
| `meta.ever_furry` | bool | `meta.json` | Grub accepted the Mirror in some playthrough (LORE §28) |
| `meta.last_name` | string | `meta.json` | Last confirmed name (`lastName` in `meta.json`, MET-004) |

> `meta.json` keys currently written by `SaveService`: `resets`, `everKilled`, `timesTransformed`, `lastClearedRoute`, `lastName`. `meta.last_ending` and `meta.ever_furry` have no writer yet (MET-005 / SAV-005 wiring).

---

## 4. Ruins: flags set by data (cutscenes, dialogs, enemies)

> Read or written through `data/cutscenes/*.json`, `data/dialogs/*.json` and `data/enemies/*.json` keys (`setFlag`, `key`, `if`). All bool unless noted.

| Flag Key | Type | Default | Meaning |
| :--- | :---: | :---: | :--- |
| `battle.barky.blushed` | bool | `0` | Set by Barky's "Cringe Check" ACT (`data/enemies/barky.json`) |
| `ruins.floofy.met` | bool | `0` | Floofy's tutorial / R2 appearance has played |
| `ruins.furiell.r2_arrived` | bool | `0` | Furiell has arrived in R2 |
| `ruins.cell.given` | bool | `0` | Player got the CELL (R7 cutscene) |
| `ruins.laser.taken` | bool | `0` | Player took the Toy Laser Pointer |
| `ruins.curtain.looks` | int | `0` | Times the 18+ curtain was examined (0-3) |
| `ruins.bowl.takes` | int | `0` | Counter of takes from the R10 bowl (drives the escalating lines) |
| `ruins.bowl.taken` | bool | `0` | Player took from the R10 bowl |
| `ruins.bowl.broken` | bool | `0` | Player knocked over the bowl |
| `ruins.dew.taken` | int | `0` | Cans taken from the Dew bowl (`OverworldScene.cpp`); overlaps `ruins.snack.taken` (code uses this one) |
| `ruins.modrock.talks` | int | `0` | Times the R10 Moderator rock was talked to |
| `ruins.modrock.mutes` | int | `0` | Times the R10 Moderator rock was pushed (muted) |
| `ruins.r6.guided` | bool | `0` | R6 guide cutscene has played |
| `ruins.r7.call` | bool | `0` | R7 phone call has played |
| `ruins.r10.call` | bool | `0` | R10 phone call has played |

---

## 5. Overworld, battle and UI flags set by C++

| Flag Key | Type | Default | Meaning |
| :--- | :---: | :---: | :--- |
| `fur_hud_seen` | bool | `0` | The FUR HUD was introduced (first FUR gain, or the tremble before it) (`BattleSystem.cpp`) |
| `fur_mirror_pending` | bool | `0` | FUR reached the Mirror threshold; the Mirror scene is queued (`BattleSystem.cpp`, `OverworldScene.cpp`) |
| `fur_toxic_cap_pending` | bool | `0` | The "FUR capped on the toxic track" message is queued after the battle (transient) |
| `fur_toxic_cap_msg.ruins` | bool | `0` | The FUR cap message was already shown for the Ruins |
| `fur_on_equip.<itemId>` | bool | `0` | The one-time FUR gain for equipping `<itemId>` was already applied (built as `"fur_on_equip." + itemDef->id`) |
| `raid_msgs_shown` | bitmask | `0` | Squad clock warnings already shown (bit per warning index, 4 bits; debug sets `0xF`) |
| `enc.<roomId>.count` | int | `0` | Random encounters started in room `<roomId>` (built as `"enc." + roomId + ".count"`) |
| `kills.<area>` | int | `0` | Kills per encounter area, where `<area>` is `ruins`, `snowdinha`, `wetfall` or `hotland` (`TOXIC_ENCOUNTER_CAPS`; read in `OverworldScene.cpp`). LORE §28 names them `kills.snow` / `kills.wet` / `kills.hot`: the code names are the ones in force until renamed |
| `kills.<area>.announced` | bool | `0` | `* But nobody logged in.` was already shown for `<area>` (`areaExhaustedAnnouncedFlag`) |

Caps per area (`TOXIC_ENCOUNTER_CAPS`): `ruins` 6, `snowdinha` 6, `wetfall` 5, `hotland` 3.

---

## 6. Furry route (`fur.*`, `route.furry`, `fluff.value`)

| Flag Key | Type | Default | Meaning |
| :--- | :---: | :---: | :--- |
| `fur.value` | int | `0` | FUR 0-100 (LORE §28; the live value is `GameState::furry.value`) |
| `fur.stage_max` | int | `0` | Highest FUR stage reached |
| `fur.mirror_refused` | int | `0` | Times the Mirror was refused |
| `route.furry` | bool | `0` | Accepted the Mirror; read by `decideEnding()`, `ShopScene` (welcome discount) and `SaveText.hpp`. Beats every other route |
| `fluff.value` | int | `0` | Current FLUFF on the Furry route (`SaveText.hpp`) |
| `cell_furiell` | bool | `0` | Furiell's contact on the CELL (`include/core/Cell.hpp`) |
| `cell_papyrus` | bool | `0` | Papyrus' contact on the CELL (`include/core/Cell.hpp`) |
| `cell_alphys` | bool | `0` | Alphys' contact on the CELL (`include/core/Cell.hpp`) |

---

## 7. Snowdinha, Wetfall, Hotland, Con Center (LORE §28)

> Defined by LORE §28. No C++ reads or writes them yet.

| Flag Key | Type | Default | Meaning |
| :--- | :---: | :---: | :--- |
| `snow.sans.met` | bool | `0` | Air horn |
| `snow.doge.state` | int | `0` | `0` none, `1` spared, `2` killed |
| `snow.grillbys.reveal` | bool | `0` | Layer 4 reveal |
| `snow.papyrus.state` | int | `0` | `0` none, `1` friend, `2` killed |
| `snow.papyrus.captures` | int | `0` | `0`-`3` |
| `wet.sharkdyne.state` | int | `0` | `0` none, `1` fled, `2` killed, `3` friend |
| `wet.flowers.barky` | bool | `0` | Heard Barky's flower |
| `wet.maddaki.state` | int | `0` | `0` none, `1` defeated |
| `hot.alphys.log` | bool | `0` | Read Floofy's log |
| `hot.quiz.wrong` | int | `0` | Wrong quiz answers |
| `hot.nightshift.done` | bool | `0` | Survived the H1a shift |
| `con.jars.seen` | bool | `0` | Saw the jars |
| `con.asgrr.state` | int | `0` | `0` none, `1` spared, `2` killed, `3` banned |
| `ruins.booth.state` | int | `0` | `0` not visited, `1` Barky sacrificed, `2` refused / test survived, `3` powered down (Barky already dead) |

---

## 8. Squad clock (`raid.*`)

> The Fortnite squad clock (LORE v3.1 renamed the guild raid to the squad; the flag prefix stays `raid.`). Written by `raidSyncFlags()` in `include/core/RaidClock.hpp`.

| Flag Key | Type | Default | Meaning |
| :--- | :---: | :---: | :--- |
| `raid.time_left` | int | `0` | Whole seconds left on the squad clock |
| `raid.expired` | bool | `0` | `1` when the squad clock ran out (player is kicked) |
| `raid.gauntlet` | int | `0` | Gamers beaten in The Drop (`0`-`5`, LORE §28; no writer yet) |

---

## 9. Endings (`ending.*`)

| Flag Key | Type | Default | Meaning |
| :--- | :---: | :---: | :--- |
| `ending.type` | int | `0` | `0` none, `1` Gamer, `2` Gamer Clean Run, `3` Toxic, `4` Furry. Written by `recordEnding()` and read by `recordedEnding()` (`include/core/Endings.hpp`). Precedence: Furry (`route.furry`), Toxic (`toxic.track == 1` with kills), Clean Run (`kills.total == 0`), Gamer |

---

## 10. Toxic route (`toxic.*`)

| Flag Key | Type | Default | Meaning |
| :--- | :---: | :---: | :--- |
| `toxic.track` | int | `0` | `0` never started, `1` running, `2` ended for good (`include/core/ToxicTrack.hpp`) |
| `toxic.kills` | int | `0` | Kills counted while the track runs (the starting kill included) |
| `toxic.streak` | int | `0` | Kills since the last save point; reset to `0` on save |
| `toxic.ks.care.earned` | bool | `0` | Care Package earned (4 kills) (`include/core/Killstreaks.hpp`, TOX-007) |
| `toxic.ks.care.dropped` | bool | `0` | The Care Package crate has dropped |
| `toxic.ks.predator.earned` | bool | `0` | Predator Missile earned (5 kills) |
| `toxic.ks.predator.used` | bool | `0` | Predator Missile was used |
| `toxic.ks.nuke.earned` | bool | `0` | Tactical Nuke earned (25 kills) |
| `toxic.streaks` | bitmask | `0` | LORE §28 bitmask (Care Package earned, Predator earned, Predator used, Nuke fired). **Superseded in code** by the separate `toxic.ks.*` flags above; unused |
| `toxic.scimitar_dropped` | bool | `0` | The Dragon Scimitar has dropped (LORE §28; no writer yet) |

---

## 11. Items

| Flag Key | Type | Default | Meaning |
| :--- | :---: | :---: | :--- |
| `item.tide_pod.taken` | bool | `0` | Took the Tide Pod pickup in R1 |
| `item.tide_pod.shown` | bool | `0` | Showed the Tide Pod to Furiell in R11 |
