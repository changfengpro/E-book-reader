#pragma once

#include <QList>
#include <QString>

struct TxtChapter {
    QString title;
    int start = 0;
    int length = 0;
};

class TxtDocument {
public:
    bool load(const QString &filePath);
    QString text() const;
    int length() const;
    QList<TxtChapter> chapters() const;
    QString chapterText(int chapterIndex) const;
    QString lastError() const;

private:
    QString decodeBytes(const QByteArray &data) const;
    void rebuildChapters();

    QString m_text;
    QList<TxtChapter> m_chapters;
    QString m_lastError;
};
