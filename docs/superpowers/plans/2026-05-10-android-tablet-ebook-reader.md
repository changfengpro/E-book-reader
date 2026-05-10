# 安卓平板电子书阅读器实现计划

> **面向 AI 代理的工作者：** 必需子技能：使用 superpowers:subagent-driven-development（推荐）或 superpowers:executing-plans 逐任务实现此计划。步骤使用复选框（`- [ ]`）语法来跟踪进度。

**目标：** 构建一个 Qt 6 + QML/C++ 安卓平板电子书阅读器，支持本地导入和阅读 TXT、PDF、EPUB 文件，并保存阅读进度。

**架构：** 使用 CMake 创建 Qt/QML 应用骨架。QML 负责书架、导入入口、阅读页和设置页；C++ 后端负责 SQLite、路径管理、文件导入、文档解析和统一阅读状态。第一轮实现以桌面可运行和安卓可打包结构为目标，避免在缺少 Android SDK 的机器上阻塞核心功能。

**技术栈：** Qt 6、QML、C++17、CMake、SQLite、Qt Test。PDF 优先预留 Qt PDF 接口，EPUB 使用 zip + XML 解析边界，TXT 使用 Qt 文本编码能力和分块读取策略。

---

## 文件结构

计划创建或维护以下文件：

```text
CMakeLists.txt
src/
  main.cpp
  book/
    Book.h
    BookImporter.h
    BookImporter.cpp
    BookRepository.h
    BookRepository.cpp
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
    EmptyLibraryView.qml
    ImportButton.qml
    ReaderToolbar.qml
tests/
  CMakeLists.txt
  tst_bookrepository.cpp
  tst_bookimporter.cpp
  tst_txtdocument.cpp
  tst_epubdocument.cpp
```

## 任务 1：创建 Qt/QML 项目骨架

**文件：**
- 创建：`CMakeLists.txt`
- 创建：`src/main.cpp`
- 创建：`qml/Main.qml`
- 创建：`qml/pages/LibraryPage.qml`
- 创建：`qml/pages/ReaderPage.qml`
- 创建：`qml/pages/SettingsPage.qml`

- [ ] **步骤 1：编写最小 CMake 项目**

创建 `CMakeLists.txt`，定义 Qt 6 应用、C++17 和 QML 模块：

```cmake
cmake_minimum_required(VERSION 3.21)

project(TabletEbookReader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Sql Test)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(TabletEbookReader
    src/main.cpp
)

qt_add_qml_module(TabletEbookReader
    URI EbookReader
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/pages/LibraryPage.qml
        qml/pages/ReaderPage.qml
        qml/pages/SettingsPage.qml
)

target_link_libraries(TabletEbookReader
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::Sql
)
```

- [ ] **步骤 2：实现应用入口**

创建 `src/main.cpp`：

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("EbookReader", "Main");

    return app.exec();
}
```

- [ ] **步骤 3：实现最小页面路由**

创建 `qml/Main.qml`：

```qml
import QtQuick
import QtQuick.Controls
import EbookReader

ApplicationWindow {
    id: window
    width: 1280
    height: 800
    visible: true
    title: "E-Book Reader"

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: LibraryPage {
            onOpenSettings: stack.push(settingsPage)
            onOpenBook: function(bookId) {
                stack.push(readerPage, { bookId: bookId })
            }
        }
    }

    Component { id: settingsPage; SettingsPage {} }
    Component { id: readerPage; ReaderPage {} }
}
```

- [ ] **步骤 4：运行配置检查**

运行：

```powershell
cmake -S . -B build
```

预期：如果本机未安装 Qt 6，配置阶段报告找不到 `Qt6Config.cmake`；记录该环境限制。若 Qt 6 可用，则配置成功。

- [ ] **步骤 5：Commit**

```bash
git add CMakeLists.txt src/main.cpp qml/Main.qml qml/pages/LibraryPage.qml qml/pages/ReaderPage.qml qml/pages/SettingsPage.qml
git commit -m "feat(项目): 创建 Qt QML 应用骨架"
```

## 任务 2：建立数据模型、路径和 SQLite 基础设施

**文件：**
- 创建：`src/book/Book.h`
- 创建：`src/storage/AppPaths.h`
- 创建：`src/storage/AppPaths.cpp`
- 创建：`src/storage/AppDatabase.h`
- 创建：`src/storage/AppDatabase.cpp`
- 修改：`CMakeLists.txt`
- 创建：`tests/CMakeLists.txt`
- 创建：`tests/tst_bookrepository.cpp`

- [ ] **步骤 1：编写 `Book` 数据模型**

创建 `src/book/Book.h`：

```cpp
#pragma once

