#include "PdfPageRenderer.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>

PdfPageRenderer::PdfPageRenderer(QString cacheRoot)
    : m_cacheRoot(cacheRoot.isEmpty()
          ? QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)).filePath(QStringLiteral("pdf-pages"))
          : std::move(cacheRoot))
{
}

QString PdfPageRenderer::renderPage(const QString &filePath, int page, qreal zoom)
{
    m_lastError.clear();

    const QString mutoolPath = defaultMutoolPath();
    if (mutoolPath.isEmpty() || !QFileInfo::exists(mutoolPath)) {
        m_lastError = QStringLiteral("未找到 MuPDF mutool.exe");
        return {};
    }

    const QFileInfo pdfInfo(filePath);
    if (!pdfInfo.exists() || !pdfInfo.isFile()) {
        m_lastError = QStringLiteral("PDF 文件不存在或无法读取");
        return {};
    }

    if (!QDir().mkpath(m_cacheRoot)) {
        m_lastError = QStringLiteral("无法创建 PDF 缓存目录");
        return {};
    }

    const QString outputPath = outputPathFor(filePath, page, zoom);
    const QFileInfo outputInfo(outputPath);
    if (outputInfo.exists() && outputInfo.isFile() && outputInfo.size() > 0) {
        return outputPath;
    }

    const int dpi = qBound(72, static_cast<int>(144 * zoom), 360);

    QProcess process;
    process.setProgram(mutoolPath);
    process.setArguments({
        QStringLiteral("draw"),
        QStringLiteral("-o"),
        outputPath,
        QStringLiteral("-r"),
        QString::number(dpi),
        filePath,
        QString::number(qMax(1, page))
    });
    process.start();
    if (!process.waitForFinished(30000)) {
        process.kill();
        m_lastError = QStringLiteral("PDF 页面渲染超时");
        return {};
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        m_lastError = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
        if (m_lastError.isEmpty()) {
            m_lastError = QStringLiteral("PDF 页面渲染失败");
        }
        return {};
    }

    if (!QFileInfo::exists(outputPath)) {
        m_lastError = QStringLiteral("PDF 页面图片未生成");
        return {};
    }

    return outputPath;
}

QString PdfPageRenderer::lastError() const
{
    return m_lastError;
}

QString PdfPageRenderer::defaultMutoolPath()
{
    const QString configuredPath = qEnvironmentVariable("MUTOOL_PATH");
    if (!configuredPath.isEmpty()) {
        return configuredPath;
    }

#ifdef Q_OS_WIN
    const QString knownPath = QStringLiteral("F:/Program Files/mupdf-1.27.0-windows/mutool.exe");
    if (QFileInfo::exists(knownPath)) {
        return knownPath;
    }
#endif

    return QStringLiteral("mutool");
}

QString PdfPageRenderer::outputPathFor(const QString &filePath, int page, qreal zoom) const
{
    const QByteArray key = QStringLiteral("%1|%2|%3")
        .arg(QFileInfo(filePath).absoluteFilePath())
        .arg(page)
        .arg(zoom, 0, 'f', 2)
        .toUtf8();
    const QString digest = QString::fromLatin1(QCryptographicHash::hash(key, QCryptographicHash::Sha1).toHex());
    return QDir(m_cacheRoot).filePath(QStringLiteral("%1.png").arg(digest));
}
