# h.proxy — Windows 桌面代理客户端

软件名称 **h.**，当前版本 **v1.0**。本仓库发布 **Windows x64** 版本，采用 C++ / Qt 界面和 Go 网络核心。

## 下载与安装

- [下载 Windows 安装包](https://github.com/Smile96899/h.proxy/releases/latest/download/h-v1.0-setup.exe)
- [查看版本、安装包和校验文件](https://github.com/Smile96899/h.proxy/releases)

安装包在 **Releases** 中单独发布，不放进源码目录。下载源码 ZIP 不能直接安装软件。

安装器默认安装到 `%LOCALAPPDATA%\Programs\h`。最后一页的“创建桌面快捷方式”默认不勾选，只有勾选并点击完成才创建。配置保存在 `%LOCALAPPDATA%\h\config`，升级不会用发布包覆盖个人配置；卸载保留配置。关闭主窗口 × 会退出主程序和核心。

安装包未做商业代码签名，Windows 可能显示未知发布者。请核对 Releases 提供的 SHA256；不需要关闭安全软件。

## 功能

- 节点与订阅分组管理、延迟测试、流量信息。
- SOCKS / HTTP、Shadowsocks、VMess、VLESS、Trojan 等核心支持的代理配置。
- 系统代理、TUN 模式、快捷键；TUN 可能需要管理员权限。
- 中文界面、暖灰与青绿色主题、在线支持入口。

这是桌面客户端，不需要部署 Web 服务器、数据库、Node.js 或 Python 服务。历史源码仍包含其他平台文件，但本仓库当前安装包只面向 Windows x64，不承诺其他平台构建通过。

## 从源码构建

**完整步骤：[Windows 依赖准备、编译与 EXE 安装包构建](docs/Build_Windows.md)。**

| 依赖 | 本版构建基线 / 用途 |
| --- | --- |
| Windows x64 | 建议 Windows 10/11；实际验证为本地 Windows 环境 |
| Visual Studio 2022 Build Tools + Windows SDK | C++ 桌面开发、MSVC x64、CMake、Ninja |
| Qt 6.7.2 MSVC x64 | Widgets、Network、Svg、LinguistTools、windeployqt |
| Go 1.22.12 | 编译 `h_core.exe`、`updater.exe`；不要直接套用最新 Go |
| Protobuf v21.4 | `protoc` 和静态库，界面与核心通信 |
| yaml-cpp 0.7.0、ZXing 2.0.0 | 配置解析、二维码识别 |
| QHotkey 子模块 | 全局快捷键，随 `git clone --recurse-submodules` 获取 |
| libneko、sing-box、sing-quic | 按仓库固定提交获取，不能只下载主仓库 ZIP |
| Inno Setup 6.5+ | 将完整运行目录制作成单个 EXE 安装包 |
| OpenSSL 3 x64、VC143 CRT、D3D 运行库、Geo 数据库 | 运行依赖，打包时统一收集 |

准备好依赖后，流程为：**编译 C++ 界面 → 编译 Go 核心/更新器 → 收集运行依赖 → Inno Setup 生成安装包**。用户安装成品时不需要安装编译工具。

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
