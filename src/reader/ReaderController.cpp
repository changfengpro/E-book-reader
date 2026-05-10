#include "ReaderController.h"

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
