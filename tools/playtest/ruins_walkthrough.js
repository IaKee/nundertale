#!/usr/bin/env node
// Scripted Ruins walkthrough: runs the game once per segment with an input script and checks that the
// scripted fights and cutscenes really happen (docs/playtest.md).
//
//   node tools/playtest/ruins_walkthrough.js <exe> [--spec file] [--segment name]... [--list]
//                                            [--trace-dir dir] [--timeout seconds] [--verbose]
//
// <exe> must be a -DNUNDERTALE_DEBUG build. Builds nothing. Run from the repo root (the game loads data/ and
// assets/ relative to its working directory). Exit code 0 only if every selected segment meets every expectation.

'use strict';

const fs = require('fs');
const os = require('os');
const path = require('path');
const { spawnSync } = require('child_process');

const PLAYTEST_DIR = __dirname;
const REPO_ROOT = path.resolve(PLAYTEST_DIR, '..', '..');

function usage(message) {
  if (message) console.error(`error: ${message}`);
  console.error('usage: node tools/playtest/ruins_walkthrough.js <exe> [--spec file] [--segment name]... [--list]');
  console.error('                                               [--trace-dir dir] [--timeout seconds] [--verbose]');
  process.exit(2);
}

function parseArgs(argv) {
  const options = {
    exe: null,
    spec: path.join(PLAYTEST_DIR, 'ruins_segments.json'),
    segments: [],
    list: false,
    traceDir: path.join(os.tmpdir(), 'nundertale_playtest'),
    timeout: 120,
    verbose: false,
  };
  for (let i = 0; i < argv.length; i += 1) {
    const arg = argv[i];
    const value = () => {
      if (i + 1 >= argv.length) usage(`${arg} needs a value`);
      i += 1;
      return argv[i];
    };
    if (arg === '--spec') options.spec = path.resolve(value());
    else if (arg === '--segment') options.segments.push(value());
    else if (arg === '--trace-dir') options.traceDir = path.resolve(value());
    else if (arg === '--timeout') options.timeout = Number(value());
    else if (arg === '--list') options.list = true;
    else if (arg === '--verbose') options.verbose = true;
    else if (arg.startsWith('--')) usage(`unknown option ${arg}`);
    else if (options.exe === null) options.exe = path.resolve(arg);
    else usage(`unexpected argument ${arg}`);
  }
  if (!Number.isFinite(options.timeout) || options.timeout <= 0) usage('--timeout must be a positive number');
  return options;
}

// "t=12 battle_start encounter=barky" -> { t: 12, event: 'battle_start', encounter: 'barky' }
function parseTraceLine(line) {
  const match = /^t=(\d+) (\S+)(?: (.*))?$/.exec(line);
  if (!match) return null;
  const entry = { t: Number(match[1]), event: match[2], line };
  const fields = match[3] || '';
  // Values never contain spaces except nothing we emit; split on " key=" boundaries to be safe.
  const pattern = /(\w+)=(.*?)(?= \w+=|$)/g;
  let field;
  while ((field = pattern.exec(fields)) !== null) entry[field[1]] = field[2];
  return entry;
}

function readTrace(file) {
  if (!fs.existsSync(file)) return [];
  return fs.readFileSync(file, 'utf8').split(/\r?\n/).filter(Boolean).map(parseTraceLine).filter(Boolean);
}

// An expectation `{ "event": "cutscene_start", "id": "r5_dummy" }` matches a trace entry whose event and every
// other listed field are equal (compared as strings). `id` of a cutscene may be several ids joined by '|' when
// two cutscene files are identical; any of them matches.
function matches(expectation, entry) {
  for (const [key, expected] of Object.entries(expectation)) {
    if (key === 'comment' || key === 'within') continue;
    const actual = entry[key];
    if (actual === undefined) return false;
    if (key === 'id' && entry.event.startsWith('cutscene_') && String(actual).split('|').includes(String(expected))) continue;
    if (String(actual) !== String(expected)) return false;
  }
  return true;
}

function describe(expectation) {
  const { comment, within, ...rest } = expectation;
  return Object.entries(rest).map(([k, v]) => (k === 'event' ? v : `${k}=${v}`)).join(' ');
}

