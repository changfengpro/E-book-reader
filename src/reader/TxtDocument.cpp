#include "TxtDocument.h"

#include <QFile>
#include <QRegularExpression>
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
    m_chapters.clear();
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

    rebuildChapters();
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

QList<TxtChapter> TxtDocument::chapters() const
{
    return m_chapters;
}

QString TxtDocument::chapterText(int chapterIndex) const
{
    if (m_chapters.isEmpty()) {
        return m_text;
    }

    if (chapterIndex < 0 || chapterIndex >= m_chapters.size()) {
        chapterIndex = 0;
    }

    const TxtChapter chapter = m_chapters.at(chapterIndex);
    return m_text.mid(chapter.start, chapter.length);
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

void TxtDocument::rebuildChapters()
{
    m_chapters.clear();

    static const QRegularExpression chapterPattern(
        QStringLiteral(R"(^\s*((第[0-9零〇一二两三四五六七八九十百千万]+[章章节卷回部集][^\r\n]*)|(序章|楔子|引子|前言|后记)\s*[^\r\n]*)\s*$)"));

    struct Marker {
        QString title;
        int start = 0;
    };
    QList<Marker> markers;

    int lineStart = 0;
    while (lineStart < m_text.size()) {
        int lineEnd = m_text.indexOf(QLatin1Char('\n'), lineStart);
        if (lineEnd < 0) {
            lineEnd = m_text.size();
        }

        QString line = m_text.mid(lineStart, lineEnd - lineStart);
        if (line.endsWith(QLatin1Char('\r'))) {
            line.chop(1);
        }

        const QString trimmedLine = line.trimmed();
        if (trimmedLine.size() <= 80 && chapterPattern.match(trimmedLine).hasMatch()) {
            markers.append({ trimmedLine, lineStart });
        }

        lineStart = lineEnd + 1;
    }

    if (markers.isEmpty()) {
        m_chapters.append({ QStringLiteral("正文"), 0, static_cast<int>(m_text.size()) });
        return;
    }

    if (markers.first().start > 0) {
        m_chapters.append({ QStringLiteral("正文"), 0, markers.first().start });
    }

    for (int i = 0; i < markers.size(); ++i) {
        const int start = markers.at(i).start;
        const int end = i + 1 < markers.size() ? markers.at(i + 1).start : static_cast<int>(m_text.size());
        m_chapters.append({ markers.at(i).title, start, end - start });
    }
}
