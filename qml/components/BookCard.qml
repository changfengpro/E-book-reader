import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Control {
    id: root

    property string bookId: ""
    property string title: ""
    property string format: ""
    property real progress: 0

    signal openBook(string bookId)

    implicitWidth: 180
    implicitHeight: 260
    hoverEnabled: true

    background: Rectangle {
        color: root.hovered ? "#f8fafc" : "#ffffff"
        radius: 8
        border.color: root.hovered ? "#7aa7bd" : "#d7dee8"
    }

    contentItem: MouseArea {
        hoverEnabled: true
        onClicked: root.openBook(root.bookId)

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 10

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                radius: 6
                color: root.format === "pdf" ? "#3f6f78"
                     : root.format === "epub" ? "#936b8f"
                     : "#8b5e3c"

                Label {
                    anchors.centerIn: parent
                    text: root.format.toUpperCase()
                    color: "white"
                    font.pixelSize: 20
                    font.bold: true
                }
            }

            Label {
                text: root.title
                font.pixelSize: 15
                font.bold: true
                elide: Text.ElideRight
                maximumLineCount: 2
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                Layout.preferredHeight: 42
            }

            ProgressBar {
                value: root.progress
                from: 0
                to: 1
                Layout.fillWidth: true
            }

            Label {
                text: Math.round(root.progress * 100) + "%"
                color: "#64748b"
                font.pixelSize: 12
            }
        }
    }
}
