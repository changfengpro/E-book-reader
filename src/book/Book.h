#pragma once

#include <QDateTime>
#include <QString>

struct Book {
    QString id;
    QString title;
    QString author;
    QString format;
    QString originalName;
    QString filePath;
    QString coverPath;
    QDateTime importedAt;
    QDateTime lastOpenedAt;
    double progress = 0.0;
};
