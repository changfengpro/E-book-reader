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
