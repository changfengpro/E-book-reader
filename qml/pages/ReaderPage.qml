import QtQuick
import QtQuick.Controls
import EbookReader
import EbookReader.Backend

Page {
    id: page

    property string bookId: ""
    property string filePath: ""
    property string formatOverride: ""
    property string titleOverride: ""

    ReaderController {
        id: controller
        bookId: page.bookId
    }

    header: ReaderToolbar {
        title: page.titleOverride.length > 0 ? page.titleOverride : controller.title
        onBackRequested: StackView.view.pop()
    }

    Rectangle {
        anchors.fill: parent
        color: "#f7f1e8"

        Loader {
            anchors.fill: parent
            sourceComponent: activeFormat === "txt" ? txtReaderComponent
                           : activeFormat === "pdf" ? pdfReaderComponent
                           : activeFormat === "epub" ? epubReaderComponent
                           : placeholderComponent
        }
    }

    readonly property string activeFormat: page.formatOverride.length > 0 ? page.formatOverride : controller.format

    Component {
        id: txtReaderComponent

        TxtReader {
            content: page.filePath.length > 0
                ? controller.loadTextFile(page.filePath)
                : "请选择一本 TXT 书籍"
            onPositionChanged: function(progress) {
                controller.saveLocator(JSON.stringify({
                    type: "txt",
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
            errorText: "当前构建未启用 PDF 渲染模块"
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
}
