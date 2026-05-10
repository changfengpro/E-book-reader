#include "BookRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <utility>

namespace {
qint64 toMillis(const QDateTime &dateTime)
{
    return dateTime.isValid() ? dateTime.toMSecsSinceEpoch() : 0;
}

QDateTime fromMillis(qint64 millis)
{
    return millis > 0 ? QDateTime::fromMSecsSinceEpoch(millis) : QDateTime();
}
}

BookRepository::BookRepository(QSqlDatabase database)
    : m_database(std::move(database))
{
}

bool BookRepository::saveBook(const Book &book)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(R"SQL(
        INSERT INTO books (
            id, title, author, format, original_name, file_path, cover_path,
            imported_at, last_opened_at, progress
        )
        VALUES (
            :id, :title, :author, :format, :original_name, :file_path, :cover_path,
            :imported_at, :last_opened_at, :progress
        )
        ON CONFLICT(id) DO UPDATE SET
            title = excluded.title,
            author = excluded.author,
            format = excluded.format,
            original_name = excluded.original_name,
            file_path = excluded.file_path,
            cover_path = excluded.cover_path,
            imported_at = excluded.imported_at,
            last_opened_at = excluded.last_opened_at,
            progress = excluded.progress
    )SQL"));
    query.bindValue(QStringLiteral(":id"), book.id);
    query.bindValue(QStringLiteral(":title"), book.title);
    query.bindValue(QStringLiteral(":author"), book.author);
    query.bindValue(QStringLiteral(":format"), book.format);
    query.bindValue(QStringLiteral(":original_name"), book.originalName);
    query.bindValue(QStringLiteral(":file_path"), book.filePath);
    query.bindValue(QStringLiteral(":cover_path"), book.coverPath);
    query.bindValue(QStringLiteral(":imported_at"), toMillis(book.importedAt));
    query.bindValue(QStringLiteral(":last_opened_at"), toMillis(book.lastOpenedAt));
    query.bindValue(QStringLiteral(":progress"), book.progress);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    return true;
}

QVector<Book> BookRepository::books() const
{
    QVector<Book> result;
    QSqlQuery query(m_database);
    if (!query.exec(QStringLiteral(R"SQL(
        SELECT id, title, author, format, original_name, file_path, cover_path,
               imported_at, last_opened_at, progress
        FROM books
        ORDER BY last_opened_at DESC, imported_at DESC
    )SQL"))) {
        m_lastError = query.lastError().text();
        return result;
    }

    while (query.next()) {
        Book book;
        book.id = query.value(0).toString();
        book.title = query.value(1).toString();
        book.author = query.value(2).toString();
        book.format = query.value(3).toString();
        book.originalName = query.value(4).toString();
        book.filePath = query.value(5).toString();
        book.coverPath = query.value(6).toString();
        book.importedAt = fromMillis(query.value(7).toLongLong());
        book.lastOpenedAt = fromMillis(query.value(8).toLongLong());
        book.progress = query.value(9).toDouble();
        result.append(book);
    }

    return result;
}

bool BookRepository::saveReadingPosition(const QString &bookId, const QString &locatorJson)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(R"SQL(
        INSERT INTO reading_positions (book_id, locator, updated_at)
        VALUES (:book_id, :locator, :updated_at)
        ON CONFLICT(book_id) DO UPDATE SET
            locator = excluded.locator,
            updated_at = excluded.updated_at
    )SQL"));
    query.bindValue(QStringLiteral(":book_id"), bookId);
    query.bindValue(QStringLiteral(":locator"), locatorJson);
    query.bindValue(QStringLiteral(":updated_at"), QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    return true;
}

QString BookRepository::readingPosition(const QString &bookId) const
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("SELECT locator FROM reading_positions WHERE book_id = :book_id"));
    query.bindValue(QStringLiteral(":book_id"), bookId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return {};
    }

    return query.next() ? query.value(0).toString() : QString();
}

QString BookRepository::lastError() const
{
    return m_lastError;
}
