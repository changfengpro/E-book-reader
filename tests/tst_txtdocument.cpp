#include <QtTest>

#include "reader/TxtDocument.h"

class TxtDocumentTest : public QObject {
    Q_OBJECT

private slots:
    void readsUtf8Text();
    void readsUtf16Text();
    void readsGb18030Text();
    void extractsTxtChapters();
};

static QString writeTempFile(const QByteArray &data)
{
    QTemporaryFile file;
    file.setAutoRemove(false);
    const bool opened = file.open();
    Q_ASSERT(opened);
    file.write(data);
    file.close();
    return file.fileName();
}

void TxtDocumentTest::readsUtf8Text()
{
    const QString expected = QString::fromUtf8("第一章\n你好，Qt");

    TxtDocument document;
    QVERIFY2(document.load(writeTempFile(expected.toUtf8())), qPrintable(document.lastError()));
    QCOMPARE(document.text(), expected);
    QCOMPARE(document.length(), expected.size());
}

void TxtDocumentTest::readsUtf16Text()
{
    const QString expected = QString::fromUtf8("你好，平板");
    QByteArray data;
    data.append(char(0xff));
    data.append(char(0xfe));
    data.append(reinterpret_cast<const char *>(expected.utf16()), expected.size() * 2);

    TxtDocument document;
    QVERIFY2(document.load(writeTempFile(data)), qPrintable(document.lastError()));
    QCOMPARE(document.text(), expected);
}

void TxtDocumentTest::readsGb18030Text()
{
    const QByteArray data = QByteArray::fromHex("d6d0cec4");

    TxtDocument document;
    QVERIFY2(document.load(writeTempFile(data)), qPrintable(document.lastError()));
    QCOMPARE(document.text(), QString::fromUtf8("中文"));
}

void TxtDocumentTest::extractsTxtChapters()
{
    const QString text = QString::fromUtf8(
        "简介\n"
        "第1章 初入修真\n"
        "这里是第一章正文\n"
        "第十二章 风起\n"
        "这里是第二章正文\n");

    TxtDocument document;
    QVERIFY2(document.load(writeTempFile(text.toUtf8())), qPrintable(document.lastError()));

    const QList<TxtChapter> chapters = document.chapters();
    QCOMPARE(chapters.size(), 3);
    QCOMPARE(chapters.at(0).title, QString::fromUtf8("正文"));
    QCOMPARE(chapters.at(1).title, QString::fromUtf8("第1章 初入修真"));
    QCOMPARE(chapters.at(2).title, QString::fromUtf8("第十二章 风起"));
    QVERIFY(document.chapterText(1).contains(QString::fromUtf8("第一章正文")));
    QVERIFY(!document.chapterText(1).contains(QString::fromUtf8("第二章正文")));
}

QTEST_MAIN(TxtDocumentTest)
#include "tst_txtdocument.moc"
