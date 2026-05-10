import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import EbookReader

Page {
    id: page

    property string bookId: ""
    readonly property string format: bookId.indexOf("pdf") >= 0 ? "pdf"
                                   : bookId.indexOf("epub") >= 0 ? "epub"
                                   : "txt"

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16

            ToolButton {
                text: "返回"
                onClicked: StackView.view.pop()
            }

            Label {
                text: bookId.length > 0 ? "阅读：" + bookId : "阅读"
                font.pixelSize: 18
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#f7f1e8"

        Loader {
            anchors.fill: parent
            sourceComponent: page.format === "txt" ? txtReaderComponent : placeholderComponent
        }
    }

    Component {
        id: txtReaderComponent

        TxtReader {
            content: "第一章\n\n夜色落在窗边，书页安静地展开。这里会显示当前书籍的正文，阅读位置会随着滚动保存。"
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
