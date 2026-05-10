#pragma once

#include "book/BookImporter.h"
#include "book/BookRepository.h"
#include "storage/AppDatabase.h"
#include "storage/AppPaths.h"

#include <QObject>
#include <QList>
#include <QUrl>
#include <QVariantList>
#include <memory>

class LibraryController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList books READ books NOTIFY booksChanged)
    Q_PROPERTY(int totalBookCount READ totalBookCount NOTIFY totalBookCountChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    Q_PROPERTY(QString formatFilter READ formatFilter WRITE setFormatFilter NOTIFY formatFilterChanged)

public:
    explicit LibraryController(QObject *parent = nullptr);
    explicit LibraryController(const QString &rootPath, QObject *parent = nullptr);

    QVariantList books() const;
    int totalBookCount() const;
    QString lastError() const;
    QString searchText() const;
    QString formatFilter() const;
    void setSearchText(const QString &searchText);
    void setFormatFilter(const QString &formatFilter);

    Q_INVOKABLE bool refresh();
    Q_INVOKABLE bool importLocalFile(const QUrl &sourceUrl);

signals:
    void booksChanged();
    void totalBookCountChanged();
    void lastErrorChanged();
    void searchTextChanged();
    void formatFilterChanged();

private:
    bool initialize();
    bool importContentUri(const QUrl &sourceUrl);
    bool saveImportedBook(const Book &book);
    void rebuildVisibleBooks();
    bool acceptsBook(const Book &book) const;
    void setLastError(const QString &error);
    QVariantMap toMap(const Book &book) const;

    AppPaths m_paths;
    std::unique_ptr<AppDatabase> m_database;
    std::unique_ptr<BookRepository> m_repository;
    std::unique_ptr<BookImporter> m_importer;
    QList<Book> m_allBooks;
    QVariantList m_books;
    QString m_lastError;
    QString m_searchText;
    QString m_formatFilter = QStringLiteral("全部");
};
