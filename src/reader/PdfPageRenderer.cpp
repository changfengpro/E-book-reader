#include "PdfPageRenderer.h"

#include "PdfDocument.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QPdfDocument>
#include <QPdfDocumentRenderOptions>
#include <QSize>
#include <QSizeF>
#include <QStandardPaths>

PdfPageRenderer::PdfPageRenderer(QString cacheRoot)
    : m_cacheRoot(cacheRoot.isEmpty()
          ? QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation))
                .filePath(QStringLiteral("pdf-pages"))
          : std::move(cacheRoot))
{
}

int PdfPageRenderer::bucketWidth(int targetWidthPx)
{
    // Snap the requested width to a coarse bucket so that small layout-driven
    // resizes don't blow away the cache. 64 px buckets give us roughly the
    // human-perceptible quantum on a tablet.
    constexpr int bucket = 64;
    constexpr int minWidth = 256;
    constexpr int maxWidth = 4096;
    const int clamped = qBound(minWidth, targetWidthPx, maxWidth);
    return ((clamped + bucket - 1) / bucket) * bucket;
}

QString PdfPageRenderer::renderPage(const QString &filePath, int page, qreal zoom)
{
    // Treat zoom == 1.0 as a 1024-px target width on the long side, which
    // matches the QML reader at 100% on a 10" tablet. This is purely a
    // compatibility shim; new code should call the int overload directly.
    const int target = static_cast<int>(1024.0 * qBound(0.25, zoom, 4.0));
    return renderPage(filePath, page, target);
}

QString PdfPageRenderer::renderPage(const QString &filePath, int page, int targetWidthPx)
{
    m_lastError.clear();

    const QFileInfo pdfInfo(filePath);
    if (!pdfInfo.exists() || !pdfInfo.isFile()) {
        m_lastError = QStringLiteral("PDF 文件不存在或无法读取");
        return {};
    }

    if (page < 1) {
        m_lastError = QStringLiteral("PDF 页码无效");
        return {};
    }

    if (!QDir().mkpath(m_cacheRoot)) {
        m_lastError = QStringLiteral("无法创建 PDF 缓存目录");
        return {};
    }

    const int widthBucket = bucketWidth(targetWidthPx);
    const QString outputPath = outputPathFor(filePath, page, widthBucket);
    const QFileInfo outputInfo(outputPath);
    if (outputInfo.exists() && outputInfo.isFile() && outputInfo.size() > 0) {
        return outputPath;
    }

    PdfDocument document;
    if (!document.load(filePath)) {
        m_lastError = document.lastError();
        return {};
    }

    QPdfDocument *handle = document.handle();
    if (!handle) {
        m_lastError = QStringLiteral("PDF 文档未就绪");
        return {};
    }

    if (page > handle->pageCount()) {
        m_lastError = QStringLiteral("PDF 页码超出范围");
        return {};
    }

    // QPdfDocument uses 0-based page indices.
    const int pageIndex = page - 1;
    const QSizeF pointSize = handle->pagePointSize(pageIndex);
    if (pointSize.width() <= 0.0 || pointSize.height() <= 0.0) {
        m_lastError = QStringLiteral("PDF 页面尺寸无效");
        return {};
    }

    const qreal aspect = pointSize.height() / pointSize.width();
    const int renderWidth = widthBucket;
    const int renderHeight = qMax(1, static_cast<int>(renderWidth * aspect));

    const QImage image = handle->render(pageIndex, QSize(renderWidth, renderHeight));
    if (image.isNull()) {
        m_lastError = QStringLiteral("PDF 页面渲染失败");
        return {};
    }

    if (!image.save(outputPath, "PNG")) {
        m_lastError = QStringLiteral("PDF 页面图片保存失败");
        return {};
    }

    return outputPath;
}

QString PdfPageRenderer::lastError() const
{
    return m_lastError;
}

QString PdfPageRenderer::cacheRoot() const
{
    return m_cacheRoot;
}

QString PdfPageRenderer::outputPathFor(const QString &filePath, int page, int widthBucket) const
{
    const QByteArray key = QStringLiteral("%1|%2|%3")
        .arg(QFileInfo(filePath).absoluteFilePath())
        .arg(page)
        .arg(widthBucket)
        .toUtf8();
    const QString digest = QString::fromLatin1(
        QCryptographicHash::hash(key, QCryptographicHash::Sha1).toHex());
    return QDir(m_cacheRoot).filePath(QStringLiteral("%1.png").arg(digest));
}
