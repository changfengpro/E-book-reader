#include "EpubDocument.h"

#include <QFileInfo>

bool EpubDocument::load(const QString &filePath)
{
    m_chapters.clear();
    m_lastError.clear();

    const QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        m_lastError = QStringLiteral("EPUB 文件不存在或无法读取");
        return false;
    }

    m_lastError = QStringLiteral("当前构建未启用 EPUB 解包模块");
    return false;
}

QVector<EpubChapter> EpubDocument::chapters() const
{
    return m_chapters;
}

QString EpubDocument::chapterHtml(const QString &chapterId) const
{
    Q_UNUSED(chapterId)
    return {};
}

QString EpubDocument::lastError() const
{
    return m_lastError;
}