#include <QDateTime>
#include <QString>

struct Book {
    QString id;
    QString title;
    QString author;
    QString format;
    QString originalName;
    QString filePath;
    QString coverPath;
    QDateTime importedAt;
    QDateTime lastOpenedAt;
    double progress = 0.0;
};
```

- [ ] **步骤 2：实现路径管理**

创建 `src/storage/AppPaths.h`：

```cpp
#pragma once

#include <QString>

class AppPaths {
public:
    explicit AppPaths(QString rootPath = {});

    QString rootPath() const;
    QString databasePath() const;
    QString booksPath() const;
    QString cachePath() const;
    QString bookDirectory(const QString &bookId) const;
    bool ensureDirectories() const;

private:
    QString m_rootPath;
};
```

创建 `src/storage/AppPaths.cpp`：

```cpp
#include "AppPaths.h"

#include <QDir>
#include <QStandardPaths>

AppPaths::AppPaths(QString rootPath)
    : m_rootPath(rootPath.isEmpty()
          ? QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          : std::move(rootPath))
{
}

QString AppPaths::rootPath() const { return m_rootPath; }
QString AppPaths::databasePath() const { return QDir(m_rootPath).filePath("library.sqlite"); }
QString AppPaths::booksPath() const { return QDir(m_rootPath).filePath("books"); }
QString AppPaths::cachePath() const { return QDir(m_rootPath).filePath("cache"); }
QString AppPaths::bookDirectory(const QString &bookId) const { return QDir(booksPath()).filePath(bookId); }

bool AppPaths::ensureDirectories() const
{
    QDir root(m_rootPath);
    return root.mkpath(".") && root.mkpath("books") && root.mkpath("cache");
}
```

- [ ] **步骤 3：实现数据库初始化**

创建 `src/storage/AppDatabase.h`：

```cpp
#pragma once

#include <QSqlDatabase>
#include <QString>

class AppDatabase {
public:
    explicit AppDatabase(QString databasePath);
    ~AppDatabase();

    bool open();
    QSqlDatabase database() const;
    QString lastError() const;

private:
    bool migrate();

    QString m_connectionName;
    QString m_databasePath;
    QString m_lastError;
};
```

创建 `src/storage/AppDatabase.cpp`，迁移 SQL 必须包含 `books`、`reading_positions`、`settings` 三张表。

- [ ] **步骤 4：编写数据库初始化测试**

创建 `tests/tst_bookrepository.cpp`，先验证 `AppPaths` 和 `AppDatabase` 能创建数据库文件：

```cpp
#include <QtTest>
#include "storage/AppDatabase.h"
#include "storage/AppPaths.h"

class BookRepositoryTest : public QObject {
    Q_OBJECT
private slots:
    void createsDatabaseFile();
};

void BookRepositoryTest::createsDatabaseFile()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    AppPaths paths(dir.path());
    QVERIFY(paths.ensureDirectories());

    AppDatabase database(paths.databasePath());
    QVERIFY2(database.open(), qPrintable(database.lastError()));
    QVERIFY(QFile::exists(paths.databasePath()));
}

