# 安卓平板电子书阅读器

基于 Qt 6 + QML/C++ 的本地电子书阅读器，目标平台是安卓平板。当前版本已经打通项目骨架、书架界面、本地导入服务、SQLite 书库、TXT 解码和真实文件阅读链路，并为 PDF/EPUB 保留可替换阅读接口。

## 支持格式

- TXT：支持 UTF-8、UTF-16 和 Windows GBK/CP936 兼容解码，已接入导入和阅读页。
- PDF：已接入文件读取、页数识别、页码导航和阅读状态保存；当前构建尚未接入真实 PDF 渲染模块。
- EPUB：已提供目录/章节接口和阅读界面，当前构建尚未接入真实 EPUB 解包模块。

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

## 导入链路

- 桌面端：`FileDialog` 返回本地文件路径，`BookImporter` 复制原始文件到应用书库目录。
- 安卓端：已为 `content://` 文档 URI 建立导入入口，尝试通过 Qt/Android 文件引擎以只读流复制到应用书库。
- 真机注意：Android 文档 URI 的长期访问权限和不同文件管理器兼容性仍需要在平板上验证。

## 测试

当前自动化测试包括：

- `tst_bookrepository`：SQLite 初始化、书籍元数据和阅读位置保存。
- `tst_bookimporter`：TXT/PDF/EPUB 格式识别，以及从已打开文件流导入。
- `tst_txtdocument`：TXT 编码读取和章节识别。
- `tst_pdfdocument`：PDF 文件识别和页数解析。
- `tst_librarycontroller`：书架控制器导入本地文件并刷新书籍列表。

## 后续实现重点

- 在安卓真机上验证并加固文档 URI 持久权限处理。
- 接入 Qt PDF 或 MuPDF。
- 接入 EPUB zip 解包和 OPF/Nav 解析。
- 将阅读设置保存到 SQLite。
