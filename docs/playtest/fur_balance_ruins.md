# Ruins FUR balance pass (FUR-012)

Date: 2026-10-29  
Scope: R1-R13, including R1a, R1b, and the optional R9a booth  
Canon: LORE v3.3 sections 8.4, 8.8, and 12

## Result

The current Ruins curve supports the three intended outcomes, provided the bed is counted once and the modeled player cleanly dodges unless a hit is explicitly listed:

| Run | Expected FUR at the R13 exit | Intended outcome |
| :--- | ---: | :--- |
| Avoid everything | **20** | Safely below 100 without Duct Tape. |
| Normal | **about 37** | Safely below 100 with several representative mistakes. |
| Hug everything | **about 69** (range **55-75**) | Leaves enough room to reach the Mirror after roughly 2-4 ordinary hugs in Snowdinha/Wetfall. |
| Deliberate Ruins conversion | **100 by R12** | Fursuit Head + one bed use + available hugs reaches the Mirror before leaving the Ruins. |

No ordinary save point changes FUR. Section 8.8 is explicit that saves heal HP but do not reduce FUR; the older `Save point | 0` row in section 8.4 is stale. This also matches the implemented `* DETERMINATION can't fix fluff.` behavior.

## Runtime source inventory

Positive bullet values below are applied **per damaging collision**, not once per wave or turn. Bullet gains are multiplied by equipped armor and rounded to the nearest integer. ACT gains, the forced R2 hit, the bed, and the Fursuit Head's one-time equip gain are not armor-multiplied. FUR is clamped to 0-100 (or 99 during the Toxic track and during Floofy's R2 encounter).

| Room | Source | FUR | When it fires |
| :--- | :--- | ---: | :--- |
| R1, The Landing | Save point | 0 | On save; heals HP and preserves FUR. |
| R1a, Registration | None | 0 | No encounter, item, or interaction changes FUR. |
| R1b, Panel Hall | Save point | 0 | On save; heals HP and preserves FUR. |
| R2, Headless Lounge | Forced tutorial petal | **+20** | Exactly once in a successful intro attempt. It cannot be dodged forever; Floofy caps the result at 99. |
| R2, Headless Lounge | Petal-fight bullet | **+8/hit** | A damaging collision in any of Floofy's six petal patterns. |
| R2, Headless Lounge | Bass-fight beat | **+6/hit** | A damaging collision in the alternate Bass Shake fight. |
| R3, Ruins Entrance | Save point | 0 | On save; heals HP and preserves FUR. |
| R4, RGB Switches | None | 0 | The puzzle has no FUR effect. |
| R5, Fursuit Dummy | Accept Hug | **+10** | On the ACT; immediately makes the dummy spareable. |
| R5, Fursuit Dummy | Slow fluff | **+2/hit** | A collision with the dummy's harmless-looking fluff rain. |
| R6, Cable Maze | None | 0 | Cable damage and Furiell's guidance do not add FUR. |
| R7, Long Hallway | None | 0 | The CELL sequence does not add FUR. |
| R8, Notices / Art Show | Froggsona: Accept Hug | **+10** | On the ACT; immediately spares Froggsona. Spawn weight 2/4. |
| R8, Notices / Art Show | Froggsona bullet | **+3/hit** | A hop or fly collision. |
| R8, Notices / Art Show | Shysona: Accept Hug | **+10** | On the ACT; immediately spares Shysona. Spawn weight 1/4. |
| R8, Notices / Art Show | Shysona bullet | **+6/hit** | A sorry-moth collision. |
| R8, Notices / Art Show | Modsmal bullet | **+2/hit** | A ban-gel collision. Modsmal offers no hug. Spawn weight 1/4. |
| R9, Barky's Arena | Accept Hug | **+15** | On the peace-path ACT. It adds one of five required progress points; it does not instantly finish this peace path. |
| R9, Barky's Arena | UwU rain | **+6/hit** | A damaging UwU collision. |
| R9, Barky's Arena | Dew bounce | **+4/hit** | A damaging can collision. |
| R9, Barky's Arena | Howl wall | **+5/hit** | A damaging howl-wall collision. |
| R9, Barky's Arena | Save point | 0 | On save; heals HP and preserves FUR. This is present in the current generated map even though the section 12 room table does not call it out. |
| R9a, Apurrture booth | Save / GLaDOwOS tests | 0 | The save preserves FUR; all booth bullet definitions explicitly use 0 FUR. |
| R10, Bake Sale | Lurkeye bullet | **+2/hit** | A seen-ring collision in the one-budget random encounter. |
| R10, Bake Sale | Reply Guy bullet | **+2/hit** | A reply collision in the one-budget random encounter. |
| R10, Bake Sale | Grasstoid bullet | **+2/hit** | A grass-blade collision in the one-budget random encounter. |
| R10, Bake Sale | Grasstoid: Touch | **-10** | On the ACT; makes Grasstoid spareable. |
| R10, Bake Sale | Napstablurk tear | **+1/hit** | A collision in the optional fixed Napstablurk battle. |
| R10, Bake Sale | Save point | 0 | On save; heals HP and preserves FUR. This is present in the current generated map. |
| R11, Furiell's House | Bed | **+5/use** | After each nap, together with a full heal. The current bed prop is repeatable; projections intentionally count one canonical use. |
| R11, Furiell's House | Fursuit Head | **+40 once** | The first time the armor is equipped (`fur_on_equip.fursuit_head` prevents repeated gain). Its 1.5x multiplier affects later bullet hits while equipped. |
| R11, Furiell's House | Glitter Pop-Tart | 0 | Pickup/use is FUR-neutral. Included to make the room inventory complete. |
| R12, Furiell's fight | Accept Hug | **+15** | On the peace-path ACT. It adds one of five required progress points. |
| R12, Furiell's fight | Fluff ball | **+6/hit** | A damaging collision. Pattern weight 2/5. |
| R12, Furiell's fight | Glitter hand | **+6/hit** | A damaging collision. Pattern weight 2/5. |
| R12, Furiell's fight | Hug ring | **+10/hit** | A damaging collision. Pattern weight 1/5. |
| R13, Exit | Save point | 0 | On save; heals HP and preserves the exit FUR total. |
| Any Ruins room, if carried | Duct Tape | **-15/use** | On consumption, floored at 0. It is not required by any modeled run. |

