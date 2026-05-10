import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property string title: "书架还是空的"
    property string description: "导入本地 TXT、PDF 或 EPUB 文件后，可以在这里继续阅读。"
    property string actionText: "导入书籍"
    property bool actionVisible: true

    signal importRequested()

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(root.width - 48, 420)
        spacing: 16

        Label {
            text: root.title
            font.pixelSize: 28
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        Label {
            text: root.description
            color: "#64748b"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        Button {
            visible: root.actionVisible
            text: root.actionText
            Layout.alignment: Qt.AlignHCenter
            onClicked: root.importRequested()
        }
    }
}
