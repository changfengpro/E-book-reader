import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import EbookReader
import EbookReader.Backend

Page {
    id: page

    property string bookId: ""
    property string filePath: ""
    property string formatOverride: ""
    property string titleOverride: ""
    property string pageTurnMode: "vertical"
    property int currentChapterIndex: 0
    property var textChapters: []
    property bool pdfLoaded: false
    property int pdfPageCount: 0
    property string pdfInfoError: ""
    property string pdfRenderError: ""
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
    }

    Component.onCompleted: refreshDocuments()
    onFilePathChanged: refreshDocuments()
    onActiveFormatChanged: refreshDocuments()

    header: ReaderToolbar {
        title: page.titleOverride.length > 0 ? page.titleOverride : controller.title
        onBackRequested: page.backRequested()
        onContentsRequested: contentsPopup.open()
        onSettingsRequested: page.settingsRequested()
    }

    Rectangle {
        anchors.fill: parent
        color: activeFormat === "pdf" ? "#eee7dc" : "#f7f1e8"

        Loader {
            anchors.fill: parent
            sourceComponent: activeFormat === "txt" ? txtReaderComponent
                           : activeFormat === "pdf" ? pdfReaderComponent
                           : activeFormat === "epub" ? epubReaderComponent
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
                text: activeFormat === "epub"
                    ? "EPUB 目录解析模块接入后会显示章节。"
                    : activeFormat === "pdf"
                        ? "PDF 目录解析模块接入后会显示书签。"
                        : page.textChapters.length === 0
                            ? "未识别到章节。"
                            : ""
                color: "#64748b"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                visible: text.length > 0
            }

            ListView {
                visible: activeFormat === "txt" && page.textChapters.length > 0
                model: page.textChapters
                clip: true
                Layout.fillWidth: true
                Layout.fillHeight: true

                delegate: ItemDelegate {
                    width: ListView.view.width
                    text: modelData.title
                    highlighted: modelData.index === page.currentChapterIndex
                    onClicked: {
                        page.currentChapterIndex = modelData.index
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

    readonly property string activeFormat: page.formatOverride.length > 0 ? page.formatOverride : controller.format

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
            pageCount: page.pdfPageCount
            statusText: page.safePdfStatusText
            onPageRequested: function(requestedPage, requestedZoom) {
                if (page.activeFormat !== "pdf" || page.filePath.length === 0) {
                    page.pdfRenderError = ""
                    return
                }

                const render = controller.renderPdfPage(page.filePath, requestedPage, requestedZoom) || {}
                page.pdfRenderError = render.rendered === true ? "" : String(render.error || "PDF 页面渲染失败")
                updatePageImage(requestedPage, render.rendered === true ? String(render.imageUrl || "") : "")
            }
            onLocatorChanged: function(locatorJson) {
                controller.saveLocator(locatorJson)
            }
        }
    }

    Component {
        id: epubReaderComponent

        EpubReader {
            errorText: "当前构建未启用 EPUB 解包模块"
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

    function refreshDocuments() {
        refreshTextChapters()
        refreshPdfInfo()
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
}
