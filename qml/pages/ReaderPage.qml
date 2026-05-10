import QtQuick
import QtQuick.Controls
import EbookReader
import EbookReader.Backend

Page {
    id: page

    property string bookId: ""

    ReaderController {
        id: controller
        bookId: page.bookId
    }

    header: ReaderToolbar {
        title: controller.title
        onBackRequested: StackView.view.pop()
    }

    Rectangle {
        anchors.fill: parent
        color: "#f7f1e8"

        Loader {
            anchors.fill: parent
            sourceComponent: controller.format === "txt" ? txtReaderComponent : placeholderComponent
        }
    }

    Component {
        id: txtReaderComponent

        TxtReader {
            content: "第一章\n\n夜色落在窗边，书页安静地展开。这里会显示当前书籍的正文，阅读位置会随着滚动保存。"
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
}
