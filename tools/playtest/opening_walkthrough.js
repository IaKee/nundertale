#!/usr/bin/env node
// Scripted walkthrough of the new opening, from a NEW GAME: title -> name entry -> R1 The Landing (Grub lying) ->
// R1a Registration -> R1b Panel Hall (save) -> R2 Headless Lounge (Floofy, polite and annoyed fight) -> Furiell's
// spray -> R3, plus a death -> WELCOME HOME -> retry path (C1-030).
//
//   node tools/playtest/opening_walkthrough.js <exe> [--spec file] [--segment name]... [--list]
//                                              [--trace-dir dir] [--timeout seconds] [--keep-frames] [--verbose]
//
// <exe> must be a -DNUNDERTALE_DEBUG build (docs/playtest.md). Builds nothing. Run from the repo root: the game
// reads data/ and assets/ relative to its working directory.
//
// The game is started with NO debug launch flags: only --input, --trace and (for the frame check) --record, so it
// begins at the WARNING screen like a player's first run. Every segment gets its own empty LOCALAPPDATA folder
// (under the trace dir), so there is no save to continue and your real save is never touched.
//
// Exit code 0 only if every selected segment meets every expectation and every static check holds.
// Spec format: tools/playtest/opening/segments.json (see tools/playtest/opening/README.md).

'use strict';

const fs = require('fs');
const os = require('os');
const path = require('path');
const zlib = require('zlib');
const { spawnSync } = require('child_process');

const PLAYTEST_DIR = __dirname;
const REPO_ROOT = path.resolve(PLAYTEST_DIR, '..', '..');
const OPENING_DIR = path.join(PLAYTEST_DIR, 'opening');

function usage(message) {
  if (message) console.error(`error: ${message}`);
  console.error('usage: node tools/playtest/opening_walkthrough.js <exe> [--spec file] [--segment name]... [--list]');
  console.error('                                                 [--trace-dir dir] [--timeout seconds] [--keep-frames] [--verbose]');
  process.exit(2);
}

function parseArgs(argv) {
  const options = {
    exe: null,
    spec: path.join(OPENING_DIR, 'segments.json'),
    segments: [],
    list: false,
    traceDir: path.join(os.tmpdir(), 'nundertale_opening'),
    timeout: 240,
    keepFrames: false,
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
    else if (arg === '--keep-frames') options.keepFrames = true;
    else if (arg === '--verbose') options.verbose = true;
    else if (arg.startsWith('--')) usage(`unknown option ${arg}`);
    else if (options.exe === null) options.exe = path.resolve(arg);
    else usage(`unexpected argument ${arg}`);
  }
  if (!Number.isFinite(options.timeout) || options.timeout <= 0) usage('--timeout must be a positive number');
  return options;
}

// ---------------------------------------------------------------------------------------------------------------
// Trace
// ---------------------------------------------------------------------------------------------------------------

// "t=12 battle_start encounter=barky" -> { t: 12, event: 'battle_start', encounter: 'barky' }
function parseTraceLine(line) {
  const match = /^t=(\d+) (\S+)(?: (.*))?$/.exec(line);
  if (!match) return null;
  const entry = { t: Number(match[1]), event: match[2], line };
  const pattern = /(\w+)=(.*?)(?= \w+=|$)/g;
  let field;
  while ((field = pattern.exec(match[3] || '')) !== null) entry[field[1]] = field[2];
  return entry;
}

function readTrace(file) {
  if (!fs.existsSync(file)) return [];
  return fs.readFileSync(file, 'utf8').split(/\r?\n/).filter(Boolean).map(parseTraceLine).filter(Boolean);
}

const NON_FIELDS = new Set(['comment', 'within', 'equals', 'min', 'max', 'after', 'plus', 'expect']);

// `{ "event": "flag", "name": "ruins.floofy.fight", "value": 2 }` matches a trace entry with the same event and every
// other listed field equal (as strings). The `id` of a cutscene may be several ids joined by '|' (two cutscene files
// with identical steps share a fingerprint); any of them matches.
function matches(expectation, entry) {
  for (const [key, expected] of Object.entries(expectation)) {
    if (NON_FIELDS.has(key)) continue;
    const actual = entry[key];
    if (actual === undefined) return false;
    if (key === 'id' && entry.event.startsWith('cutscene_') && String(actual).split('|').includes(String(expected))) continue;
    if (String(actual) !== String(expected)) return false;
  }
  return true;
}