QTEST_MAIN(BookRepositoryTest)
#include "tst_bookrepository.moc"
```

- [ ] **步骤 5：运行测试**

运行：

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

预期：Qt 6 可用时测试通过；Qt 6 不可用时记录环境缺失。

- [ ] **步骤 6：Commit**

```bash
git add CMakeLists.txt src/book/Book.h src/storage/AppPaths.* src/storage/AppDatabase.* tests/CMakeLists.txt tests/tst_bookrepository.cpp
git commit -m "feat(存储): 添加应用路径和 SQLite 初始化"
```

## 任务 3：实现书籍仓库

**文件：**
- 创建：`src/book/BookRepository.h`
- 创建：`src/book/BookRepository.cpp`
- 修改：`tests/tst_bookrepository.cpp`
- 修改：`CMakeLists.txt`

- [ ] **步骤 1：扩展失败测试**

在 `tests/tst_bookrepository.cpp` 中添加：

```cpp
void savesAndLoadsBook();
void savesAndLoadsReadingPosition();
```

测试应构造 `Book`，调用 `BookRepository::saveBook()`，再调用 `BookRepository::books()` 验证字段一致；阅读位置用 JSON 字符串验证。

- [ ] **步骤 2：实现仓库接口**

创建 `src/book/BookRepository.h`：

```cpp
#pragma once

#include "book/Book.h"

#include <QSqlDatabase>
#include <QString>
#include <QVector>

class BookRepository {
public:
    explicit BookRepository(QSqlDatabase database);

    bool saveBook(const Book &book);
    QVector<Book> books() const;
    bool saveReadingPosition(const QString &bookId, const QString &locatorJson);
    QString readingPosition(const QString &bookId) const;
    QString lastError() const;

private:
    QSqlDatabase m_database;
    mutable QString m_lastError;
};
```

- [ ] **步骤 3：实现最小 SQL 读写**

创建 `src/book/BookRepository.cpp`，使用参数绑定，不拼接用户输入。`books()` 按 `last_opened_at DESC, imported_at DESC` 排序。

- [ ] **步骤 4：运行仓库测试**

运行：

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

预期：仓库相关测试通过。

- [ ] **步骤 5：Commit**

```bash
git add src/book/BookRepository.* tests/tst_bookrepository.cpp CMakeLists.txt
git commit -m "feat(书库): 添加书籍仓库读写能力"
```

## 任务 4：实现导入服务和书架 UI 绑定

**文件：**
- 创建：`src/book/BookImporter.h`
- 创建：`src/book/BookImporter.cpp`
- 创建：`tests/tst_bookimporter.cpp`
- 修改：`qml/pages/LibraryPage.qml`
- 创建：`qml/components/BookCard.qml`
- 创建：`qml/components/EmptyLibraryView.qml`
- 创建：`qml/components/ImportButton.qml`
- 修改：`CMakeLists.txt`

- [ ] **步骤 1：编写格式识别测试**

创建 `tests/tst_bookimporter.cpp`：

```cpp
#include <QtTest>
#include "book/BookImporter.h"

class BookImporterTest : public QObject {
    Q_OBJECT
private slots:
    void detectsSupportedFormats();
};

void BookImporterTest::detectsSupportedFormats()
{
    QCOMPARE(BookImporter::detectFormat("novel.txt", "text/plain"), QString("txt"));
    QCOMPARE(BookImporter::detectFormat("manual.pdf", "application/pdf"), QString("pdf"));
    QCOMPARE(BookImporter::detectFormat("book.epub", "application/epub+zip"), QString("epub"));
    QCOMPARE(BookImporter::detectFormat("archive.zip", "application/zip"), QString());
}

QTEST_MAIN(BookImporterTest)
#include "tst_bookimporter.moc"
```

- [ ] **步骤 2：实现导入接口**

创建 `src/book/BookImporter.h`：

```cpp
#pragma once

#include "book/Book.h"
#include "storage/AppPaths.h"

#include <QObject>
#include <QString>

class BookImporter : public QObject {
    Q_OBJECT
public:
    explicit BookImporter(AppPaths paths, QObject *parent = nullptr);

