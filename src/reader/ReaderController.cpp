#include "ReaderController.h"

#include <QVariantMap>

ReaderController::ReaderController(QObject *parent)
    : QObject(parent)
{
}

QString ReaderController::bookId() const
{
    return m_bookId;
}

void ReaderController::setBookId(const QString &bookId)
{
    if (m_bookId == bookId) {
        return;
    }

    m_bookId = bookId;
    emit bookChanged();
}

QString ReaderController::format() const
{
    if (m_bookId.contains(QStringLiteral("pdf"), Qt::CaseInsensitive)) {
        return QStringLiteral("pdf");
    }
    if (m_bookId.contains(QStringLiteral("epub"), Qt::CaseInsensitive)) {
        return QStringLiteral("epub");
    }
    return QStringLiteral("txt");
}

QString ReaderController::title() const
{
    return m_bookId.isEmpty() ? QStringLiteral("阅读") : m_bookId;
}

void ReaderController::saveLocator(const QString &locatorJson)
{
    m_locatorJson = locatorJson;
}

QString ReaderController::savedLocator() const
{
    return m_locatorJson;
}

QString ReaderController::loadTextFile(const QString &filePath)
{
    if (!ensureTextDocumentLoaded(filePath)) {
        return m_textDocument.lastError();
    }
    return m_textDocument.text();
}

QVariantList ReaderController::loadTextChapters(const QString &filePath)
{
    QVariantList chapterList;
    if (!ensureTextDocumentLoaded(filePath)) {
        return chapterList;
    }

    const QList<TxtChapter> chapters = m_textDocument.chapters();
    for (int i = 0; i < chapters.size(); ++i) {
        const TxtChapter chapter = chapters.at(i);
        chapterList.append(QVariantMap {
            { QStringLiteral("index"), i },
            { QStringLiteral("title"), chapter.title }
        });
    }
    return chapterList;
}

QString ReaderController::loadTextChapter(const QString &filePath, int chapterIndex)
{
    if (!ensureTextDocumentLoaded(filePath)) {
        return m_textDocument.lastError();
    }
    return m_textDocument.chapterText(chapterIndex);
}

bool ReaderController::ensureTextDocumentLoaded(const QString &filePath)
{
    if (filePath.isEmpty()) {
        m_loadedTextPath.clear();
        return false;
    }

    if (m_loadedTextPath == filePath) {
        return true;
    }

    if (!m_textDocument.load(filePath)) {
        m_loadedTextPath.clear();
        return false;
    }

    m_loadedTextPath = filePath;
    return true;
}
