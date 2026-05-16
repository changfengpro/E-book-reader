#include "EpubDocument.h"

#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QMimeDatabase>
#include <QMimeType>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QXmlStreamReader>

#include <private/qzipreader_p.h>

namespace {

QString normalizePath(const QString &path)
{
    QString out = path;
    out.replace('\\', '/');
    while (out.startsWith("./")) {
        out.remove(0, 2);
    }
    // collapse "a/b/../c" segments
    QStringList parts;
    for (const QString &part : out.split('/', Qt::KeepEmptyParts)) {
        if (part == "..") {
            if (!parts.isEmpty()) {
                parts.removeLast();
            }
        } else if (part != ".") {
            parts.append(part);
        }
    }
    return parts.join('/');
}

QString joinPath(const QString &dir, const QString &rel)
{
    if (rel.startsWith('/')) {
        return normalizePath(rel.mid(1));
    }
    if (dir.isEmpty()) {
        return normalizePath(rel);
    }
    return normalizePath(dir + '/' + rel);
}

QString dirOf(const QString &path)
{
    const int slash = path.lastIndexOf('/');
    return slash < 0 ? QString() : path.left(slash);
}

// Strips a fragment ("chap1.html#section") off an href and returns just the path.
QString stripFragment(const QString &href)
{
    const int hash = href.indexOf('#');
    return hash < 0 ? href : href.left(hash);
}

QString findOpfPath(const QByteArray &containerXml)
{
    QXmlStreamReader reader(containerXml);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name().compare(QLatin1String("rootfile"), Qt::CaseInsensitive) == 0) {
            const QString fullPath = reader.attributes().value(QLatin1String("full-path")).toString();
            if (!fullPath.isEmpty()) {
                return normalizePath(fullPath);
            }
        }
    }
    return {};
}

struct OpfModel {
    QString opfDir;
    // manifest id -> { href (relative to opf dir), media-type, properties }
    struct Item {
        QString href;
        QString mediaType;
        QString properties;
    };
    QHash<QString, Item> manifest;
    QStringList spineIds;          // order of idrefs in spine
    QString tocNcxId;              // toc attribute on <spine> (epub 2)
    QString navId;                 // manifest id of item with properties="nav" (epub 3)
};

OpfModel parseOpf(const QByteArray &opfXml, const QString &opfPath)
{
    OpfModel model;
    model.opfDir = dirOf(opfPath);

    QXmlStreamReader reader(opfXml);
    while (!reader.atEnd()) {
        reader.readNext();
        if (!reader.isStartElement()) {
            continue;
        }
        const QStringView name = reader.name();
        if (name.compare(QLatin1String("item"), Qt::CaseInsensitive) == 0) {
            const auto attrs = reader.attributes();
            OpfModel::Item item;
            const QString id = attrs.value(QLatin1String("id")).toString();
            item.href = attrs.value(QLatin1String("href")).toString();
            item.mediaType = attrs.value(QLatin1String("media-type")).toString();
            item.properties = attrs.value(QLatin1String("properties")).toString();
            if (!id.isEmpty()) {
                model.manifest.insert(id, item);
                if (item.properties.split(' ', Qt::SkipEmptyParts).contains(QStringLiteral("nav"))) {
                    model.navId = id;
                }
            }
        } else if (name.compare(QLatin1String("spine"), Qt::CaseInsensitive) == 0) {
            model.tocNcxId = reader.attributes().value(QLatin1String("toc")).toString();
        } else if (name.compare(QLatin1String("itemref"), Qt::CaseInsensitive) == 0) {
            const QString idref = reader.attributes().value(QLatin1String("idref")).toString();
            if (!idref.isEmpty()) {
                model.spineIds.append(idref);
            }
        }
    }
    return model;
}

