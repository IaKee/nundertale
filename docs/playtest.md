# Playtest harness: scripted Ruins walkthrough

An automated way to play parts of the Ruins and prove that every scripted fight and cutscene really happens
(the C1-030 "Ruins playable start to finish" check; the base for QA-020 pacing). The game logs what it does to a
trace file, and a node script drives the game segment by segment and checks the trace.

## Build

The harness needs a build with `NUNDERTALE_DEBUG` (the debug flags, including `--trace`, are compiled out of release
builds). From the repo root, with the toolchain paths from the contracts:

```
set GXX=C:/Users/delux/OneDrive/Documentos/Programas/C/nundertale/vendor/w64devkit/bin/g++.exe
set RL=C:/Users/delux/OneDrive/Documentos/Programas/C/nundertale/vendor/raylib-6.2_win64_mingw-w64
%GXX% -std=c++20 -O1 -DNUNDERTALE_DEBUG -Iinclude -I%RL%/include src/*.cpp src/*/*.cpp -L%RL%/lib -lraylib -lopengl32 -lgdi32 -lwinmm -o nundertale_modern.exe
```

(`build.bat` already defines `NUNDERTALE_DEBUG`.) The exe is run with the repo root as working directory, because the
game reads `data/` and `assets/` relative to it.

## Run

```
node tools/playtest/ruins_walkthrough.js nundertale_modern.exe
```

Options: `--segment <name>` (repeatable) runs only those segments, `--list` prints the segments, `--verbose` prints the
command line and the last trace lines of passing segments too, `--trace-dir <dir>` changes where traces are written
(default: `nundertale_playtest` in the system temp folder), `--timeout <s>` is the wall-clock limit per segment
(default 120), `--spec <file>` uses another segment spec.

Each segment is one game run (about 1-3 s: a `--trace` run with `--input` steps the game at fixed 30 Hz, one logic tick
per frame, with no frame cap and random seed 0, so it is deterministic). The exit code is 0 only if every selected
segment meets every expectation; a segment also fails if the game exits non-zero or the trace has no `end` event
(a crash). A failing expectation that points at a real game bug is a valid result: fix the game, do not loosen the
expectation.

Current segments (`tools/playtest/ruins_segments.json`):

| segment | what it proves |
| --- | --- |
| `r4_to_r5_dummy_fight_starts` | R4 hint, R4 -> R5 door, Furiell's `r5_dummy` intro, then the Fursuit Dummy battle starts |
| `r5_dummy_spare` | the dummy fight played to a spare (ACT > Shake x3, MERCY > Spare) and the `r5_dummy_spare` reaction plays |
| `r8_to_r9_barky_fight_starts` | R8 -> R9 door, `barky_entrance`, Barky battle |
| `r9_barky_doorway_fight_starts` | the same fight from R9's own spawn (isolates the fight from the R8 -> R9 door) |
| `r12_basement_to_furiell_fight_starts` | `r12_entry` walk, `r12_boss`, Furiell battle |

## The `--trace <file>` flag

`--trace <file>` (debug builds) appends one line per event to `<file>`, nothing is written or checked when the flag is
absent. Format: `t=<logic tick> <event> key=value ...` (30 ticks = 1 s):

| event | fields | meaning |
| --- | --- | --- |
| `begin` | `room`, `spawn` | the launch arguments |
| `room` | `id`, `spawn` | `state.currentRoom` changed (door, debug launch); `spawn` is the spawn the room was entered at |
| `scene_push` / `scene_pop` | `scene` | the scene stack changed (`OverworldScene`, `EncounterTransitionScene`, `BattleScene`, `GameOverScene`, ...) |
| `cutscene_start` / `cutscene_end` | `id` | the Overworld's cutscene runner started / finished a cutscene; the id is the `data/cutscenes/<id>.json` it matches (`?` if none, `a|b` if two files are identical) |
| `battle_start` | `encounter` | a `BattleScene` entered the stack |
| `battle_state` | `state` | the battle's state machine moved (`MenuSelect`, `ActSelect`, `DialogMessage`, `EnemyTurn`, `MercySelect`, `Victory`, ...); this is what input scripts are timed against |
| `battle_end` | `encounter`, `outcome`, `hp` | the `BattleScene` left the stack; `outcome` is `killed`, `spared`, `fled`, `lost` (game over / 0 HP) or `won` (anything else) |
| `game_over` | `room` | a `GameOverScene` appeared |
| `flag` | `name`, `value`, `prev` | an int flag changed (`ruins.*`, `battle.*`, `kills.*`, everything) |
| `end` | `reason` | the run finished: `tick-limit`, `quit`, `window-closed`, `shot` |

