#!/usr/bin/env python3
"""Deterministically synthesize the opening SFX placeholders.

Only NumPy creates the sounds. FFmpeg encodes and decodes the Ogg/Vorbis files.
No source recordings or downloaded assets are used.
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
ROOT = Path(__file__).resolve().parents[2]
OUT_DIR = ROOT / "assets" / "sfx"
RNG = np.random.default_rng(9824)


def timebase(seconds: float) -> np.ndarray:
    return np.arange(round(seconds * RATE), dtype=np.float64) / RATE


def fade(signal: np.ndarray, attack: float = 0.008, release: float = 0.08) -> np.ndarray:
    result = signal.copy()
    attack_n = min(len(result), max(1, round(attack * RATE)))
    release_n = min(len(result), max(1, round(release * RATE)))
    result[:attack_n] *= np.sin(np.linspace(0.0, math.pi / 2.0, attack_n)) ** 2
    result[-release_n:] *= np.cos(np.linspace(0.0, math.pi / 2.0, release_n)) ** 2
    return result


def lowpass(signal: np.ndarray, cutoff: float) -> np.ndarray:
    # Stable one-pole filter, sufficient for these deliberately lo-fi placeholders.
    alpha = 1.0 - math.exp(-2.0 * math.pi * cutoff / RATE)
    result = np.empty_like(signal)
    state = 0.0
    for i, value in enumerate(signal):
        state += alpha * (value - state)
        result[i] = state
    return result


def highpass(signal: np.ndarray, cutoff: float) -> np.ndarray:
    return signal - lowpass(signal, cutoff)


def chirp(t: np.ndarray, start_hz: float, end_hz: float, curve: float = 1.0) -> np.ndarray:
    progress = np.linspace(0.0, 1.0, len(t)) ** curve
    frequency = start_hz + (end_hz - start_hz) * progress
    phase = 2.0 * math.pi * np.cumsum(frequency) / RATE
    return np.sin(phase)


def at(signal: np.ndarray, sound: np.ndarray, seconds: float, gain: float = 1.0) -> None:
    start = round(seconds * RATE)
    end = min(len(signal), start + len(sound))
    signal[start:end] += gain * sound[: end - start]


def spray() -> np.ndarray:
    t = timebase(0.52)
    air = highpass(RNG.normal(0.0, 1.0, len(t)), 1_250.0)
    hiss_env = np.exp(-5.2 * t) * (0.65 + 0.35 * np.sin(2 * math.pi * 27 * t) ** 2)
    droplets = np.zeros_like(t)
    for offset, frequency in [(0.04, 3_800), (0.09, 4_700), (0.15, 3_300), (0.22, 4_200)]:
        td = timebase(0.055)
        at(droplets, fade(chirp(td, frequency, frequency * 0.55), 0.001, 0.045), offset, 0.16)
    return fade(0.60 * air * hiss_env + droplets, 0.004, 0.13)


def record_scratch() -> np.ndarray:
    t = timebase(0.78)
    grit = highpass(RNG.normal(0.0, 1.0, len(t)), 900.0)
    speed = np.maximum(0.0, 1.0 - t / 0.68)
    grooves = np.sin(2 * math.pi * (1_900 * t - 1_050 * t * t))
    bumps = np.sin(2 * math.pi * 31 * t) ** 18
    body = (0.42 * grit + 0.25 * grooves) * speed * (0.55 + 0.45 * bumps)
    stop = np.zeros_like(t)
    td = timebase(0.16)
    at(stop, chirp(td, 1_300, 120, 1.6) * np.exp(-15 * td), 0.56, 0.42)
    return fade(body + stop, 0.003, 0.11)


def pounce() -> np.ndarray:
    t = timebase(0.43)
    thump = chirp(t, 115, 42, 0.65) * np.exp(-11.0 * t)
    plush = lowpass(RNG.normal(0.0, 1.0, len(t)), 900.0) * np.exp(-16.0 * t)
    landing = np.zeros_like(t)
    td = timebase(0.13)
    at(landing, lowpass(RNG.normal(0.0, 1.0, len(td)), 2_400.0) * np.exp(-28 * td), 0.055, 0.3)
    return fade(0.80 * thump + 0.46 * plush + landing, 0.003, 0.12)


def sparkle() -> np.ndarray:
    result = np.zeros(round(0.74 * RATE), dtype=np.float64)
    for offset, frequency, gain in [(0.00, 1_318.5, 0.50), (0.08, 1_661.2, 0.62),
                                    (0.17, 2_093.0, 0.72), (0.29, 2_637.0, 0.62)]:
        t = timebase(0.40)
        bell = (np.sin(2 * math.pi * frequency * t)
                + 0.34 * np.sin(2 * math.pi * frequency * 2.01 * t)
                + 0.13 * np.sin(2 * math.pi * frequency * 3.97 * t))
        bell *= np.exp(-9.0 * t)
        at(result, fade(bell, 0.002, 0.12), offset, gain)
    return fade(result, 0.002, 0.14)


def petal_buzz() -> np.ndarray:
    t = timebase(0.58)
    wobble = 1.0 + 0.055 * np.sin(2 * math.pi * 9.0 * t)
    frequency = 154.0 * wobble
    phase = 2 * math.pi * np.cumsum(frequency) / RATE
    motor = np.sin(phase) + 0.30 * np.sin(2 * phase) + 0.12 * np.sin(3 * phase)
    flutter = 0.72 + 0.28 * np.sin(2 * math.pi * 18.0 * t) ** 2
    hum = 0.13 * lowpass(RNG.normal(0.0, 1.0, len(t)), 1_100.0)
    return fade((motor * flutter + hum) * np.exp(-0.65 * t), 0.025, 0.12)


def human_vowel(seconds: float, f0_start: float, f0_end: float) -> np.ndarray:
    """Small source/filter-style 'uh' synthesizer, intentionally plain and subdued."""
    t = timebase(seconds)
    frequency = np.linspace(f0_start, f0_end, len(t))
    phase = 2 * math.pi * np.cumsum(frequency) / RATE
    voice = np.zeros_like(t)
    formants = ((520.0, 155.0, 1.0), (1_050.0, 210.0, 0.62), (2_350.0, 350.0, 0.20))
    for harmonic in range(1, 35):
        hz = harmonic * (f0_start + f0_end) * 0.5
        weight = sum(gain * math.exp(-0.5 * ((hz - center) / width) ** 2)
                     for center, width, gain in formants)
        weight += 0.035
        voice += weight * np.sin(harmonic * phase + 0.08 * harmonic) / harmonic
    breath = lowpass(RNG.normal(0.0, 1.0, len(t)), 3_200.0)
    return voice + 0.11 * breath


def grunt_effort() -> np.ndarray:
    t = timebase(0.48)
    body = human_vowel(0.48, 112.0, 91.0)
    weary = (1.0 - np.exp(-22 * t)) * np.exp(-3.4 * t)
    return fade(body * weary, 0.025, 0.13)


def grunt_hurt() -> np.ndarray:
    t = timebase(0.29)
    breath = highpass(RNG.normal(0.0, 1.0, len(t)), 480.0) * np.exp(-14.0 * t)
    catch = human_vowel(0.29, 142.0, 105.0) * np.exp(-18.0 * t)
    return fade(0.78 * breath + 0.42 * catch, 0.003, 0.09)


def grunt_death() -> np.ndarray:
    t = timebase(1.18)
    breath = highpass(lowpass(RNG.normal(0.0, 1.0, len(t)), 4_000.0), 180.0)
    exhale = (1.0 - np.exp(-13.0 * t)) * np.exp(-2.15 * t)
    voice = human_vowel(1.18, 99.0, 67.0) * np.exp(-3.1 * t)
    return fade(0.62 * breath * exhale + 0.34 * voice, 0.035, 0.24)


SOUNDS = {
    "spray.ogg": spray,
    "record_scratch.ogg": record_scratch,
    "pounce.ogg": pounce,
    "sparkle.ogg": sparkle,
    "petal_buzz.ogg": petal_buzz,
    "grunt_effort.ogg": grunt_effort,
    "grunt_hurt.ogg": grunt_hurt,
    "grunt_death.ogg": grunt_death,
}


def normalize(signal: np.ndarray, target_rms_db: float = -16.0, peak_db: float = -3.5) -> np.ndarray:
    signal = np.nan_to_num(signal)
    # Keep brief transients in the same crest-factor range as the project's
    # existing SFX. This is gentle saturation, not a brick-wall limiter.
    for drive in (1.4, 2.0, 3.0, 4.5):
        rms = math.sqrt(float(np.mean(signal * signal)))
        peak = float(np.max(np.abs(signal)))
        crest_db = 20.0 * math.log10(peak / max(rms, 1e-12))
        if crest_db <= 12.0:
            break
        normalized = signal / max(peak, 1e-12)
        signal = np.tanh(drive * normalized) / math.tanh(drive)
    rms = math.sqrt(float(np.mean(signal * signal)))
    if rms == 0.0:
        raise ValueError("silent synthesized sound")
    signal *= 10.0 ** (target_rms_db / 20.0) / rms
    peak_limit = 10.0 ** (peak_db / 20.0)
    peak = float(np.max(np.abs(signal)))
    if peak > peak_limit:
        signal *= peak_limit / peak
    return signal.astype(np.float32)


def executable_works(path: str | None) -> bool:
    if not path:
        return False
    try:
        return subprocess.run([path, "-version"], stdout=subprocess.DEVNULL,
                              stderr=subprocess.DEVNULL, timeout=5, check=False).returncode == 0
    except (OSError, subprocess.SubprocessError):
        return False


def find_ffmpeg(override: str | None) -> str:
    candidates = [override, os.environ.get("FFMPEG"), shutil.which("ffmpeg")]
    try:
        import imageio_ffmpeg  # Optional locator fallback; FFmpeg still performs all encoding.
        candidates.append(imageio_ffmpeg.get_ffmpeg_exe())
    except (ImportError, RuntimeError):
        pass
    for candidate in candidates:
        if executable_works(candidate):
            return str(candidate)
    raise RuntimeError("No runnable FFmpeg found. Pass --ffmpeg PATH or set FFMPEG.")


def write_wav(path: Path, signal: np.ndarray) -> None:
    pcm = np.round(np.clip(signal, -1.0, 1.0) * 32_767.0).astype("<i2")
    with wave.open(str(path), "wb") as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(RATE)
        wav.writeframes(pcm.tobytes())


def encode(ffmpeg: str, signal: np.ndarray, output: Path) -> None:
    with tempfile.TemporaryDirectory(prefix="nundertale-sfx-") as temp_dir:
        wav_path = Path(temp_dir) / "source.wav"
        write_wav(wav_path, signal)
        subprocess.run([ffmpeg, "-hide_banner", "-loglevel", "error", "-y", "-i", str(wav_path),
                        "-ac", "1", "-ar", str(RATE), "-c:a", "libvorbis", "-q:a", "5", str(output)],
                       check=True)


def verify(ffmpeg: str, path: Path) -> tuple[float, float]:
    decoded = subprocess.run([ffmpeg, "-hide_banner", "-loglevel", "error", "-i", str(path),
                              "-f", "f32le", "-ac", "1", "-ar", str(RATE), "pipe:1"],
                             check=True, stdout=subprocess.PIPE).stdout
    samples = np.frombuffer(decoded, dtype="<f4")
    if not len(samples) or not np.all(np.isfinite(samples)):
        raise RuntimeError(f"{path.name}: invalid decoded samples")
    peak = float(np.max(np.abs(samples)))
    duration = len(samples) / RATE
    if peak >= 0.99:
        raise RuntimeError(f"{path.name}: clipping ({peak:.4f})")
    if duration > 1.25:
        raise RuntimeError(f"{path.name}: placeholder is too long ({duration:.3f}s)")
    return duration, 20.0 * math.log10(max(peak, 1e-12))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ffmpeg", help="FFmpeg executable (also read from the FFMPEG environment variable)")
    args = parser.parse_args()
    ffmpeg = find_ffmpeg(args.ffmpeg)
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    for filename, synth in SOUNDS.items():
        output = OUT_DIR / filename
        encode(ffmpeg, normalize(synth()), output)
        duration, peak_db = verify(ffmpeg, output)
        print(f"{output.relative_to(ROOT)}: {duration:.3f}s, peak {peak_db:.1f} dBFS")


if __name__ == "__main__":
    main()
