#pragma once

#include "book/Book.h"
#include "storage/AppPaths.h"

#include <QObject>
#include <QFile>
#include <QString>

class BookImporter : public QObject {
    Q_OBJECT

public:
    explicit BookImporter(AppPaths paths, QObject *parent = nullptr);

    static QString detectFormat(const QString &fileName, const QString &mimeType);
    Book importLocalFile(const QString &sourcePath, const QString &mimeType);
    Book importFromReadableFile(const QString &displayName, const QString &mimeType, QFile &sourceFile);
    QString lastError() const;

private:
    Book copyIntoLibrary(const QString &displayName, const QString &format, QFile &sourceFile);

    AppPaths m_paths;
    QString m_lastError;
};
