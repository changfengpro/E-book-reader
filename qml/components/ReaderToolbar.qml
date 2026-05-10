import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ToolBar {
    id: root

    property string title: "阅读"
    implicitHeight: 56

    signal backRequested()
    signal contentsRequested()
    signal settingsRequested()

    background: Rectangle {
        color: "#fbf7ef"

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 1
            color: "#e7dccd"
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 8

        ToolButton {
            text: "‹"
            font.pixelSize: 30
            implicitWidth: 44
            implicitHeight: 44
            onClicked: root.backRequested()

            ToolTip.visible: hovered
            ToolTip.text: "返回"
        }

        Label {
            text: root.title
            font.pixelSize: 17
            font.bold: true
            elide: Text.ElideRight
            color: "#241f1a"
            Layout.fillWidth: true
        }

        ToolButton {
            text: "目录"
            implicitHeight: 40
            onClicked: root.contentsRequested()
        }

        ToolButton {
            text: "设置"
            implicitHeight: 40
            onClicked: root.settingsRequested()
        }
    }
}