function describe(expectation) {
  return Object.entries(expectation)
    .filter(([k]) => !NON_FIELDS.has(k))
    .map(([k, v]) => (k === 'event' ? v : `${k}=${v}`))
    .join(' ');
}

// Every `expect` entry must appear, in order (each after the previous match; the same tick counts). `within` is the
// optional maximum number of ticks since the previous match.
function checkOrder(segment, trace) {
  const problems = [];
  let cursor = 0;
  let previousTick = 0;
  for (const expectation of segment.expect || []) {
    let found = -1;
    for (let i = cursor; i < trace.length; i += 1) {
      if (matches(expectation, trace[i])) { found = i; break; }
    }
    if (found < 0) {
      const anywhere = trace.find((entry) => matches(expectation, entry));
      problems.push(anywhere
        ? `out of order: ${describe(expectation)} happened (t=${anywhere.t}) but before an earlier expectation`
        : `missing: ${describe(expectation)}`);
      continue;
    }
    if (expectation.within !== undefined && trace[found].t - previousTick > expectation.within) {
      problems.push(`too late: ${describe(expectation)} at t=${trace[found].t}, wanted within ${expectation.within} ticks of t=${previousTick}`);
    }
    previousTick = trace[found].t;
    cursor = found + 1;
  }
  return problems;
}

// `counts`: how many trace lines match (`equals`, or `min` / `max`).
function checkCounts(segment, trace) {
  const problems = [];
  for (const rule of segment.counts || []) {
    const n = trace.filter((entry) => matches(rule, entry)).length;
    const bad = (rule.equals !== undefined && n !== rule.equals)
      || (rule.min !== undefined && n < rule.min)
      || (rule.max !== undefined && n > rule.max);
    if (bad) {
      const want = rule.equals !== undefined ? `exactly ${rule.equals}` : `${rule.min !== undefined ? `at least ${rule.min}` : ''}${rule.max !== undefined ? ` at most ${rule.max}` : ''}`.trim();
      problems.push(`count: ${describe(rule)} happened ${n} times, wanted ${want}`);
    }
  }
  return problems;
}

function checkForbid(segment, trace) {
  const problems = [];
  for (const forbidden of segment.forbid || []) {
    const hit = trace.find((entry) => matches(forbidden, entry));
    if (hit) problems.push(`forbidden: ${describe(forbidden)} at t=${hit.t}`);
  }
  return problems;
}

// ---------------------------------------------------------------------------------------------------------------
// Frames (the "Grub is lying" check): a minimal PNG reader, 8-bit RGBA / RGB, no interlace
// ---------------------------------------------------------------------------------------------------------------

function readPng(file) {
  const bytes = fs.readFileSync(file);
  let offset = 8;
  let width = 0;
  let height = 0;
  let colorType = 0;
  const idat = [];
  while (offset < bytes.length) {
    const length = bytes.readUInt32BE(offset);
    const type = bytes.toString('latin1', offset + 4, offset + 8);
    if (type === 'IHDR') {
      width = bytes.readUInt32BE(offset + 8);
      height = bytes.readUInt32BE(offset + 12);
      if (bytes[offset + 16] !== 8 || bytes[offset + 20] !== 0) throw new Error(`${file}: only 8-bit non-interlaced PNGs`);
      colorType = bytes[offset + 17];
    } else if (type === 'IDAT') {
      idat.push(bytes.subarray(offset + 8, offset + 8 + length));
    }
    offset += 12 + length;
  }
  if (colorType !== 6 && colorType !== 2) throw new Error(`${file}: PNG color type ${colorType} is not supported`);
  const channels = colorType === 6 ? 4 : 3;
  const raw = zlib.inflateSync(Buffer.concat(idat));
  const stride = width * channels;
  const pixels = Buffer.alloc(height * stride);
  for (let y = 0; y < height; y += 1) {
    const filter = raw[y * (stride + 1)];
    for (let x = 0; x < stride; x += 1) {
      const value = raw[y * (stride + 1) + 1 + x];
      const left = x >= channels ? pixels[y * stride + x - channels] : 0;
      const up = y > 0 ? pixels[(y - 1) * stride + x] : 0;
      const upLeft = x >= channels && y > 0 ? pixels[(y - 1) * stride + x - channels] : 0;
      let result;
      if (filter === 0) result = value;
      else if (filter === 1) result = value + left;
      else if (filter === 2) result = value + up;
      else if (filter === 3) result = value + ((left + up) >> 1);
      else {
        const p = left + up - upLeft;
        const pa = Math.abs(p - left);
        const pb = Math.abs(p - up);
        const pc = Math.abs(p - upLeft);
        result = value + ((pa <= pb && pa <= pc) ? left : (pb <= pc ? up : upLeft));
      }
      pixels[y * stride + x] = result & 255;
    }
  }
  return { width, height, channels, pixels };
}

