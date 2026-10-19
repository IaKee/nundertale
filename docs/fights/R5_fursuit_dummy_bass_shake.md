# R5 — Fursuit Dummy: The Bass Shake (fight spec)

> **Status:** design locked for implementation (2026-10-18). Backlog item: `C1-005` (R5 Fursuit Dummy). Lore: `docs/LORE.md` §18.1 and §26.3.
> **Supersedes** the earlier v3.1 idea of a fast pattern and a slow pattern (`bass_fluff_slow`) switching on the track's tempo changes. Now the menu turn only plays the calm part, and every enemy turn plays the meme's fast part **three times**, with one burst of bullets on each.

## 1. The idea in one paragraph

The battle music is the **"bass shake" meme loop** ("The Bass", DJ Wave). During the player's menu turn the camera holds the **wide shot**: the whole coat rack swaying to the calm part of the loop. When the dummy's turn starts, the game **hard-cuts into the meme exactly as it plays**: close-up on the fursuit head shaking to the **accelerated part** (~1.4 s), cut to the wide shot for the calm part, and so on. **Each accelerated part is one burst of fluff fired on its notes, and each turn has 3 bursts.** Every close-up opens with a snap zoom and a **new facial expression**. After the third burst the cut back to the wide shot ends the turn, and the music keeps going into the calm part under the menu.

Only the meme's choreography, editing and music are referenced. The meme's character, outfit and colors are **not** used (the battle stays white on black).

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

One file holds the whole loop: `assets/music/bass_shake.ogg`. Trim the lead-in so the first downbeat is at t = 0, and make the file exactly 3.692 s long (pad the last few ms with silence if the crop is short), OGG Vorbis:

```bash
ffmpeg -ss 0.070 -i closeup.mp4 -vn -af "apad=whole_dur=3.692" -t 3.692 -c:a libvorbis -q:a 6 assets/music/bass_shake.ogg
```

Check the seam by ear. Sections inside the file (with a downbeat at 0):

| Name | Time | Use |
| :--- | :--- | :--- |
| `fast` | 0.000–1.385 (beats 1–3) | an enemy-turn burst |
| `calm` | 1.385–3.692 (beats 4–8) | gaps between bursts |
| `menu` | 1.846–3.692 (bar 2, beats 5–8) | menu-turn loop (a clean 4/4 bar of the calm part) |

`data/music.json` gets `battle.bass_shake` with `loopStart` 1.846 and `loopEnd` 3.692 (the menu loop). `data/encounters/fursuit_dummy.json` sets `"music": "battle.bass_shake"`.

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
- **Entering the turn is a hard cut** (seek to 0 = the close-up downbeat). Optional polish: wait for the next beat of the menu loop (max 0.46 s) before seeking, so the cut lands on the beat.
- **Leaving the turn needs no music change** except restoring the menu loop points: the track is already in the calm part.

## 5. Bullets: `bass_fluff_fast`

- **One fluff tuft per onset inside a burst:** 4 (sixteenths, beat 1) + 2 (eighths, beat 2) + 4 (sixteenths, beat 3) = **10 per burst, 30 per turn**, spawned exactly on the notes.
- **Side follows the head shake:** the head swings left/right on every note; each tuft is thrown from the side the head swung to, across the box. The sixteenth runs read as rapid left-right volleys, the beat-2 eighths as two slower, bigger throws.
- **On each beat's downbeat** (0, 0.462, 0.923 s into the burst): a 1–2 px nudge of the battle box in the shake direction (the "drop" shake from the LORE, kept tiny so the soul stays readable).
- **Wide-shot gaps: no new bullets.** Tufts from the burst finish crossing the box, then the soul gets ~2 s of breathing room while the coat rack dances.
- **Tutorial tuning:** ATK 0 → damage 1 per hit; tufts slow and small enough that a first-time player can dodge a sixteenth run. Bullets are white with no outline (battle bullet rule). Anything still alive at turn end is cleared on the cut, like every other enemy.

## 6. Engine work

