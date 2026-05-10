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

    signal backRequested()
    signal settingsRequested()

    ReaderController {
        id: controller
        bookId: page.bookId
    }

    header: ReaderToolbar {
        title: page.titleOverride.length > 0 ? page.titleOverride : controller.title
        onBackRequested: page.backRequested()
        onContentsRequested: contentsPopup.open()
        onSettingsRequested: page.settingsRequested()
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
                        : "TXT 文件暂按连续文本阅读。"
                color: "#64748b"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.fillHeight: true
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