function pixelAt(image, x, y) {
  const i = (y * image.width + x) * image.channels;
  return [image.pixels[i], image.pixels[i + 1], image.pixels[i + 2], image.channels === 4 ? image.pixels[i + 3] : 255];
}

// The opaque pixels of one sprite-sheet cell.
function cellPixels(sheet, cellW, cellH, col, row) {
  const list = [];
  for (let y = 0; y < cellH; y += 1) {
    for (let x = 0; x < cellW; x += 1) {
      const [r, g, b, a] = pixelAt(sheet, col * cellW + x, row * cellH + y);
      if (a > 200) list.push([x, y, r, g, b]);
    }
  }
  return list;
}

// Best fraction of the cell's opaque pixels (drawn `scale` times bigger, nearest neighbour) that match the frame
// exactly, over the top-left positions in `window` = [x0, y0, x1, y1]. Sampled every third pixel for speed.
function bestMatch(frame, pixels, cellW, cellH, scale, window) {
  const x0 = Math.max(0, window[0]);
  const y0 = Math.max(0, window[1]);
  const x1 = Math.min(frame.width - cellW * scale, window[2]);
  const y1 = Math.min(frame.height - cellH * scale, window[3]);
  let best = 0;
  for (let oy = y0; oy <= y1; oy += 1) {
    for (let ox = x0; ox <= x1; ox += 1) {
      let hits = 0;
      let total = 0;
      for (let k = 0; k < pixels.length; k += 3) {
        const p = pixels[k];
        const [r, g, b] = pixelAt(frame, ox + p[0] * scale, oy + p[1] * scale);
        total += 1;
        if (Math.abs(r - p[2]) + Math.abs(g - p[3]) + Math.abs(b - p[4]) < 12) hits += 1;
      }
      if (total > 0 && hits / total > best) best = hits / total;
    }
  }
  return best;
}

function spawnOf(mapId, spawnName) {
  const map = JSON.parse(fs.readFileSync(path.join(REPO_ROOT, 'data', 'maps', `${mapId}.json`), 'utf8'));
  for (const layer of map.layers) {
    if (layer.type !== 'objectgroup' || layer.name !== 'objects') continue;
    for (const object of layer.objects) {
      if (object.type === 'spawn' && object.name === spawnName) {
        const properties = {};
        for (const property of object.properties || []) properties[property.name] = property.value;
        return { x: object.x, y: object.y, properties };
      }
    }
  }
  return null;
}

