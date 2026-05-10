#include "LibraryController.h"

#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>

LibraryController::LibraryController(QObject *parent)
    : LibraryController(QString(), parent)
{
}

LibraryController::LibraryController(const QString &rootPath, QObject *parent)
    : QObject(parent)
    , m_paths(rootPath)
{
    initialize();
}

QVariantList LibraryController::books() const
{
    return m_books;
}

QString LibraryController::lastError() const
{
    return m_lastError;
}

bool LibraryController::refresh()
{
    if (!m_repository && !initialize()) {
        return false;
    }

    QVariantList nextBooks;
    for (const Book &book : m_repository->books()) {
        nextBooks.append(toMap(book));
    }

    m_books = nextBooks;
    emit booksChanged();
    return true;
}

bool LibraryController::importLocalFile(const QUrl &sourceUrl)
{
    if (!m_repository && !initialize()) {
        return false;
    }

    if (sourceUrl.isLocalFile()) {
        const QString sourcePath = sourceUrl.toLocalFile();
        if (sourcePath.isEmpty()) {
            setLastError(QStringLiteral("请选择本地文件"));
            return false;
        }

        const QString mimeType = QMimeDatabase().mimeTypeForFile(QFileInfo(sourcePath)).name();
        return saveImportedBook(m_importer->importLocalFile(sourcePath, mimeType));
    }

    if (sourceUrl.scheme() == QStringLiteral("content")) {
        return importContentUri(sourceUrl);
    }

    setLastError(QStringLiteral("请选择本地文件或 Android 文档"));
    return false;
}

bool LibraryController::initialize()
{
    if (!m_paths.ensureDirectories()) {
        setLastError(QStringLiteral("无法创建应用数据目录"));
        return false;
    }

    m_database = std::make_unique<AppDatabase>(m_paths.databasePath());
    if (!m_database->open()) {
        setLastError(m_database->lastError());
        return false;
    }

    m_repository = std::make_unique<BookRepository>(m_database->database());
    m_importer = std::make_unique<BookImporter>(m_paths);
    return refresh();
}

bool LibraryController::importContentUri(const QUrl &sourceUrl)
{
#ifdef Q_OS_ANDROID
    QFile sourceFile(sourceUrl.toString());
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        setLastError(QStringLiteral("无法读取 Android 文档"));
        return false;
    }

    QString displayName = sourceUrl.fileName();
    if (displayName.isEmpty()) {
        displayName = QStringLiteral("android-document");
    }

    const QString mimeType = QMimeDatabase().mimeTypeForFile(displayName).name();
    return saveImportedBook(m_importer->importFromReadableFile(displayName, mimeType, sourceFile));
#else
    Q_UNUSED(sourceUrl);
    setLastError(QStringLiteral("当前平台不支持 Android 文档 URI"));
    return false;
#endif
}

bool LibraryController::saveImportedBook(const Book &book)
{
    if (book.id.isEmpty()) {
        setLastError(m_importer->lastError());
        return false;
    }

    if (!m_repository->saveBook(book)) {
        setLastError(m_repository->lastError());
        return false;
    }

    setLastError(QString());
    return refresh();
}

void LibraryController::setLastError(const QString &error)
{
    if (m_lastError == error) {
        return;
    }

    m_lastError = error;
    emit lastErrorChanged();
}

QVariantMap LibraryController::toMap(const Book &book) const
{
    return {
        { QStringLiteral("bookId"), book.id },
        { QStringLiteral("title"), book.title },
        { QStringLiteral("format"), book.format },
        { QStringLiteral("progress"), book.progress },
        { QStringLiteral("filePath"), book.filePath },
        { QStringLiteral("originalName"), book.originalName }
    };
}
