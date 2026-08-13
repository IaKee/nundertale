# R2 — Floofy: The Bass Shake (fight spec)

> **Status:** implemented as data + engine hooks (2026-10-29). Backlog item: `C1-028` (Floofy's Bass Shake fight). Lore: `docs/LORE.md` §19.0 ("Bass Shake fight"), §10.10, §26.
> **History:** this spec was `docs/fights/R5_fursuit_dummy_bass_shake.md` (v3.1, the Fursuit Dummy's fight, `C1-005`). LORE v3.2 moved The Bass Shake to Floofy's alternate fight; the measurements in §2 and §3 are unchanged, the rest is retargeted. The earlier v3.1 idea of a fast pattern and a slow pattern (`bass_fluff_slow`) is still dropped.

## 1. The idea in one paragraph

The battle music is the **"bass shake" meme loop** ("The Bass", DJ Wave). It is Floofy's alternate fight, picked when `ruins.floofy.annoyed >= 2` at the twist (`ruins.floofy.fight = 2`, reused on retry). The scene ends on `...` / `You think you're FUNNY?` / `I've been nice. I've been SO nice.` / `Now hold still, darling~ ...no. Actually? Don't.` / `DANCE.`; the battle opens on the narrator line `* ...DANCE.`. During the player's menu turn the camera holds the **wide shot**: Floofy swaying with his bouquet tail to the calm part of the loop. When his turn starts, the game **hard-cuts into the meme exactly as it plays**: close-up on his head shaking to the **accelerated part** (~1.4 s), cut to the wide shot for the calm part, and so on. **Each accelerated part is one burst of thorny petals fired on its notes, and each turn has 3 bursts.** Every close-up opens with a snap zoom and a **new facial expression** (smug, wink, pout, smirk). After the third burst the cut back to the wide shot ends the turn, and the music keeps going into the calm part under the menu.

Only the meme's choreography, editing and music are referenced. The meme's character (Milk, by SeanWayStudio), outfit and colors are **not** used (the battle stays white on black).

## 2. Source clip

- **File:** `C:\Users\delux\OneDrive\Área de Trabalho\closeup.mp4`: the user's crop of the section the meme loops, **with both camera angles**. Copy it to a repo/WSL-reachable path before processing; the accented path can break tools.
- **Container:** H.264 854×480 @ 30 fps (114 frames) + AAC 48 kHz stereo. Video 3.800 s, audio **3.755 s**.
- **Do not re-analyze the video frame by frame.** The numbers below came from ffmpeg + numpy (a 16×9 grayscale per-frame difference and an audio onset pass). Gemini's description in §8 covers the look.

### 2.1 Measured structure

**Tempo ≈ 130 BPM, 4/4** (beat 0.4615 s, eighth 0.231 s, sixteenth 0.115 s). The loop is **2 bars = 8 beats = 3.692 s**, first downbeat at ≈**0.07 s**.

| Beats | Time (s) | Shot | Rhythm (audio onsets) |
| :--- | :--- | :--- | :--- |
| 1 | 0.07–0.54 | **close-up** | **4 sixteenths:** 0.081 · 0.197 · 0.313 · 0.424 |
| 2 | 0.54–1.00 | close-up | 2 eighths: 0.540 · 0.772 |
| 3 | 1.00–1.46 | close-up | **4 sixteenths:** 0.998 · 1.120 · 1.231 · 1.347 |
| 4–8 | 1.46–3.76 | **wide** (hard cut at **1.467 s**, frame 44) | 10 plain eighths: 1.463 · 1.695 · 1.921 · 2.159 · 2.386 · 2.618 · 2.844 · 3.082 · 3.309 · 3.541 |

- **The accelerated part = the close-up = beats 1–3 (~1.4 s):** two sixteenth-note runs (beats 1 and 3) around a beat of eighths (beat 2). The head shakes continuously through it.
- **The calm part = the wide shot = beats 4–8 (~2.3 s):** plain eighths; the body steps **once per beat** (motion pulses at ≈1.97 · 2.43 · 2.87 · 3.33 s).
- The cut back to the close-up happens at the loop point (end → start).
- Detection resolution is ~6 ms. 0.07 + 3.692 = 3.762, about 7 ms past the audio's end, so the crop is a hair short or the downbeat is slightly earlier. **Confirm the downbeat by ear** (Audacity) before locking `offset` in the beat map.

## 3. Audio asset

One file holds the whole loop: `assets/music/bass_shake.ogg` (already in the repo). The first downbeat is at t = 0 and the file is exactly 3.692 s long (OGG Vorbis). The original encode:

```bash
ffmpeg -ss 0.070 -i closeup.mp4 -vn -af "apad=whole_dur=3.692" -t 3.692 -c:a libvorbis -q:a 6 assets/music/bass_shake.ogg
```

Sections inside the file (with a downbeat at 0):

| Name | Time | Use |
| :--- | :--- | :--- |
| `fast` | 0.000–1.385 (beats 1–3) | an enemy-turn burst |
| `calm` | 1.385–3.692 (beats 4–8) | gaps between bursts |
| `menu` | 1.846–3.692 (bar 2, beats 5–8) | menu-turn loop (a clean 4/4 bar of the calm part) |

`data/music.json` has `battle.bass_shake` with `loopStart` 1.846 and `loopEnd` 3.692 (the menu loop). `data/encounters/floofy_bass.json` sets `"music": "battle.bass_shake"`.

## 4. Turn structure

```
MENU TURN    wide shot · music loops bar 2 (calm) · no bullets
   │  player acts → text box → enemy turn starts
   ▼
ENEMY TURN   seek music to 0 · loop points = whole file (0 – 3.692)
   pass 1   beats 1–3  CLOSE-UP  burst A  (expression 1)
            beats 4–8  wide      gap
   pass 2   beats 1–3  CLOSE-UP  burst B  (expression 2)
            beats 4–8  wide      gap
   pass 3   beats 1–3  CLOSE-UP  burst C  (expression 3)
            beat 4     cut to wide → the turn ends here (t = 2×3.692 + 1.385 = 8.769 s)
   ▼
MENU TURN    loop points back to bar 2; the music rolls on into beats 4–8 and loops the calm bar
```

- **Enemy turn = 8.77 s**, from the beat map, not the default turn constant.
- **The camera follows the music, not a timer:** close-up exactly on beats 1–3 of every pass, wide on beats 4–8, hard cuts with no fades, as in the meme.
- **Entering the turn is a hard cut** (seek to 0 = the close-up downbeat).
- **Leaving the turn needs no music change** except restoring the menu loop points: the track is already in the calm part.

### 4.1 The fight rules (same as the petal fight, C1-027)

- **Can't be hurt:** FIGHT → `* You hit him. Your fist sinks into the fursuit padding.` / `* It's too soft to hurt.` and `Aww~ was that a hug?`
- **No Spare:** MERCY has no `Spare`/`Yiff` entry; `Flee` → `* The petals block the exit.`
- ACTs: `Check` · `Plead` (`* You say you're not into it.` → `That's what they ALL say~`) · `Compliment the Crown` (`* You say the crown is nice.` → `I KNOW.`). None of them end the fight. (`Shake to the Bass`, `Turn It Down` and `stopsBeat` were the dummy's and are dropped.)
- **Six enemy turns that get harder:** bullet speed grows +6 % per turn (`bullet.speedPerTurn`), 3 damage on the small notes and 4 on the big ones (ATK 3, `damage` 1, `bigDamageBonus` 1, armor DF 1 at the start), FUR +6 on every petal, capped at 99 in R2. Dying is allowed: HP 0 → WELCOME HOME (§23.1), back to the R1b save.
- **The ending (turn 6, or earlier after 3+ deaths via `ruins.floofy.deaths`):** that turn plays normally until the **finale drop** (`finale` in the beat map: pass 3's downbeat, 7.385 s into the turn). There Furiell's `*SPRITZ*` lands: `assets/sfx/spray.ogg` + `assets/sfx/record_scratch.ogg`, the track stops dead, bullets clear, and the battle ends through `scriptedEnd.endCutscene` = `r2_floofy_end` (beats 19–26).

## 5. Bullets: thorny petals (`thorn_petal`)

- **One thorny petal per onset inside a burst:** 4 (sixteenths, beat 1) + 2 (eighths, beat 2) + 4 (sixteenths, beat 3) = **10 per burst, 30 per turn**, spawned exactly on the notes. Sprite `assets/sprites/bullets/thorn_petal.png` (the spiked petal shared with `petal_storm` and `bouquet_burst`; D-20: white on black, tiny, no anatomy). Until it exists the engine draws the plain circle fallback.
- **Side follows the head shake:** the head swings left/right on every note; each petal is thrown from the side the head swung to, across the box. The sixteenth runs read as rapid left-right volleys, the beat-2 eighths as two slower, bigger throws (the "big" notes, damage +1).
- **On each beat's downbeat** (0, 0.462, 0.923 s into the burst): a 1–2 px nudge of the battle box in the shake direction (the "drop" shake from the LORE, kept tiny so the soul stays readable).
- **Wide-shot gaps: no new bullets.** Petals from the burst finish crossing the box, then the soul gets ~2 s of breathing room while Floofy sways.
- Bullets are white with no outline (battle bullet rule). Anything still alive at turn end is cleared on the cut, like every other enemy.

## 6. Engine work (done)

1. **Music clock and seek.** `IAudioService::musicTime()` and `seekMusic()` exist. The beat scheduler and the camera read this clock, **not** the frame timer. Fallback to the turn timer when no track is loaded.
2. **Beat map in data.** `data/beatmaps/floofy_bass.json` (the Floofy fight) and `data/beatmaps/bass_shake.json` (the dummy's, kept for the inline `BSK-*` suites; same timing). Times are seconds from the downbeat of a pass, and `notes` are the grid positions of the measured onsets (§2.1). `turnEndsAt` is measured in the last pass. `latencyMs` shifts spawns earlier to cover audio output latency (tune in playtest). Fields added for C1-028, all optional so the dummy's map reads as before:

   | Field | Meaning |
   | :--- | :--- |
   | `closeupSprite` | battle sprite base used for the close-up when `<enemy sprite>_close` does not exist (stand-in `fursuit_dummy_close` until Floofy's own `floofy_close` lands, which then wins automatically) |
   | `bullet.sprite` / `spriteBig` / `radius` / `radiusBig` | the bullet look (`thorn_petal.png`); empty = the fluff tufts |
   | `bullet.bigDamageBonus` | extra damage on the big notes |
   | `bullet.speedPerTurn` | fraction of speed added each enemy turn after the first |
   | `wide.stepSixteenths` | the wide shot advances one idle frame every N sixteenths (4 = one step per beat) |
   | `finale.pass` / `time` / `sfx` | the drop where the scripted end lands, and the sounds played there |

3. **Beat pattern kind.** `EnemyPatternKind::Beat` (`"kind": "beat"`, `"beatmap": "floofy_bass"`). A **pure function** (`beatEventsBetween`) tells which notes fell in `(prev, now]`. It is free of raylib so `tests/test_runner.cpp` covers it: exactly 10 spawns per close-up and 0 in the wide shot, no double spawn across a frame boundary or the loop wrap, 30 spawns and the turn end at pass 3 + 1.385 s, and the shot lookup at the section edges. `tests/suites/ru36.hpp` covers the Floofy data.
4. **Turn hooks.** On `startEnemyTurn`, set the loop points to the whole file and seek to 0. Count the passes by watching the clock wrap. End the turn at pass 3 + `turnEndsAt`, then restore the `menuLoop` points. On the scripted last turn, `beatFinaleTurn()` keeps the battle from cutting at the turn start and `finishBeatFinale()` ends it at the finale drop (a stalled clock still ends it at the turn's safety timeout).
5. **Shot rendering.** The shot (close-up or wide) comes from the beat map at the current music time, in the menu turn too (always wide there). The close-up replaces the enemy sprite above the box. On each cut to the close-up: a snap zoom (scale ~1.15 → 1.0 over one sixteenth) and the next expression. Head shake = horizontal offset alternating on every burst note. Wide shot: one sway step per beat.
6. **Optional:** a white halftone-dot overlay behind the close-up (the meme's pop-art panel), off by default.

## 7. Sprites

White on black, per the battle sprite rules.

| Sprite | Frames | Size |
| :--- | :--- | :---: |
| `floofy` (wide shot) | swaying with the bouquet tail, one step per beat (idle, now 2f) + talking, sinister | ~50×70 |
| `floofy_close` (ART-183 / T24) | Floofy's head, direct eye contact, **4 expressions** (smug grin, wink, pout / kissy, confident smirk) × shake lean L/R, flower crown and ear tufts whipping, petals flying off | ~120×120 |
| `thorn_petal` bullet | spiked petal, small (sixteenths) and big (eighths/kicks) | ~8×8 / ~12×12 |

Until `floofy_close` exists the fight uses `fursuit_dummy_close` (the dummy's close-up, 4 expressions × 3 lean frames) through `closeupSprite`. Each turn shows 3 of the 4 expressions (one per close-up). The starting expression moves forward by one every turn, so consecutive turns don't repeat the same sequence.

### 7.1 Key poses from the clip (2026-10-18)

The sprites copy the clip's **key poses and camera**, not its character:
- **Wide shot:** feet planted wide, hands on hips. A 4-step cycle on the eighth notes: up + lean/head tilt to one side → bounce down (knees bent, centered) → up + lean to the other side → bounce down.
- **Close-up:** a high-angle shot (huge head on top), tilting left/right on every note. Each of the 4 expressions is drawn tilting left; the right tilt is its mirror.
- Pose reference frames (in the clip): wide 1.70 s (lean) and 1.93 s (bounce); close-up 0.09 s and 0.20 s (the two tilts).

## 8. Reference: the meme's editing (Gemini frame-by-frame summary, condensed)

The character's design, outfit and colors are **ignored**. We keep:

- Two shots alternate on the music: an **extreme close-up on the face whenever the tempo speeds up**, and a **wide full-body shot** dancing to the 4/4 beat otherwise (measured: close-up beats 1–3, wide beats 4–8).
- Close-up: fast side-to-side head shaking locked to the bassline, bouncy secondary motion on the hair (→ Floofy's flower crown and ear tufts), direct eye contact, and **a new expression on every snap zoom** (smug grin → playful wink → mischievous pout → confident smirk).
- Wide shot: hips swaying side to side, shoulders bouncing, a step on every beat (→ Floofy swaying with his bouquet tail).
- Editing: instant hard cuts and whip-zooms on the big hits, in a loop that repeats seamlessly.

## 9. Open questions

| # | Question | Default if unanswered |
| :--- | :--- | :--- |
| Q1 | ~~Is an 8.8 s enemy turn too long?~~ | **Decided (2026-10-18): keep 8.8 s**, the meme's structure. |
| Q2 | Should the wide-shot gaps inside the turn fire anything? | No: the 3 bursts are the whole attack. |
| Q3 | LORE says the last turn also ends in the `pounce_ring`, on the beat. | Not built: the finale drop (spray + scratch) ends the turn instead. Add a ring on the finale pass if playtest wants it. |
| Q4 | `Compliment the Crown` and `Plead` change nothing mechanically. | Yes: none of the ACTs end or soften the fight. |

## 10. Wiring

- Encounter `floofy_bass` (`data/encounters/floofy_bass.json`, enemy `data/enemies/floofy_bass.json`, beat map `data/beatmaps/floofy_bass.json`). The R2 scene starts it when `ruins.floofy.fight == 2` (`r2_floofy_scene`); `r2_floofy_end` plays after the spray.
- Doesn't count for the toxic track.
