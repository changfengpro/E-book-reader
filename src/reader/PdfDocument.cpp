#include "PdfDocument.h"

#include <QFileInfo>

bool PdfDocument::load(const QString &filePath)
{
    m_pageCount = 0;
    m_lastError.clear();

    const QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        m_lastError = QStringLiteral("PDF 文件不存在或无法读取");
        return false;
    }

    m_lastError = QStringLiteral("当前构建未启用 PDF 渲染模块");
    return false;
}

int PdfDocument::pageCount() const
{
    return m_pageCount;
}

QString PdfDocument::lastError() const
{
    return m_lastError;
}
