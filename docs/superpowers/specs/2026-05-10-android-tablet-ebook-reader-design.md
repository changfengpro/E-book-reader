# 安卓平板电子书阅读器设计规格

## 背景

本项目从空目录开始，目标是构建一个运行在安卓平板上的本地电子书阅读器。第一版聚焦本地书库管理和稳定阅读，支持 `txt`、`pdf`、`epub` 三种格式。

技术路线采用 Qt 6 + QML/C++。QML 负责平板界面和交互，C++ 负责文件导入、格式解析、数据存储和阅读状态管理。

## 第一版目标

第一版要形成完整的本地阅读闭环：

1. 用户从安卓文件选择器导入本地书籍。
2. 应用识别格式并复制文件到私有书库目录。
3. 书架展示书籍信息、格式和阅读进度。
4. 用户点击书籍进入阅读页。
5. 阅读页按格式加载对应阅读组件。
6. 应用持续保存阅读进度。
7. 用户返回书架后看到最新进度。

## 范围

### 包含

- 书架首页：展示已导入书籍、封面或格式占位、文件名、格式、阅读进度、最近阅读时间。
- 文件导入：支持 `.txt`、`.pdf`、`.epub`。
- 阅读页：支持打开书籍、返回书架、记住进度。
- 阅读设置：支持字体大小、行距、主题、亮度入口和横竖屏适配。
- PDF 基础阅读：支持翻页、缩放、适配页面宽度。
- EPUB 基础阅读：支持章节目录、正文阅读和进度记录。
- TXT 基础阅读：支持基础编码识别、正文阅读和进度记录。

### 不包含

- 在线书城。
- 账号系统。
- 云同步。
- 多设备同步。
- OCR。
- 复杂批注、划线和笔记系统。
- 全文搜索。

## 产品体验

第一版采用书架优先体验。应用启动后默认进入书架页，用户可以直接看到本地书库、最近阅读进度和导入入口。

阅读页不作为启动首页，但要支持从书架快速恢复上次阅读位置。这样可以同时满足本地书库管理和沉浸阅读。

## 页面结构

### 书架页

书架页是默认首页，负责本地书库管理。

核心能力：

- 展示所有导入书籍。
- 按最近阅读时间排序。
- 支持按书名搜索。
- 支持按格式筛选。
- 显示每本书的阅读进度。
- 提供导入入口。

### 导入流程

导入流程调用安卓系统文件选择器。用户选择文件后，应用执行以下操作：

1. 获取文件 URI。
2. 判断扩展名和 MIME 类型。
3. 校验是否为支持格式。
4. 复制文件到应用私有书库目录。
5. 生成书籍元数据。
6. 写入 SQLite。
7. 返回书架并刷新列表。

导入失败时，应给出明确错误提示，例如格式不支持、文件无法读取、复制失败或数据库写入失败。

### 阅读页

阅读页是统一容器，外层交互保持一致，内部根据格式切换阅读组件。

统一操作包括：

- 返回书架。
- 显示当前书名。
- 打开目录或页码导航。
- 打开阅读设置。
- 保存阅读进度。

内部组件包括：

- `TxtReader.qml`
- `PdfReader.qml`
- `EpubReader.qml`

### 设置页

设置页用于管理全局阅读偏好和本地缓存。

第一版包含：

- 默认字体大小。
- 默认行距。
- 默认主题。
- 缓存清理。
- 书库位置说明。

## 技术架构

### UI 层

UI 层使用 QML 实现。

建议文件结构：

```text
qml/
  Main.qml
  pages/
    LibraryPage.qml
    ReaderPage.qml
    SettingsPage.qml
  readers/
    TxtReader.qml
    PdfReader.qml
    EpubReader.qml
  components/
    BookCard.qml
    ReaderToolbar.qml
    ImportButton.qml
    EmptyLibraryView.qml
```

职责划分：

- `Main.qml`：应用入口、窗口尺寸适配和页面路由。
- `LibraryPage.qml`：书架列表、搜索、筛选和导入入口。
- `ReaderPage.qml`：阅读容器，根据书籍格式加载具体阅读组件。
- `SettingsPage.qml`：全局设置。
- `BookCard.qml`：书架卡片。
- `ReaderToolbar.qml`：阅读页工具栏。

### C++ 后端层

C++ 后端负责稳定性和平台能力封装。

建议模块：

```text
src/
  main.cpp
  book/
    Book.h
    BookRepository.h
    BookRepository.cpp
    BookImporter.h
    BookImporter.cpp
  reader/
    ReaderController.h
    ReaderController.cpp
    TxtDocument.h
    TxtDocument.cpp
    PdfDocument.h
    PdfDocument.cpp
    EpubDocument.h
    EpubDocument.cpp
  storage/
    AppDatabase.h
    AppDatabase.cpp
    AppPaths.h
    AppPaths.cpp
```

职责划分：

- `BookRepository`：读写书籍元数据、阅读进度和设置。
- `BookImporter`：处理文件导入、格式识别和文件复制。
- `ReaderController`：向 QML 提供统一阅读接口。
- `TxtDocument`：读取文本、识别编码、提供正文内容。
- `PdfDocument`：封装 PDF 页面读取、页数和渲染入口。
- `EpubDocument`：解包 EPUB、解析目录和章节。
- `AppDatabase`：封装 SQLite 初始化和迁移。
- `AppPaths`：统一管理私有书库目录和缓存目录。

## 文件格式处理

### 统一导入入口

`BookImporter` 是所有格式的统一入口。

输入：

- 安卓文件选择器返回的 URI。
- 文件显示名。
- MIME 类型。

