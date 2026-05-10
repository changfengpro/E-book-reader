#pragma once

#include <QSqlDatabase>
#include <QString>

class AppDatabase {
public:
    explicit AppDatabase(QString databasePath);
    ~AppDatabase();

    bool open();
    QSqlDatabase database() const;
    QString lastError() const;

private:
    bool migrate();

    QString m_connectionName;
    QString m_databasePath;
    QString m_lastError;
};
