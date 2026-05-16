#pragma once

#include <QString>

class PdfPageRenderer {
public:
    explicit PdfPageRenderer(QString cacheRoot = {});

    // Renders `page` (1-based) of `filePath` at a target paper width of
    // `targetWidthPx` device pixels. Returns the absolute path to a cached PNG
    // on success, or an empty string on failure (see lastError()).
    QString renderPage(const QString &filePath, int page, int targetWidthPx);

    // Convenience overload that converts a UI zoom factor (1.0 == 100%) into a
    // sensible target pixel width. Kept for backwards compatibility with the
    // QML caller that still thinks in zoom factors.
    QString renderPage(const QString &filePath, int page, qreal zoom);

    QString lastError() const;
    QString cacheRoot() const;

private:
    QString outputPathFor(const QString &filePath, int page, int widthBucket) const;
    static int bucketWidth(int targetWidthPx);

    QString m_cacheRoot;
    QString m_lastError;
};
