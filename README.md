# h.proxy for Windows · 代理客户端

[中文](https://github.com/Smile96899/h.proxy/blob/main/README.md) | [English](https://github.com/Smile96899/h.proxy/blob/main/README.en.md)

软件名称 **h.**，当前版本 **v1.0**。面向 **Windows x64**，采用 C++ / Qt 界面和 Go 网络核心。

## 下载安装包直接使用

- **[h.proxy for Windows — 下载安装包](https://github.com/Smile96899/h.proxy/releases/latest/download/h-v1.0-setup.exe)**


安装完成时可选择创建桌面快捷方式，默认不勾选。升级与卸载保留个人配置；点击主窗口 × 会退出程序。

此安装包尚未签名，Windows 可能提示“未知发布者”。请仅从本仓库下载。

## 功能

- 节点与订阅分组管理、延迟测试、流量信息。
- SOCKS / HTTP、Shadowsocks、VMess、VLESS、Trojan 等核心支持的代理配置。
- 系统代理、TUN 模式、快捷键；TUN 可能需要管理员权限。
- 中文界面、暖灰与青绿色主题、在线支持入口。

这是桌面客户端，不需要部署 Web 服务器、数据库、Node.js 或 Python 服务。历史源码包含其他平台文件，但当前安装包只面向 Windows x64，不承诺其他平台构建通过。

## 依赖、准备、编译与安装包构建

仅开发者需要：**[阅读完整中文构建教程](docs/Build_Windows.md)**。

| 依赖 | 版本 | 用途 |
| --- | --- | --- |
| Windows | x64，Windows 10/11 | 构建与运行 |
| Visual Studio Build Tools + Windows SDK | 2022，MSVC x64 | C++ 桌面开发、CMake、Ninja |
| Qt MSVC x64 | 6.7.2 | 界面、网络、SVG、翻译与部署工具 |
| Go | 1.22.12 | 编译核心和更新器，不建议直接改用未经验证的新版本 |
| Protobuf | v21.4 | protoc 和通信静态库 |
| yaml-cpp / ZXing | 0.7.0 / 2.0.0 | 配置解析与二维码识别 |
| QHotkey | Git 子模块 | 全局快捷键，需要递归克隆 |
| libneko / sing-box / sing-quic | 固定提交 | 核心依赖源码 |
| Inno Setup | 6.x，至少 6.5 | 生成 EXE 安装包 |
| OpenSSL / VC143 CRT / D3D / Geo 数据库 | x64 运行文件 | 打包时统一收集，随程序提供 |

准备好依赖后：**编译 C++ 界面 → 编译 Go 核心与更新器 → 收集运行依赖 → 使用 Inno Setup 生成安装包**。

完整教程包含下载来源、目录结构、固定依赖版本、可执行构建命令、便携版与安装版区别，以及发布前检查项目。

## 源码目录

- `main/`、`ui/`、`db/`：桌面界面与配置逻辑。
- `go/`：网络核心接口与更新器。
- `libs/package_installer.ps1`：从独立依赖目录收集安装文件并生成 SHA256 清单。
- `libs/package_h.ps1`：生成便携 ZIP。
- `installer/h.iss`：中文安装器与最后一页快捷方式选项。
- `h_version.txt`：软件显示版本。
