#include "PdfDocument.h"

#include <QFileInfo>
#include <QPdfDocument>

namespace {
QString errorToMessage(QPdfDocument::Error error)
{
    switch (error) {
    case QPdfDocument::Error::None:
        return {};
    case QPdfDocument::Error::DataNotYetAvailable:
        return QStringLiteral("PDF 数据尚未就绪");
    case QPdfDocument::Error::FileNotFound:
        return QStringLiteral("PDF 文件不存在");
    case QPdfDocument::Error::InvalidFileFormat:
        return QStringLiteral("文件不是有效的 PDF");
    case QPdfDocument::Error::IncorrectPassword:
        return QStringLiteral("PDF 受密码保护");
    case QPdfDocument::Error::UnsupportedSecurityScheme:
        return QStringLiteral("不支持的 PDF 加密方式");
    case QPdfDocument::Error::Unknown:
        break;
    }
    return QStringLiteral("无法打开 PDF 文件");
}
}

PdfDocument::PdfDocument() = default;
PdfDocument::~PdfDocument() = default;
PdfDocument::PdfDocument(PdfDocument &&) noexcept = default;
PdfDocument &PdfDocument::operator=(PdfDocument &&) noexcept = default;

bool PdfDocument::load(const QString &filePath)
{
    m_pageCount = 0;
    m_lastError.clear();
    m_document.reset();

    const QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        m_lastError = QStringLiteral("PDF 文件不存在或无法读取");
        return false;
    }

    auto document = std::make_unique<QPdfDocument>();
    const QPdfDocument::Error error = document->load(filePath);
    if (error != QPdfDocument::Error::None) {
        m_lastError = errorToMessage(error);
        return false;
    }

    m_pageCount = document->pageCount();
    if (m_pageCount <= 0) {
        m_lastError = QStringLiteral("无法识别 PDF 页数");
        return false;
    }

    m_document = std::move(document);
    return true;
}

int PdfDocument::pageCount() const
{
    return m_pageCount;
}

QString PdfDocument::lastError() const
{
    return m_lastError;
}

QPdfDocument *PdfDocument::handle() const
{
    return m_document.get();
}
