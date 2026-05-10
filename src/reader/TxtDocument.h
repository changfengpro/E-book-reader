#pragma once

#include <QString>

class TxtDocument {
public:
    bool load(const QString &filePath);
    QString text() const;
    int length() const;
    QString lastError() const;

private:
    QString decodeBytes(const QByteArray &data) const;

    QString m_text;
    QString m_lastError;
};
