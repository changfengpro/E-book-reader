#include <QtTest>

#include "book/LibraryController.h"

class LibraryControllerTest : public QObject {
    Q_OBJECT

private slots:
    void importsLocalFileIntoBooks();
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

QTEST_MAIN(LibraryControllerTest)
#include "tst_librarycontroller.moc"
