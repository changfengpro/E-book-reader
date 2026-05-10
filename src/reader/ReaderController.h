#pragma once

#include <QObject>
#include <QString>

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

signals:
    void bookChanged();

private:
    QString m_bookId;
    QString m_locatorJson;
};
