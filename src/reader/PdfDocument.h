#pragma once

#include <QString>

class PdfDocument {
public:
    bool load(const QString &filePath);
    int pageCount() const;
    QString lastError() const;

private:
    int m_pageCount = 0;
    QString m_lastError;
};
