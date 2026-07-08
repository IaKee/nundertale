# Grub's thoughts in the Ruins

Status register for NAR-029 / TXT-011. This inventory covers player-visible strings in the current Ruins maps, Ruins cutscenes, and enemies available there. A thought is a line whose complete text uses the canonical `* (...)` form; parenthetical monster speech such as Modsmal's `*squorch* (...)` is not a Grub thought. Mixed text fields are split into their individual displayed lines below.

The register deliberately describes the recurring clue set only as **the secret**. Neither this document's recommendations nor the proposed copy should make the answer explicit in-game.

## Implemented register

### R1 — Fall Room

| Thought | Source | Situation |
| :--- | :--- | :--- |
| `* (Ugh... where's my wireless mouse? And why does it smell like body spray and wet dog?)` | `data/cutscenes/r1_wake.json`, step 3 | Wake-up cutscene. |

### R6 — Cable Maze

| Thought | Source | Situation |
| :--- | :--- | :--- |
| `* (Good stitching.)` | `data/cutscenes/r6_guide.json`, step 11 | Furiell guides Grub through the cables. Secret hint 2 of 5. |
| `* (The note is taped down with duct tape.)` | `data/maps/ruins_cables.json`, object `cable_notice` | Second page of the cable notice. |

### R7 — Hallway

| Thought | Source | Situation |
| :--- | :--- | :--- |
| `* (You can see her tail. She can see you seeing her tail.)` | `data/maps/ruins_hallway.json`, object `tail_note` | Hallway examine. |

### R8 — Notices, CRT and Art Show

| Thought | Source | Situation |
| :--- | :--- | :--- |
| `* (...No.)` | `data/maps/ruins_notices.json`, object `curtain_18`, `lines3` | Third curtain examination. Secret hint 3 of 5; the first two reactions are narration, not thoughts. |

### R9 — Barky's Arena

| Thought | Source | Situation |
| :--- | :--- | :--- |
| `* (The northern gate is sealed.)` | `data/maps/ruins_barky.json`, object `north_gate` | Gate before Barky's resolution. |
| `* (Something with a very loud voice is guarding it.)` | Same as above | Second line of the gate examine. |
| `* (A small mound of gray fur and gamer dust.)` | `data/maps/ruins_barky.json`, object `barky_dust` | Barky aftermath. |
| `* (It smells faintly of regret and Doritos.)` | Same as above | Second line of the aftermath examine. |
| `* (You took a can of warm Mountain Dew.)` | `data/cutscenes/r9_dew_bowl.json`, step 0 | First successful bowl pickup. |
| `* (Your inventory feels slightly heavier.)` | Same as above | Second line of the first pickup. |
| `* (You tried to take a snack...)` | `data/cutscenes/r9_dew_bowl.json`, steps 0–3 `fullLines` | Full-inventory branch; repeated at every pickup stage. |
| `* (You're carrying too much.)` | Same as above | Second line of the full-inventory branch; repeated at every pickup stage. |
| `* (You took another Mountain Dew.)` | `data/cutscenes/r9_dew_bowl.json`, step 1 | Second successful pickup. |
| `* (How greedy.)` | Same as above | Second line of the second pickup. |
| `* (You took yet another Mountain Dew.)` | `data/cutscenes/r9_dew_bowl.json`, step 2 | Third successful pickup. |
| `* (You feel like scum.)` | Same as above | Second line of the third pickup. |
| `* (You took the last Mountain Dew.)` | `data/cutscenes/r9_dew_bowl.json`, step 3 | Fourth successful pickup. |
| `* (You knocked the bowl over. Disgusting.)` | Same as above | Second line of the fourth pickup. |
| `* (Look at what you've done.)` | `data/cutscenes/r9_dew_bowl.json`, step 4 | Empty-bowl revisit. |
| `* (The bowl is completely empty.)` | Same as above | Second line of the empty-bowl revisit. |

### R9a — Apurrture booth

| Thought | Source | Situation |
| :--- | :--- | :--- |
| `* (You got the Apurrture Cake.)` | `data/cutscenes/r9a_sacrifice.json`, step 10 | Cake reward. |
| `* (You're carrying too much. The cake stays warm on the grate.)` | Same as above, `fullLines` | Full-inventory reward branch. |
| `* (A small mound of gray fur and gamer dust.)` | `data/maps/ruins_booth.json`, object `barky_dust` | Powered-down/dead-Barky booth state. |
| `* (It is still warm.)` | Same as above | Second page of the dust examine. |

### R10 — Bowl room

| Thought | Source | Situation |
| :--- | :--- | :--- |
| `* (The bowl is knocked over. Look at what you've done.)` | `data/cutscenes/r10_bowl.json`, step 0 | Already-broken branch. |
| `* (You take one stale Dorito. Vintage cheese dust coats your fingers.)` | `data/cutscenes/r10_bowl.json`, step 1 | First successful pickup. |
| `* (You try to take one, but you're carrying too much.)` | Same as above, `fullLines` | First full-inventory branch. |
| `* (You take a second Dorito. How greedy.)` | `data/cutscenes/r10_bowl.json`, step 3 | Second successful pickup. |
| `* (You reach for another Dorito, but your inventory is full.)` | `data/cutscenes/r10_bowl.json`, steps 3–4 `fullLines` | Later full-inventory branch; identical text is stored twice. |
| `* (You take a third Dorito. You feel like the scum of the earth.)` | `data/cutscenes/r10_bowl.json`, step 4 | Third successful pickup. |

