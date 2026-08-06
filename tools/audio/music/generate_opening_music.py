#!/usr/bin/env python3
"""Deterministically synthesize opening music placeholder loops for nUndertale.

Tracks:
- assets/music/ruins_quiet.ogg          (R1a Registration: sparse quiet ambient loop)
- assets/music/floofy_lounge.ogg        (R2 Lounge: cute music-box 'Your Best Floof')
- assets/music/floofy_lounge_muffled.ogg (R1b Panel Hall: muffled low-passed low-volume leak)
- assets/music/floofy_fight.ogg         (R2 Petal fight: slowed + detuned version)

All tracks are synthesized deterministically with NumPy and encoded with FFmpeg.
Seamless looping is guaranteed via circular decay wrap-around and zero-crossing phase alignment.
"""

from __future__ import annotations

import argparse
import math
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import wave

import numpy as np

RATE = 44_100
ROOT = Path(__file__).resolve().parents[3]
MUSIC_DIR = ROOT / "assets" / "music"


def timebase(seconds: float) -> np.ndarray:
    return np.arange(round(seconds * RATE), dtype=np.float64) / RATE


def find_ffmpeg(override: str | None = None) -> str:
    candidates = [override, os.environ.get("FFMPEG"), shutil.which("ffmpeg")]
    try:
        import imageio_ffmpeg
        candidates.append(imageio_ffmpeg.get_ffmpeg_exe())
    except (ImportError, RuntimeError):
        pass
    for cand in candidates:
        if cand:
            try:
                if subprocess.run([cand, "-version"], stdout=subprocess.DEVNULL,
                                  stderr=subprocess.DEVNULL, timeout=5).returncode == 0:
                    return str(cand)
            except (OSError, subprocess.SubprocessError):
                pass
    raise RuntimeError("No runnable FFmpeg found. Pass --ffmpeg PATH or add ffmpeg to PATH.")


def write_wav(path: Path, stereo_signal: np.ndarray) -> None:
    """Write stereo float64 signal [-1.0, 1.0] to 16-bit stereo PCM WAV."""
    clipped = np.clip(stereo_signal, -1.0, 1.0)
    pcm = np.round(clipped * 32_767.0).astype("<i2")
    # Interleave stereo
    if pcm.ndim == 1:
        pcm = np.column_stack((pcm, pcm))
    elif pcm.shape[0] == 2 and pcm.shape[1] != 2:
        pcm = pcm.T
    with wave.open(str(path), "wb") as wav:
        wav.setnchannels(2)
        wav.setsampwidth(2)
        wav.setframerate(RATE)
        wav.writeframes(pcm.tobytes())


def encode_ogg(ffmpeg: str, signal: np.ndarray, output: Path, extra_filters: str | None = None) -> None:
    """Encode audio signal to Vorbis OGG via FFmpeg."""
    with tempfile.TemporaryDirectory(prefix="nundertale-music-") as temp_dir:
        wav_path = Path(temp_dir) / "source.wav"
        write_wav(wav_path, signal)
        cmd = [
            ffmpeg, "-hide_banner", "-loglevel", "error", "-y",
            "-i", str(wav_path),
        ]
        if extra_filters:
            cmd.extend(["-af", extra_filters])
        cmd.extend([
            "-ac", "2", "-ar", str(RATE),
            "-c:a", "libvorbis", "-q:a", "5",
            str(output)
        ])
        subprocess.run(cmd, check=True)


