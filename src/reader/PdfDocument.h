#pragma once

#include <QString>

#include <memory>

class QPdfDocument;

class PdfDocument {
public:
    PdfDocument();
    ~PdfDocument();
    PdfDocument(const PdfDocument &) = delete;
    PdfDocument &operator=(const PdfDocument &) = delete;
    PdfDocument(PdfDocument &&) noexcept;
    PdfDocument &operator=(PdfDocument &&) noexcept;

    bool load(const QString &filePath);
    int pageCount() const;
    QString lastError() const;

    // Exposed so PdfPageRenderer can share an already-loaded document. Returns
    // nullptr until load() has succeeded.
    QPdfDocument *handle() const;

private:
    std::unique_ptr<QPdfDocument> m_document;
    int m_pageCount = 0;
    QString m_lastError;
};
