import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    signal openBook(string bookId)
    signal openSettings()

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16

            Label {
                text: "书架"
                font.pixelSize: 22
                font.bold: true
                Layout.fillWidth: true
            }

            Button {
                text: "设置"
                onClicked: page.openSettings()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            text: "本地书库"
            font.pixelSize: 28
            font.bold: true
        }

        Label {
            text: "导入本地书籍后，可在这里继续最近阅读。"
            color: "#64748b"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Button {
            text: "打开阅读页"
            onClicked: page.openBook("sample")
        }
    }
}
