#pragma once

#include "reader/TxtDocument.h"

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class ReaderController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString bookId READ bookId WRITE setBookId NOTIFY bookChanged)
    Q_PROPERTY(QString format READ format NOTIFY bookChanged)
    Q_PROPERTY(QString title READ title NOTIFY bookChanged)

public:
    explicit ReaderController(QObject *parent = nullptr);

    QString bookId() const;
    void setBookId(const QString &bookId);
    QString format() const;
    QString title() const;

    Q_INVOKABLE void saveLocator(const QString &locatorJson);
    Q_INVOKABLE QString savedLocator() const;
    Q_INVOKABLE QString loadTextFile(const QString &filePath);
    Q_INVOKABLE QVariantList loadTextChapters(const QString &filePath);
    Q_INVOKABLE QString loadTextChapter(const QString &filePath, int chapterIndex);
    Q_INVOKABLE QVariantMap loadPdfInfo(const QString &filePath);

signals:
    void bookChanged();

private:
    bool ensureTextDocumentLoaded(const QString &filePath);

    QString m_bookId;
    QString m_locatorJson;
    QString m_loadedTextPath;
    TxtDocument m_textDocument;
};
