import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import EbookReader
import EbookReader.Backend

Page {
    id: page

    property string bookId: ""
    property string filePath: ""
    property string bookFormat: ""    // authoritative format from the library
    property string bookTitle: ""     // authoritative title from the library
    property string pageTurnMode: "vertical"

    // TXT state
    property int currentChapterIndex: 0
    property var textChapters: []

    // PDF state
    property bool pdfLoaded: false
    property int pdfPageCount: 0
    property string pdfInfoError: ""
    property string pdfRenderError: ""

    // EPUB state
    property var epubChapters: []
    property int currentEpubChapterIndex: 0
    property string epubChapterHtml: ""
    property string epubError: ""

    readonly property string activeFormat: page.bookFormat.length > 0
        ? page.bookFormat.toLowerCase()
        : controller.format

    readonly property string safePdfStatusText: pdfRenderError.length > 0
        ? pdfRenderError
        : pdfLoaded
            ? "正在渲染 PDF 页面..."
            : pdfInfoError.length > 0
                ? pdfInfoError
                : "请选择一本 PDF 书籍"

    signal backRequested()
    signal settingsRequested()

    ReaderController {
        id: controller
        bookId: page.bookId
        format: page.bookFormat
        title: page.bookTitle
    }

    Component.onCompleted: refreshDocuments()
    onFilePathChanged: refreshDocuments()
    onActiveFormatChanged: refreshDocuments()

    header: ReaderToolbar {
        title: page.bookTitle.length > 0 ? page.bookTitle : controller.title
        onBackRequested: page.backRequested()
        onContentsRequested: contentsPopup.open()
        onSettingsRequested: page.settingsRequested()
    }

    Rectangle {
        anchors.fill: parent
        color: page.activeFormat === "pdf" ? "#eee7dc" : "#f7f1e8"

        Loader {
            id: readerLoader
            anchors.fill: parent
            sourceComponent: page.activeFormat === "txt" ? txtReaderComponent
                           : page.activeFormat === "pdf" ? pdfReaderComponent
                           : page.activeFormat === "epub" ? epubReaderComponent
                           : placeholderComponent
        }
    }

    Popup {
        id: contentsPopup
        modal: true
        focus: true
        width: Math.min(page.width - 48, 360)
        height: Math.min(page.height - 96, 420)
        x: page.width - width - 24
        y: 24
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Label {
                text: "目录"
                font.pixelSize: 20
                font.bold: true
                Layout.fillWidth: true
            }

            Label {
                text: page.activeFormat === "pdf"
                    ? "PDF 暂未提供书签解析。"
                    : page.activeFormat === "epub"
                        ? (page.epubChapters.length === 0
                            ? (page.epubError.length > 0 ? page.epubError : "EPUB 章节加载中...")
                            : "")
                        : page.textChapters.length === 0
                            ? "未识别到章节。"
                            : ""
                color: "#64748b"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                visible: text.length > 0
            }

            ListView {
                id: contentsList
                visible: (page.activeFormat === "txt" && page.textChapters.length > 0)
                      || (page.activeFormat === "epub" && page.epubChapters.length > 0)
                model: page.activeFormat === "txt" ? page.textChapters : page.epubChapters
                clip: true
                Layout.fillWidth: true
                Layout.fillHeight: true

                delegate: ItemDelegate {
                    width: ListView.view.width
                    text: modelData.title
                    highlighted: page.activeFormat === "txt"
                        ? (modelData.index === page.currentChapterIndex)
                        : (modelData.index === page.currentEpubChapterIndex)
                    onClicked: {
                        if (page.activeFormat === "txt") {
                            page.currentChapterIndex = modelData.index
                        } else if (page.activeFormat === "epub") {
                            page.selectEpubChapter(modelData.index)
                        }
                        contentsPopup.close()
                    }
                }
            }

            Button {
                text: "关闭"
                Layout.alignment: Qt.AlignRight
                onClicked: contentsPopup.close()
            }
        }
    }

    Component {
        id: txtReaderComponent

        TxtReader {
            content: page.filePath.length > 0
                ? controller.loadTextChapter(page.filePath, page.currentChapterIndex)
                : "请选择一本 TXT 书籍"
            pageTurnMode: page.pageTurnMode
            onPreviousBoundaryRequested: page.previousChapter()
            onNextBoundaryRequested: page.nextChapter()
            onPositionChanged: function(progress) {
                controller.saveLocator(JSON.stringify({
                    type: "txt",
                    chapterIndex: page.currentChapterIndex,
                    progress: progress
                }))
            }
        }
    }

    Component {
        id: placeholderComponent

        Label {
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "请选择一本书开始阅读"
            color: "#4b5563"
        }
    }

    Component {
        id: pdfReaderComponent

        PdfReader {
            id: pdfView
            pageCount: page.pdfPageCount
            statusText: page.safePdfStatusText
            onPageRequested: function(requestedPage, requestedZoom) {
                if (page.activeFormat !== "pdf" || page.filePath.length === 0) {
                    page.pdfRenderError = ""
                    return
                }

                const render = controller.renderPdfPage(page.filePath, requestedPage, requestedZoom) || {}
                page.pdfRenderError = render.rendered === true ? "" : String(render.error || "PDF 页面渲染失败")
                pdfView.updatePageImage(requestedPage, render.rendered === true ? String(render.imageUrl || "") : "")
            }
            onLocatorChanged: function(locatorJson) {
                controller.saveLocator(locatorJson)
            }
        }
    }

    Component {
        id: epubReaderComponent

        EpubReader {
            chapters: page.epubChapters
            currentIndex: page.currentEpubChapterIndex
            chapterHtml: page.epubChapterHtml
            errorText: page.epubError
            onChapterSelected: function(index) {
                page.selectEpubChapter(index)
            }
            onLocatorChanged: function(locatorJson) {
                controller.saveLocator(locatorJson)
            }
        }
    }

    function refreshTextChapters() {
        if (activeFormat !== "txt" || filePath.length === 0) {
            textChapters = []
            currentChapterIndex = 0
            return
        }

        textChapters = controller.loadTextChapters(filePath)
        if (currentChapterIndex >= textChapters.length) {
            currentChapterIndex = 0
        }
    }

    function refreshPdfInfo() {
        if (activeFormat !== "pdf" || filePath.length === 0) {
            pdfLoaded = false
            pdfPageCount = 0
            pdfInfoError = ""
            pdfRenderError = ""
            return
        }

        const info = controller.loadPdfInfo(filePath) || {}
        pdfLoaded = info.loaded === true
        pdfPageCount = Number(info.pageCount || 0)
        pdfInfoError = String(info.error || "")
        pdfRenderError = pdfLoaded ? "" : pdfInfoError
    }

    function refreshEpubInfo() {
        if (activeFormat !== "epub" || filePath.length === 0) {
            epubChapters = []
            currentEpubChapterIndex = 0
            epubChapterHtml = ""
            epubError = ""
            return
        }

        epubChapters = controller.loadEpubChapters(filePath) || []
        if (epubChapters.length === 0) {
            epubChapterHtml = ""
            epubError = controller.epubLastError()
            return
        }

        epubError = ""
        if (currentEpubChapterIndex >= epubChapters.length) {
            currentEpubChapterIndex = 0
        }
        epubChapterHtml = controller.loadEpubChapter(filePath, epubChapters[currentEpubChapterIndex].id) || ""
    }

    function refreshDocuments() {
        refreshTextChapters()
        refreshPdfInfo()
        refreshEpubInfo()
    }

    function previousChapter() {
        if (currentChapterIndex > 0) {
            currentChapterIndex -= 1
        }
    }

    function nextChapter() {
        if (currentChapterIndex + 1 < textChapters.length) {
            currentChapterIndex += 1
        }
    }

    function selectEpubChapter(index) {
        if (index < 0 || index >= epubChapters.length) {
            return
        }
        currentEpubChapterIndex = index
        epubChapterHtml = controller.loadEpubChapter(filePath, epubChapters[index].id) || ""
        controller.saveLocator(JSON.stringify({
            type: "epub",
            chapterId: epubChapters[index].id,
            chapterIndex: index
        }))
    }
}
