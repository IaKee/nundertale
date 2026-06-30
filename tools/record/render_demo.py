#!/usr/bin/env python3
"""Rebuild a deterministic nUndertale recording's audio and mux its PNG frames."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import wave
from pathlib import Path

import numpy as np


SAMPLE_RATE = 48_000
FPS = 30
SAMPLES_PER_TICK = SAMPLE_RATE // FPS
PROJECT_ROOT = Path(__file__).resolve().parents[2]


def executable(name: str) -> str:
    found = shutil.which(name)
    if found is None:
        raise RuntimeError(f"{name} was not found on PATH")
    return found


def decode_audio(ffmpeg: str, path: Path) -> np.ndarray:
    command = [
        ffmpeg,
        "-v", "error",
        "-i", str(path),
        "-f", "f32le",
        "-acodec", "pcm_f32le",
        "-ar", str(SAMPLE_RATE),
        "-ac", "2",
        "pipe:1",
    ]
    decoded = subprocess.run(command, check=True, stdout=subprocess.PIPE).stdout
    samples = np.frombuffer(decoded, dtype="<f4")
    if samples.size % 2 != 0:
        samples = samples[:-1]
    return samples.reshape((-1, 2)).copy()


def source_path(record_directory: Path, logged_path: str) -> Path:
    candidate = Path(logged_path)
    if candidate.is_absolute() or candidate.exists():
        return candidate
    relative_to_recording = record_directory / candidate
    if relative_to_recording.exists():
        return relative_to_recording
    relative_to_project = PROJECT_ROOT / candidate
    if relative_to_project.exists():
        return relative_to_project
    return candidate


def music_slice(source: np.ndarray, position: float, pitch: float) -> np.ndarray:
    pitch = pitch if pitch > 0.0 else 1.0
    first_sample = max(0.0, position * SAMPLE_RATE)
    source_positions = first_sample + np.arange(SAMPLES_PER_TICK, dtype=np.float64) * pitch
    indices = np.arange(source.shape[0], dtype=np.float64)
    result = np.empty((SAMPLES_PER_TICK, 2), dtype=np.float32)
    for channel in range(2):
        result[:, channel] = np.interp(
            source_positions, indices, source[:, channel], left=0.0, right=0.0
        ).astype(np.float32)
    return result


def build_audio(record_directory: Path, frame_count: int, ffmpeg: str) -> Path:
    log_path = record_directory / "audio_log.jsonl"
    if not log_path.is_file():
        raise RuntimeError(f"missing audio log: {log_path}")

    records: list[dict[str, object]] = []
    with log_path.open("r", encoding="utf-8") as log_file:
        for line_number, line in enumerate(log_file, 1):
            if not line.strip():
                continue
            try:
                records.append(json.loads(line))
            except json.JSONDecodeError as error:
                raise RuntimeError(f"invalid JSON on {log_path}:{line_number}: {error}") from error

    referenced = {
        str(record.get(field, ""))
        for record in records
        for field in ("music", "sound")
        if str(record.get(field, ""))
    }
    decoded: dict[str, np.ndarray] = {}
    for logged_path in sorted(referenced):
        path = source_path(record_directory, logged_path)
        if not path.is_file():
            raise RuntimeError(f"referenced audio file does not exist: {logged_path}")
        decoded[logged_path] = decode_audio(ffmpeg, path)

    mixed = np.zeros((frame_count * SAMPLES_PER_TICK, 2), dtype=np.float32)
    for record in records:
        tick = int(record.get("t", -1))
        if tick < 0 or tick >= frame_count:
            continue
        destination_start = tick * SAMPLES_PER_TICK

        music = str(record.get("music", ""))
        if music:
            volume = float(record.get("vol", 1.0))
            position = float(record.get("pos", 0.0))
            pitch = float(record.get("pitch", 1.0))
            mixed[destination_start:destination_start + SAMPLES_PER_TICK] += (
                music_slice(decoded[music], position, pitch) * volume
            )

        sound = str(record.get("sound", ""))
        if sound:
            volume = float(record.get("vol", 1.0))
            source = decoded[sound]
            count = min(source.shape[0], mixed.shape[0] - destination_start)
            if count > 0:
                mixed[destination_start:destination_start + count] += source[:count] * volume

    pcm = np.rint(np.clip(mixed, -1.0, 1.0) * 32767.0).astype("<i2")
    wav_path = record_directory / "rendered_audio.wav"
    with wave.open(str(wav_path), "wb") as output:
        output.setnchannels(2)
        output.setsampwidth(2)
        output.setframerate(SAMPLE_RATE)
        output.writeframes(pcm.tobytes())
    return wav_path


def render(record_directory: Path, output_path: Path, scale: int) -> None:
    frames = sorted(record_directory.glob("frame_*.png"))
    if not frames:
        raise RuntimeError(f"no frame_*.png files found in {record_directory}")
    expected = [f"frame_{index:05d}.png" for index in range(len(frames))]
    actual = [frame.name for frame in frames]
    if actual != expected:
        raise RuntimeError("frame sequence must be contiguous and start at frame_00000.png")

    ffmpeg = executable("ffmpeg")
    wav_path = build_audio(record_directory, len(frames), ffmpeg)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    duration = len(frames) / FPS
    command = [
        ffmpeg,
        "-y",
        "-v", "error",
        "-framerate", str(FPS),
        "-start_number", "0",
        "-i", str(record_directory / "frame_%05d.png"),
        "-i", str(wav_path),
        "-vf", f"scale=iw*{scale}:ih*{scale}:flags=neighbor",
        "-c:v", "libx264",
        "-pix_fmt", "yuv420p",
        "-crf", "18",
        "-c:a", "aac",
        "-b:a", "192k",
        "-t", f"{duration:.9f}",
        "-shortest",
        str(output_path),
    ]
    subprocess.run(command, check=True)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("record_directory", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--scale", type=int, default=2)
    args = parser.parse_args()
    if args.scale <= 0:
        parser.error("--scale must be positive")
    return args


def main() -> int:
    args = parse_args()
    try:
        render(args.record_directory.resolve(), args.output.resolve(), args.scale)
    except (RuntimeError, OSError, subprocess.CalledProcessError) as error:
        print(f"render_demo.py: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
