# Windows x64：从源码到 EXE 安装包

**[h.proxy for Windows — 下载安装包直接使用，无需编译](https://github.com/Smile96899/h.proxy/releases/latest/download/h-v1.0-setup.exe)**

普通用户直接下载安装，无需以下开发依赖。

[中文](Build_Windows.md) | [English](Build_Windows.en.md) · [返回项目介绍](../README.md) · [SHA256 校验文件](https://github.com/Smile96899/h.proxy/releases/latest/download/SHA256SUMS.txt)

本文用于 **h.proxy / h. v1.0**。请按顺序执行；不能仅编译 `h.exe` 就发布，网络核心、Qt 插件、OpenSSL 和规则数据库均需包含。

## 1. 安装构建工具

1. 安装 [Git for Windows](https://git-scm.com/downloads/win)。
2. 安装 [Visual Studio 2022 Build Tools](https://visualstudio.microsoft.com/downloads/)，选择“使用 C++ 的桌面开发”，包含 MSVC x64/x86、Windows SDK、C++ CMake 工具。使用配套 CMake / Ninja；本版不是按 CMake 4 的兼容性测试的。
3. 安装 [Go 1.22.12](https://go.dev/dl/#go1.22.12)，确认 `go version`。不要把主模块 `go 1.19` 误当作整个依赖树均支持 Go 1.19；旧核心与最新 Go 也可能不兼容。
4. 安装 [Inno Setup 6.5 或更新的 6.x](https://jrsoftware.org/isdl.php)。
5. 安装 7-Zip，用于解压 Qt SDK 的 `.7z` 文件。

后面的命令在 **Developer PowerShell for VS 2022，x64** 中执行。确认 `cl`、`cmake`、`ninja`、`git`、`go` 均可调用，且 Go 为 amd64。

## 2. 克隆源码及目录约定

在一个可写的工作目录执行：

```powershell
git clone --recurse-submodules https://github.com/Smile96899/h.proxy.git
Set-Location h.proxy
git submodule update --init --recursive
$repo = (Get-Location).Path
$deps = Join-Path (Split-Path $repo -Parent) 'h-build-deps'
New-Item -ItemType Directory -Path $deps -Force | Out-Null
```

最终目录应为：

```text
工作目录/
  h.proxy/                       # 本仓库
  libneko/                       # 固定版本的核心依赖源码
  sing-box/
  sing-quic/
  h-build-deps/                   # 不上传 Git 的 SDK 和运行依赖
    qtsdk/Qt6.7.2-Windows-x86_64-VS2022-17.10.3/
    src/                         # Protobuf、yaml-cpp、ZXing 源码
    runtime/vc143-x64/            # VC++ CRT DLL
    runtime/d3d-x64/              # D3D DLL
    runtime-data/                # geoip.db、geosite.db
```

下载 GitHub 的源码 ZIP 不含子模块，也不含另外三个 Go 依赖仓库，推荐使用 Git 克隆。

## 3. 准备 Qt SDK

本版使用 [上游固定 Qt 6.7.2 MSVC x64 SDK](https://github.com/MatsuriDayo/nekoray_qt_runtime/releases/download/20220503/Qt6.7.2-Windows-x86_64-VS2022-17.10.3-20240621.7z)。解压到上述 `qtsdk` 目录，确保下面路径直接含 `bin`、`lib`、`plugins`、`translations`，不要多套一层目录。

```powershell
$qt = Join-Path $deps 'qtsdk/Qt6.7.2-Windows-x86_64-VS2022-17.10.3'
Test-Path "$qt/bin/windeployqt.exe"
Test-Path "$qt/bin/libcrypto-3-x64.dll"
Test-Path "$qt/bin/libssl-3-x64.dll"
```

三项均应为 `True`。如果改用 Qt 官方 SDK，必须另行准备匹配的 OpenSSL 3 x64 DLL，并保持打包脚本要求的目录结构；不要从不明 DLL 下载站获取。

## 4. 构建三个 C++ 静态依赖

以下命令以全新目录为前提，已有同名目录时先确认内容，不要覆盖未提交修改。

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

每一步出现错误应停止处理，不要继续打包。三个依赖必须与 Qt 和主程序使用相同的 x64 MSVC 工具链；不能混入 MinGW 或 x86 库。

## 5. 编译界面

```powershell
Set-Location $repo
cmake -S . -B build-h-release -G Ninja -DQT_VERSION_MAJOR=6 -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=$qt;$prefix"
cmake --build build-h-release --parallel 4
```

得到 `build-h-release/h.exe`，翻译资源由 CMake 编入程序。移动源码目录后不要复用旧 CMake 缓存，请在新克隆中重建，或先将旧 `build-h-release` 移走备份。

## 6. 获取核心源码并编译 Go 程序

三个依赖是主仓库的**同级目录**，路径由 `go/cmd/nekobox_core/go.mod` 中的 `replace` 指定。

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

不要对已有修改的依赖目录强制 checkout。源码已带接口生成文件，正常构建不需要重新生成 Go protobuf 代码。

## 7. 准备运行依赖

VC DLL 从本机 VS 2022 的 **Redistributable** 目录收集，不能复制 Debug DLL。以下版本号是本次构建环境示例；如果路径不同，修改 `$crtSource` 为实际安装版本。

```powershell
New-Item -ItemType Directory -Path "$deps/runtime/vc143-x64","$deps/runtime/d3d-x64","$deps/runtime-data" -Force | Out-Null
$crtSource = 'C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Redist/MSVC/14.44.35112/x64/Microsoft.VC143.CRT'
Copy-Item "$crtSource/*.dll" "$deps/runtime/vc143-x64"
$d3dSource = 'C:/Program Files (x86)/Windows Kits/10/Redist/D3D/x64'
Copy-Item "$d3dSource/d3dcompiler_47.dll","$d3dSource/dxcompiler.dll","$d3dSource/dxil.dll" "$deps/runtime/d3d-x64"
Invoke-WebRequest 'https://github.com/SagerNet/sing-geoip/releases/latest/download/geoip.db' -OutFile "$deps/runtime-data/geoip.db"
Invoke-WebRequest 'https://github.com/SagerNet/sing-geosite/releases/latest/download/geosite.db' -OutFile "$deps/runtime-data/geosite.db"
```

Geo 数据会更新，若要求相同构建产物，请固定数据库文件并保存 SHA256，不要每次获取 latest。Windows 自带系统 DLL/UCRT 不从开发机任意复制；本程序发布面向现代 Windows x64。

## 8. 生成可以安装的 EXE

```powershell
Set-Location $repo
./libs/package_installer.ps1 -Dependencies $deps
& 'C:/Program Files (x86)/Inno Setup 6/ISCC.exe' installer/h.iss
```

输出：`deployment/h-v1.0-setup.exe`。打包脚本拒绝覆盖已有 `deployment/h-install-payload`；重打前请将旧输出目录移走备份。不要把使用中的 `config` 复制到安装目录，也不要把 SDK 放进发布包。

脚本调用 windeployqt 收集 Qt DLL 与插件，然后补齐 OpenSSL、VC143、D3D 和数据库，并生成 `dependency-manifest.csv`。所有输入来自构建目录和独立依赖目录，不依赖以前的安装包或测试包。

安装包内的 `installed.mode` 使程序自动使用 `%LOCALAPPDATA%/h/config`。便携 ZIP 使用：

```powershell
./libs/package_h.ps1 -Name h-portable-v1.0 -Dependencies $deps
```

便携版没有 `installed.mode`，其配置保存在程序旁边的 `config`。

## 9. 发布前检查

- 在没有 Qt/Go 开发环境的干净 Windows x64 系统安装测试，核对 UI、核心启动、节点测试和联网。
- 检查安装目录包含 Qt 插件、OpenSSL、VC DLL、`geoip.db` 和 `geosite.db`。
- 检查最后一页不勾选时不创建桌面快捷方式，勾选时才创建。
- 检查关闭 ×、配置保存、升级保留配置与卸载。
- 安装 EXE 上传 GitHub Releases，不提交到 Git；可以上传 SHA256 文本供下载者核对。
- 当前安装包没有商业签名，签名需要合法代码签名证书或云签名账号。
- 外部核心模式依赖用户另外指定的外部程序，这些可选程序不随包提供。

本文记录构建方法，不代表每种 Windows 版本、代理协议或第三方依赖组合都完成过测试。
