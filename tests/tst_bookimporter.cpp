#include <QtTest>

#include "book/BookImporter.h"

class BookImporterTest : public QObject {
    Q_OBJECT

private slots:
    void detectsSupportedFormats();
};

void BookImporterTest::detectsSupportedFormats()
{
    QCOMPARE(BookImporter::detectFormat("novel.txt", "text/plain"), QString("txt"));
    QCOMPARE(BookImporter::detectFormat("manual.pdf", "application/pdf"), QString("pdf"));
    QCOMPARE(BookImporter::detectFormat("book.epub", "application/epub+zip"), QString("epub"));
    QCOMPARE(BookImporter::detectFormat("archive.zip", "application/zip"), QString());
}

QTEST_MAIN(BookImporterTest)
#include "tst_bookimporter.moc"
