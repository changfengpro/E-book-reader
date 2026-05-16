#pragma once

#include "reader/EpubDocument.h"
#include "reader/TxtDocument.h"

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class ReaderController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString bookId READ bookId WRITE setBookId NOTIFY bookChanged)
    Q_PROPERTY(QString format READ format WRITE setFormat NOTIFY bookChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY bookChanged)

public:
    explicit ReaderController(QObject *parent = nullptr);

    QString bookId() const;
    void setBookId(const QString &bookId);

    QString format() const;
    void setFormat(const QString &format);

    QString title() const;
    void setTitle(const QString &title);

    Q_INVOKABLE void saveLocator(const QString &locatorJson);
    Q_INVOKABLE QString savedLocator() const;
    Q_INVOKABLE QString loadTextFile(const QString &filePath);
    Q_INVOKABLE QVariantList loadTextChapters(const QString &filePath);
    Q_INVOKABLE QString loadTextChapter(const QString &filePath, int chapterIndex);
    Q_INVOKABLE QVariantMap loadPdfInfo(const QString &filePath);
    Q_INVOKABLE QVariantMap renderPdfPage(const QString &filePath, int page, double zoom);
    Q_INVOKABLE QVariantMap renderPdfPageAtWidth(const QString &filePath, int page, int targetWidthPx);
    Q_INVOKABLE QVariantList loadEpubChapters(const QString &filePath);
    Q_INVOKABLE QString loadEpubChapter(const QString &filePath, const QString &chapterId);
    Q_INVOKABLE QString epubLastError() const;

signals:
    void bookChanged();

private:
    bool ensureTextDocumentLoaded(const QString &filePath);
    bool ensureEpubLoaded(const QString &filePath);

    QString m_bookId;
    QString m_format;
    QString m_title;
    QString m_locatorJson;

    QString m_loadedTextPath;
    TxtDocument m_textDocument;

    QString m_loadedEpubPath;
    EpubDocument m_epubDocument;
};
