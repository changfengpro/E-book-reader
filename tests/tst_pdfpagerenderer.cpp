#include <QtTest>

#include "reader/PdfPageRenderer.h"

class PdfPageRendererTest : public QObject {
    Q_OBJECT

private slots:
    void rendersPageWithMutool();
    void reusesRenderedPageCache();
};

static QString writeTempPdf(const QByteArray &data)
{
    QTemporaryFile file;
    file.setFileTemplate(QDir::tempPath() + "/render-sample-XXXXXX.pdf");
    file.setAutoRemove(false);
    const bool opened = file.open();
    Q_ASSERT(opened);
    file.write(data);
    file.close();
    return file.fileName();
}

void PdfPageRendererTest::rendersPageWithMutool()
{
    const QString mutoolPath = PdfPageRenderer::defaultMutoolPath();
    if (mutoolPath.isEmpty() || !QFileInfo::exists(mutoolPath)) {
        QSKIP("mutool.exe is not installed");
    }

    const QByteArray pdf = R"PDF(%PDF-1.4
1 0 obj
<< /Type /Catalog /Pages 2 0 R >>
endobj
2 0 obj
<< /Type /Pages /Count 1 /Kids [3 0 R] >>
endobj
3 0 obj
<< /Type /Page /Parent 2 0 R /MediaBox [0 0 300 180] /Contents 4 0 R /Resources << /Font << /F1 5 0 R >> >> >>
endobj
4 0 obj
<< /Length 66 >>
stream
BT /F1 24 Tf 40 100 Td (MuPDF render test) Tj ET
endstream
endobj
5 0 obj
<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>
endobj
%%EOF
)PDF";

    QTemporaryDir cacheDir;
    QVERIFY(cacheDir.isValid());

    PdfPageRenderer renderer(cacheDir.path());
    const QString imagePath = renderer.renderPage(writeTempPdf(pdf), 1, 1.0);
    QVERIFY2(!imagePath.isEmpty(), qPrintable(renderer.lastError()));
    QVERIFY(QFileInfo::exists(imagePath));
    QVERIFY(QFileInfo(imagePath).size() > 0);
}

void PdfPageRendererTest::reusesRenderedPageCache()
{
    const QString mutoolPath = PdfPageRenderer::defaultMutoolPath();
    if (mutoolPath.isEmpty() || !QFileInfo::exists(mutoolPath)) {
        QSKIP("mutool.exe is not installed");
    }

    const QByteArray pdf = R"PDF(%PDF-1.4
1 0 obj
<< /Type /Catalog /Pages 2 0 R >>
endobj
2 0 obj
<< /Type /Pages /Count 1 /Kids [3 0 R] >>
endobj
3 0 obj
<< /Type /Page /Parent 2 0 R /MediaBox [0 0 300 180] /Contents 4 0 R /Resources << /Font << /F1 5 0 R >> >> >>
endobj
4 0 obj
<< /Length 58 >>
stream
BT /F1 24 Tf 40 100 Td (Cached render test) Tj ET
endstream
endobj
5 0 obj
<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>
endobj
%%EOF
)PDF";

    QTemporaryDir cacheDir;
    QVERIFY(cacheDir.isValid());

    PdfPageRenderer renderer(cacheDir.path());
    const QString pdfPath = writeTempPdf(pdf);
    const QString firstImagePath = renderer.renderPage(pdfPath, 1, 1.0);
    QVERIFY2(!firstImagePath.isEmpty(), qPrintable(renderer.lastError()));

    const QDateTime firstModified = QFileInfo(firstImagePath).lastModified();
    QTest::qWait(1100);

    const QString secondImagePath = renderer.renderPage(pdfPath, 1, 1.0);
    QVERIFY2(!secondImagePath.isEmpty(), qPrintable(renderer.lastError()));
    QCOMPARE(secondImagePath, firstImagePath);
    QCOMPARE(QFileInfo(secondImagePath).lastModified(), firstModified);
}

QTEST_MAIN(PdfPageRendererTest)
#include "tst_pdfpagerenderer.moc"
