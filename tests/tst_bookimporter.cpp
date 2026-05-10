#include <QtTest>

#include "book/BookImporter.h"

class BookImporterTest : public QObject {
    Q_OBJECT

private slots:
    void detectsSupportedFormats();
    void importsReadableFileIntoLibrary();
};

void BookImporterTest::detectsSupportedFormats()
{
    QCOMPARE(BookImporter::detectFormat("novel.txt", "text/plain"), QString("txt"));
    QCOMPARE(BookImporter::detectFormat("manual.pdf", "application/pdf"), QString("pdf"));
    QCOMPARE(BookImporter::detectFormat("book.epub", "application/epub+zip"), QString("epub"));
    QCOMPARE(BookImporter::detectFormat("archive.zip", "application/zip"), QString());
}

void BookImporterTest::importsReadableFileIntoLibrary()
{
    QTemporaryDir libraryDir;
    QVERIFY(libraryDir.isValid());

    QTemporaryFile sourceFile;
    QVERIFY(sourceFile.open());
    sourceFile.write("stream content");
    sourceFile.seek(0);

    BookImporter importer(AppPaths(libraryDir.path()));
    const Book book = importer.importFromReadableFile("remote-document.txt", "text/plain", sourceFile);

    QVERIFY2(!book.id.isEmpty(), qPrintable(importer.lastError()));
    QCOMPARE(book.title, QString("remote-document"));
    QCOMPARE(book.format, QString("txt"));
    QCOMPARE(book.originalName, QString("remote-document.txt"));
    QVERIFY(QFile::exists(book.filePath));

    QFile importedFile(book.filePath);
    QVERIFY(importedFile.open(QIODevice::ReadOnly));
    QCOMPARE(importedFile.readAll(), QByteArray("stream content"));
}

QTEST_MAIN(BookImporterTest)
#include "tst_bookimporter.moc"
