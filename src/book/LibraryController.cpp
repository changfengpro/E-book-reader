#include "LibraryController.h"

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

    const QString sourcePath = sourceUrl.isLocalFile() ? sourceUrl.toLocalFile() : sourceUrl.toString();
    if (sourcePath.isEmpty()) {
        setLastError(QStringLiteral("请选择本地文件"));
        return false;
    }

    const QString mimeType = QMimeDatabase().mimeTypeForFile(QFileInfo(sourcePath)).name();
    Book book = m_importer->importLocalFile(sourcePath, mimeType);
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
