#include <QtTest>

#include "reader/PdfDocument.h"

class PdfDocumentTest : public QObject {
    Q_OBJECT

private slots:
    void readsPageCountFromPdfObjects();
    void readsPageCountWithMutoolWhenObjectsAreCompressed();
    void rejectsMissingFile();
};

static QString writeTempPdf(const QByteArray &data)
{
    QTemporaryFile file;
    file.setFileTemplate(QDir::tempPath() + "/sample-XXXXXX.pdf");
    file.setAutoRemove(false);
    const bool opened = file.open();
    Q_ASSERT(opened);
    file.write(data);
    file.close();
    return file.fileName();
}

void PdfDocumentTest::readsPageCountFromPdfObjects()
{
    const QByteArray pdf = R"PDF(%PDF-1.4
1 0 obj
<< /Type /Catalog /Pages 2 0 R >>
endobj
2 0 obj
<< /Type /Pages /Count 3 /Kids [3 0 R 4 0 R 5 0 R] >>
endobj
3 0 obj
<< /Type /Page /Parent 2 0 R >>
endobj
4 0 obj
<< /Type /Page /Parent 2 0 R >>
endobj
5 0 obj
<< /Type /Page /Parent 2 0 R >>
endobj
%%EOF
)PDF";

    PdfDocument document;
    QVERIFY2(document.load(writeTempPdf(pdf)), qPrintable(document.lastError()));
    QCOMPARE(document.pageCount(), 3);
}

void PdfDocumentTest::readsPageCountWithMutoolWhenObjectsAreCompressed()
{
    const QString pdfPath = QStringLiteral("G:/RoboMaster/quaternion.pdf");
    if (!QFileInfo::exists(pdfPath)) {
        QSKIP("quaternion.pdf is not available");
    }

    PdfDocument document;
    QVERIFY2(document.load(pdfPath), qPrintable(document.lastError()));
    QCOMPARE(document.pageCount(), 73);
}

void PdfDocumentTest::rejectsMissingFile()
{
    PdfDocument document;
    QVERIFY(!document.load(QDir::tempPath() + "/missing-file.pdf"));
    QVERIFY(document.pageCount() == 0);
    QVERIFY(!document.lastError().isEmpty());
}

QTEST_MAIN(PdfDocumentTest)
#include "tst_pdfdocument.moc"
