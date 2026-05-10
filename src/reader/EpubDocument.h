#pragma once

#include <QString>
#include <QVector>

struct EpubChapter {
    QString id;
    QString title;
    QString href;
};

class EpubDocument {
public:
    bool load(const QString &filePath);
    QVector<EpubChapter> chapters() const;
    QString chapterHtml(const QString &chapterId) const;
    QString lastError() const;

private:
    QVector<EpubChapter> m_chapters;
    QString m_lastError;
};
