#include <QtTest>

#include <QImage>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <QTemporaryDir>

#include "reader/PdfPageRenderer.h"

class PdfPageRendererTest : public QObject {
    Q_OBJECT

private slots:
    void rendersPageWithQtPdf();
    void reusesRenderedPageCache();
    void rejectsMissingFile();
};

static QString writeSimplePdf(QTemporaryDir &dir, const QString &message)
{
    const QString path = dir.filePath(QStringLiteral("render-sample.pdf"));
    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    QPainter painter(&writer);
    painter.drawText(100, 200, message);
    painter.end();
    return path;
}

void PdfPageRendererTest::rendersPageWithQtPdf()
{
    QTemporaryDir pdfDir;
    QTemporaryDir cacheDir;
    QVERIFY(pdfDir.isValid());
    QVERIFY(cacheDir.isValid());

    const QString pdfPath = writeSimplePdf(pdfDir, QStringLiteral("Qt PDF render test"));
    PdfPageRenderer renderer(cacheDir.path());

    const QString imagePath = renderer.renderPage(pdfPath, 1, 1024);
    QVERIFY2(!imagePath.isEmpty(), qPrintable(renderer.lastError()));
    QVERIFY(QFileInfo::exists(imagePath));
    QVERIFY(QFileInfo(imagePath).size() > 0);

    QImage image(imagePath);
    QVERIFY(!image.isNull());
    QVERIFY(image.width() > 0);
    QVERIFY(image.height() > 0);
}

void PdfPageRendererTest::reusesRenderedPageCache()
{
    QTemporaryDir pdfDir;
    QTemporaryDir cacheDir;
    QVERIFY(pdfDir.isValid());
    QVERIFY(cacheDir.isValid());

    const QString pdfPath = writeSimplePdf(pdfDir, QStringLiteral("Cached render test"));
    PdfPageRenderer renderer(cacheDir.path());
    const QString firstImagePath = renderer.renderPage(pdfPath, 1, 1024);
    QVERIFY2(!firstImagePath.isEmpty(), qPrintable(renderer.lastError()));

    const QDateTime firstModified = QFileInfo(firstImagePath).lastModified();
    QTest::qWait(1100);

    const QString secondImagePath = renderer.renderPage(pdfPath, 1, 1024);
    QVERIFY2(!secondImagePath.isEmpty(), qPrintable(renderer.lastError()));
    QCOMPARE(secondImagePath, firstImagePath);
    QCOMPARE(QFileInfo(secondImagePath).lastModified(), firstModified);
}

void PdfPageRendererTest::rejectsMissingFile()
{
    QTemporaryDir cacheDir;
    QVERIFY(cacheDir.isValid());

    PdfPageRenderer renderer(cacheDir.path());
    const QString imagePath = renderer.renderPage(
        cacheDir.filePath(QStringLiteral("nope.pdf")), 1, 1024);
    QVERIFY(imagePath.isEmpty());
    QVERIFY(!renderer.lastError().isEmpty());
}

QTEST_MAIN(PdfPageRendererTest)
#include "tst_pdfpagerenderer.moc"
