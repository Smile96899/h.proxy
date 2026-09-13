# h.proxy for Windows · Proxy client

[中文](https://github.com/Smile96899/h.proxy/blob/main/README.md) | [English](https://github.com/Smile96899/h.proxy/blob/main/README.en.md)

**h. v1.0** is a **Windows x64** desktop proxy client with a C++ / Qt interface and a Go networking core.

## Download and install — no build required

- **[h.proxy for Windows — Download the installer](https://github.com/Smile96899/h.proxy/releases/latest/download/h-v1.0-setup.exe)**


Creating a desktop shortcut is optional and unchecked by default. Upgrades and uninstalling preserve personal settings. Closing the main window exits the application and its core.

This installer is unsigned, so Windows may show an “unknown publisher” warning. Download it only from this repository.

## Features

- Node and subscription group management, latency tests and traffic information.
- SOCKS / HTTP, Shadowsocks, VMess, VLESS, Trojan and other proxy configurations supported by the core.
- System proxy, TUN mode and hotkeys; TUN may require administrator privileges.
- A Chinese application interface, warm-gray and teal theme, and an online support entry point.

This is a desktop application: no web server, database, Node.js or Python service is required. Historical source files for other platforms remain, but the current installer targets Windows x64 only; other platform builds are not guaranteed.

## Dependencies, preparation, compilation and installer packaging

For developers only: **[Read the complete English build guide](docs/Build_Windows.en.md)**.

| Dependency | Version | Purpose |
| --- | --- | --- |
| Windows | x64, Windows 10/11 | Build and run |
| Visual Studio Build Tools + Windows SDK | 2022, MSVC x64 | Desktop C++ toolchain, CMake and Ninja |
| Qt MSVC x64 | 6.7.2 | GUI, networking, SVG, translations and deployment tools |
| Go | 1.22.12 | Build the core and updater; avoid untested newer toolchains |
| Protobuf | v21.4 | protoc and the communication static library |
| yaml-cpp / ZXing | 0.7.0 / 2.0.0 | Configuration parsing and QR decoding |
| QHotkey | Git submodule | Global hotkeys; clone recursively |
| libneko / sing-box / sing-quic | Pinned commits | Core source dependencies |
| Inno Setup | 6.x, at least 6.5 | Create the EXE installer |
| OpenSSL / VC143 CRT / D3D / Geo databases | x64 runtime files | Collected during packaging and bundled with the application |

After preparation: **build the C++ GUI → build the Go core and updater → collect runtime dependencies → create the installer with Inno Setup**.

The complete guide includes download sources, directory layout, pinned dependencies, executable build commands, portable-versus-installed behavior and a pre-release checklist.

## Source layout

- `main/`, `ui/`, `db/`: desktop interface and configuration logic.
- `go/`: networking core interfaces and updater.
- `libs/package_installer.ps1`: collect installation files from an independent dependency directory and generate a SHA256 manifest.
- `libs/package_h.ps1`: create a portable ZIP.
- `installer/h.iss`: Chinese installer and final-page shortcut option.
- `h_version.txt`: displayed application version.
