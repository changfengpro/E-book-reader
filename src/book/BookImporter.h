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