    static QString detectFormat(const QString &fileName, const QString &mimeType);
    Book importLocalFile(const QString &sourcePath, const QString &mimeType);
    QString lastError() const;

private:
    AppPaths m_paths;
    QString m_lastError;
};
```

- [ ] **步骤 3：实现本地路径导入**

`BookImporter::importLocalFile()` 生成 UUID，创建 `books/<id>/`，复制原始文件为 `original.<format>`，返回 `Book`。安卓 URI 适配放在 QML 文件选择器接入步骤中处理。

- [ ] **步骤 4：实现书架页面静态绑定**

`LibraryPage.qml` 先使用 `ListModel` 呈现书架布局，保留 `openBook(string bookId)`、`openSettings()`、`importRequested()` 信号。

- [ ] **步骤 5：运行测试**

运行：

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

预期：导入格式识别测试通过。

- [ ] **步骤 6：Commit**

```bash
git add src/book/BookImporter.* tests/tst_bookimporter.cpp qml/pages/LibraryPage.qml qml/components/BookCard.qml qml/components/EmptyLibraryView.qml qml/components/ImportButton.qml CMakeLists.txt
git commit -m "feat(导入): 添加书籍导入服务和书架界面"
```

## 任务 5：实现 TXT 阅读能力

**文件：**
- 创建：`src/reader/TxtDocument.h`
- 创建：`src/reader/TxtDocument.cpp`
- 创建：`tests/tst_txtdocument.cpp`
- 创建：`qml/readers/TxtReader.qml`
- 修改：`qml/pages/ReaderPage.qml`
- 修改：`CMakeLists.txt`

- [ ] **步骤 1：编写 TXT 读取测试**

`tests/tst_txtdocument.cpp` 应覆盖 UTF-8 和 UTF-16 文本。GBK 样例使用固定字节数组写入临时文件。

```cpp
void readsUtf8Text();
void readsUtf16Text();
void readsGbkText();
```

- [ ] **步骤 2：实现 TXT 文档接口**

创建 `src/reader/TxtDocument.h`：

```cpp
#pragma once

#include <QString>

class TxtDocument {
public:
    bool load(const QString &filePath);
    QString text() const;
    int length() const;
    QString lastError() const;

private:
    QString decodeBytes(const QByteArray &data) const;

    QString m_text;
    QString m_lastError;
};
```

- [ ] **步骤 3：实现编码识别**

`decodeBytes()` 按 BOM 判断 UTF-8/UTF-16；无 BOM 时先尝试 UTF-8，再尝试系统可用的 GBK/GB18030 解码。若当前 Qt 版本缺少对应 codec，测试应明确跳过 GBK 分支并说明原因。

- [ ] **步骤 4：实现 QML 阅读组件**

`TxtReader.qml` 使用 `Flickable + TextArea/Text` 显示正文，提供 `positionChanged(real progress)` 信号。第一版允许滚动阅读。

- [ ] **步骤 5：运行测试**

运行：

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

预期：TXT 文档测试通过，GBK 在缺少 codec 时显示跳过信息。

- [ ] **步骤 6：Commit**

```bash
git add src/reader/TxtDocument.* tests/tst_txtdocument.cpp qml/readers/TxtReader.qml qml/pages/ReaderPage.qml CMakeLists.txt
git commit -m "feat(TXT): 添加文本阅读能力"
```

## 任务 6：实现阅读控制器和进度恢复

**文件：**
- 创建：`src/reader/ReaderController.h`
- 创建：`src/reader/ReaderController.cpp`
- 修改：`qml/pages/ReaderPage.qml`
- 创建：`qml/components/ReaderToolbar.qml`
- 修改：`tests/tst_bookrepository.cpp`
- 修改：`CMakeLists.txt`

- [ ] **步骤 1：扩展进度保存测试**

在仓库测试中添加 TXT locator 保存：

```cpp
const QString locator = R"({"type":"txt","offset":120,"progress":0.4})";
QVERIFY(repository.saveReadingPosition(book.id, locator));
QCOMPARE(repository.readingPosition(book.id), locator);
```

- [ ] **步骤 2：实现统一控制器接口**

创建 `ReaderController.h`：

```cpp
#pragma once

#include <QObject>
#include <QString>

class ReaderController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString bookId READ bookId WRITE setBookId NOTIFY bookChanged)
    Q_PROPERTY(QString format READ format NOTIFY bookChanged)
    Q_PROPERTY(QString title READ title NOTIFY bookChanged)
public:
    explicit ReaderController(QObject *parent = nullptr);

    QString bookId() const;
    void setBookId(const QString &bookId);
    QString format() const;
    QString title() const;

    Q_INVOKABLE void saveLocator(const QString &locatorJson);
    Q_INVOKABLE QString savedLocator() const;

