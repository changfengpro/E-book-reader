#include "BookImporter.h"

#include <QDir>
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

    QFile sourceFile(sourcePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        m_lastError = QStringLiteral("文件无法打开");
        return {};
    }

    const QString format = detectFormat(sourceInfo.fileName(), effectiveMime);
    return copyIntoLibrary(sourceInfo.fileName(), format, sourceFile);
}

Book BookImporter::importFromReadableFile(const QString &displayName, const QString &mimeType, QFile &sourceFile)
{
    m_lastError.clear();
    return copyIntoLibrary(displayName, detectFormat(displayName, mimeType), sourceFile);
}

QString BookImporter::lastError() const
{
    return m_lastError;
}

Book BookImporter::copyIntoLibrary(const QString &displayName, const QString &format, QFile &sourceFile)
{
    if (format.isEmpty()) {
        m_lastError = QStringLiteral("不支持的文件格式");
        return {};
    }

    if (!sourceFile.isOpen() && !sourceFile.open(QIODevice::ReadOnly)) {
        m_lastError = QStringLiteral("文件无法打开");
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
    QFile targetFile(targetPath);
    if (!targetFile.open(QIODevice::WriteOnly)) {
        m_lastError = QStringLiteral("无法创建书籍文件");
        return {};
    }

    if (targetFile.write(sourceFile.readAll()) < 0) {
        m_lastError = QStringLiteral("无法复制书籍文件");
        return {};
    }

    const QFileInfo displayInfo(displayName);
    Book book;
    book.id = id;
    book.title = displayInfo.completeBaseName();
    book.format = format;
    book.originalName = displayInfo.fileName();
    book.filePath = targetPath;
    book.importedAt = QDateTime::currentDateTimeUtc();
    return book;
}
