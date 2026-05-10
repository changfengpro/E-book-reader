#include "PdfDocument.h"

#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>

namespace {
QString defaultMutoolPath()
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

int maxPagesCountFromPagesTree(const QString &text)
{
    static const QRegularExpression countPattern(QStringLiteral(R"(/Count\s+(\d+))"));

    int maxCount = 0;
    QRegularExpressionMatchIterator matches = countPattern.globalMatch(text);
    while (matches.hasNext()) {
        const QRegularExpressionMatch match = matches.next();
        bool ok = false;
        const int count = match.captured(1).toInt(&ok);
        if (ok && count > maxCount) {
            maxCount = count;
        }
    }
    return maxCount;
}

int pageObjectCount(const QString &text)
{
    static const QRegularExpression pagePattern(QStringLiteral(R"(/Type\s*/Page(?!s)\b)"));

    int count = 0;
    QRegularExpressionMatchIterator matches = pagePattern.globalMatch(text);
    while (matches.hasNext()) {
        matches.next();
        ++count;
    }
    return count;
}

int pageCountFromMutool(const QString &filePath, QString *error)
{
    const QString mutoolPath = defaultMutoolPath();
    if (mutoolPath.isEmpty() || !QFileInfo::exists(mutoolPath)) {
        if (error) {
            *error = QStringLiteral("未找到 MuPDF mutool.exe");
        }
        return 0;
    }

    QProcess process;
    process.setProgram(mutoolPath);
    process.setArguments({ QStringLiteral("info"), filePath });
    process.start();
    if (!process.waitForFinished(30000)) {
        process.kill();
        if (error) {
            *error = QStringLiteral("PDF 页数解析超时");
        }
        return 0;
    }

    const QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    const QString errorOutput = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        if (error) {
            *error = errorOutput.isEmpty() ? QStringLiteral("PDF 页数解析失败") : errorOutput;
        }
        return 0;
    }

    static const QRegularExpression pagesPattern(QStringLiteral(R"(\bPages:\s+(\d+))"));
    const QRegularExpressionMatch match = pagesPattern.match(output);
    if (!match.hasMatch()) {
        if (error) {
            *error = QStringLiteral("无法识别 PDF 页数");
        }
        return 0;
    }

    bool ok = false;
    const int pageCount = match.captured(1).toInt(&ok);
    return ok ? pageCount : 0;
}
}

bool PdfDocument::load(const QString &filePath)
{
    m_pageCount = 0;
    m_lastError.clear();

    const QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        m_lastError = QStringLiteral("PDF 文件不存在或无法读取");
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = QStringLiteral("无法打开 PDF 文件");
        return false;
    }

    const QByteArray data = file.readAll();
    if (!data.startsWith("%PDF-")) {
        m_lastError = QStringLiteral("文件不是有效的 PDF");
        return false;
    }

    const QString text = QString::fromLatin1(data);
    const int objectPageCount = pageObjectCount(text);
    const int treePageCount = maxPagesCountFromPagesTree(text);
    m_pageCount = objectPageCount > 0 ? objectPageCount : treePageCount;

    if (m_pageCount <= 0) {
        m_pageCount = pageCountFromMutool(filePath, &m_lastError);
    }

    if (m_pageCount <= 0) {
        if (m_lastError.isEmpty()) {
            m_lastError = QStringLiteral("无法识别 PDF 页数");
        }
        return false;
    }

    return true;
}

int PdfDocument::pageCount() const
{
    return m_pageCount;
}

QString PdfDocument::lastError() const
{
    return m_lastError;
}