### R10a — Commission Bake Sale

| Thought | Source | Situation |
| :--- | :--- | :--- |
| `* (You take the free sample.)` | `data/maps/ruins_bakesale.json`, object `sample_plate`, `lines` | Successful sample pickup. |
| `* (The sample plate is empty. A business card reads: 'Slots closed.')` | Same as above, `emptyLines` | Empty plate. |
| `* (You try to take the free sample, but you're carrying too much.)` | Same as above, `fullLines` | Full-inventory branch. |

### R11 — Furiell's house

| Thought | Source | Situation |
| :--- | :--- | :--- |
| `* (You take the Glitter Pop-Tart.)` | `data/maps/ruins_house.json`, object `poptart_plate`, `lines` | Successful pickup. |
| `* (The plate is empty.)` | Same as above, `emptyLines` | Empty plate. |
| `* (You touch your head. Nothing there.)` | `data/maps/ruins_house_hallway.json`, object `hall_mirror`, `furLines` | Second page of the FUR-reactive mirror examine. This is not the route-choice Mirror hint. |

### R13 — Exit

| Thought | Source | Situation |
| :--- | :--- | :--- |
| `* (Snowdinha. The demo ends here. For now.)` | `data/maps/ruins_exit.json`, object `snow_view` | Final page of the exit view. |

### Ruins random encounters

These lines are stored as battle flavor and may appear in any Ruins room whose encounter table includes the named enemy.

| Thought | Source | Condition |
| :--- | :--- | :--- |
| `* (It keeps staring at your ears.)` | `data/enemies/froggsona.json`, `flavor.default[2]` | Froggsona default flavor. |
| `* (It looks like you. A little. Don't think about it.)` | `data/enemies/froggsona.json`, `flavor.default[3]` | Froggsona default flavor. |
| `* (It is still staring at your ears.)` | `data/enemies/froggsona.json`, `flavor.lowHp[1]` | Froggsona at low HP. |
| `* (You haven't been outside in a while. Neither has the grass, technically.)` | `data/enemies/grasstoid.json`, `flavor.default[3]` | Grasstoid default flavor. |
| `* (It has been in this channel longer than you have been alive. Probably.)` | `data/enemies/lurkeye.json`, `flavor.default[3]` | Lurkeye default flavor. |
| `* (He will not budge while anyone else is watching.)` | `data/enemies/reply_guy.json`, `flavor.default[3]` | Reply Guy default flavor. |

## Five-hint tracker

The canonical set is defined in `docs/LORE.md` §10.1. “Present” below means the player-visible string exists in current data, not merely in LORE or BACKLOG prose.

| # | Canonical beat | Current status | Evidence / gap |
| :-: | :--- | :--- | :--- |
| 1 | Digitigrade-leg observation | **Missing** | Neither of the two canonical thoughts is present in player-visible data. Barky's current map has no matching examine copy. |
| 2 | `Good stitching.` observation | **Present, partial** | `data/cutscenes/r6_guide.json` contains `* (Good stitching.)`. The canonical follow-up that preserves uncertainty is absent. |
| 3 | Curtain examined three times | **Present** | `data/maps/ruins_notices.json` has all three escalating reactions; only the third, `* (...No.)`, is a thought. Counter: `ruins.curtain.looks`. |
| 4 | Fandom-discount thoughts | **Missing** | Shop price logic exists, but neither canonical thought is present in player-visible data. This beat belongs to the Snowdinha shop rather than a Ruins room. |
| 5 | Route-choice Mirror thought | **Missing** | `* (You always kind of wondered.)` appears only in design documents. The R11 hall mirror has different reactive copy and must not be counted as this beat. |

The optional Sans-room body-pillow payoff is also **missing from player-visible data**: neither `* (You don't remember buying this.)` nor `* (You do.)` is implemented. It is outside the Ruins, but NAR-029 requires it to remain in the cross-area register.

## Recommendations

1. Add the missing digitigrade beat to an examine on Barky's suit or arena aftermath. Preserve both canonical lines from LORE §10.1 so the second line immediately undercuts the observation.
2. Add the canonical uncertainty follow-up after `* (Good stitching.)`; the current single line reads more confidently than the intended clue.
3. Keep the curtain closed and retain the three-step escalation. Do not add an opening state, contents, or explanatory narration.
4. Wire the two fandom-discount thoughts to the first eligible discounted purchase in the Snowdinha shop, not to Ruins content; avoid repeating them on every transaction.
5. Use the canonical route-choice Mirror line only at acceptance. Do not reuse it for the ordinary R11 hall mirror, and do not add a narrator explanation after it.
6. Add the two-line body-pillow payoff only to the designated Sans-room prop. Keep both lines as thoughts and leave the object closed/undescribed.
7. Tag Grub thoughts as `speaker: "pepe_thought"` where the relevant data schema supports speakers, as TXT-011 currently requires that legacy tag. A future schema migration may rename it, but copy should not wait on that rename.
8. On future content passes, lint exact `* (...)` lines separately from parenthetical monster speech and narration. This prevents Modsmal speech and mixed `* ...` prose from entering the thought register.

No recommendation states what the secret is, and no added clue should do so either. The player should only be able to infer it by combining the five beats and the later optional payoff.