// `pose`: at the listed trace moments, the frame the game recorded must (or must not) show Grub's `anim` cell of the
// spawn's pose sheet. "lying" = the lying cell matches near the spawn; "up" = it does not (Grub stands again).
function checkPose(segment, trace, framesDir, verbose) {
  const problems = [];
  const pose = segment.pose;
  const spawn = spawnOf(pose.map, pose.spawn);
  if (!spawn) return [`pose: spawn ${pose.map}/${pose.spawn} not found`];
  const sheetPng = path.join(REPO_ROOT, spawn.properties.poseSheet || '');
  const sheetJson = JSON.parse(fs.readFileSync(sheetPng.replace(/\.png$/, '.json'), 'utf8'));
  const [cellW, cellH] = sheetJson.cell;
  const anim = sheetJson.anims[spawn.properties.pose];
  const sheet = readPng(sheetPng);
  const pixels = cellPixels(sheet, cellW, cellH, 0, anim.row);
  const scale = pose.scale || 2;
  const window = [spawn.x - 100, spawn.y - 100, spawn.x + 60, spawn.y + 60];
  for (const sample of pose.frames) {
    const anchor = trace.find((entry) => matches(sample.after, entry));
    const label = `${describe(sample.after)} +${sample.plus}`;
    if (!anchor) { problems.push(`pose: no trace line for ${label}`); continue; }
    const frameName = `frame_${String(anchor.t - 1 + sample.plus).padStart(5, '0')}.png`;
    const frameFile = path.join(framesDir, frameName);
    if (!fs.existsSync(frameFile)) { problems.push(`pose: ${frameName} (${label}) was not recorded`); continue; }
    const score = bestMatch(readPng(frameFile), pixels, cellW, cellH, scale, window);
    if (verbose) console.log(`      pose ${label}: ${frameName} lying-match ${score.toFixed(3)} (want ${sample.expect === 'lying' ? `>= ${pose.lyingMin}` : `<= ${pose.upMax}`})`);
    if (sample.expect === 'lying' && score < pose.lyingMin) {
      problems.push(`pose: Grub is not lying at ${label} (${frameName}: match ${score.toFixed(2)}, wanted >= ${pose.lyingMin})`);
    } else if (sample.expect === 'up' && score > pose.upMax) {
      problems.push(`pose: Grub is still lying at ${label} (${frameName}: match ${score.toFixed(2)}, wanted <= ${pose.upMax})`);
    }
  }
  return problems;
}

// ---------------------------------------------------------------------------------------------------------------
// Static checks on the shipped data (things the trace cannot see)
// ---------------------------------------------------------------------------------------------------------------

function readJson(relative) {
  return JSON.parse(fs.readFileSync(path.join(REPO_ROOT, relative), 'utf8'));
}

function runStaticCheck(check) {
  const problems = [];
  if (check.kind === 'spawnPose') {
    const spawn = spawnOf(check.map, check.spawn);
    if (!spawn) return [`${check.map}/${check.spawn}: spawn not found`];
    const p = spawn.properties;
    if (p.pose !== check.pose) problems.push(`${check.map}/${check.spawn}: pose is "${p.pose}", wanted "${check.pose}" (Grub must start lying)`);
    if (!p.poseSheet || !fs.existsSync(path.join(REPO_ROOT, p.poseSheet))) problems.push(`${check.map}/${check.spawn}: poseSheet "${p.poseSheet}" does not exist`);
    else {
      const sheet = readJson(p.poseSheet.replace(/\.png$/, '.json'));
      if (!sheet.anims || !sheet.anims[check.pose]) problems.push(`${p.poseSheet}: no "${check.pose}" animation`);
    }
    if (check.untilFlag && p.poseUntil !== check.untilFlag) problems.push(`${check.map}/${check.spawn}: poseUntil is "${p.poseUntil}", wanted "${check.untilFlag}"`);
  } else if (check.kind === 'cutsceneSteps') {
    const cutscene = readJson(`data/cutscenes/${check.cutscene}.json`);
    for (const need of check.require) {
      const n = cutscene.steps.filter((step) => {
        if (need.type && step.type !== need.type) return false;
        if (need.path && step.path !== need.path) return false;
        if (need.npc && step.npc !== need.npc) return false;
        if (need.text) {
          const text = (step.lines || []).join('\n');
          if (!text.includes(need.text)) return false;
        }
        return true;
      }).length;
      if (n < (need.min || 1)) problems.push(`${check.cutscene}: wanted at least ${need.min || 1} step(s) ${JSON.stringify(need)}, found ${n}`);
    }
  } else {
    problems.push(`unknown static check kind "${check.kind}"`);
  }
  return problems;
}

// ---------------------------------------------------------------------------------------------------------------
// Running
// ---------------------------------------------------------------------------------------------------------------