1. **Music clock and seek.** `IAudioService` has neither today. Add `float musicTime() const` (raylib `GetMusicTimePlayed`) and `void seekMusic(float seconds)` (`SeekMusicStream`) to `IAudioService`/`AudioService` and the test fake. The beat scheduler and the camera read this clock, **not** the frame timer, so nothing drifts from the audio. Fallback to the turn timer when no track is loaded (missing asset, tests). `setMusicLoopPoints` already exists.
2. **Beat map in data.** New `data/beatmaps/bass_shake.json`:
   ```json
   {
     "track": "battle.bass_shake",
     "bpm": 130,
     "offset": 0.0,
     "loopLength": 3.692,
     "menuLoop": [1.846, 3.692],
     "passesPerTurn": 3,
     "turnEndsAt": 1.385,
     "latencyMs": 0,
     "shots": [
       { "from": 0.0,   "to": 1.385, "shot": "closeup" },
       { "from": 1.385, "to": 3.692, "shot": "wide" }
     ],
     "burst": {
       "notes": [0.000, 0.115, 0.231, 0.346, 0.462, 0.692, 0.923, 1.038, 1.154, 1.269],
       "kicks": [0.000, 0.462, 0.923]
     },
     "expressions": ["smug", "wink", "pout", "smirk"]
   }
   ```
   Times are seconds from the downbeat of a pass, and `notes` are the grid positions of the measured onsets (§2.1). `turnEndsAt` is measured in the last pass. `latencyMs` shifts spawns earlier to cover audio output latency (tune in playtest).