// Every `expect` entry must appear, in order (each after the previous match). `forbid` entries must not appear
// anywhere. `within` (ticks after the previous match) is optional.
function check(segment, trace) {
  const problems = [];
  let cursor = 0;
  let previousTick = 0;
  for (const expectation of segment.expect || []) {
    let found = -1;
    for (let i = cursor; i < trace.length; i += 1) {
      if (matches(expectation, trace[i])) { found = i; break; }
    }
    if (found < 0) {
      const anywhere = trace.findIndex((entry) => matches(expectation, entry));
      problems.push(anywhere >= 0
        ? `out of order: ${describe(expectation)} happened (t=${trace[anywhere].t}) but before an earlier expectation`
        : `missing: ${describe(expectation)}`);
      continue;
    }
    if (expectation.within !== undefined && trace[found].t - previousTick > expectation.within) {
      problems.push(`too late: ${describe(expectation)} at t=${trace[found].t}, wanted within ${expectation.within} ticks of t=${previousTick}`);
    }
    previousTick = trace[found].t;
    cursor = found + 1;
  }
  for (const forbidden of segment.forbid || []) {
    const hit = trace.find((entry) => matches(forbidden, entry));
    if (hit) problems.push(`forbidden: ${describe(forbidden)} at t=${hit.t}`);
  }
  return problems;
}

function runSegment(segment, options) {
  const traceFile = path.join(options.traceDir, `${segment.name}.trace`);
  fs.rmSync(traceFile, { force: true }); // the game appends
  const args = [];
  if (segment.room) args.push('--room', segment.room);
  if (segment.spawn) args.push('--spawn', segment.spawn);
  for (const [key, value] of Object.entries(segment.flags || {})) args.push('--flag', `${key}=${value}`);
  if (segment.input) args.push('--input', path.resolve(PLAYTEST_DIR, 'ruins', segment.input));
  args.push('--seconds', String(segment.seconds));
  args.push('--trace', traceFile);

  if (options.verbose) console.log(`  $ ${options.exe} ${args.join(' ')}`);
  const started = Date.now();
  const result = spawnSync(options.exe, args, {
    cwd: REPO_ROOT,
    encoding: 'utf8',
    timeout: options.timeout * 1000,
    windowsHide: true,
  });
  const seconds = ((Date.now() - started) / 1000).toFixed(1);

  const problems = [];
  const trace = readTrace(traceFile);
  if (result.error) problems.push(`could not run the game: ${result.error.message}`);
  else if (result.status !== 0) problems.push(`game exited with ${result.status === null ? `signal ${result.signal}` : `code ${result.status}`}`);
  if (trace.length === 0) problems.push('no trace written (is the exe a -DNUNDERTALE_DEBUG build?)');
  else if (!trace.some((entry) => entry.event === 'end')) problems.push('trace has no end event (the game crashed or was killed)');
  if (trace.length > 0) problems.push(...check(segment, trace));
  return { problems, trace, traceFile, seconds, stderr: result.stderr || '' };
}

function main() {
  const options = parseArgs(process.argv.slice(2));
  const spec = JSON.parse(fs.readFileSync(options.spec, 'utf8'));
  const all = spec.segments || [];
  const names = new Set();
  for (const segment of all) {
    if (!segment.name || names.has(segment.name)) usage(`segment names must be unique and present (${segment.name})`);
    names.add(segment.name);
    segment.seconds = segment.seconds || (spec.defaults && spec.defaults.seconds) || 30;
  }

  if (options.list) {
    for (const segment of all) console.log(`${segment.name}  — ${segment.description || ''}`);
    return 0;
  }
  if (options.exe === null) usage('missing <exe>');
  if (!fs.existsSync(options.exe)) usage(`exe not found: ${options.exe}`);

  for (const wanted of options.segments) {
    if (!names.has(wanted)) usage(`unknown segment ${wanted} (see --list)`);
  }
  const selected = options.segments.length > 0 ? all.filter((s) => options.segments.includes(s.name)) : all;
  fs.mkdirSync(options.traceDir, { recursive: true });

  let failed = 0;
  for (const segment of selected) {
    const outcome = runSegment(segment, options);
    const ok = outcome.problems.length === 0;
    if (!ok) failed += 1;
    console.log(`${ok ? 'PASS' : 'FAIL'}  ${segment.name}  (${outcome.seconds}s, ${outcome.trace.length} trace lines)`);
    for (const problem of outcome.problems) console.log(`      - ${problem}`);
    if (!ok || options.verbose) {
      console.log(`      trace: ${outcome.traceFile}`);
      const interesting = outcome.trace.filter((e) => e.event !== 'flag' || options.verbose).slice(-12);
      for (const entry of interesting) console.log(`        ${entry.line}`);
      if (segment.note) console.log(`      note: ${segment.note}`);
      if (outcome.stderr && !outcome.trace.some((e) => e.event === 'end')) {
        console.log(`      stderr: ${outcome.stderr.trim().split(/\r?\n/).slice(-5).join(' | ')}`);
      }
    }
  }
  console.log(`${selected.length - failed}/${selected.length} segments passed`);
  return failed === 0 ? 0 : 1;
}

process.exit(main());
