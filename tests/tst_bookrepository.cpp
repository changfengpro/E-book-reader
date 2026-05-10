#include <QtTest>

#include "book/BookRepository.h"
#include "storage/AppDatabase.h"
#include "storage/AppPaths.h"

class BookRepositoryTest : public QObject {
    Q_OBJECT

private slots:
    void createsDatabaseFile();
    void savesAndLoadsBook();
    void savesAndLoadsReadingPosition();
};

static AppDatabase openTemporaryDatabase(QTemporaryDir &dir)
{
    AppPaths paths(dir.path());
    const bool ready = paths.ensureDirectories();
    Q_ASSERT(ready);

    AppDatabase database(paths.databasePath());
    const bool opened = database.open();
    Q_ASSERT(opened);
    return database;
}

void BookRepositoryTest::createsDatabaseFile()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    AppPaths paths(dir.path());
    QVERIFY(paths.ensureDirectories());

    AppDatabase database(paths.databasePath());
    QVERIFY2(database.open(), qPrintable(database.lastError()));
    QVERIFY(QFile::exists(paths.databasePath()));
}

void BookRepositoryTest::savesAndLoadsBook()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    AppDatabase database = openTemporaryDatabase(dir);
    BookRepository repository(database.database());

    Book book;
    book.id = "book-1";
    book.title = "长篇小说";
    book.author = "作者";
    book.format = "txt";
    book.originalName = "novel.txt";
    book.filePath = "/books/book-1/original.txt";
    book.coverPath = "";
    book.importedAt = QDateTime::fromMSecsSinceEpoch(1000);
    book.lastOpenedAt = QDateTime::fromMSecsSinceEpoch(2000);
    book.progress = 0.37;

    QVERIFY2(repository.saveBook(book), qPrintable(repository.lastError()));

    const QVector<Book> books = repository.books();
    QCOMPARE(books.size(), 1);
    QCOMPARE(books.first().id, book.id);
    QCOMPARE(books.first().title, book.title);
    QCOMPARE(books.first().format, book.format);
    QCOMPARE(books.first().originalName, book.originalName);
    QCOMPARE(books.first().filePath, book.filePath);
    QCOMPARE(books.first().progress, book.progress);
}

void BookRepositoryTest::savesAndLoadsReadingPosition()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    AppDatabase database = openTemporaryDatabase(dir);
    BookRepository repository(database.database());

    const QString locator = R"({"type":"txt","offset":120,"progress":0.4})";
    QVERIFY2(repository.saveReadingPosition("book-1", locator), qPrintable(repository.lastError()));
    QCOMPARE(repository.readingPosition("book-1"), locator);
}

QTEST_MAIN(BookRepositoryTest)
#include "tst_bookrepository.moc"