// Parse epub3 nav HTML: collect mapping href (relative to nav file) -> title.
QHash<QString, QString> parseNavTitles(const QByteArray &navHtml)
{
    QHash<QString, QString> titles;
    QXmlStreamReader reader(navHtml);
    QString currentHref;
    QString currentText;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name().compare(QLatin1String("a"), Qt::CaseInsensitive) == 0) {
            currentHref = reader.attributes().value(QLatin1String("href")).toString();
            currentText.clear();
        } else if (reader.isCharacters() && !currentHref.isEmpty()) {
            currentText.append(reader.text().toString());
        } else if (reader.isEndElement() && reader.name().compare(QLatin1String("a"), Qt::CaseInsensitive) == 0) {
            if (!currentHref.isEmpty()) {
                const QString cleanedTitle = currentText.simplified();
                if (!cleanedTitle.isEmpty()) {
                    titles.insert(stripFragment(currentHref), cleanedTitle);
                }
            }
            currentHref.clear();
            currentText.clear();
        }
    }
    return titles;
}

// Parse epub2 NCX: <navPoint><navLabel><text>Title</text></navLabel><content src="chap1.html"/>
QHash<QString, QString> parseNcxTitles(const QByteArray &ncxXml)
{
    QHash<QString, QString> titles;
    QXmlStreamReader reader(ncxXml);
    QString currentTitle;
    bool insideLabelText = false;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            const QStringView n = reader.name();
            if (n.compare(QLatin1String("navPoint"), Qt::CaseInsensitive) == 0) {
                currentTitle.clear();
            } else if (n.compare(QLatin1String("text"), Qt::CaseInsensitive) == 0) {
                insideLabelText = true;
            } else if (n.compare(QLatin1String("content"), Qt::CaseInsensitive) == 0) {
                const QString src = reader.attributes().value(QLatin1String("src")).toString();
                if (!src.isEmpty() && !currentTitle.isEmpty()) {
                    titles.insert(stripFragment(src), currentTitle.simplified());
                }
            }
        } else if (reader.isCharacters() && insideLabelText) {
            currentTitle.append(reader.text().toString());
        } else if (reader.isEndElement()) {
            if (reader.name().compare(QLatin1String("text"), Qt::CaseInsensitive) == 0) {
                insideLabelText = false;
            }
        }
    }
    return titles;
}

bool isHtmlMediaType(const QString &mediaType)
{
    const QString lower = mediaType.toLower();
    return lower == QLatin1String("application/xhtml+xml")
        || lower == QLatin1String("text/html")
        || lower == QLatin1String("application/xml");
}

} // namespace

