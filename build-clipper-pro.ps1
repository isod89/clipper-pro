# Clipper Pro (Clipper on the M-Vave SMK-37): build the three model packages in WSL, make the site, open the installer.
#   INSTALL-CLIPPER-PRO.bat          build, then serve the installer and the editor on http://localhost:8768
#   build-clipper-pro.ps1 -NoServe   build only (the packages and their SHA-256 in build\release\)
#   build-clipper-pro.ps1 -SiteOnly  no compile: the installer site again from the packages already in build\release\
#                                    (OUVRIR-INSTALLATEUR-PRO.bat)
#   build-clipper-pro.ps1 -SiteOnly -Publish  the site into docs\ (GitHub Pages: https://isod89.github.io/clipper-pro/)
#   build-clipper-pro.ps1 -Diag      a diagnostic build: the USB serial console in (MENU > USB SERIAL ON, then a
#                                    terminal on the keyboard's COM port: status, smk, padtest, leds)
#   build-clipper-pro.ps1 -Voices 12 a 12-voice build (8, 12 or 16; Clipper's CPU trials)
# Settings (environment, all optional):
#   CLIPPER_WSL_DISTRO   the WSL distribution with the JieLi toolchain (default MinUI-Build, as SLOOP's and Clipper's)
#   CLIPPER_TOOLCHAIN    the toolchain's Linux path (default /root/fm1-codex-deps/jieli/toolchain)
#   CLIPPER_SDK          a Windows folder with cpu/wl82/tools from the AC79 SDK (default: build\deps\ac79 here, else the
#                        one of a CLIPPER, sloop-pro or SLOOP folder next to this one)
# Port 8768 (SLOOP Pro and Clipper use 8767): the windows do not get mixed up.
param([switch]$NoServe, [switch]$Diag, [switch]$SiteOnly, [switch]$Publish, [string]$Voices = "8")
$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
if ($Voices -notin @("8", "12", "16")) { throw "Voices: 8, 12 or 16" }
$distro = if ($env:CLIPPER_WSL_DISTRO) { $env:CLIPPER_WSL_DISTRO } else { 'MinUI-Build' }
$tc = if ($env:CLIPPER_TOOLCHAIN) { $env:CLIPPER_TOOLCHAIN } else { '/root/fm1-codex-deps/jieli/toolchain' }
$sdk = $env:CLIPPER_SDK
if (-not $sdk) {
  foreach ($c in @('build\deps\ac79', '..\CLIPPER\build\deps\ac79', '..\sloop-pro\build\deps\ac79', '..\SLOOP\build\deps\ac79',
                   '..\SLOOP\sloop-2.8.1\build\deps\ac79', '..\sloop-fm1\build\deps\ac79')) {
    if (Test-Path (Join-Path $c 'cpu\wl82\tools\uboot.boot')) { $sdk = $c; break }
  }
}
if (-not $SiteOnly -and (-not $sdk -or -not (Test-Path (Join-Path $sdk 'cpu\wl82\tools\uboot.boot')))) {
  throw "The AC79 SDK files were not found: copy build\deps\ac79 from your SLOOP or CLIPPER folder here, or set CLIPPER_SDK"
}
foreach ($b in @('..\CLIPPER\build', '..\SLOOP\build', '..\sloop-pro\build')) {   # (optional: the build caches)
  if (-not (Test-Path $b)) { continue }
  New-Item -ItemType Directory -Force build | Out-Null
  foreach ($c in @('genwav', 'hiphop-src')) {
    if (-not (Test-Path "build\$c") -and (Test-Path "$b\$c")) { Copy-Item -Recurse "$b\$c" "build\$c" }
  }
  if (-not (Test-Path build\gen_samples.cache) -and (Test-Path "$b\gen_samples.cache")) { Copy-Item "$b\gen_samples.cache" build\ }
}
$m = Select-String -Path firmware\src\ui.c -Pattern 'define FELUCCA_VERSION "CLIPPER PRO ([^"]+)"' | Select-Object -First 1
if (-not $m) { throw "firmware\src\ui.c: no CLIPPER PRO version" }
$ver = $m.Matches[0].Groups[1].Value
Write-Host ""
Write-Host "  C L I P P E R   P R O   $ver   ($Voices voices)" -ForegroundColor White
Write-Host "  ---- ---- ---- ----" -ForegroundColor DarkGray
Remove-Item Env:FELUCCA_SLICE -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force build\release | Out-Null
$models = @(@('pro', 'clipper-pro.fwsc', 'SMK-37 Pro / Starrykey 37 Play'), @('elite', 'clipper-pro-elite.fwsc', 'SMK-37 Elite'), @('mke', 'clipper-pro-mke.fwsc', 'MKE-P37'))
$pk = $models | ForEach-Object { Join-Path build\release $_[1] }
if ($SiteOnly) {
  foreach ($p in $pk) { if (-not (Test-Path $p)) { throw "$p is missing: run INSTALL-CLIPPER-PRO.bat once first" } }
} else {
  Write-Host "  WSL $distro, toolchain $tc, SDK $sdk" -ForegroundColor DarkGray
  $first = $true
  foreach ($mdl in $models) {
    Write-Host "== Building Clipper Pro for the $($mdl[2]) (WSL $distro)" -ForegroundColor Cyan
    $a = @('tools/build_windows.py', '--distro', $distro, '--toolchain', $tc, '--sdk', $sdk, '--slice', '0', '--nvoice', $Voices,
           '--board', 'smk37', '--model', $mdl[0])
    if (-not $first) { $a += '--skip-generate' }
    if ($Diag) { $a += '--cdc' }
    python @a
    if ($LASTEXITCODE -ne 0) { throw "The build failed ($($mdl[2]))" }
    Copy-Item (Join-Path build $mdl[1]) (Join-Path build\release $mdl[1]) -Force
    $first = $false
  }
}
Write-Host "== Packages (build\release)" -ForegroundColor Cyan
foreach ($p in $pk) { $h = (Get-FileHash -Algorithm SHA256 $p).Hash.ToLower(); Write-Host "  $p  $h" }
Get-FileHash -Algorithm SHA256 $pk | ForEach-Object { "$($_.Hash.ToLower())  $(Split-Path $_.Path -Leaf)" } | Set-Content build\release\SHA256SUMS.txt
Write-Host "== Making the installer site" -ForegroundColor Cyan
python web/make_site.py --pro $ver build/clipper-pro-site @pk
if ($LASTEXITCODE -ne 0) { throw "make_site failed" }
if ($Publish) {                                    # docs\ = the site GitHub Pages serves (main, /docs)
  foreach ($o in @('webapp', 'firmware', 'index.html')) {   # (only the site's own: docs\*.md and docs\sloop stay)
    if (Test-Path "docs\$o") { Remove-Item -Recurse -Force "docs\$o" }
  }
  New-Item -ItemType Directory -Force docs | Out-Null
  Copy-Item -Recurse -Force build\clipper-pro-site\* docs\
  New-Item -ItemType File -Force docs\.nojekyll | Out-Null
  Write-Host "== docs\ is the Clipper Pro site (installer, editor, the three packages)" -ForegroundColor Cyan
  exit 0
}
if ($NoServe) { exit 0 }
Write-Host ""
Write-Host "Installer: http://localhost:8768/webapp/installer/  (Chrome or Edge, the SMK-37 on USB)" -ForegroundColor Green
Write-Host "Editor:    http://localhost:8768/webapp/editor/" -ForegroundColor Green
Write-Host "Keep this window open during the install. Ctrl+C stops the server."
Start-Process "http://localhost:8768/webapp/installer/"
python -m http.server 8768 --bind 127.0.0.1 --directory build/clipper-pro-site
