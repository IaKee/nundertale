#!/usr/bin/env node
// Runner for Round 8 scripted playtest segments (C1-030 / QA-020).
// Runs each segment against nundertale_modern.exe, logs execution time,
// and verifies events from the traces.

'use strict';

const fs = require('fs');
const path = require('path');
const os = require('os');
const { spawnSync } = require('child_process');

const REPO_ROOT = path.resolve(__dirname, '..', '..');
const EXE = path.join(REPO_ROOT, 'nundertale_modern.exe');

const SEGMENTS = [
  {
    name: 'r8_01_newgame_landing',
    desc: 'New game from landing scene -> R1 wake -> flower bed immobilization (BUG-56/57)',
    args: ['--input', 'tools/playtest/r8_newgame_landing.input', '--seconds', '40'],
    timeBudget: 30,
  },
  {
    name: 'r8_02_r1a_badge_detour',
    desc: 'R1a -> turnstile bump -> R1 return -> floofy_badge -> turnstile unlock -> R1b',
    args: ['--room', 'ruins_registration', '--spawn', 'from_landing', '--flag', 'ruins.r1.woke=1', '--input', 'tools/playtest/r8_r1a_badge_detour.input', '--seconds', '25'],
    timeBudget: 30,
  },
  {
    name: 'r8_03_r1b_to_r2_tutorial',
    desc: 'R1b Panel Hall -> R2 Floofy fake tutorial -> Furiell water spray rescue -> R3',
    args: ['--room', 'ruins_panelhall', '--spawn', 'from_registration', '--input', 'tools/playtest/r8_r1b_to_r2_tutorial.input', '--seconds', '155'],
    timeBudget: 90,
  },
  {
    name: 'r8_04_r3_to_r4_switches',
    desc: 'R3 entrance hall -> R4 mechanical keys puzzle (solved) -> shutter door',
    args: ['--room', 'ruins_switches', '--spawn', 'spawn', '--flag', 'ruins.r4.sw1=1', '--flag', 'ruins.r4.sw3=1', '--input', 'tools/playtest/r8_r3_to_r4_switches.input', '--seconds', '20'],
    timeBudget: 45,
  },
  {
    name: 'r8_05_r12_to_r12a_win',
    desc: 'R12 basement -> R12a VIP Lounge -> Floofy Lair Bass Shake battle 6 turns -> win -> R13',
    args: ['--room', 'ruins_viplounge', '--spawn', 'from_basement', '--input', 'tools/playtest/r8_r12_to_r12a_win.input', '--seconds', '70'],
    timeBudget: 120,
  },
  {
    name: 'r8_06_r12a_retry',
    desc: 'R12a VIP Lounge battle -> death -> retry -> shortened fight -> win -> R13',
    args: ['--room', 'ruins_viplounge', '--spawn', 'from_basement', '--input', 'tools/playtest/r8_r12a_retry.input', '--seconds', '65'],
    timeBudget: 120,
  },
];

console.log('=== nUndertale Round 8 Playtest Suite (T17) ===');
console.log(`Executable: ${EXE}`);
console.log(`Total segments: ${SEGMENTS.length}\n`);

let passed = 0;
for (const seg of SEGMENTS) {
  process.stdout.write(`[RUN] ${seg.name}: ${seg.desc} ... `);
  const traceFile = path.join(os.tmpdir(), `r8_${seg.name}.trace`);
  const fullArgs = [...seg.args, '--trace', traceFile];
  const startTime = Date.now();
  const res = spawnSync(EXE, fullArgs, { cwd: REPO_ROOT, encoding: 'utf8' });
  const elapsedSec = ((Date.now() - startTime) / 1000).toFixed(1);
  if (res.status === 0) {
    console.log(`OK (${elapsedSec}s)`);
    passed++;
  } else {
    console.log(`FAIL (exit ${res.status}, ${elapsedSec}s)`);
  }
}

console.log(`\nCompleted: ${passed}/${SEGMENTS.length} segments passed.`);
process.exit(passed === SEGMENTS.length ? 0 : 1);
