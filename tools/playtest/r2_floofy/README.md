# R2 Floofy playtest inputs (C1-002 / BUG-42)

Recorded input scripts (no frames, no traces) for the R2 chain, from the R1b start with no debug flags except the
launch room. 30 ticks = 1 s, seed 0 (lockstep with `--trace`).

| file | what it plays |
| --- | --- |
| `polite.input` | R1b save, R2 thought, intro, `Sorry.` / `...Okay.`, petal fight (variant 1), SPRITZ, Furiell, R3 |
| `annoyed.input` | same walk with `You broke my fall.` / `Those are NOT petals.`: Bass Shake fight (variant 2) |
| `retry.input` | dies in turn 4, WELCOME HOME at the R1b save, walks back in: `r2_floofy_retry`, no intro, same variant |
| `segments.json` | trace expectations for the three, for `ruins_walkthrough.js --spec` |

**The inputs save at the R1b star, and the game saves to `%LOCALAPPDATA%\nUndertale`. Point `LOCALAPPDATA` at a scratch
folder before running**, or your own save is overwritten.

```
set LOCALAPPDATA=%TEMP%\s2_r2_floofy
node tools/playtest/ruins_walkthrough.js nundertale_modern.exe --spec tools/playtest/r2_floofy/segments.json
```

(`nundertale_modern.exe` must be a `-DNUNDERTALE_DEBUG` build, see `docs/playtest.md`.) Single run with a trace:

```
nundertale_modern.exe --room ruins_panelhall --spawn from_registration --input tools/playtest/r2_floofy/polite.input --seconds 160 --trace tmp/polite.trace
```

The dodge moves in each file (everything after the first enemy turn start, listed in the file header) come from a
search on this seed, so they are only valid while the dialog, intro and turn lengths stay as they are. If a change
shifts the timing, re-measure the enemy turn starts in the trace (`battle_state state=EnemyTurn`) and shift or redo the
movement lines; the segments then fail on a missing event and the trace shows where.
