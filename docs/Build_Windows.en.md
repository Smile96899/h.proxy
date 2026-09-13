# h.proxy for Windows — build and create an EXE installer

**[Download h.proxy for Windows and install directly — no build required](https://github.com/Smile96899/h.proxy/releases/latest/download/h-v1.0-setup.exe)**

End users do not need the dependencies below. Download and run the installer instead.

[中文](Build_Windows.md) | [English](Build_Windows.en.md) · [Back to project overview](../README.en.md) · [SHA256 checksums](https://github.com/Smile96899/h.proxy/releases/latest/download/SHA256SUMS.txt)

These instructions cover **Windows x64, h. v1.0**. Building only `h.exe` is not enough: the Go core, Qt plugins, OpenSSL and Geo databases must also be deployed.

## 1. Install the development tools

- [Git for Windows](https://git-scm.com/downloads/win), with recursive submodule support.
- [Visual Studio 2022 Build Tools](https://visualstudio.microsoft.com/downloads/): select **Desktop development with C++**, MSVC x64/x86, Windows SDK and C++ CMake tools. Use the bundled CMake/Ninja; this procedure was not validated with CMake 4.
- [Go 1.22.12](https://go.dev/dl/#go1.22.12). The top-level module's `go 1.19` declaration is not the minimum for the entire dependency tree. Newer Go releases may be incompatible with this pinned older core.
- [Inno Setup 6.5 or later in the 6.x series](https://jrsoftware.org/isdl.php).
- 7-Zip to extract the Qt SDK archive.
- Qt 6.7.2 MSVC x64, Protobuf v21.4, yaml-cpp 0.7.0 and ZXing 2.0.0, prepared below.
- QHotkey is a Git submodule. libneko, sing-box and sing-quic are separate pinned source repositories.
- Runtime packaging also needs OpenSSL 3 x64, the matching VC143 CRT, D3D runtime DLLs and Geo databases.

Run all PowerShell commands below in an **x64 Developer PowerShell for VS 2022**. Confirm that `cl`, `cmake`, `ninja`, `git` and `go` are available. Stop if any command fails.

## 2. Clone and establish the directory layout

Use a writable workspace:

```powershell
git clone --recurse-submodules https://github.com/Smile96899/h.proxy.git
Set-Location h.proxy
git submodule update --init --recursive
$repo = (Get-Location).Path
$deps = Join-Path (Split-Path $repo -Parent) 'h-build-deps'
New-Item -ItemType Directory -Path $deps -Force | Out-Null
```

Keep the following directories next to one another:

```text
workspace/
  h.proxy/                       # this repository
  libneko/                       # pinned core dependencies
  sing-box/
  sing-quic/
  h-build-deps/                   # SDKs and runtime files, not committed to Git
    qtsdk/Qt6.7.2-Windows-x86_64-VS2022-17.10.3/
    src/                         # C++ dependency sources
    runtime/vc143-x64/
    runtime/d3d-x64/
    runtime-data/
```

GitHub's source ZIP does not contain submodules or the three sibling core repositories. A recursive Git clone is recommended.

## 3. Prepare Qt

This build uses the [pinned upstream Qt 6.7.2 MSVC x64 SDK](https://github.com/MatsuriDayo/nekoray_qt_runtime/releases/download/20220503/Qt6.7.2-Windows-x86_64-VS2022-17.10.3-20240621.7z).
Extract it into the directory shown below. That directory must directly contain `bin`, `lib`, `plugins` and `translations`, without an extra nested folder.

```powershell
$qt = Join-Path $deps 'qtsdk/Qt6.7.2-Windows-x86_64-VS2022-17.10.3'
Test-Path "$qt/bin/windeployqt.exe"
Test-Path "$qt/bin/libcrypto-3-x64.dll"
Test-Path "$qt/bin/libssl-3-x64.dll"
```

All three checks must return `True`. If using an official Qt SDK instead, separately supply matching OpenSSL 3 x64 DLLs and retain the directory layout expected by the packaging script. Do not download arbitrary DLLs from unknown sites.

## 4. Build the C++ static dependencies

The commands assume fresh directories. If any destination already exists, inspect it first; do not overwrite uncommitted work.

```powershell
New-Item -ItemType Directory -Path "$deps/src" -Force | Out-Null
git clone --branch yaml-cpp-0.7.0 --depth 1 https://github.com/jbeder/yaml-cpp.git "$deps/src/yaml-cpp"
git clone --branch v2.0.0 --depth 1 https://github.com/zxing-cpp/zxing-cpp.git "$deps/src/zxing-cpp"
git clone --branch v21.4 --depth 1 --recurse-submodules https://github.com/protocolbuffers/protobuf.git "$deps/src/protobuf"
$prefix = Join-Path $repo 'libs/deps/built'

cmake -S "$deps/src/yaml-cpp" -B "$deps/build-yaml" -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DYAML_CPP_BUILD_TESTS=OFF "-DCMAKE_INSTALL_PREFIX=$prefix"
cmake --build "$deps/build-yaml" --parallel 4
cmake --install "$deps/build-yaml"

cmake -S "$deps/src/zxing-cpp" -B "$deps/build-zxing" -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_BLACKBOX_TESTS=OFF "-DCMAKE_INSTALL_PREFIX=$prefix"
cmake --build "$deps/build-zxing" --parallel 4
cmake --install "$deps/build-zxing"

cmake -S "$deps/src/protobuf" -B "$deps/build-protobuf" -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -Dprotobuf_MSVC_STATIC_RUNTIME=OFF -Dprotobuf_BUILD_TESTS=OFF "-DCMAKE_INSTALL_PREFIX=$prefix"
cmake --build "$deps/build-protobuf" --parallel 4
cmake --install "$deps/build-protobuf"
```

All libraries must use the same x64 MSVC toolchain as Qt and the application. Do not mix x86 or MinGW libraries into this build. Stop and fix dependency build errors before continuing.

## 5. Build the desktop GUI

```powershell
Set-Location $repo
cmake -S . -B build-h-release -G Ninja -DQT_VERSION_MAJOR=6 -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=$qt;$prefix"
cmake --build build-h-release --parallel 4
```

Output: `build-h-release/h.exe`. CMake compiles the application's translation resources into the executable.
If you move the repository, do not reuse the old CMake cache: use a fresh clone or move the old `build-h-release` directory aside as a backup before rebuilding.

## 6. Fetch the pinned core sources and build Go executables

The three source repositories must be **siblings** of `h.proxy`, because `go/cmd/nekobox_core/go.mod` uses relative `replace` paths.

```powershell
$parent = Split-Path $repo -Parent
git clone https://github.com/MatsuriDayo/libneko.git "$parent/libneko"
git -C "$parent/libneko" checkout 1c47a3af71990a7b2192e03292b4d246c308ef0b
git clone https://github.com/MatsuriDayo/sing-box.git "$parent/sing-box"
git -C "$parent/sing-box" checkout 06557f6cef23160668122a17a818b378b5a216b5
git clone https://github.com/MatsuriDayo/sing-quic.git "$parent/sing-quic"
git -C "$parent/sing-quic" checkout b49ce60d9b3622d5238fee96bfd3c5f6e3915b42

$env:GOOS = 'windows'
$env:GOARCH = 'amd64'
$env:CGO_ENABLED = '0'
$env:GOTOOLCHAIN = 'local'
$version = (Get-Content "$repo/h_version.txt" -Raw).Trim()
Push-Location "$repo/go/cmd/nekobox_core"
go mod download
go build -trimpath -o "$repo/build-h-release/h_core.exe" -ldflags "-w -s -X github.com/matsuridayo/libneko/neko_common.Version_neko=$version" -tags 'with_clash_api,with_gvisor,with_quic,with_wireguard,with_utls,with_ech'
Pop-Location
Push-Location "$repo/go/cmd/updater"
go mod download
go build -trimpath -ldflags '-w -s' -o "$repo/build-h-release/updater.exe"
Pop-Location
```

Do not force-checkout an existing repository with local modifications. Generated Go interface files are already included; normal builds do not require regenerating Go protobuf code.

## 7. Collect the runtime dependencies

Copy the release CRT from the Visual Studio **Redistributable** directory, not Debug DLLs. The version below is the verified build environment's example; adjust `$crtSource` to the actual installed version on your machine.

```powershell
New-Item -ItemType Directory -Path "$deps/runtime/vc143-x64","$deps/runtime/d3d-x64","$deps/runtime-data" -Force | Out-Null
$crtSource = 'C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Redist/MSVC/14.44.35112/x64/Microsoft.VC143.CRT'
Copy-Item "$crtSource/*.dll" "$deps/runtime/vc143-x64"
$d3dSource = 'C:/Program Files (x86)/Windows Kits/10/Redist/D3D/x64'
Copy-Item "$d3dSource/d3dcompiler_47.dll","$d3dSource/dxcompiler.dll","$d3dSource/dxil.dll" "$deps/runtime/d3d-x64"
Invoke-WebRequest 'https://github.com/SagerNet/sing-geoip/releases/latest/download/geoip.db' -OutFile "$deps/runtime-data/geoip.db"
Invoke-WebRequest 'https://github.com/SagerNet/sing-geosite/releases/latest/download/geosite.db' -OutFile "$deps/runtime-data/geosite.db"
```

Geo databases change over time. To reproduce identical binaries/packages, preserve the database files and their hashes rather than downloading `latest` each time.
Windows system DLLs/UCRT are supplied by the supported Windows installation; do not indiscriminately copy System32 files into the package.

## 8. Create an installable EXE

```powershell
Set-Location $repo
./libs/package_installer.ps1 -Dependencies $deps
& 'C:/Program Files (x86)/Inno Setup 6/ISCC.exe' installer/h.iss
```

Output: `deployment/h-v1.0-setup.exe`.

The script refuses to overwrite an existing `deployment/h-install-payload`. Move the previous output aside as a backup before rebuilding. Never copy personal `config` files or entire SDKs into the distribution.

The script invokes `windeployqt`, adds OpenSSL, VC143 CRT, D3D and Geo files from the independent dependency directory, validates required files and writes `dependency-manifest.csv` with SHA256 hashes. It does not copy dependencies from an older test package.

The installer includes `installed.mode`, which makes the application store settings in `%LOCALAPPDATA%/h/config`. The default application directory is `%LOCALAPPDATA%/Programs/h`. The final-page desktop-shortcut checkbox is unchecked by default; a shortcut is created only if selected when clicking Finish. Silent installation does not create one.

For a portable ZIP instead:

```powershell
./libs/package_h.ps1 -Name h-portable-v1.0 -Dependencies $deps
```

Portable builds omit `installed.mode` and keep configuration in the adjacent `config` directory.

## 9. Validate and publish

- Install on a clean Windows x64 machine without Qt/Go development tools. Check the GUI, core startup, node tests and network operation.
- Confirm Qt plugins, OpenSSL, VC DLLs, `geoip.db` and `geosite.db` are present.
- Test both the unchecked and checked final-page shortcut choices.
- Check window-close behavior, settings persistence, upgrades and uninstalling.
- Upload the EXE and a SHA256 checksum file to **GitHub Releases**, not the source tree.
- The current installer is unsigned. Trusted signing requires a legitimate code-signing certificate or cloud-signing account.
- Optional external core executables are not bundled; users who explicitly configure an external program must provide it separately.
- TUN mode may require administrator privileges.

This guide describes the build procedure, not exhaustive validation of every Windows version, protocol or third-party dependency combination.