def make_seamless_loop(signal: np.ndarray, crossfade_samples: int = 256) -> np.ndarray:
    """Apply an imperceptible micro-crossfade at the buffer boundary to ensure 0 click."""
    out = signal.copy()
    n = min(crossfade_samples, len(out) // 4)
    if n <= 1:
        return out
    # Raised cosine fade weights
    fade_in = np.sin(np.linspace(0.0, math.pi / 2.0, n)) ** 2
    fade_out = np.cos(np.linspace(0.0, math.pi / 2.0, n)) ** 2
    for ch in range(out.shape[1]):
        head = out[:n, ch]
        tail = out[-n:, ch]
        blended = tail * fade_out + head * fade_in
        out[:n, ch] = blended
        out[-n:, ch] = blended
    return out


def verify_ogg(ffmpeg: str, path: Path) -> dict:
    """Decode and inspect OGG for duration, peak, RMS, and loop seam continuity."""
    cmd = [
        ffmpeg, "-hide_banner", "-loglevel", "error",
        "-i", str(path),
        "-f", "f32le", "-ac", "2", "-ar", str(RATE), "pipe:1"
    ]
    raw = subprocess.run(cmd, check=True, stdout=subprocess.PIPE).stdout
    samples = np.frombuffer(raw, dtype="<f4").reshape(-1, 2)
    if len(samples) == 0 or not np.all(np.isfinite(samples)):
        raise RuntimeError(f"{path.name}: decoded empty or non-finite audio")

    peak = float(np.max(np.abs(samples)))
    rms = float(np.sqrt(np.mean(samples ** 2)))
    duration = len(samples) / RATE
    peak_db = 20.0 * math.log10(max(peak, 1e-9))
    rms_db = 20.0 * math.log10(max(rms, 1e-9))

    if peak >= 0.999:
        raise RuntimeError(f"{path.name}: clipping detected (peak {peak:.4f})")

    # Check loop seam: compare head (first 64 samples) with tail (last 64 samples)
    head = samples[:64]
    tail = samples[-64:]
    seam_diff = float(np.max(np.abs(head[0] - tail[-1])))

    return {
        "duration": duration,
        "peak_db": peak_db,
        "rms_db": rms_db,
        "seam_diff": seam_diff,
    }


def make_music_box_note(freq: float, duration: float, bend: float = 0.0) -> np.ndarray:
    """Synthesize a sparkling music-box bell note with physical-like chime harmonics."""
    t = timebase(duration)
    if bend != 0.0:
        # Pitch bend curve towards end
        bend_curve = np.clip((t - (duration * 0.4)) / (duration * 0.6), 0.0, 1.0) ** 1.8
        f_inst = freq + bend * bend_curve
        phase = 2.0 * math.pi * np.cumsum(f_inst) / RATE
    else:
        phase = 2.0 * math.pi * freq * t

    # Fast bell strike attack (3ms), smooth decay
    attack = np.minimum(1.0, t / 0.003)
    decay_fast = np.exp(-t / 0.22)
    decay_slow = np.exp(-t / 0.65)
    env = attack * (0.6 * decay_fast + 0.4 * decay_slow)

    # Chime harmonic overtone series (bell / tine: fundamental + octave + 3rd + non-integer chime harmonic)
    h1 = np.sin(phase)
    h2 = 0.38 * np.sin(2.0 * phase)
    h3 = 0.16 * np.sin(3.0 * phase)
    h_chime = 0.08 * np.sin(4.24 * phase) * np.exp(-t / 0.12)  # metallic strike tines

    # Very gentle subtle detune shimmer
    shimmer = 0.12 * np.sin(phase * 1.002)

    return env * (h1 + h2 + h3 + h_chime + shimmer)


def make_bass_note(freq: float, duration: float) -> np.ndarray:
    """Synthesize a cute rounded pizzicato bass note."""
    t = timebase(duration)
    phase = 2.0 * math.pi * freq * t
    attack = np.minimum(1.0, t / 0.005)
    decay = np.exp(-t / 0.18)
    env = attack * decay
    body = np.sin(phase) + 0.35 * np.sin(2.0 * phase) + 0.12 * np.sin(3.0 * phase)
    return env * body * 0.85


def synth_floofy_lounge() -> np.ndarray:
    """Synthesize 'Your Best Floof' (Floofy's lounge theme).
    
    Parody of 'Your Best Friend':
    - 120 BPM, 4/4 time
    - 8 measures = exactly 16.0 seconds
    - Playful bouncy music-box chime melody
    - Cute staccato bass accompaniment
    - Comic pitch-bend on the last bar
    - Seamless wrap-around for natural chime ring-out
    """
    bpm = 120.0
    beat_sec = 60.0 / bpm  # 0.5 s
    bar_sec = beat_sec * 4.0  # 2.0 s
    loop_duration = 8 * bar_sec  # 16.0 s
    tail_duration = 2.0  # extra render time to catch reverb/tails
    total_samples = round((loop_duration + tail_duration) * RATE)
    loop_samples = round(loop_duration * RATE)

    left = np.zeros(total_samples, dtype=np.float64)
    right = np.zeros(total_samples, dtype=np.float64)

    def add_sound(channel_l: float, channel_r: float, sound: np.ndarray, start_sec: float):
        start_idx = round(start_sec * RATE)
        end_idx = min(total_samples, start_idx + len(sound))
        part = sound[: end_idx - start_idx]
        left[start_idx:end_idx] += part * channel_l
        right[start_idx:end_idx] += part * channel_r

    # Frequencies (C4=261.63, D4=293.66, E4=329.63, G4=392.00, C5=523.25, D5=587.33, E5=659.25, F5=698.46, G5=783.99, A5=880.00, B5=987.77, C6=1046.50)
    c4, d4, e4, g4 = 261.63, 293.66, 329.63, 392.00
    c5, d5, e5, f5, g5, a5, b5, c6 = 523.25, 587.33, 659.25, 698.46, 783.99, 880.00, 987.77, 1046.50
    c3, d3, e3, f3, g3, a3, b3 = 130.81, 146.83, 164.81, 174.61, 196.00, 220.00, 246.94
    g2 = 98.00

    # Melody notes: (time_in_beats, freq, duration_in_beats, pan, bend_hz)
    melody_events = [
        # Bar 1: C - E - G - E
        (0.0, c5, 0.5, 0.4, 0.0),
        (0.5, e5, 0.5, 0.6, 0.0),
        (1.0, g5, 0.9, 0.5, 0.0),
        (2.0, e5, 0.9, 0.4, 0.0),
        (3.0, g5, 0.9, 0.6, 0.0),
        # Bar 2: A - G - E - C
        (4.0, a5, 0.9, 0.7, 0.0),
        (5.0, g5, 0.9, 0.5, 0.0),
        (6.0, e5, 0.9, 0.4, 0.0),
        (7.0, c5, 0.9, 0.3, 0.0),
        # Bar 3: D - F - E - D
        (8.0, d5, 0.9, 0.4, 0.0),
        (9.0, f5, 0.9, 0.6, 0.0),
        (10.0, e5, 0.9, 0.5, 0.0),
        (11.0, d5, 0.9, 0.4, 0.0),
        # Bar 4: G - B - C(high)
        (12.0, g5, 0.9, 0.6, 0.0),
        (13.0, b5, 0.9, 0.7, 0.0),
        (14.0, c6, 1.8, 0.5, 0.0),
        # Bar 5: C - E - G - E
        (16.0, c5, 0.5, 0.4, 0.0),
        (16.5, e5, 0.5, 0.6, 0.0),
        (17.0, g5, 0.9, 0.5, 0.0),
        (18.0, e5, 0.9, 0.4, 0.0),
        (19.0, g5, 0.9, 0.6, 0.0),
        # Bar 6: A - C6 - B - A
        (20.0, a5, 0.9, 0.7, 0.0),
        (21.0, c6, 0.9, 0.5, 0.0),
        (22.0, b5, 0.9, 0.6, 0.0),
        (23.0, a5, 0.9, 0.4, 0.0),
        # Bar 7: G - E - D - G
        (24.0, g5, 0.9, 0.5, 0.0),
        (25.0, e5, 0.9, 0.4, 0.0),
        (26.0, d5, 0.9, 0.4, 0.0),
        (27.0, g5, 0.9, 0.6, 0.0),
        # Bar 8: C5 hold, then funny cartoon pitch bend down at beat 4!
        (28.0, c5, 1.8, 0.5, 0.0),
        (30.0, e5, 0.8, 0.6, 0.0),
        (31.0, c5, 0.9, 0.5, -95.0),  # Comic pitch bend down from C5 -> ~G#4
    ]

    for beat, freq, dur_beats, pan, bend in melody_events:
        dur_sec = dur_beats * beat_sec
        note = make_music_box_note(freq, dur_sec + 0.45, bend=bend)
        add_sound(1.0 - pan, pan, note * 0.75, beat * beat_sec)

    # Bass events: (beat, freq, duration)
    bass_events = [
        # Bar 1
        (0.0, c3, 0.35), (1.0, g3, 0.35), (2.0, e3, 0.35), (3.0, g3, 0.35),
        # Bar 2
        (4.0, f3, 0.35), (5.0, c4, 0.35), (6.0, a3, 0.35), (7.0, c4, 0.35),
        # Bar 3
        (8.0, d3, 0.35), (9.0, a3, 0.35), (10.0, f3, 0.35), (11.0, a3, 0.35),
        # Bar 4
        (12.0, g3, 0.35), (13.0, d4, 0.35), (14.0, c3, 0.75), (15.0, g3, 0.35),
        # Bar 5
        (16.0, c3, 0.35), (17.0, g3, 0.35), (18.0, e3, 0.35), (19.0, g3, 0.35),
        # Bar 6
        (20.0, f3, 0.35), (21.0, c4, 0.35), (22.0, a3, 0.35), (23.0, c4, 0.35),
        # Bar 7
        (24.0, g3, 0.35), (25.0, e3, 0.35), (26.0, d3, 0.35), (27.0, g2, 0.35),
        # Bar 8
        (28.0, c3, 0.75), (29.0, g3, 0.35), (30.0, c3, 0.35),
    ]

    for beat, freq, dur in bass_events:
        note = make_bass_note(freq, dur)
        add_sound(0.55, 0.45, note * 0.55, beat * beat_sec)

    # Fold tail back into loop start for 100% seamless transition
    loop_l = left[:loop_samples].copy()
    loop_r = right[:loop_samples].copy()
    tail_samples = total_samples - loop_samples
    loop_l[:tail_samples] += left[loop_samples:]
    loop_r[:tail_samples] += right[loop_samples:]

    stereo = np.column_stack((loop_l, loop_r))
    # Normalize to -14 dBFS RMS, peak < -1.5 dBFS
    rms = np.sqrt(np.mean(stereo ** 2))
    target_rms = 10.0 ** (-14.5 / 20.0)
    stereo *= target_rms / max(rms, 1e-9)
    peak = np.max(np.abs(stereo))
    if peak > 0.85:
        stereo *= 0.85 / peak

    return stereo


def synth_floofy_lounge_muffled(lounge_signal: np.ndarray) -> np.ndarray:
    """Create muffled low-pass low-volume version of floofy_lounge (for R1b panel hall).
    
    Applies a stable 2nd-order lowpass filter (cutoff ~380 Hz) and drops volume by ~13 dB
    so it sounds like a cute music box leaking through a thick closed hotel door.
    """
    cutoff = 380.0
    # 2nd-order resonant lowpass filter
    w0 = 2.0 * math.pi * cutoff / RATE
    alpha = math.sin(w0) / (2.0 * 0.707)
    cos_w0 = math.cos(w0)
    b0 = (1.0 - cos_w0) / 2.0
    b1 = 1.0 - cos_w0
    b2 = (1.0 - cos_w0) / 2.0
    a0 = 1.0 + alpha
    a1 = -2.0 * cos_w0
    a2 = 1.0 - alpha

    b0 /= a0
    b1 /= a0
    b2 /= a0
    a1 /= a0
    a2 /= a0

    out = np.zeros_like(lounge_signal)
    for ch in range(2):
        sig = lounge_signal[:, ch]
        filtered = np.zeros_like(sig)
        # Run filter over 2 cycles to reach steady-state periodic condition for seamless loop
        x1 = x2 = y1 = y2 = 0.0
        # Warm up filter with one pass of the loop
        for s in sig:
            y = b0 * s + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2
            x2, x1 = x1, s
            y2, y1 = y1, y
        # Real pass
        for i, s in enumerate(sig):
            y = b0 * s + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2
            x2, x1 = x1, s
            y2, y1 = y1, y
            filtered[i] = y
        out[:, ch] = filtered

    # Attenuate volume so it sits quietly in the background (~ -25 dBFS RMS)
    target_rms = 10.0 ** (-25.0 / 20.0)
    rms = np.sqrt(np.mean(out ** 2))
    out *= target_rms / max(rms, 1e-9)

    return out


def synth_ruins_quiet() -> np.ndarray:
    """Synthesize sparse quiet ambient loop for R1a Registration.
    
    - 16.0 seconds duration
    - Warm, low hollow drone (frequencies are exact harmonic multiples of 1/16 Hz)
    - Sparse delicate chime notes with reverberant decay
    - Very soft (-21 dBFS RMS), peaceful, atmospheric
    - Seamless loop
    """
    loop_duration = 16.0
    tail_duration = 3.0
    total_samples = round((loop_duration + tail_duration) * RATE)
    loop_samples = round(loop_duration * RATE)

    t_loop = timebase(loop_duration)
    # Fundamental frequency of 16-second loop
    f0 = 1.0 / loop_duration  # 0.0625 Hz

    # Choose drone frequencies that are EXACT integer multiples of f0 for mathematical phase closure:
    # 73.4 Hz (D2) -> 73.4 / 0.0625 = 1174 cycles
    # 110.0 Hz (A2) -> 110.0 / 0.0625 = 1760 cycles
    # 146.8 Hz (D3) -> 146.8 / 0.0625 = 2349 cycles
    # 174.6 Hz (F3) -> 174.6 / 0.0625 = 2794 cycles
    k1, k2, k3, k4 = 1174, 1760, 2349, 2794
    drone_l = (
        0.045 * np.sin(2.0 * math.pi * k1 * f0 * t_loop) +
        0.035 * np.sin(2.0 * math.pi * k2 * f0 * t_loop + 0.5) +
        0.020 * np.sin(2.0 * math.pi * k3 * f0 * t_loop + 1.2) +
        0.015 * np.sin(2.0 * math.pi * k4 * f0 * t_loop + 2.0)
    )
    drone_r = (
        0.045 * np.sin(2.0 * math.pi * k1 * f0 * t_loop + 0.8) +
        0.035 * np.sin(2.0 * math.pi * k2 * f0 * t_loop) +
        0.020 * np.sin(2.0 * math.pi * k3 * f0 * t_loop + 1.8) +
        0.015 * np.sin(2.0 * math.pi * k4 * f0 * t_loop + 0.3)
    )

    # Slow gentle breathing modulation across the 16 seconds (2 cycles = 0.125 Hz)
    mod = 0.85 + 0.15 * np.sin(2.0 * math.pi * 2.0 * f0 * t_loop)
    drone_l *= mod
    drone_r *= mod

    # Add sparse chime pings over a full render buffer
    left = np.zeros(total_samples, dtype=np.float64)
    right = np.zeros(total_samples, dtype=np.float64)
    left[:loop_samples] += drone_l
    right[:loop_samples] += drone_r

    def add_chime(time_sec: float, freq: float, pan: float):
        t_chime = timebase(2.8)
        # Gentle crystalline chime note
        phase = 2.0 * math.pi * freq * t_chime
        env = np.minimum(1.0, t_chime / 0.01) * np.exp(-t_chime / 0.85)
        # Fundamental + octave + soft 5th
        chime = env * (np.sin(phase) + 0.3 * np.sin(2 * phase) + 0.12 * np.sin(3 * phase))
        # Add primary note
        start_idx = round(time_sec * RATE)
        end_idx = min(total_samples, start_idx + len(chime))
        part = chime[: end_idx - start_idx]
        left[start_idx:end_idx] += part * (1.0 - pan) * 0.09
        right[start_idx:end_idx] += part * pan * 0.09

        # Gentle soft echo 0.38s later
        echo_idx = start_idx + round(0.38 * RATE)
        if echo_idx < total_samples:
            end_echo = min(total_samples, echo_idx + len(chime))
            echo_part = chime[: end_echo - echo_idx] * 0.35
            left[echo_idx:end_echo] += echo_part * pan * 0.09
            right[echo_idx:end_echo] += echo_part * (1.0 - pan) * 0.09

    # 4 sparse, meditative chimes: D5, F5, A4, C5
    add_chime(1.8, 587.33, 0.35)
    add_chime(5.9, 698.46, 0.65)
    add_chime(10.2, 440.00, 0.40)
    add_chime(13.8, 523.25, 0.60)

    # Wrap decay tail back into loop
    loop_l = left[:loop_samples].copy()
    loop_r = right[:loop_samples].copy()
    tail_samples = total_samples - loop_samples
    loop_l[:tail_samples] += left[loop_samples:]
    loop_r[:tail_samples] += right[loop_samples:]

    stereo = np.column_stack((loop_l, loop_r))
    target_rms = 10.0 ** (-21.0 / 20.0)
    rms = np.sqrt(np.mean(stereo ** 2))
    stereo *= target_rms / max(rms, 1e-9)

    return stereo


def generate_all(ffmpeg: str | None = None) -> None:
    ff = find_ffmpeg(ffmpeg)
    MUSIC_DIR.mkdir(parents=True, exist_ok=True)

    print("Synthesizing floofy_lounge...")
    lounge = synth_floofy_lounge()
    lounge_ogg = MUSIC_DIR / "floofy_lounge.ogg"
    encode_ogg(ff, lounge, lounge_ogg)
    stats_lounge = verify_ogg(ff, lounge_ogg)
    print(f"  -> {lounge_ogg.name}: {stats_lounge['duration']:.2f}s, peak {stats_lounge['peak_db']:.1f} dBFS, "
          f"RMS {stats_lounge['rms_db']:.1f} dBFS, seam diff {stats_lounge['seam_diff']:.4f}")

    print("Synthesizing floofy_lounge_muffled...")
    muffled = synth_floofy_lounge_muffled(lounge)
    muffled_ogg = MUSIC_DIR / "floofy_lounge_muffled.ogg"
    encode_ogg(ff, muffled, muffled_ogg)
    stats_muffled = verify_ogg(ff, muffled_ogg)
    print(f"  -> {muffled_ogg.name}: {stats_muffled['duration']:.2f}s, peak {stats_muffled['peak_db']:.1f} dBFS, "
          f"RMS {stats_muffled['rms_db']:.1f} dBFS, seam diff {stats_muffled['seam_diff']:.4f}")

    print("Synthesizing floofy_fight (slowed + detuned via FFmpeg asetrate/atempo/lowpass)...")
    fight_ogg = MUSIC_DIR / "floofy_fight.ogg"
    # Process 2 repetitions of lounge through FFmpeg filters so resampler filter state
    # reaches exact periodic steady-state, ensuring 100% seamless looping.
    double_lounge = np.vstack([lounge, lounge])
    with tempfile.TemporaryDirectory(prefix="nundertale-fight-") as temp_dir:
        wav_in = Path(temp_dir) / "in.wav"
        wav_out = Path(temp_dir) / "out.wav"
        write_wav(wav_in, double_lounge)
        fight_filter = "asetrate=44100*0.75,aresample=44100,lowpass=f=3200,volume=0.92"
        subprocess.run([
            ff, "-hide_banner", "-loglevel", "error", "-y",
            "-i", str(wav_in), "-af", fight_filter,
            "-ac", "2", "-ar", str(RATE), str(wav_out)
        ], check=True)
        # Read back wav_out and extract the 2nd periodic cycle
        with wave.open(str(wav_out), "rb") as w:
            n_frames = w.getnframes()
            frames = w.readframes(n_frames)
            raw = np.frombuffer(frames, dtype="<i2").astype(np.float64) / 32767.0
            fight_stereo = raw.reshape(-1, 2)
        one_cycle = 940800  # exactly 16.0 / 0.75 * 44100 samples
        fight_cycle = fight_stereo[one_cycle:one_cycle * 2].copy()
        fight_seamless = make_seamless_loop(fight_cycle, crossfade_samples=64)
        encode_ogg(ff, fight_seamless, fight_ogg)

    stats_fight = verify_ogg(ff, fight_ogg)
    print(f"  -> {fight_ogg.name}: {stats_fight['duration']:.2f}s, peak {stats_fight['peak_db']:.1f} dBFS, "
          f"RMS {stats_fight['rms_db']:.1f} dBFS, seam diff {stats_fight['seam_diff']:.4f}")

    print("Synthesizing ruins_quiet...")
    quiet = synth_ruins_quiet()
    quiet_ogg = MUSIC_DIR / "ruins_quiet.ogg"
    encode_ogg(ff, quiet, quiet_ogg)
    stats_quiet = verify_ogg(ff, quiet_ogg)
    print(f"  -> {quiet_ogg.name}: {stats_quiet['duration']:.2f}s, peak {stats_quiet['peak_db']:.1f} dBFS, "
          f"RMS {stats_quiet['rms_db']:.1f} dBFS, seam diff {stats_quiet['seam_diff']:.4f}")

    print("\nAll 4 opening music placeholders generated and verified successfully!")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ffmpeg", help="Path to ffmpeg executable")
    args = parser.parse_args()
    generate_all(args.ffmpeg)


if __name__ == "__main__":
    main()
