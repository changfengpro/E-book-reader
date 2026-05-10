#include "TxtDocument.h"

#include <QFile>
#include <QStringConverter>
#include <QStringDecoder>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace {
#ifdef Q_OS_WIN
QString decodeWindowsCodePage(const QByteArray &data, UINT codePage)
{
    const int requiredLength = MultiByteToWideChar(
        codePage,
        MB_ERR_INVALID_CHARS,
        data.constData(),
        data.size(),
        nullptr,
        0);
    if (requiredLength <= 0) {
        return {};
    }

    QString result(requiredLength, Qt::Uninitialized);
    const int convertedLength = MultiByteToWideChar(
        codePage,
        MB_ERR_INVALID_CHARS,
        data.constData(),
        data.size(),
        reinterpret_cast<wchar_t *>(result.data()),
        result.size());
    return convertedLength > 0 ? result : QString();
}
#endif
}

bool TxtDocument::load(const QString &filePath)
{
    m_text.clear();
    m_lastError.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = QStringLiteral("无法打开文本文件");
        return false;
    }

    const QByteArray data = file.readAll();
    m_text = decodeBytes(data);
    if (m_text.isNull()) {
        m_lastError = QStringLiteral("无法识别文本编码");
        return false;
    }

    return true;
}

QString TxtDocument::text() const
{
    return m_text;
}

int TxtDocument::length() const
{
    return m_text.size();
}

QString TxtDocument::lastError() const
{
    return m_lastError;
}

QString TxtDocument::decodeBytes(const QByteArray &data) const
{
    if (data.startsWith("\xef\xbb\xbf")) {
        QStringDecoder decoder(QStringDecoder::Utf8);
        return decoder.decode(data.mid(3));
    }

    if (data.size() >= 2
        && static_cast<unsigned char>(data[0]) == 0xff
        && static_cast<unsigned char>(data[1]) == 0xfe) {
        QStringDecoder decoder(QStringDecoder::Utf16LE);
        return decoder.decode(data.mid(2));
    }

    if (data.size() >= 2
        && static_cast<unsigned char>(data[0]) == 0xfe
        && static_cast<unsigned char>(data[1]) == 0xff) {
        QStringDecoder decoder(QStringDecoder::Utf16BE);
        return decoder.decode(data.mid(2));
    }

    {
        QStringDecoder decoder(QStringDecoder::Utf8);
        const QString decoded = decoder.decode(data);
        if (!decoder.hasError()) {
            return decoded;
        }
    }

    const auto gb18030 = QStringConverter::encodingForName("GB18030");
    if (gb18030.has_value()) {
        QStringDecoder decoder(*gb18030);
        const QString decoded = decoder.decode(data);
        if (!decoder.hasError()) {
            return decoded;
        }
    }

#ifdef Q_OS_WIN
    const QString gbkText = decodeWindowsCodePage(data, 936);
    if (!gbkText.isNull()) {
        return gbkText;
    }
#endif

    return QString();
}
