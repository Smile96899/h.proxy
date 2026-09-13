# h.proxy for Windows · 代理客户端

**Windows proxy client · 中文 / English**

软件名称 **h.**，当前版本 **v1.0**。本仓库发布 **Windows x64** 版本，采用 C++ / Qt 界面和 Go 网络核心。

## 下载安装包直接使用 / Download and install — no build required

- **[h.proxy for Windows — 下载安装包 / Download Windows x64 installer](https://github.com/Smile96899/h.proxy/releases/latest/download/h-v1.0-setup.exe)**
- [SHA256 校验文件 / SHA256 checksums](https://github.com/Smile96899/h.proxy/releases/latest/download/SHA256SUMS.txt)
- [所有版本 / All releases](https://github.com/Smile96899/h.proxy/releases)

**普通用户：下载安装 EXE 即可，不需要准备以下开发依赖，也不需要自己编译。**

**For users: download and run the EXE installer. You do not need the development dependencies or build steps below.** Runtime dependencies are bundled. The installer is published separately under Releases, not stored with the source code. GitHub's “Source code” ZIP is not an installer.

安装包在 **Releases** 中单独发布，不放进源码目录。下载源码 ZIP 不能直接安装软件。

安装器默认安装到 `%LOCALAPPDATA%\Programs\h`。最后一页的“创建桌面快捷方式”默认不勾选，只有勾选并点击完成才创建。配置保存在 `%LOCALAPPDATA%\h\config`，升级不会用发布包覆盖个人配置；卸载保留配置。关闭主窗口 × 会退出主程序和核心。

安装包未做商业代码签名，Windows 可能显示未知发布者。请核对 Releases 提供的 SHA256；不需要关闭安全软件。

The default installation directory is `%LOCALAPPDATA%\Programs\h`; settings are stored in `%LOCALAPPDATA%\h\config`. The final-page desktop-shortcut checkbox is **unchecked by default**. A shortcut is created only when you select it and click Finish. Closing the main window exits the app and its core. Uninstalling preserves user settings. The installer is unsigned and Windows may show an unknown-publisher warning; verify the SHA256 and do not disable security software.

## 功能

- 节点与订阅分组管理、延迟测试、流量信息。
- SOCKS / HTTP、Shadowsocks、VMess、VLESS、Trojan 等核心支持的代理配置。
- 系统代理、TUN 模式、快捷键；TUN 可能需要管理员权限。
- 中文界面、暖灰与青绿色主题、在线支持入口。

这是桌面客户端，不需要部署 Web 服务器、数据库、Node.js 或 Python 服务。历史源码仍包含其他平台文件，但本仓库当前安装包只面向 Windows x64，不承诺其他平台构建通过。

## 依赖、准备、编译与安装包构建 / Dependencies, preparation, build and packaging

仅开发者需要 / For developers only:

- **[中文完整教程](docs/Build_Windows.md)**
- **[Complete English build guide](docs/Build_Windows.en.md)**

| Dependency / 依赖 | Version / 版本 | Purpose / 用途 |
| --- | --- | --- |
| Windows | x64, Windows 10/11 | 构建与运行 / Build and run |
| Visual Studio Build Tools + Windows SDK | 2022, MSVC x64 | C++ 桌面开发、CMake、Ninja / Desktop C++ toolchain |
| Qt MSVC x64 | 6.7.2 | 界面与部署 / Widgets, Network, Svg, LinguistTools, windeployqt |
| Go | 1.22.12 | 核心和更新器 / Build core and updater; avoid untested newer toolchains |
| Protobuf | v21.4 | 通信代码 / protoc and static library |
| yaml-cpp / ZXing | 0.7.0 / 2.0.0 | 配置解析与二维码 / Configuration parsing and QR decoding |
| QHotkey | Git submodule | 全局快捷键 / Global hotkeys; clone recursively |
| libneko / sing-box / sing-quic | Pinned commits / 固定提交 | 核心依赖 / Core source dependencies |
| Inno Setup | 6.5+ within 6.x | 生成安装 EXE / Create the EXE installer |
| OpenSSL / VC143 CRT / D3D / Geo databases | x64 runtime files | 随包提供的运行依赖 / Bundled runtime dependencies |

准备好依赖后，流程为：**编译 C++ 界面 → 编译 Go 核心/更新器 → 收集运行依赖 → Inno Setup 生成安装包**。用户安装成品时不需要安装编译工具。

After preparation: **build the C++ GUI → build the Go core/updater → collect runtime dependencies → compile the Inno Setup installer**. No web server, database, Node.js or Python service is required. This release targets Windows x64 only; other historical platform files are not a promise of supported builds.

## 目录

- `main/`、`ui/`、`db/`：桌面界面与配置逻辑。
- `go/`：网络核心接口、更新器。
- `libs/package_installer.ps1`：从独立依赖目录收集安装文件并生成 SHA256 清单。
- `libs/package_h.ps1`：生成便携 ZIP。
- `installer/h.iss`：中文安装器与最后一页快捷方式选项。
- `h_version.txt`：软件显示版本。

旧上游发布工作流保存在 `docs/legacy-workflows/`，不会在本仓库自动运行。当前发布方式以 Windows 构建文档为准。

## 来源与许可证

本项目基于 [MatsuriDayo/nekoray](https://github.com/MatsuriDayo/nekoray) 二次开发，保留其 Git 历史；源代码使用 [GPL-3.0](LICENSE)。Qt、sing-box、QHotkey 等第三方组件适用各自的许可证。品牌调整不改变原有许可条款。
