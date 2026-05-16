#pragma once

#include <QByteArray>
#include <QHash>
#include <QString>
#include <QVector>

struct EpubChapter {
    QString id;       // OPF manifest id (or generated id when fallback)
    QString title;    // Display title from nav/ncx, falls back to filename stem
    QString href;     // path inside the zip, normalised to forward slashes
};

class EpubDocument {
public:
    bool load(const QString &filePath);
    QVector<EpubChapter> chapters() const;

    // Returns the raw HTML for a chapter id. The HTML is rewritten so that
    // image/style references become embedded data URIs, which lets a
    // QML Text/WebView render the chapter without filesystem access.
    QString chapterHtml(const QString &chapterId) const;

    QString lastError() const;

private:
    QString resolveRelative(const QString &basePath, const QString &target) const;
    QString rewriteHtml(const QString &basePath, const QString &html) const;

    QHash<QString, QByteArray> m_files;     // zip path -> raw bytes
    QVector<EpubChapter> m_chapters;
    QHash<QString, QString> m_chapterById;  // id -> normalized path
    QString m_lastError;
};
