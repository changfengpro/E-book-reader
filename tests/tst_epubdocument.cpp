#include <QtTest>

#include <QBuffer>
#include <QByteArray>
#include <QFile>
#include <QTemporaryDir>

#include <private/qzipwriter_p.h>

#include "reader/EpubDocument.h"

class EpubDocumentTest : public QObject {
    Q_OBJECT

private slots:
    void parsesEpub3WithNav();
    void rejectsMissingFile();
};

static void writeEpub3(const QString &path)
{
    QFile out(path);
    QVERIFY(out.open(QIODevice::WriteOnly));
    QZipWriter zip(&out);

    // mimetype must be the first entry, stored uncompressed per the EPUB spec.
    zip.setCompressionPolicy(QZipWriter::NeverCompress);
    zip.addFile(QStringLiteral("mimetype"), QByteArrayLiteral("application/epub+zip"));
    zip.setCompressionPolicy(QZipWriter::AutoCompress);

    const QByteArray container =
        "<?xml version=\"1.0\"?>"
        "<container version=\"1.0\" xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">"
        "  <rootfiles>"
        "    <rootfile full-path=\"OEBPS/content.opf\" media-type=\"application/oebps-package+xml\"/>"
        "  </rootfiles>"
        "</container>";
    zip.addFile(QStringLiteral("META-INF/container.xml"), container);

    const QByteArray opf =
        "<?xml version=\"1.0\"?>"
        "<package xmlns=\"http://www.idpf.org/2007/opf\" version=\"3.0\" unique-identifier=\"bid\">"
        "  <metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\">"
        "    <dc:title>Sample</dc:title>"
        "  </metadata>"
        "  <manifest>"
        "    <item id=\"nav\" href=\"nav.xhtml\" media-type=\"application/xhtml+xml\" properties=\"nav\"/>"
        "    <item id=\"c1\" href=\"chapter1.xhtml\" media-type=\"application/xhtml+xml\"/>"
        "    <item id=\"c2\" href=\"chapter2.xhtml\" media-type=\"application/xhtml+xml\"/>"
        "  </manifest>"
        "  <spine>"
        "    <itemref idref=\"c1\"/>"
        "    <itemref idref=\"c2\"/>"
        "  </spine>"
        "</package>";
    zip.addFile(QStringLiteral("OEBPS/content.opf"), opf);

    const QByteArray nav =
        "<?xml version=\"1.0\"?>"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\" xmlns:epub=\"http://www.idpf.org/2007/ops\">"
        "  <body>"
        "    <nav epub:type=\"toc\">"
        "      <ol>"
        "        <li><a href=\"chapter1.xhtml\">First Chapter</a></li>"
        "        <li><a href=\"chapter2.xhtml\">Second Chapter</a></li>"
        "      </ol>"
        "    </nav>"
        "  </body>"
        "</html>";
    zip.addFile(QStringLiteral("OEBPS/nav.xhtml"), nav);

    const QByteArray chapter1 =
        "<?xml version=\"1.0\"?>"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\"><body><h1>One</h1><p>Hello</p></body></html>";
    zip.addFile(QStringLiteral("OEBPS/chapter1.xhtml"), chapter1);

    const QByteArray chapter2 =
        "<?xml version=\"1.0\"?>"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\"><body><h1>Two</h1><p>World</p></body></html>";
    zip.addFile(QStringLiteral("OEBPS/chapter2.xhtml"), chapter2);

    zip.close();
    out.close();
}

void EpubDocumentTest::parsesEpub3WithNav()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath(QStringLiteral("sample.epub"));
    writeEpub3(path);

    EpubDocument document;
    QVERIFY2(document.load(path), qPrintable(document.lastError()));

    const QVector<EpubChapter> chapters = document.chapters();
    QCOMPARE(chapters.size(), 2);
    QCOMPARE(chapters.at(0).id, QStringLiteral("c1"));
    QCOMPARE(chapters.at(0).title, QStringLiteral("First Chapter"));
    QCOMPARE(chapters.at(1).title, QStringLiteral("Second Chapter"));

    const QString html = document.chapterHtml(QStringLiteral("c1"));
    QVERIFY(html.contains(QStringLiteral("Hello")));
    QVERIFY(html.contains(QStringLiteral("<h1>One</h1>")));
}

void EpubDocumentTest::rejectsMissingFile()
{
    EpubDocument document;
    QVERIFY(!document.load(QDir::tempPath() + "/missing.epub"));
    QVERIFY(!document.lastError().isEmpty());
}

QTEST_MAIN(EpubDocumentTest)
#include "tst_epubdocument.moc"