3. **Beat pattern kind.** A new `EnemyPatternKind::Beat` (`"kind": "beat"`, `"beatmap": "bass_shake"`). Its update calls a **pure function**: given the previous and current music time (and the pass count, since the clock wraps at the loop), which notes fell in `(prev, now]`? It spawns one tuft per note. Keep that function free of raylib so `tests/test_runner.cpp` can cover it: exactly 10 spawns per close-up and 0 in the wide shot, no double spawn across a frame boundary or the loop wrap, 30 spawns and the turn end at pass 3 + 1.385 s, and the shot lookup at the section edges.
4. **Turn hooks.** For an enemy with a beat pattern: on `startEnemyTurn`, set the loop points to the whole file and seek to 0. Count the passes by watching the clock wrap. End the turn at pass 3 + `turnEndsAt`, then restore the `menuLoop` points. Barky and the generic patterns are untouched.
5. **Shot rendering.** The shot (close-up or wide) comes from the beat map at the current music time, in the menu turn too (always wide there). The close-up replaces the enemy sprite above the box (the box stays where it is, for readability). On each cut to the close-up: a snap zoom (scale ~1.15 → 1.0 over ~0.08 s) and the next expression. Head shake = horizontal offset/tilt alternating on every burst note, with the ears and fur tufts lagging one frame behind for the bounce. Wide shot: one sway step per beat.
6. **Optional:** a white halftone-dot overlay behind the close-up (the meme's pop-art panel), off by default.

## 7. Sprites

White on black, per the battle sprite rules. **Generate them with the `chatgpt-imagegen` skill**; small sizes go through macro-pixel (`macro_pixel.py`), and follow the battle outline rule (white mass, black-line decode).

| Sprite | Frames | Size |
| :--- | :--- | :---: |
| `fursuit_dummy` (wide shot) | coat rack + spare fursuit, one step per beat: center / lean L / lean R (3f, sleeves swinging) + hurt | ~40×60 |
| `fursuit_dummy_closeup` | fursuit head, direct eye contact, **4 expressions** × shake lean L / lean R (8f). Expressions (the meme's rotation adapted to a fursuit head): **smug grin**, **wink**, **pout / kissy**, **confident smirk**. Ears and cheek fluff sit differently on each lean so the bounce reads | ~120×120 |
| fluff tuft bullet | small (sixteenths) + big (eighths/kicks) | ~8×8 / ~12×12 |

Each turn shows 3 of the 4 expressions (one per close-up). The starting expression moves forward by one every turn, so consecutive turns don't repeat the same sequence.

### 7.1 Key poses from the clip (2026-10-18, revised on the user's request)

The sprites copy the clip's **key poses and camera**, not its character:
- **Wide shot:** feet planted wide, hands on hips holding the fluffy paws at hip level. A 4-step cycle on the eighth notes: up + lean/head tilt to one side → bounce down (knees bent, centered) → up + lean to the other side → bounce down. Sheet: 48×64 cells, `idle` 4 frames (the 2nd lean is the mirror of the 1st), stepped by the music clock.
- **Close-up:** a high-angle chibi shot (huge head on top, small body below, fluffy paws sticking out sideways like wings), tilting left/right on every note. Each of the 4 expressions is drawn tilting left; the right tilt is its mirror.
- Pose reference frames (in the clip): wide 1.70 s (lean) and 1.93 s (bounce); close-up 0.09 s and 0.20 s (the two tilts).

## 8. Reference: the meme's editing (Gemini frame-by-frame summary, condensed)

The user had Gemini describe the meme frame by frame. The character's design, outfit and colors are **ignored**. We keep:

- Two shots alternate on the music: an **extreme close-up on the face whenever the tempo speeds up**, and a **wide full-body shot** dancing to the 4/4 beat otherwise (measured: close-up beats 1–3, wide beats 4–8).
- Close-up: fast side-to-side head shaking locked to the bassline, bouncy secondary motion on the hair (→ the fursuit's ears and fur tufts), direct eye contact, and **a new expression on every snap zoom** (smug grin → playful wink → mischievous pout → confident smirk).
- Wide shot: hips swaying side to side, shoulders bouncing, sleeves fluttering, a step on every beat (→ the coat rack swaying, the fursuit sleeves swinging).
- Editing: instant hard cuts and whip-zooms on the big hits, in a loop that repeats seamlessly.

## 9. Open questions

| # | Question | Default if unanswered |
| :--- | :--- | :--- |
| Q1 | ~~Is an 8.8 s enemy turn too long for the tutorial?~~ | **Decided (2026-10-18): keep 8.8 s**, the meme's structure. |
| Q2 | Should the wide-shot gaps inside the turn fire anything (e.g. one slow tuft per beat)? | No: the 3 bursts are the whole attack. |
| Q3 | Should `Shake to the Bass` change the next turns (e.g. sparser bullets, Furiell dancing beside the box)? | Furiell dances on the 3rd use (LORE); bullets unchanged. |
| Q4 | `Turn It Down`: does the music stop for the rest of the battle? | Yes: the menu loop drops to a quiet volume, the dummy is sad, there are no more attack turns, and it's spareable. |

## 10. ACTs and reactions (from LORE §18.1, unchanged)

- `Talk` · `Shake to the Bass` ×3 (you join in; on the 3rd, Furiell joins too and it becomes spareable) · `Turn It Down` (it stops shaking, sad; spareable) · `Accept Hug` (`* You hug the dummy. It's damp. FUR +10.`; spares).
- `data/enemies/fursuit_dummy.json` still has the old ACT set (`Talk`, `Accept Hug`) and Check text from before v3.1. Update it to the LORE list as part of this fight.
- Furiell: Shake → she dances (`uwu~ pump the bass, pup!`); Accept Hug → beaming; FIGHT → `Hey! That's my fwiend's suit!`; flee → `...that is also a choice.`
- Doesn't count for the toxic track.

## 11. Suggested split for the synergy session

Synergy does its own routing. This is only how the work divides into pieces that don't share files:

| Piece | Files | Kind |
| :--- | :--- | :--- |
| Audio prep (§3) | `assets/music/bass_shake.ogg`, `data/music.json` | mechanical |
| Music clock/seek + beat map + scheduler + tests (§6.1–6.3) | `IAudioService`, `AudioService`, new beat-map loader, `EnemyPatterns.hpp`, `tests/test_runner.cpp` | engine, testable |
| Turn hooks + shot rendering (§6.4–6.5) | `BattleSystem.cpp`, `Enemy.*` | engine, visual check |
| Sprites (§7), via `chatgpt-imagegen` + macro-pixel | `assets/sprites/battle/fursuit_dummy*`, `assets/_gen/ruins/` | visual |
| Data: ACTs, Check, Furiell reactions (§10) | `data/enemies/fursuit_dummy.json`, `data/encounters/fursuit_dummy.json`, R5 cutscene | data |

**Done when:** in R5 the menu turn shows the coat rack swaying on the calm loop. Every enemy turn plays the meme three times over, cutting to the close-up for each accelerated part and firing a 10-tuft burst on its notes, with a new expression per close-up. The turn ends on the third cut back to the wide shot, with the music running on. The beat scheduler tests pass, and all four ACT outcomes reach their Furiell reaction.
