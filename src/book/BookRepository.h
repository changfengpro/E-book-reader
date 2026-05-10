#pragma once

#include "book/Book.h"

#include <QSqlDatabase>
#include <QString>
#include <QVector>

class BookRepository {
public:
    explicit BookRepository(QSqlDatabase database);

    bool saveBook(const Book &book);
    QVector<Book> books() const;
    bool saveReadingPosition(const QString &bookId, const QString &locatorJson);
    QString readingPosition(const QString &bookId) const;
    QString lastError() const;

private:
    QSqlDatabase m_database;
    mutable QString m_lastError;
};
