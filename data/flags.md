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
| `ruins.furiell.met` | bool | `0` | `1` when player meets Furiell at the ruins gate |
| `ruins.furiell.state` | int | `0` | `0` = Not encountered, `1` = Spared, `2` = Killed |
| `ruins.exit.open` | bool | `0` | `1` when the heavy blast door to Snowdinha is unlocked |
| `ruins.cell.furiell` | bool | `0` | `1` when Furiell gives her Discord tag / Cell contact |

---

## 2. Global Progress & Route Tracking (PRG-030)

| Flag Key | Type | Default | Meaning |
| :--- | :---: | :---: | :--- |
| `kills.ruins` | int | `0` | Number of enemies slain in Ruins (Cap: 20 for Toxic Gamer route) |
| `kills.total` | int | `0` | Total kills across the entire playthrough |
| `spares.total` | int | `0` | Total enemies spared across the entire playthrough |
| `flees.total` | int | `0` | Total battles escaped via Flee |
| `route.toxic_gamer` | bool | `0` | `1` if player exhausts ruins kill quota ("* But nobody logged in.") |
| `route.wholesome` | bool | `1` | `1` as long as `kills.total == 0` |
| `route.normie` | bool | `0` | `1` if player kills some monsters but does not exhaust the area |

---

## 3. Meta Persistence (SAV-005)

| Key | Type | Location | Meaning |
| :--- | :---: | :---: | :--- |
| `meta.resets` | int | `meta.json` | Total number of game resets performed |
| `meta.ever_killed` | bool | `meta.json` | Remembers if player ever killed anyone even after Reset |
| `meta.ever_transformed` | int | `meta.json` | Times Pepe reached 100% furry meter across all timelines |
