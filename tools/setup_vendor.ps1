[CmdletBinding()]
param(
    [string] $Destination = (Join-Path (Split-Path -Parent $PSScriptRoot) 'vendor')
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

$packages = @(
    @{ Name = 'w64devkit'; Repository = 'skeeto/w64devkit'; Tag = 'v2.10.1'; Asset = 'w64devkit-x64-2.10.1.zip'; Directory = 'w64devkit' },
    @{ Name = 'raylib MinGW'; Repository = 'raysan5/raylib'; Tag = '6.2'; Asset = 'raylib-6.2_win64_mingw-w64.zip'; Directory = 'raylib-6.2_win64_mingw-w64' },
    @{ Name = 'raylib MSVC 16'; Repository = 'raysan5/raylib'; Tag = '6.2'; Asset = 'raylib-6.2_win64_msvc16.zip'; Directory = 'raylib-6.2_win64_msvc16' }
)

function Get-ReleaseAsset {
    param([hashtable] $Package)

    $headers = @{
        Accept = 'application/vnd.github+json'
        'X-GitHub-Api-Version' = '2022-11-28'
        'User-Agent' = 'nUndertale-vendor-setup'
    }
    $uri = "https://api.github.com/repos/$($Package.Repository)/releases/tags/$($Package.Tag)"
    $release = Invoke-RestMethod -Uri $uri -Headers $headers
    $asset = @($release.assets | Where-Object name -EQ $Package.Asset)
    if ($asset.Count -ne 1) {
        throw "Expected exactly one '$($Package.Asset)' asset in $uri; found $($asset.Count)."
    }
    if ([string]::IsNullOrWhiteSpace($asset[0].digest) -or
        $asset[0].digest -notmatch '^sha256:[0-9a-fA-F]{64}$') {
        throw "GitHub did not provide a SHA-256 digest for '$($Package.Asset)'. Refusing an unchecked download."
    }
    return $asset[0]
}

function Install-Package {
    param([hashtable] $Package, [string] $VendorDirectory)

    $target = Join-Path $VendorDirectory $Package.Directory
    $stamp = Join-Path $target '.nundertale-vendor.json'
    if (Test-Path -LiteralPath $stamp -PathType Leaf) {
        $installed = Get-Content -Raw -LiteralPath $stamp | ConvertFrom-Json
        if ($installed.asset -eq $Package.Asset -and $installed.tag -eq $Package.Tag) {
            Write-Host "$($Package.Name) is already installed."
            return
        }
    }
    if (Test-Path -LiteralPath $target) {
        throw "'$target' already exists but was not installed by this script at the requested version. Remove or rename it first."
    }

    $asset = Get-ReleaseAsset -Package $Package
    $expectedHash = $asset.digest.Substring(7).ToUpperInvariant()
    $archive = Join-Path $script:TemporaryDirectory $Package.Asset
    Write-Host "Downloading $($Package.Name) ($($Package.Tag))..."
    Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $archive -Headers @{ 'User-Agent' = 'nUndertale-vendor-setup' }
    $actualHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $archive).Hash
    if ($actualHash -ne $expectedHash) {
        throw "SHA-256 mismatch for '$($Package.Asset)': expected $expectedHash, got $actualHash."
    }

    $extractRoot = Join-Path $script:TemporaryDirectory ($Package.Directory + '-extract')
    Expand-Archive -LiteralPath $archive -DestinationPath $extractRoot
    $children = @(Get-ChildItem -Force -LiteralPath $extractRoot)
    $source = if ($children.Count -eq 1 -and $children[0].PSIsContainer) { $children[0].FullName } else { $extractRoot }

    Move-Item -LiteralPath $source -Destination $target
    @{
        repository = $Package.Repository
        tag = $Package.Tag
        asset = $Package.Asset
        sha256 = $actualHash
    } | ConvertTo-Json | Set-Content -Encoding utf8 -LiteralPath $stamp
    Write-Host "Installed $($Package.Name) in '$target'."
}

$Destination = [System.IO.Path]::GetFullPath($Destination)
New-Item -ItemType Directory -Force -Path $Destination | Out-Null
$script:TemporaryDirectory = Join-Path ([System.IO.Path]::GetTempPath()) ("nundertale-vendor-" + [guid]::NewGuid())
New-Item -ItemType Directory -Path $script:TemporaryDirectory | Out-Null

try {
    foreach ($package in $packages) {
        Install-Package -Package $package -VendorDirectory $Destination
    }
    Write-Host 'Vendor setup complete.'
} finally {
    if (Test-Path -LiteralPath $script:TemporaryDirectory) {
        Remove-Item -Recurse -Force -LiteralPath $script:TemporaryDirectory
    }
}
