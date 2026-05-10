#include "AppPaths.h"

#include <QDir>
#include <QStandardPaths>

#include <utility>

AppPaths::AppPaths(QString rootPath)
    : m_rootPath(rootPath.isEmpty()
          ? QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          : std::move(rootPath))
{
}

QString AppPaths::rootPath() const
{
    return m_rootPath;
}

QString AppPaths::databasePath() const
{
    return QDir(m_rootPath).filePath("library.sqlite");
}

QString AppPaths::booksPath() const
{
    return QDir(m_rootPath).filePath("books");
}

QString AppPaths::cachePath() const
{
    return QDir(m_rootPath).filePath("cache");
}

QString AppPaths::bookDirectory(const QString &bookId) const
{
    return QDir(booksPath()).filePath(bookId);
}

bool AppPaths::ensureDirectories() const
{
    QDir root(m_rootPath);
    return root.mkpath(".") && root.mkpath("books") && root.mkpath("cache");
}
