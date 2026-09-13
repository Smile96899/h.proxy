# Windows release

Build the C++ application and Go core/updater into `build-h-release` first.
Run `libs/package_installer.ps1`, then compile `installer/h.iss` with Inno Setup 6.
Output: `deployment/h-v1.0-setup.exe`.

The package script accepts `-Dependencies` (default sibling `h-build-deps`).
It requires `qtsdk/Qt6.7.2-Windows-x86_64-VS2022-17.10.3` and
`runtime-data/geoip.db`, `runtime-data/geosite.db`. The Qt SDK bin directory
must include OpenSSL 3 x64. Matching MSVC CRT DLLs are collected from
`runtime/vc143-x64` (provisioned from the VS 2022 14.44.35112 x64 redist).
No previous release is used.
The three D3D runtime DLLs are pinned in `runtime/d3d-x64`, provisioned from
the Windows SDK x64 redistributables, overriding environment-dependent Qt discovery.
All release files have SHA256 hashes in `dependency-manifest.csv`.

Installed packages contain `installed.mode`, enabling Qt AppConfigLocation
plus `/config`. Portable packages omit the marker and use adjacent config.
Uninstall preserves user configuration. Desktop shortcut creation exists
only on the final wizard page, defaults unchecked, and is disabled in silent mode.

For a portable ZIP run `libs/package_h.ps1` with a new output name.
Windows system DLLs/UCRT are supplied by the supported Windows installation.
Optional external proxy executables are not included; they are only needed
for profiles explicitly configured to use external programs.
