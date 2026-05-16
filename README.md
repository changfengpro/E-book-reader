# 安卓平板电子书阅读器

基于 Qt 6 + QML/C++ 的本地电子书阅读器，目标平台是安卓平板。

## 支持格式

- **TXT**：UTF-8 / UTF-16 / GB18030 自动识别，中文章节正则切分。
- **PDF**：使用 `Qt6::Pdf`（QPdfDocument）解析与渲染，按目标宽度分桶缓存为 PNG，**不再依赖外部 mutool 进程**，可直接在 Android 构建中工作。
- **EPUB**：使用 Qt 私有的 `QZipReader` 解包，按 EPUB 3 nav / EPUB 2 NCX 解析目录，章节 HTML 中的图片会被转成 base64 内嵌，便于 QML 直接渲染。

## 依赖

- Qt 6.5+，需启用以下模块：
  - `Qt6::Core` / `Qt6::Gui` / `Qt6::Quick` / `Qt6::Sql`
  - `Qt6::Pdf`（PDF 渲染）
  - `Qt6::GuiPrivate`（用于 `QZipReader` / `QZipWriter`，EPUB 解包）

EPUB 解析使用了 Qt 提供的私有头 `private/qzipreader_p.h`。这是 Qt 长期维护的内部能力，但 API 可能在大版本之间发生变化；升级 Qt 时需要重新验证。

## 本地构建（Windows / MinGW）

```powershell
$env:PATH="D:\Qt\Tools\mingw1310_64\bin;D:\Qt\6.10.2\mingw_64\bin;D:\Qt\Tools\Ninja;$env:PATH"
cmake -S . -B build-qt6 -G Ninja `
  -DCMAKE_PREFIX_PATH=D:\Qt\6.10.2\mingw_64
cmake --build build-qt6
ctest --test-dir build-qt6 --output-on-failure
```

## 测试

- `tst_bookrepository`：SQLite 元数据 / 阅读位置。
- `tst_bookimporter`：格式识别 + 流式导入。
- `tst_txtdocument`：TXT 编码与章节识别。
- `tst_pdfdocument`：通过 `QPdfWriter` 即时合成 PDF，验证 `QPdfDocument` 解析路径与错误处理。
- `tst_pdfpagerenderer`：渲染缓存命中 / miss。
- `tst_epubdocument`：通过 `QZipWriter` 即时合成 EPUB 3，验证 nav/spine 解析与章节 HTML 输出。
- `tst_librarycontroller`：导入 + 搜索 + 格式筛选。

测试**全部不再依赖外部可执行文件**，CI 上稳定可跑。

## 已知限制

- EPUB 章节渲染走 `Text { textFormat: RichText }`，对复杂 CSS / 嵌入字体支持有限；后续可切到 `WebView`。
- PDF 缩放当前按 64 px 宽度桶量化，更平滑的连续缩放需要在 QML 层做仿射变换。
- Android 真机的 `content://` URI 长期权限矩阵还需要在多个文件管理器上回归。
