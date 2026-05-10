#include <QtTest>

#include "storage/AppDatabase.h"
#include "storage/AppPaths.h"

class BookRepositoryTest : public QObject {
    Q_OBJECT

private slots:
    void createsDatabaseFile();
};

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

QTEST_MAIN(BookRepositoryTest)
#include "tst_bookrepository.moc"
