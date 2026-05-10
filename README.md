# 安卓平板电子书阅读器

基于 Qt 6 + QML/C++ 的本地电子书阅读器，目标平台是安卓平板。当前版本实现了项目骨架、书架界面、本地导入服务、SQLite 书库、TXT 解码阅读能力，以及 PDF/EPUB 的可替换阅读接口。

## 支持格式

- TXT：支持 UTF-8、UTF-16 和 Windows GBK/CP936 兼容解码。
- PDF：已提供文档接口和阅读界面，当前构建未接入真实 PDF 渲染模块。
- EPUB：已提供目录/章节接口和阅读界面，当前构建未接入真实 EPUB 解包模块。

## 本地构建

推荐使用 Qt 自带 MinGW 和 Ninja：

```powershell
$env:PATH="D:\Qt\Tools\mingw1310_64\bin;D:\Qt\6.10.2\mingw_64\bin;D:\Qt\Tools\Ninja;$env:PATH"
cmake -S . -B build-qt6 -G Ninja `
  -DCMAKE_PREFIX_PATH=D:\Qt\6.10.2\mingw_64 `
  -DCMAKE_CXX_COMPILER=D:\Qt\Tools\mingw1310_64\bin\g++.exe `
  -DCMAKE_MAKE_PROGRAM=D:\Qt\Tools\Ninja\ninja.exe
cmake --build build-qt6
ctest --test-dir build-qt6 --output-on-failure
```

## 运行桌面预览

```powershell
$env:PATH="D:\Qt\Tools\mingw1310_64\bin;D:\Qt\6.10.2\mingw_64\bin;$env:PATH"
.\build-qt6\TabletEbookReader.exe
```

## 测试

当前自动化测试包括：

- `tst_bookrepository`：SQLite 初始化、书籍元数据和阅读位置保存。
- `tst_bookimporter`：TXT、PDF、EPUB 格式识别。
- `tst_txtdocument`：TXT 编码读取。

## 后续实现重点

- 接入 Android 文件选择器 URI。
- 将导入服务与 QML 书架模型打通。
- 接入 Qt PDF 或 MuPDF。
- 接入 EPUB zip 解包和 OPF/Nav 解析。
- 将阅读设置保存到 SQLite。
