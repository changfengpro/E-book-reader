# Android 构建说明

本文档说明如何用 Qt Creator 构建安卓平板版本。

## 环境要求

- Qt 6.10.2 或更新版本。
- Qt Android 组件。
- JDK。
- Android SDK。
- Android NDK。
- CMake。
- Ninja。

## Qt Creator 配置

1. 打开 Qt Creator。
2. 进入「编辑」->「首选项」->「设备」->「Android」。
3. 配置 JDK、Android SDK 和 Android NDK 路径。
4. 确认 Qt Creator 能识别 Android Kit。
5. 打开项目根目录下的 `CMakeLists.txt`。
6. 选择 Android Kit。
7. 执行 Configure。
8. 构建 APK 或 AAB。

## 命令行构建提示

命令行 Android 构建需要使用 Qt Android Kit 对应的 toolchain 配置。建议优先用 Qt Creator 生成第一版配置，确认 SDK/NDK 路径无误后再固化为脚本。

## 当前限制

- PDF 当前是可替换接口，尚未接入 Qt PDF 或 MuPDF。
- EPUB 当前是可替换接口，尚未接入 zip 解包和 OPF/Nav 解析。
- Android 文件选择器 URI 尚未与 `BookImporter` 打通。

这些限制不会影响桌面构建和现有单元测试，但会影响安卓端完整阅读体验。
