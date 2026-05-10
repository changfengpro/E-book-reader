import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    property string bookId: ""

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

        Label {
            anchors.centerIn: parent
            text: "请选择一本书开始阅读"
            color: "#4b5563"
        }
    }
}
