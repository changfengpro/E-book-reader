import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ToolBar {
    id: root

    property string title: "阅读"

    signal backRequested()
    signal contentsRequested()
    signal settingsRequested()

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 10

        ToolButton {
            text: "返回"
            onClicked: root.backRequested()
        }

        Label {
            text: root.title
            font.pixelSize: 18
            font.bold: true
            elide: Text.ElideRight
            Layout.fillWidth: true
        }

        ToolButton {
            text: "目录"
            onClicked: root.contentsRequested()
        }

        ToolButton {
            text: "设置"
            onClicked: root.settingsRequested()
        }
    }
}