bool EpubDocument::load(const QString &filePath)
{
    m_files.clear();
    m_chapters.clear();
    m_chapterById.clear();
    m_lastError.clear();

    const QFileInfo info(filePath);
    if (!info.exists() || !info.isFile()) {
        m_lastError = QStringLiteral("EPUB 文件不存在或无法读取");
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = QStringLiteral("无法打开 EPUB 文件");
        return false;
    }

    QZipReader zip(&file);
    if (!zip.isReadable()) {
        m_lastError = QStringLiteral("EPUB 不是有效的 zip 容器");
        return false;
    }

    const auto entries = zip.fileInfoList();
    for (const QZipReader::FileInfo &entry : entries) {
        if (!entry.isFile) {
            continue;
        }
        m_files.insert(normalizePath(entry.filePath), zip.fileData(entry.filePath));
    }

    const QByteArray containerXml = m_files.value(QStringLiteral("META-INF/container.xml"));
    if (containerXml.isEmpty()) {
        m_lastError = QStringLiteral("EPUB 缺少 META-INF/container.xml");
        return false;
    }

    const QString opfPath = findOpfPath(containerXml);
    if (opfPath.isEmpty() || !m_files.contains(opfPath)) {
        m_lastError = QStringLiteral("EPUB 缺少 OPF 描述文件");
        return false;
    }

    const OpfModel opf = parseOpf(m_files.value(opfPath), opfPath);
    if (opf.spineIds.isEmpty()) {
        m_lastError = QStringLiteral("EPUB spine 为空");
        return false;
    }

    // Resolve TOC titles. Prefer epub3 nav, fall back to NCX.
    QHash<QString, QString> spinePathToTitle;
    auto loadTitlesFrom = [&](const QString &id, auto parser) {
        if (id.isEmpty() || !opf.manifest.contains(id)) {
            return;
        }
        const auto &item = opf.manifest.value(id);
        const QString fullPath = joinPath(opf.opfDir, item.href);
        if (!m_files.contains(fullPath)) {
            return;
        }
        const auto rawTitles = parser(m_files.value(fullPath));
        const QString tocDir = dirOf(fullPath);
        for (auto it = rawTitles.constBegin(); it != rawTitles.constEnd(); ++it) {
            spinePathToTitle.insert(joinPath(tocDir, it.key()), it.value());
        }
    };

    loadTitlesFrom(opf.navId, parseNavTitles);
    if (spinePathToTitle.isEmpty()) {
        loadTitlesFrom(opf.tocNcxId, parseNcxTitles);
    }

    // Build the ordered chapter list from the spine.
    int counter = 0;
    for (const QString &idref : opf.spineIds) {
        if (!opf.manifest.contains(idref)) {
            continue;
        }
        const auto &item = opf.manifest.value(idref);
        if (!isHtmlMediaType(item.mediaType)) {
            continue;
        }
        const QString fullPath = joinPath(opf.opfDir, item.href);
        if (!m_files.contains(fullPath)) {
            continue;
        }

        EpubChapter chapter;
        chapter.id = idref;
        chapter.href = fullPath;
        chapter.title = spinePathToTitle.value(fullPath);
        if (chapter.title.isEmpty()) {
            chapter.title = QFileInfo(fullPath).completeBaseName();
            if (chapter.title.isEmpty()) {
                chapter.title = QStringLiteral("第 %1 章").arg(++counter);
            }
        }

        m_chapters.append(chapter);
        m_chapterById.insert(chapter.id, fullPath);
    }

    if (m_chapters.isEmpty()) {
        m_lastError = QStringLiteral("EPUB 未找到可阅读的章节");
        return false;
    }

    return true;
}

QVector<EpubChapter> EpubDocument::chapters() const
{
    return m_chapters;
}

QString EpubDocument::chapterHtml(const QString &chapterId) const
{
    const QString path = m_chapterById.value(chapterId);
    if (path.isEmpty() || !m_files.contains(path)) {
        return {};
    }

    const QString html = QString::fromUtf8(m_files.value(path));
    return rewriteHtml(dirOf(path), html);
}

QString EpubDocument::lastError() const
{
    return m_lastError;
}

QString EpubDocument::resolveRelative(const QString &basePath, const QString &target) const
{
    return joinPath(basePath, target);
}

QString EpubDocument::rewriteHtml(const QString &basePath, const QString &html) const
{
    static const QRegularExpression imgPattern(
        QStringLiteral("(<img[^>]*\\ssrc\\s*=\\s*[\"'])([^\"']+)([\"'])"),
        QRegularExpression::CaseInsensitiveOption);

    static const QMimeDatabase mimeDb;

    QString result = html;

    auto encode = [&](const QString &relPath) -> QString {
        const QString resolved = joinPath(basePath, stripFragment(relPath));
        if (!m_files.contains(resolved)) {
            return {};
        }
        const QByteArray data = m_files.value(resolved);
        const QString mime = mimeDb.mimeTypeForFileNameAndData(resolved, data).name();
        return QStringLiteral("data:%1;base64,%2").arg(mime, QString::fromLatin1(data.toBase64()));
    };

    QString rewritten;
    rewritten.reserve(result.size());
    int last = 0;
    QRegularExpressionMatchIterator it = imgPattern.globalMatch(result);
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        rewritten.append(QStringView{result}.mid(last, match.capturedStart() - last));
        const QString src = match.captured(2);
        QString replacement = match.captured(1);
        if (src.startsWith(QLatin1String("data:")) || src.startsWith(QLatin1String("http"))) {
            replacement.append(src);
        } else {
            const QString dataUri = encode(src);
            replacement.append(dataUri.isEmpty() ? src : dataUri);
        }
        replacement.append(match.captured(3));
        rewritten.append(replacement);
        last = match.capturedEnd();
    }
    rewritten.append(QStringView{result}.mid(last));
    return rewritten;
}
