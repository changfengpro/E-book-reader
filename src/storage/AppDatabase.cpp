#include "AppDatabase.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QUuid>

#include <utility>

AppDatabase::AppDatabase(QString databasePath)
    : m_connectionName(QStringLiteral("ebook_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces)))
    , m_databasePath(std::move(databasePath))
{
}

AppDatabase::~AppDatabase()
{
    if (!m_connectionName.isEmpty()) {
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName, false);
            if (db.isValid()) {
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool AppDatabase::open()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    db.setDatabaseName(m_databasePath);

    if (!db.open()) {
        m_lastError = db.lastError().text();
        return false;
    }

    return migrate();
}

QSqlDatabase AppDatabase::database() const
{
    return QSqlDatabase::database(m_connectionName);
}

QString AppDatabase::lastError() const
{
    return m_lastError;
}

bool AppDatabase::migrate()
{
    QSqlDatabase db = database();
    QSqlQuery query(db);

    const QStringList statements = {
        QStringLiteral(R"SQL(
            CREATE TABLE IF NOT EXISTS books (
              id TEXT PRIMARY KEY,
              title TEXT NOT NULL,
              author TEXT,
              format TEXT NOT NULL,
              original_name TEXT NOT NULL,
              file_path TEXT NOT NULL,
              cover_path TEXT,
              imported_at INTEGER NOT NULL,
              last_opened_at INTEGER,
              progress REAL NOT NULL DEFAULT 0
            )
        )SQL"),
        QStringLiteral(R"SQL(
            CREATE TABLE IF NOT EXISTS reading_positions (
              book_id TEXT PRIMARY KEY,
              locator TEXT NOT NULL,
              updated_at INTEGER NOT NULL
            )
        )SQL"),
        QStringLiteral(R"SQL(
            CREATE TABLE IF NOT EXISTS settings (
              key TEXT PRIMARY KEY,
              value TEXT NOT NULL
            )
        )SQL")
    };

    for (const QString &statement : statements) {
        if (!query.exec(statement)) {
            m_lastError = query.lastError().text();
            return false;
        }
    }

    return true;
}
