# Windows setup

nUndertale builds with C++20 and raylib 6.2. The repository does not commit the
large third-party toolchain under `vendor/`; the setup script installs the exact
layout expected by `build.bat` and `CMakeLists.txt`.

## Prerequisites

- 64-bit Windows
- PowerShell 5.1 or newer
- Internet access to `api.github.com` and GitHub release downloads

From the repository root, run:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/setup_vendor.ps1
```

The script installs:

- w64devkit 2.10.1 in `vendor/w64devkit`
- raylib 6.2 for MinGW-w64 in `vendor/raylib-6.2_win64_mingw-w64`
- raylib 6.2 for MSVC 16 in `vendor/raylib-6.2_win64_msvc16`

Each archive is selected from its exact GitHub release tag. Before extraction,
the script compares its SHA-256 hash with the digest published in the GitHub
release API and refuses downloads without a valid digest. Partial downloads and
extractions use a temporary directory, which is removed even after a failure.

The command is idempotent: a package carrying the script's installation stamp
at the requested version is skipped. An existing unmarked or different-version
directory is never overwritten; rename or remove that directory explicitly and
run the command again.

To stage the dependencies somewhere else (for example, in CI), pass a destination:

```powershell
tools/setup_vendor.ps1 -Destination build/vendor-staging
```

## Build

After setup completes, compile the standard debug-enabled build:

```powershell
./build.bat
```

Other supported configurations are:

```powershell
./build.bat debug
./build.bat release
```

The executable is written to `nundertale_modern.exe`. Run all commands from the
repository root so asset and data paths resolve correctly.

## Troubleshooting

If GitHub rate-limits an unauthenticated request, wait for the limit to reset and
run the script again. A checksum mismatch is a hard failure; do not bypass it.
Retry on a trusted network, and verify the release upstream if it persists. The
script never modifies an existing directory it cannot identify as its own.
