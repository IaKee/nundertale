# QA-002: drive a debug build with real key presses and save a screenshot.
# Build with `build.bat debug` first. Example:
#   .\tools\qa_walk.ps1 -Room ruins_fall -Out shot.png -Frame 300 -Keys "X:60,Z:60,WAIT:250,UP:2500"
# Keys = comma list of KEY:holdMs (UP DOWN LEFT RIGHT Z X ENTER) or WAIT:ms; NUNDERTALE_ROOM/SHOT/SHOT_FRAME do the rest.
param([string]$Room, [string]$Out, [int]$Frame, [string]$Keys)
Add-Type @"
using System; using System.Runtime.InteropServices;
public class K { [DllImport("user32.dll")] public static extern void keybd_event(byte b, byte s, uint f, UIntPtr e);
[DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h); }
"@
$vk = @{ UP=0x26; DOWN=0x28; LEFT=0x25; RIGHT=0x27; Z=0x5A; X=0x58; ENTER=0x0D }
$env:NUNDERTALE_ROOM=$Room; $env:NUNDERTALE_SHOT=$Out; $env:NUNDERTALE_SHOT_FRAME="$Frame"
$p = Start-Process .\nundertale_modern.exe -PassThru
Start-Sleep -Milliseconds 1500
$p.Refresh()
[K]::keybd_event(0x12,0,0,[UIntPtr]::Zero); [K]::keybd_event(0x12,0,2,[UIntPtr]::Zero)
$ok=[K]::SetForegroundWindow($p.MainWindowHandle)
(New-Object -ComObject WScript.Shell).AppActivate($p.Id) | Out-Null
Write-Output "hwnd=$($p.MainWindowHandle) fg=$ok"
Start-Sleep -Milliseconds 300
foreach ($step in $Keys.Split(",")) { if (-not $step) { continue }
  $k,$ms = $step.Split(":"); $code=[byte]$vk[$k]
  if ($k -eq "WAIT") { Start-Sleep -Milliseconds ([int]$ms); continue }
  $sc=@{ UP=0x48; DOWN=0x50; LEFT=0x4B; RIGHT=0x4D; Z=0x2C; X=0x2D; ENTER=0x1C }[$k]; $ext = if ($k -in "UP","DOWN","LEFT","RIGHT") {1} else {0}
  [K]::keybd_event($code,[byte]$sc,$ext,[UIntPtr]::Zero); Start-Sleep -Milliseconds ([int]$ms); [K]::keybd_event($code,[byte]$sc,($ext -bor 2),[UIntPtr]::Zero); Start-Sleep -Milliseconds 120 }
if (-not $p.WaitForExit(30000)) { $p.Kill() }
Remove-Item Env:NUNDERTALE_ROOM,Env:NUNDERTALE_SHOT,Env:NUNDERTALE_SHOT_FRAME
