#pragma once

#include <QString>

class PdfPageRenderer {
public:
    explicit PdfPageRenderer(QString cacheRoot = {});

    QString renderPage(const QString &filePath, int page, qreal zoom);
    QString lastError() const;

    static QString defaultMutoolPath();

private:
    QString outputPathFor(const QString &filePath, int page, qreal zoom) const;

    QString m_cacheRoot;
    QString m_lastError;
};
