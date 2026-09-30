# Opening walkthrough (C1-030)

Plays the new opening **from a new game** with no debug launch flags (only `--input`, `--trace`, and `--record` for the
frame check) and asserts from the trace that it happens: WARNING → lore → title → name entry → R1 The Landing (Grub
lying) → R1a Registration → R1b Panel Hall (save) → R2 Headless Lounge → Floofy's intro and fight (petals for a polite
run, Bass Shake for an annoyed one) → Furiell's spray → R3, and a death → WELCOME HOME → retry path.

```
node tools/playtest/opening_walkthrough.js nundertale_modern.exe            # all segments, about 1 minute
node tools/playtest/opening_walkthrough.js nundertale_modern.exe --segment opening_polite --verbose
node tools/playtest/opening_walkthrough.js --list
```

`nundertale_modern.exe` must be a `-DNUNDERTALE_DEBUG` build (see `docs/playtest.md`). The runner gives every segment its
own empty `LOCALAPPDATA` under `%TEMP%\nundertale_opening\<segment>\appdata`, so there is no save to "Continue" and your own
save is never touched (the scripts save at the R1b star). Traces stay in `%TEMP%\nundertale_opening\<segment>\run.trace`
(`--trace-dir` changes that, `--keep-frames` keeps the frames of the frame check).

| segment | input | proves |
| --- | --- | --- |
| `opening_frame0` | `polite.input` + `--record` | R1 starts with `r1_wake` within 2 ticks of the Overworld; the recorded frames show Grub's lying cell at tick +30 and at the squad message, and not after `r1_wake` (white fade hides the first ~20 frames) |
| `opening_polite` | `polite.input` | every scene of the front end, every room (`ruins_fall`, `registration`, `panelhall`, `floofy`, `entrance`) with its spawn, `r2_lair_thought`, `r2_floofy_scene` → `r2_floofy_intro` (once), `choice1=1`, `battle floofy`, `fight=1`, petals won, `r2_floofy_end` (Furiell's spray), `met=1`, R3 + `r3_furiell`; the save file was written at R1b; no game over, no bass fight, no retry |
| `opening_annoyed` | `annoyed.input` | the same walk with `choice1=2`, `choice2=2`, `annoyed` 1 → 2, `fight=2`, `floofy_bass` won, same ending |
| `opening_pokes` | `pokes.input` | exactly three `r1_floofy_poke`, then `annoyed` is 1 at the start of the R2 intro and 2 after `choice1=2` (stops in the intro) |
| `opening_death_retry` | `retry.input` | dies in the petal fight, `game_over`, `deaths=1`, reload at the R1b save, back in R2: `r2_floofy_scene` → `r2_floofy_retry` (intro and choice 1 not repeated, same fight variant, `met` not set), the retry battle starts, a second death counts `deaths=2` |

Besides the trace the spec has `static` checks on shipped data for what the trace cannot see: the R1 new-game spawn
poses Grub as `lying` until `ruins.r1.woke`, and `r2_floofy_end` still has the two spray sounds, the water burst on Grub,
`healFull` and the hand grab.

## The input scripts are generated

`node tools/playtest/opening/build_inputs.js` writes `polite`, `annoyed`, `pokes` and `retry` `.input`: a shared prefix (front end,
name entry, `r1_wake`, optional pokes, walk up through R1 and R1a to R1b) followed by the R2..R3 part of the matching
`tools/playtest/r2_floofy/*.input`, shifted to the tick Grub enters R1b. `--check` fails if a file on disk is stale. The
prefix ticks are measured (see the header of each file); dialog only advances with Confirm on a finished page, so Confirm is
tapped every 10 ticks.

The R2 part keeps the dialog and enemy-turn ticks of the r2_floofy files, but not their dodges: the game seeds its RNG
at the start of the run and a longer prefix draws more of it, so the bullets differ. The polite run still wins the petal
fight (it always ends with 5 HP of 15 here; the r2_floofy segments end with more). If a content change
shifts a tick, the segment fails on a missing event; re-measure from the trace as in `docs/playtest.md` and move the
constants at the top of `build_inputs.js` (or fix the r2_floofy files, which this derives from).

## Known nondeterminism (not a harness bug)

The Bass Shake fight (`floofy_bass`, variant 2) is timed against `context.audio.musicTime()`, which in a `--trace --input`
run is the real audio playback position (the `RecordingAudioService` clock is only used with `--record`). The same input
therefore ends the bass fight with 9, 12 or 15 HP from run to run (about 15 runs, always won; hits are 3 HP each). The petal fight and everything else in the segments reproduce tick for tick. The segments only require `won`, so
they stay green, but a fix that changes the bass damage a lot could make `opening_annoyed` flaky: look at the trace first.

## Trace caveat

On WELCOME HOME the `room ruins_panelhall` line reports `spawn=from_panelhall`: that is the spawn of the still-alive R2
Overworld under the GameOverScene (the trace reads the topmost Overworld), not where Grub is put. The script therefore does
not assert the spawn of that line.
