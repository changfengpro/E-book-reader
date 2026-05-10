#pragma once

#include "book/BookImporter.h"
#include "book/BookRepository.h"
#include "storage/AppDatabase.h"
#include "storage/AppPaths.h"

#include <QObject>
#include <QUrl>
#include <QVariantList>
#include <memory>

class LibraryController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList books READ books NOTIFY booksChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
    explicit LibraryController(QObject *parent = nullptr);
    explicit LibraryController(const QString &rootPath, QObject *parent = nullptr);

    QVariantList books() const;
    QString lastError() const;

    Q_INVOKABLE bool refresh();
    Q_INVOKABLE bool importLocalFile(const QUrl &sourceUrl);

signals:
    void booksChanged();
    void lastErrorChanged();

private:
    bool initialize();
    void setLastError(const QString &error);
    QVariantMap toMap(const Book &book) const;

    AppPaths m_paths;
    std::unique_ptr<AppDatabase> m_database;
    std::unique_ptr<BookRepository> m_repository;
    std::unique_ptr<BookImporter> m_importer;
    QVariantList m_books;
    QString m_lastError;
};