输出：

- 导入成功后的 `Book` 记录。
- 失败时返回错误类型和可展示错误信息。

导入过程应避免直接依赖原始 URI。文件导入后统一复制到应用私有目录，后续阅读只访问私有目录中的副本。

### TXT

TXT 第一版目标是可靠显示正文。

支持：

- UTF-8。
- UTF-16。
- GBK / GB18030 基础识别。

阅读进度保存方式：

- 字符偏移。
- 百分比。

第一版不强制识别章节目录。后续可以增加章节规则，例如按「第 x 章」或 Markdown 标题拆分。

### PDF

PDF 第一版目标是稳定打开和翻页。

优先方案：

- 优先使用 Qt PDF 模块。
- 如果安卓构建或运行效果不满足要求，再切换到 MuPDF。

能力范围：

- 获取页数。
- 渲染单页。
- 上一页和下一页。
- 缩放。
- 适配页面宽度。

阅读进度保存方式：

- 当前页码。
- 缩放比例。
- 页面内滚动位置。

### EPUB

EPUB 第一版目标是能读取目录和章节正文。

处理流程：

1. 将 EPUB 作为 zip 包读取。
2. 解析 `META-INF/container.xml`。
3. 定位 OPF 文件。
4. 读取 manifest、spine 和元数据。
5. 解析 NCX 或 EPUB 3 Nav 目录。
6. 读取章节 HTML。
7. 清洗章节内容并交给 QML 展示。

阅读进度保存方式：

- 章节 ID。
- 章节内位置。
- 全书进度百分比。

第一版不追求复杂 CSS 还原，优先保证内容可读、目录可用、进度可恢复。

## 数据存储

### SQLite

SQLite 保存结构化状态。

建议表：

```sql
CREATE TABLE books (
  id TEXT PRIMARY KEY,
  title TEXT NOT NULL,
  author TEXT,
  format TEXT NOT NULL,
  original_name TEXT NOT NULL,
  file_path TEXT NOT NULL,
  cover_path TEXT,
  imported_at INTEGER NOT NULL,
  last_opened_at INTEGER,
  progress REAL NOT NULL DEFAULT 0
);

CREATE TABLE reading_positions (
  book_id TEXT PRIMARY KEY,
  locator TEXT NOT NULL,
  updated_at INTEGER NOT NULL
);

CREATE TABLE settings (
  key TEXT PRIMARY KEY,
  value TEXT NOT NULL
);
```

`reading_positions.locator` 使用 JSON 字符串保存不同格式的位置，例如：

```json
{
  "type": "pdf",
  "page": 42,
  "zoom": 1.2,
  "offsetY": 128
}
```

### 文件目录

应用私有目录建议分为：

```text
books/
  <book-id>/
    original.txt
    original.pdf
    original.epub
cache/
  covers/
  pdf-thumbnails/
  epub/
```

这样可以避免原始文件名冲突，也便于删除单本书时清理相关缓存。

## 错误处理

第一版需要覆盖以下错误：

- 用户取消文件选择。
- 文件格式不支持。
- 文件无法读取。
- 私有目录写入失败。
- SQLite 写入失败。
- TXT 编码识别失败。
- PDF 打开失败。
- EPUB 包结构不完整。
- EPUB 目录为空或章节缺失。

错误提示应面向用户，例如「这个文件不是受支持的 EPUB 文件」；日志中保留技术原因，方便调试。

## 测试计划

### 单元测试

- `BookImporter` 可以识别 `txt`、`pdf`、`epub`。
- `BookRepository` 可以写入和读取书籍记录。
- `BookRepository` 可以保存和恢复阅读位置。
- `TxtDocument` 可以读取 UTF-8、UTF-16、GBK 文本。
- `EpubDocument` 可以解析最小 EPUB 样例的目录和章节。

### 集成测试

- 导入 TXT 后可以在书架显示，并能进入阅读页。
- 导入 PDF 后可以显示页数，并能翻页。
- 导入 EPUB 后可以显示目录，并能打开章节。
- 退出阅读页后重新打开，能恢复上次位置。

### UI 验证

- 安卓平板横屏下，书架页不重叠。
- 安卓平板竖屏下，书架页不重叠。
- 阅读页工具栏不遮挡正文。
- 字体大小和行距调整后，正文区域仍可正常滚动或翻页。

## 实现顺序建议

1. 搭建 Qt 6 + QML 项目骨架。
2. 建立 SQLite、路径管理和基础数据模型。
3. 实现书架页和导入入口。
4. 实现 TXT 导入与阅读。
5. 实现阅读进度保存和恢复。
6. 实现 PDF 阅读。
7. 实现 EPUB 解析和阅读。
8. 补充设置页。
9. 做安卓平板横竖屏验证。

## 已确认决策

- 体验方向：书架优先。
- 技术路线：Qt 6 + QML/C++ 原生应用。
- 首版格式：TXT、PDF、EPUB。
- 首版重点：本地导入、书架管理、稳定阅读、进度恢复。
- 首版排除：账号、云同步、在线书城、OCR、复杂批注。

## 风险与后续观察

- Qt PDF 在安卓端的能力需要尽早验证。如果构建或渲染表现不稳定，应切换到 MuPDF。
- EPUB 的 CSS 和复杂排版可能差异较大，第一版只承诺内容可读和目录可用。
- TXT 编码识别无法做到 100% 准确，失败时需要允许用户手动选择编码。
- 大文件 PDF 和长篇 TXT 需要避免一次性加载全部内容，后续实现中应采用分页或分块读取。