Everything is derived by looking at the game once per logic tick (scene stack, `state.currentRoom`, the running
cutscene, `state.flags`), so scenes carry no tracing code; the private members it reads are reached with an explicit
template instantiation in `src/core/Game.cpp`. A cutscene that starts and ends inside one tick is still reported (the
runner keeps its last cutscene). `--trace` together with `--input` (and no `--record`) is the fast lockstep mode
described above; `--trace` alone just logs a normal interactive session.

Use it by hand too, for example to see what a room does:

```
nundertale_modern.exe --room ruins_dummy --spawn from_switches --input tools/playtest/ruins/r5_dummy_spare.input --seconds 60 --trace tmp/x.trace
```

`--seconds` is the length of the run (default 30 s, 30 ticks per second). The game appends to the trace file, so delete
it between runs (the harness does).

## Add a segment

1. Find the room, the spawn and the flags the segment starts from (`data/maps/<room>.json`, spawn objects), then write
   an input script `tools/playtest/ruins/<segment>.input`. Format (same as `tools/record/*.input`): one line per
   key, `<tick> <key>` for a one-tick press, `<from>-<to> <key>` to hold; keys `up down left right confirm cancel
   menu`; `#` starts a comment. Ticks count from the start of the run. Dialogs type out slowly, so tap `confirm` every
   10 ticks to skip them (a held confirm counts as one press).
2. Run it once with `--trace` and read the trace: when does the cutscene start, when does the battle start, which
   `battle_state` events follow. Time fight inputs against `battle_state` (menu returns at `MenuSelect`, your
   confirm skips the ACT text during `DialogMessage`, the enemy turn is `EnemyTurn`). If a segment walks into a fight
   with the confirm spam still running, stop the spam shortly before the battle starts.
3. Add the segment to `tools/playtest/ruins_segments.json`:

```json
{
  "name": "r9_barky_doorway_fight_starts",
  "description": "shown by --list",
  "room": "ruins_barky",
  "spawn": "spawn",
  "flags": { "ruins.some.flag": 1 },
  "input": "r9_barky_doorway.input",
  "seconds": 20,
  "expect": [
    { "event": "cutscene_start", "id": "barky_entrance" },
    { "event": "battle_start", "encounter": "barky" }
  ],
  "forbid": [{ "event": "game_over" }],
  "note": "printed when the segment fails"
}
```

`expect` entries must all appear in the trace **in that order** (each after the previous match); an entry matches a
trace line with the same `event` whose listed fields are equal (`within: <ticks>` additionally limits the time since the
previous match). `forbid` entries must not appear anywhere. `room`, `spawn`, `flags` and `input` are optional (a
segment without `room` starts at the game's default room).

## Limits

- The inputs are time-based, so a change in dialog length, turn length or walking speed can desync a script; the
  segment then fails on a missing event and the trace shows where it stopped. Re-measure and shift the ticks.
- `r5_dummy_spare` shifts its fight inputs by the tick the battle starts (335). If the intro cutscene gets longer,
  move the fight inputs by the same amount.
- The harness checks that things happen (and in what order), not how they look: use `--shot` / `--record` for that.
- Only the Ruins segments above exist so far. Furiell's fight itself (R12) and the rest of the rooms are not played.
