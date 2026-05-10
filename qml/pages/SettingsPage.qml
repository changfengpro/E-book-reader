import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

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
                text: "设置"
                font.pixelSize: 18
                font.bold: true
                Layout.fillWidth: true
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            text: "阅读设置"
            font.pixelSize: 28
            font.bold: true
        }

        Label {
            text: "调整默认阅读偏好和本地缓存。"
            color: "#64748b"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
}