function runSegment(segment, options) {
  const segmentDir = path.join(options.traceDir, segment.name);
  fs.rmSync(segmentDir, { recursive: true, force: true });
  const appData = path.join(segmentDir, 'appdata');
  const framesDir = path.join(segmentDir, 'frames');
  fs.mkdirSync(appData, { recursive: true });
  const traceFile = path.join(segmentDir, 'run.trace');

  // Only these flags: a player's first run plus the harness hooks.
  const args = ['--input', path.resolve(PLAYTEST_DIR, segment.input), '--seconds', String(segment.seconds), '--trace', traceFile];
  if (segment.pose) args.push('--record', framesDir);

  if (options.verbose) console.log(`  $ LOCALAPPDATA=${appData} ${options.exe} ${args.join(' ')}`);
  const started = Date.now();
  const result = spawnSync(options.exe, args, {
    cwd: REPO_ROOT,
    env: { ...process.env, LOCALAPPDATA: appData },
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
  if (trace.length > 0) {
    // A new game really starts at the WARNING screen, with no debug launch.
    const begin = trace.find((entry) => entry.event === 'begin');
    if (begin && (begin.room || begin.spawn)) problems.push(`the run did not start from a new game (begin ${begin.line})`);
    problems.push(...checkOrder(segment, trace), ...checkCounts(segment, trace), ...checkForbid(segment, trace));
  }
  for (const file of segment.files || []) {
    const target = path.join(appData, file.path);
    if (!fs.existsSync(target)) { problems.push(`file: ${file.path} was not written in the scratch LOCALAPPDATA`); continue; }
    if (file.contains && !fs.readFileSync(target, 'utf8').includes(file.contains)) problems.push(`file: ${file.path} does not contain ${JSON.stringify(file.contains)}`);
  }
  if (segment.pose && trace.length > 0) {
    try {
      problems.push(...checkPose(segment, trace, framesDir, options.verbose));
    } catch (error) {
      problems.push(`pose check failed: ${error.message}`);
    }
  }
  if (segment.pose && !options.keepFrames) fs.rmSync(framesDir, { recursive: true, force: true });
  return { problems, trace, traceFile, seconds, stderr: result.stderr || '' };
}

function main() {
  const options = parseArgs(process.argv.slice(2));
  const spec = JSON.parse(fs.readFileSync(options.spec, 'utf8'));
  const all = spec.segments || [];
  const names = new Set();
  for (const segment of all) {
    if (!segment.name || names.has(segment.name)) usage(`segment names must be unique and present (${segment.name})`);
    if (!segment.input) usage(`segment ${segment.name} has no input`);
    names.add(segment.name);
    segment.seconds = segment.seconds || (spec.defaults && spec.defaults.seconds) || 200;
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
  const staticProblems = [];
  for (const check of spec.static || []) {
    for (const problem of runStaticCheck(check)) staticProblems.push(`${check.comment || check.kind}: ${problem}`);
  }
  console.log(`${staticProblems.length === 0 ? 'PASS' : 'FAIL'}  static data checks (${(spec.static || []).length})`);
  for (const problem of staticProblems) console.log(`      - ${problem}`);
  if (staticProblems.length > 0) failed += 1;

  for (const segment of selected) {
    const outcome = runSegment(segment, options);
    const ok = outcome.problems.length === 0;
    if (!ok) failed += 1;
    console.log(`${ok ? 'PASS' : 'FAIL'}  ${segment.name}  (${outcome.seconds}s, ${outcome.trace.length} trace lines)`);
    for (const problem of outcome.problems) console.log(`      - ${problem}`);
    if (!ok || options.verbose) {
      console.log(`      trace: ${outcome.traceFile}`);
      const interesting = outcome.trace.filter((e) => e.event !== 'flag' && e.event !== 'battle_state' && e.event !== 'scene_push' && e.event !== 'scene_pop').slice(-12);
      for (const entry of interesting) console.log(`        ${entry.line}`);
      if (segment.note) console.log(`      note: ${segment.note}`);
      if (outcome.stderr && !outcome.trace.some((e) => e.event === 'end')) {
        console.log(`      stderr: ${outcome.stderr.trim().split(/\r?\n/).slice(-5).join(' | ')}`);
      }
    }
  }
  console.log(`${selected.length + 1 - failed}/${selected.length + 1} checks passed (static data checks + ${selected.length} segments)`);
  return failed === 0 ? 0 : 1;
}

process.exit(main());
