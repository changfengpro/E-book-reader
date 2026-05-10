#include "BookImporter.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QUuid>

#include <utility>

BookImporter::BookImporter(AppPaths paths, QObject *parent)
    : QObject(parent)
    , m_paths(std::move(paths))
{
}

QString BookImporter::detectFormat(const QString &fileName, const QString &mimeType)
{
    const QString suffix = QFileInfo(fileName).suffix().toLower();
    const QString normalizedMime = mimeType.toLower();

    if (suffix == "txt" || normalizedMime == "text/plain") {
        return QStringLiteral("txt");
    }
    if (suffix == "pdf" || normalizedMime == "application/pdf") {
        return QStringLiteral("pdf");
    }
    if (suffix == "epub" || normalizedMime == "application/epub+zip") {
        return QStringLiteral("epub");
    }

    return {};
}

Book BookImporter::importLocalFile(const QString &sourcePath, const QString &mimeType)
{
    m_lastError.clear();

    QFileInfo sourceInfo(sourcePath);
    if (!sourceInfo.exists() || !sourceInfo.isFile()) {
        m_lastError = QStringLiteral("文件不存在或无法读取");
        return {};
    }

    QString effectiveMime = mimeType;
    if (effectiveMime.isEmpty()) {
        effectiveMime = QMimeDatabase().mimeTypeForFile(sourceInfo).name();
    }

    const QString format = detectFormat(sourceInfo.fileName(), effectiveMime);
    if (format.isEmpty()) {
        m_lastError = QStringLiteral("不支持的文件格式");
        return {};
    }

    if (!m_paths.ensureDirectories()) {
        m_lastError = QStringLiteral("无法创建书库目录");
        return {};
    }

    const QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    const QString bookDirectory = m_paths.bookDirectory(id);
    if (!QDir().mkpath(bookDirectory)) {
        m_lastError = QStringLiteral("无法创建书籍目录");
        return {};
    }

    const QString targetPath = QDir(bookDirectory).filePath(QStringLiteral("original.%1").arg(format));
    if (!QFile::copy(sourcePath, targetPath)) {
        m_lastError = QStringLiteral("无法复制书籍文件");
        return {};
    }

    Book book;
    book.id = id;
    book.title = sourceInfo.completeBaseName();
    book.format = format;
    book.originalName = sourceInfo.fileName();
    book.filePath = targetPath;
    book.importedAt = QDateTime::currentDateTimeUtc();
    return book;
}

QString BookImporter::lastError() const
{
    return m_lastError;
}