signals:
    void bookChanged();
};
```

- [ ] **步骤 3：注册控制器到 QML**

在 `main.cpp` 中使用 `qmlRegisterType<ReaderController>("EbookReader.Backend", 1, 0, "ReaderController");`。

- [ ] **步骤 4：实现阅读页工具栏**

`ReaderToolbar.qml` 包含返回、标题、目录按钮、设置按钮。`ReaderPage.qml` 根据 `format` 加载 `TxtReader`、`PdfReader` 或 `EpubReader`。

- [ ] **步骤 5：运行测试**

运行：

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

预期：仓库进度测试通过，应用仍可启动。

- [ ] **步骤 6：Commit**

```bash
git add src/reader/ReaderController.* qml/pages/ReaderPage.qml qml/components/ReaderToolbar.qml tests/tst_bookrepository.cpp CMakeLists.txt
git commit -m "feat(阅读): 添加阅读控制器和进度恢复"
```

## 任务 7：实现 PDF 阅读占位接口和 Qt PDF 接入点

**文件：**
- 创建：`src/reader/PdfDocument.h`
- 创建：`src/reader/PdfDocument.cpp`
- 创建：`qml/readers/PdfReader.qml`
- 修改：`qml/pages/ReaderPage.qml`
- 修改：`CMakeLists.txt`

- [ ] **步骤 1：定义 PDF 文档边界**

创建 `src/reader/PdfDocument.h`：

```cpp
#pragma once

#include <QString>

class PdfDocument {
public:
    bool load(const QString &filePath);
    int pageCount() const;
    QString lastError() const;

private:
    int m_pageCount = 0;
    QString m_lastError;
};
```

- [ ] **步骤 2：实现可替换适配层**

`PdfDocument.cpp` 先封装文件存在性校验和错误信息。若 CMake 检测到 Qt PDF，则链接 `Qt6::Pdf` 并使用真实页数；否则返回「当前构建未启用 PDF 渲染模块」错误。

- [ ] **步骤 3：实现 PDF QML 组件**

`PdfReader.qml` 提供页码、上一页、下一页、缩放控件和错误展示区域。没有 Qt PDF 时显示清晰错误，不让应用崩溃。

- [ ] **步骤 4：运行构建**

运行：

```powershell
cmake --build build
```

预期：即使当前机器缺少 Qt PDF 模块，项目仍能构建到可展示错误状态。

- [ ] **步骤 5：Commit**

```bash
git add src/reader/PdfDocument.* qml/readers/PdfReader.qml qml/pages/ReaderPage.qml CMakeLists.txt
git commit -m "feat(PDF): 添加可替换的 PDF 阅读接口"
```

## 任务 8：实现 EPUB 解析基础能力

**文件：**
- 创建：`src/reader/EpubDocument.h`
- 创建：`src/reader/EpubDocument.cpp`
- 创建：`tests/tst_epubdocument.cpp`
- 创建：`qml/readers/EpubReader.qml`
- 修改：`qml/pages/ReaderPage.qml`
- 修改：`CMakeLists.txt`

- [ ] **步骤 1：编写最小 EPUB 解析测试**

测试构造一个临时 EPUB zip 文件，包含：

```text
META-INF/container.xml
OEBPS/content.opf
OEBPS/nav.xhtml
OEBPS/chapter1.xhtml
```

验证 `EpubDocument::chapters()` 返回 1 个章节，标题和正文文件路径正确。

- [ ] **步骤 2：定义 EPUB 接口**

创建 `src/reader/EpubDocument.h`：

```cpp
#pragma once

#include <QString>
#include <QVector>

struct EpubChapter {
    QString id;
    QString title;
    QString href;
};

