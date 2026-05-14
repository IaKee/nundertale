// Generates original placeholder SFX (synthesized, no third-party audio) as 16-bit mono WAV.
// usage: node tools/gen_placeholder_sfx.js            (writes into assets/sfx/)
// Replace any of these with final sounds by overwriting the file (same name).
const fs = require('fs');
const path = require('path');

const RATE = 44100;
const OUT = path.join(__dirname, '..', 'assets', 'sfx');

const square = (phase) => (phase % 1 < 0.5 ? 1 : -1);
const triangle = (phase) => 1 - 4 * Math.abs((phase % 1) - 0.5);

// notes: [{ freq, freqEnd?, start, dur, wave, gain }] ; times in seconds
function render(notes, totalSeconds) {
  const n = Math.ceil(totalSeconds * RATE);
  const buf = new Float32Array(n);
  for (const note of notes) {
    const startI = Math.floor(note.start * RATE);
    const len = Math.floor(note.dur * RATE);
    let phase = 0;
    for (let i = 0; i < len && startI + i < n; i++) {
      const t = i / len;
      const freq = note.freq + ((note.freqEnd ?? note.freq) - note.freq) * t;
      phase += freq / RATE;
      const attack = Math.min(1, i / (0.003 * RATE));
      const env = attack * Math.pow(1 - t, note.decay ?? 2);
      buf[startI + i] += note.wave(phase) * env * note.gain;
    }
  }
  return buf;
}

function writeWav(file, samples) {
  const data = Buffer.alloc(samples.length * 2);
  for (let i = 0; i < samples.length; i++) {
    const v = Math.max(-1, Math.min(1, samples[i]));
    data.writeInt16LE(Math.round(v * 0x7fff * 0.7), i * 2); // about -3 dB peak
  }
  const header = Buffer.alloc(44);
  header.write('RIFF', 0);
  header.writeUInt32LE(36 + data.length, 4);
  header.write('WAVE', 8);
  header.write('fmt ', 12);
  header.writeUInt32LE(16, 16);
  header.writeUInt16LE(1, 20); // PCM
  header.writeUInt16LE(1, 22); // mono
  header.writeUInt32LE(RATE, 24);
  header.writeUInt32LE(RATE * 2, 28);
  header.writeUInt16LE(2, 32);
  header.writeUInt16LE(16, 34);
  header.write('data', 36);
  header.writeUInt32LE(data.length, 40);
  fs.writeFileSync(file, Buffer.concat([header, data]));
  console.log('wrote', path.relative(process.cwd(), file), (samples.length / RATE).toFixed(2) + 's');
}

const sounds = {
  // AUD-003: a short high squeak when the cursor moves
  menu_move: render([{ freq: 1250, freqEnd: 1500, start: 0, dur: 0.035, wave: square, gain: 0.35, decay: 1.5 }], 0.04),
  // AUD-003: confirm, two quick rising tones
  menu_select: render([
    { freq: 660, start: 0, dur: 0.05, wave: square, gain: 0.35 },
    { freq: 990, start: 0.045, dur: 0.08, wave: square, gain: 0.35 },
  ], 0.13),
  // BUG-32: heal, a rising major arpeggio
  heal: render([
    { freq: 523.25, start: 0.0, dur: 0.12, wave: triangle, gain: 0.6, decay: 1.2 },
    { freq: 659.25, start: 0.07, dur: 0.12, wave: triangle, gain: 0.6, decay: 1.2 },
    { freq: 783.99, start: 0.14, dur: 0.12, wave: triangle, gain: 0.6, decay: 1.2 },
    { freq: 1046.5, start: 0.21, dur: 0.22, wave: triangle, gain: 0.6, decay: 1.5 },
  ], 0.45),
};

fs.mkdirSync(OUT, { recursive: true });
for (const [name, samples] of Object.entries(sounds)) {
  writeWav(path.join(OUT, name + '.wav'), samples);
}
