#include <QtTest>

#include "book/LibraryController.h"

class LibraryControllerTest : public QObject {
    Q_OBJECT

private slots:
    void importsLocalFileIntoBooks();
    void filtersBooksBySearchTextAndFormat();
};

void LibraryControllerTest::importsLocalFileIntoBooks()
{
    QTemporaryDir libraryDir;
    QVERIFY(libraryDir.isValid());

    QTemporaryFile sourceFile;
    sourceFile.setFileTemplate(QDir::tempPath() + "/novel-XXXXXX.txt");
    QVERIFY(sourceFile.open());
    sourceFile.write("hello");
    sourceFile.close();

    LibraryController controller(libraryDir.path());
    QCOMPARE(controller.books().size(), 0);

    QVERIFY2(controller.importLocalFile(QUrl::fromLocalFile(sourceFile.fileName())),
             qPrintable(controller.lastError()));

    const QVariantList books = controller.books();
    QCOMPARE(books.size(), 1);
    const QVariantMap book = books.first().toMap();
    QCOMPARE(book.value("title").toString(), QFileInfo(sourceFile.fileName()).completeBaseName());
    QCOMPARE(book.value("format").toString(), QString("txt"));
    QVERIFY(QFile::exists(book.value("filePath").toString()));
}

void LibraryControllerTest::filtersBooksBySearchTextAndFormat()
{
    QTemporaryDir libraryDir;
    QVERIFY(libraryDir.isValid());

    LibraryController controller(libraryDir.path());

    QTemporaryFile txtFile;
    txtFile.setFileTemplate(QDir::tempPath() + "/alpha-XXXXXX.txt");
    QVERIFY(txtFile.open());
    txtFile.write("alpha");
    txtFile.close();

    QTemporaryFile pdfFile;
    pdfFile.setFileTemplate(QDir::tempPath() + "/manual-XXXXXX.pdf");
    QVERIFY(pdfFile.open());
    pdfFile.write("%PDF-1.4");
    pdfFile.close();

    QVERIFY2(controller.importLocalFile(QUrl::fromLocalFile(txtFile.fileName())),
             qPrintable(controller.lastError()));
    QVERIFY2(controller.importLocalFile(QUrl::fromLocalFile(pdfFile.fileName())),
             qPrintable(controller.lastError()));
    QCOMPARE(controller.totalBookCount(), 2);
    QCOMPARE(controller.books().size(), 2);

    controller.setSearchText("alpha");
    QCOMPARE(controller.books().size(), 1);
    QCOMPARE(controller.books().first().toMap().value("format").toString(), QString("txt"));

    controller.setFormatFilter("PDF");
    QCOMPARE(controller.books().size(), 0);

    controller.setSearchText(QString());
    QCOMPARE(controller.books().size(), 1);
    QCOMPARE(controller.books().first().toMap().value("format").toString(), QString("pdf"));
}

QTEST_MAIN(LibraryControllerTest)
#include "tst_librarycontroller.moc"
