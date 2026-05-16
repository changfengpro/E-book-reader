#include <QtTest>

#include <QPageLayout>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <QTemporaryDir>
#include <QTemporaryFile>

#include "reader/PdfDocument.h"

class PdfDocumentTest : public QObject {
    Q_OBJECT

private slots:
    void readsPageCount();
    void rejectsMissingFile();
    void rejectsNonPdfFile();
};

static QString writeMultiPagePdf(QTemporaryDir &dir, int pages)
{
    const QString path = dir.filePath(QStringLiteral("sample.pdf"));
    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    QPainter painter(&writer);
    for (int i = 1; i <= pages; ++i) {
        painter.drawText(100, 100, QStringLiteral("Page %1").arg(i));
        if (i < pages) {
            writer.newPage();
        }
    }
    painter.end();
    return path;
}

void PdfDocumentTest::readsPageCount()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString pdfPath = writeMultiPagePdf(dir, 3);
    QVERIFY(QFileInfo::exists(pdfPath));

    PdfDocument document;
    QVERIFY2(document.load(pdfPath), qPrintable(document.lastError()));
    QCOMPARE(document.pageCount(), 3);
}

void PdfDocumentTest::rejectsMissingFile()
{
    PdfDocument document;
    QVERIFY(!document.load(QDir::tempPath() + "/missing-file.pdf"));
    QCOMPARE(document.pageCount(), 0);
    QVERIFY(!document.lastError().isEmpty());
}

void PdfDocumentTest::rejectsNonPdfFile()
{
    QTemporaryFile file;
    file.setAutoRemove(false);
    QVERIFY(file.open());
    file.write("this is not a pdf");
    file.close();

    PdfDocument document;
    QVERIFY(!document.load(file.fileName()));
    QVERIFY(!document.lastError().isEmpty());
}

QTEST_MAIN(PdfDocumentTest)
#include "tst_pdfdocument.moc"
