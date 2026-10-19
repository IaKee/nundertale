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
| `kills.ruins` | int | `0` | Number of enemies slain in Ruins (Cap: 20 for Toxic Gamer route) |
| `kills.total` | int | `0` | Total kills across the entire playthrough |
| `spares.total` | int | `0` | Total enemies spared across the entire playthrough |
| `flees.total` | int | `0` | Total battles escaped via Flee |
| `toxic.track` | int | `0` | Toxic track state: `0` never started, `1` running, `2` ended for good |
| `toxic.kills` | int | `0` | Counted monster kills while `toxic.track == 1` (including the kill that starts it) |
| `route.toxic_gamer` | bool | `0` | `1` if player exhausts ruins kill quota ("* But nobody logged in.") |
| `route.wholesome` | bool | `1` | `1` as long as `kills.total == 0` |
| `route.normie` | bool | `0` | `1` if player kills some monsters but does not exhaust the area |

---

## 3. Meta Persistence (SAV-005)

| Key | Type | Location | Meaning |
| :--- | :---: | :---: | :--- |
| `meta.resets` | int | `meta.json` | Total number of game resets performed |
| `meta.ever_killed` | bool | `meta.json` | Remembers if player ever killed anyone even after Reset |
| `meta.ever_transformed` | int | `meta.json` | Times Grub reached 100% furry meter across all timelines |
