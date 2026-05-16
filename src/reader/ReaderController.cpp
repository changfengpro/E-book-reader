#include "ReaderController.h"

#include "reader/PdfDocument.h"
#include "reader/PdfPageRenderer.h"

#include <QUrl>

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
    return m_format;
}

void ReaderController::setFormat(const QString &format)
{
    const QString normalized = format.toLower();
    if (m_format == normalized) {
        return;
    }
    m_format = normalized;
    emit bookChanged();
}

QString ReaderController::title() const
{
    if (!m_title.isEmpty()) {
        return m_title;
    }
    return m_bookId.isEmpty() ? QStringLiteral("阅读") : m_bookId;
}

void ReaderController::setTitle(const QString &title)
{
    if (m_title == title) {
        return;
    }
    m_title = title;
    emit bookChanged();
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

QVariantMap ReaderController::loadPdfInfo(const QString &filePath)
{
    PdfDocument document;
    if (!document.load(filePath)) {
        return {
            { QStringLiteral("loaded"), false },
            { QStringLiteral("pageCount"), 0 },
            { QStringLiteral("error"), document.lastError() }
        };
    }

    return {
        { QStringLiteral("loaded"), true },
        { QStringLiteral("pageCount"), document.pageCount() },
        { QStringLiteral("error"), QString() }
    };
}

QVariantMap ReaderController::renderPdfPage(const QString &filePath, int page, double zoom)
{
    PdfPageRenderer renderer;
    const QString imagePath = renderer.renderPage(filePath, page, zoom);
    if (imagePath.isEmpty()) {
        return {
            { QStringLiteral("rendered"), false },
            { QStringLiteral("imageUrl"), QString() },
            { QStringLiteral("error"), renderer.lastError() }
        };
    }

    return {
        { QStringLiteral("rendered"), true },
        { QStringLiteral("imageUrl"), QUrl::fromLocalFile(imagePath).toString() },
        { QStringLiteral("error"), QString() }
    };
}

QVariantMap ReaderController::renderPdfPageAtWidth(const QString &filePath, int page, int targetWidthPx)
{
    PdfPageRenderer renderer;
    const QString imagePath = renderer.renderPage(filePath, page, targetWidthPx);
    if (imagePath.isEmpty()) {
        return {
            { QStringLiteral("rendered"), false },
            { QStringLiteral("imageUrl"), QString() },
            { QStringLiteral("error"), renderer.lastError() }
        };
    }

    return {
        { QStringLiteral("rendered"), true },
        { QStringLiteral("imageUrl"), QUrl::fromLocalFile(imagePath).toString() },
        { QStringLiteral("error"), QString() }
    };
}

QVariantList ReaderController::loadEpubChapters(const QString &filePath)
{
    QVariantList list;
    if (!ensureEpubLoaded(filePath)) {
        return list;
    }
    const QVector<EpubChapter> chapters = m_epubDocument.chapters();
    list.reserve(chapters.size());
    for (int i = 0; i < chapters.size(); ++i) {
        const EpubChapter &chapter = chapters.at(i);
        list.append(QVariantMap {
            { QStringLiteral("index"), i },
            { QStringLiteral("id"), chapter.id },
            { QStringLiteral("title"), chapter.title },
            { QStringLiteral("href"), chapter.href },
        });
    }
    return list;
}

QString ReaderController::loadEpubChapter(const QString &filePath, const QString &chapterId)
{
    if (!ensureEpubLoaded(filePath)) {
        return {};
    }
    return m_epubDocument.chapterHtml(chapterId);
}

QString ReaderController::epubLastError() const
{
    return m_epubDocument.lastError();
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

bool ReaderController::ensureEpubLoaded(const QString &filePath)
{
    if (filePath.isEmpty()) {
        m_loadedEpubPath.clear();
        return false;
    }
    if (m_loadedEpubPath == filePath) {
        return true;
    }
    if (!m_epubDocument.load(filePath)) {
        m_loadedEpubPath.clear();
        return false;
    }
    m_loadedEpubPath = filePath;
    return true;
}
