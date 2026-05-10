import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    signal importRequested()

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(root.width - 48, 420)
        spacing: 16

        Label {
            text: "书架还是空的"
            font.pixelSize: 28
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        Label {
            text: "导入本地 TXT、PDF 或 EPUB 文件后，可以在这里继续阅读。"
            color: "#64748b"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        Button {
            text: "导入书籍"
            Layout.alignment: Qt.AlignHCenter
            onClicked: root.importRequested()
        }
    }
}
