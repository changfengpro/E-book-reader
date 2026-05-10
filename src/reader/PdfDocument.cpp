#include "PdfDocument.h"

#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

namespace {
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
        m_lastError = QStringLiteral("无法识别 PDF 页数");
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