The table is exhaustive for normal R1-R13 content. `Gamer Fuel 9000` (+3) and armor found in later areas are global item definitions but are not normally obtainable in the Ruins, so they are not included in the route totals.

## Projection ledger

### Avoid everything: 20

- Forced R2 tutorial hit: +20.
- Use non-hug peaceful ACTs, flee random encounters, skip optional Napstablurk, bed, and Fursuit Head, and take no bullet hits: +0.
- Exit: **20**.

This is the lower bound for a legitimate first playthrough because R2 forces one +20 hit. It satisfies the careful-player target with 80 points of headroom and no Duct Tape.

### Normal: approximately 37

This is a deterministic expectation model, not a claim that every player will take exactly these hits:

| Assumption | Delta | Running total |
| :--- | ---: | ---: |
| Forced R2 tutorial hit | +20.0 | 20.0 |
| One dummy fluff hit | +2.0 | 22.0 |
| One R8 hit, spawn-weighted: 1/2 Froggsona, 1/4 Shysona, 1/4 Modsmal | +3.5 | 25.5 |
| One representative Barky hit | +5.0 | 30.5 |
| One R10 random-encounter hit | +2.0 | 32.5 |
| Touch if the one-budget R10 spawn is Grasstoid (1/3 chance) | -3.3 expected | 29.2 |
| One optional Napstablurk hit | +1.0 | 30.2 |
| One Furiell hit, pattern-weighted (2:2:1) | +6.8 | **37.0** |

The player uses ordinary peaceful ACTs rather than hugs and skips the optional bed/head conversion package. Even doubling the modeled bullet mistakes leaves the run comfortably below 100.

### Hug everything: approximately 69, range 55-75

The model means "take every offered hug"; it does not include the deliberate +40 Fursuit Head conversion item. It assumes clean dodges, one bed use, and the normal peaceful Touch if Grasstoid is selected.

| Source | Expected delta |
| :--- | ---: |
| Forced R2 hit | +20.0 |
| Dummy hug | +10.0 |
| R8 offered hug: Froggsona/Shysona in 3/4 of weighted spawns | +7.5 |
| Barky hug | +15.0 |
| R10 Grasstoid Touch in 1/3 of random spawns | -3.3 |
| One bed use | +5.0 |
| Furiell hug | +15.0 |
| **Expected exit** | **69.2** |

The spawn-dependent clean-dodge range is **55** (Modsmal at R8 and Grasstoid at R10) to **75** (a hug-capable R8 spawn and no Grasstoid). Two to four +10/+15 hugs after the Ruins then put this route at the Mirror in Snowdinha or Wetfall, matching FUR-012.

### Deliberate conversion: 100 by R12

The deliberate package is forced R2 (+20), Dummy hug (+10), Barky hug (+15), one bed use (+5), and the Fursuit Head (+40). That is **90** before the variable R8 hug. A hug-capable R8 spawn makes it **100 in R11**; otherwise Furiell's +15 hug reaches the 100 clamp in R12. Bullet hits can only make it earlier. Thus the deliberate LORE path reaches the Mirror while still in the Ruins without relying on random encounter selection.

## Data decision

`data/fur.json` now sets `accept_hug_furiell` to **15**, down from 30. The runtime peace-path value is already 15, which is inside LORE section 8.4's +10 to +15 range; the edit removes a misleading stale fallback and prevents future consumers from reintroducing a +30 spike. The other shared values remain within the LORE bands and preserve the successful route totals above.

Enemy-specific bullet and ACT values currently live in `data/enemies/*.json`, while the forced R2 hit lives in `data/encounters/floofy*.json`. Those runtime values were audited here but are outside this task's write scope.

## Follow-up risk

The R11 bed currently has no once flag and adds +5 every time it is examined. That makes FUR technically farmable to 100 and turns scenario totals into "one bed use" totals. A follow-up should make the +5 gain one-shot while retaining repeatable full healing and nap text. This does not affect the expected first-pass curve above, but it should be fixed before treating the Ruins balance as exploit-proof.