class EpubDocument {
public:
    bool load(const QString &filePath);
    QVector<EpubChapter> chapters() const;
    QString chapterHtml(const QString &chapterId) const;
    QString lastError() const;

private:
    QVector<EpubChapter> m_chapters;
    QString m_lastError;
};
```

- [ ] **步骤 3：实现 zip 和 XML 解析边界**

优先使用 Qt 可用模块处理压缩包；若当前 Qt 环境没有公开 zip API，则实现 `EpubDocument` 的接口和错误状态，并在 CMake 中把真实解包依赖隔离为可替换模块。

- [ ] **步骤 4：实现 EPUB QML 组件**

`EpubReader.qml` 提供目录列表、章节标题、正文区域和上一章/下一章按钮。

- [ ] **步骤 5：运行测试**

运行：

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

预期：具备 zip 支持时 EPUB 测试通过；缺少 zip 支持时测试用 `QSKIP` 说明环境限制。

- [ ] **步骤 6：Commit**

```bash
git add src/reader/EpubDocument.* tests/tst_epubdocument.cpp qml/readers/EpubReader.qml qml/pages/ReaderPage.qml CMakeLists.txt
git commit -m "feat(EPUB): 添加电子书目录和章节解析"
```

## 任务 9：完善设置页和平板适配

**文件：**
- 修改：`qml/pages/SettingsPage.qml`
- 修改：`qml/pages/LibraryPage.qml`
- 修改：`qml/pages/ReaderPage.qml`
- 修改：`qml/components/BookCard.qml`

- [ ] **步骤 1：实现设置项界面**

`SettingsPage.qml` 包含主题、默认字体大小、默认行距、缓存清理入口和书库位置说明。

- [ ] **步骤 2：实现响应式布局**

`LibraryPage.qml` 使用宽度断点：

```qml
readonly property bool tabletWide: width >= 900
readonly property int columnCount: tabletWide ? 4 : 2
```

书架卡片使用稳定宽高和文本省略，避免横竖屏切换时重叠。

- [ ] **步骤 3：实现阅读页布局保护**

`ReaderPage.qml` 中工具栏固定高度，正文区域使用 `anchors.top: toolbar.bottom` 或 `ColumnLayout`，确保工具栏不遮挡正文。

- [ ] **步骤 4：手动运行界面**

运行：

```powershell
cmake --build build
.\build\TabletEbookReader.exe
```

预期：桌面窗口中横向和窄宽度调整时，书架与阅读页没有文本重叠。

- [ ] **步骤 5：Commit**

```bash
git add qml/pages/SettingsPage.qml qml/pages/LibraryPage.qml qml/pages/ReaderPage.qml qml/components/BookCard.qml
git commit -m "feat(界面): 完善设置页和平板布局适配"
```

## 任务 10：安卓构建配置和最终验证

**文件：**
- 修改：`CMakeLists.txt`
- 创建：`README.md`
- 创建：`docs/android-build.md`

- [ ] **步骤 1：补充项目说明**

`README.md` 包含项目目标、支持格式、桌面构建命令和安卓构建入口。

- [ ] **步骤 2：补充安卓构建说明**

`docs/android-build.md` 写明 Qt Creator 配置：

```text
1. 安装 Qt 6 Android 组件。
2. 配置 JDK、Android SDK、Android NDK。
3. 使用 Qt Creator 打开 CMakeLists.txt。
4. 选择 Android Kit。
5. 构建 APK 或 AAB。
```

- [ ] **步骤 3：运行全量验证**

运行：

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
git status --short
```

预期：Qt 6 可用时构建和测试通过，`git status --short` 为空。Qt 6 不可用时，在最终报告中说明本机 Qt 版本限制和需要安装的组件。

- [ ] **步骤 4：Commit**

```bash
git add CMakeLists.txt README.md docs/android-build.md
git commit -m "docs(安卓): 添加构建说明和最终验证记录"
```

## 覆盖关系

- 书架首页：任务 1、任务 4、任务 9。
- 文件导入：任务 4。
- TXT 阅读：任务 5、任务 6。
- PDF 阅读：任务 7。
- EPUB 阅读：任务 8。
- 阅读进度：任务 3、任务 6。
- SQLite 数据存储：任务 2、任务 3。
- 平板横竖屏适配：任务 9。
- 安卓构建说明：任务 10。

## 已知环境检查

当前机器可用 CMake 4.2.3。`qmake --version` 显示的是 Qt 5.15.2，且来自 Anaconda 路径；计划目标是 Qt 6，因此实现时需要优先确认 Qt 6 CMake 包是否安装。若本机暂时没有 Qt 6，可先完成代码结构和文档，构建验证需要安装 Qt 6 后执行。
