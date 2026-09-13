param([string]$Name = 'h-install-payload', [string]$Dependencies = "$PSScriptRoot/../../h-build-deps", [switch]$Portable)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$qt = Join-Path $Dependencies 'qtsdk/Qt6.7.2-Windows-x86_64-VS2022-17.10.3'
$dest = Join-Path $root "deployment/$Name"
if (Test-Path $dest) { throw "Output already exists: $dest" }
foreach ($file in 'h.exe','h_core.exe','updater.exe') {
    if (!(Test-Path "$root/build-h-release/$file")) { throw "Missing build output: $file" }
}
New-Item -ItemType Directory -Path $dest | Out-Null
Copy-Item "$root/build-h-release/h.exe","$root/build-h-release/h_core.exe","$root/build-h-release/updater.exe" $dest
& "$qt/bin/windeployqt.exe" --release --no-compiler-runtime --dir $dest "$dest/h.exe"
if ($LASTEXITCODE -ne 0) { throw 'Qt deployment failed' }
foreach ($file in 'libcrypto-3-x64.dll','libssl-3-x64.dll') { Copy-Item "$qt/bin/$file" $dest }
$crt = Join-Path $Dependencies 'runtime/vc143-x64'
if (!(Test-Path $crt)) { throw "Missing VC runtime: $crt" }
Copy-Item "$crt/*.dll" $dest
foreach ($file in 'd3dcompiler_47.dll','dxcompiler.dll','dxil.dll') { Copy-Item "$Dependencies/runtime/d3d-x64/$file" $dest -Force }
foreach ($file in 'geoip.db','geosite.db') { Copy-Item "$Dependencies/runtime-data/$file" $dest }
Copy-Item "$qt/translations/qtbase_zh_CN.qm" $dest
Copy-Item "$root/h_version.txt" $dest
if (!$Portable) { Copy-Item "$root/installer/installed.mode" $dest }
foreach ($file in 'h.exe','h_core.exe','updater.exe','Qt6Core.dll','Qt6Gui.dll','Qt6Widgets.dll','Qt6Network.dll','Qt6Svg.dll','platforms/qwindows.dll','tls/qopensslbackend.dll','libcrypto-3-x64.dll','libssl-3-x64.dll','vcruntime140.dll','vcruntime140_1.dll','msvcp140.dll','geoip.db','geosite.db') {
    if (!(Test-Path "$dest/$file") -or (Get-Item "$dest/$file").Length -eq 0) { throw "Incomplete package: $file" }
}
$manifest = Get-ChildItem $dest -Recurse -File | Get-FileHash -Algorithm SHA256 | Select-Object @{n='File';e={$_.Path.Substring($dest.Length+1)}},Hash
$manifest | Export-Csv "$dest/dependency-manifest.csv" -NoTypeInformation -Encoding UTF8
if ($Portable) { Compress-Archive "$dest/*" "$dest.zip" }
Write-Output $dest
