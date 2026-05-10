import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property string title: "EPUB"
    property string bodyText: ""
    property string errorText: ""

    signal locatorChanged(string locatorJson)

    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Frame {
            Layout.preferredWidth: 240
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 12

                Label {
                    text: "目录"
                    font.pixelSize: 18
                    font.bold: true
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: [root.title]

                    delegate: ItemDelegate {
                        width: ListView.view.width
                        text: modelData
                        onClicked: root.locatorChanged(JSON.stringify({ type: "epub", chapter: modelData }))
                    }
                }
            }
        }

        Flickable {
            clip: true
            contentWidth: width
            contentHeight: chapterText.implicitHeight + 48
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                id: chapterText
                x: 24
                y: 24
                width: parent.width - 48
                text: root.errorText.length > 0 ? root.errorText
                    : root.bodyText.length > 0 ? root.bodyText
                    : "EPUB 章节将在这里显示"
                wrapMode: Text.WordWrap
                font.pixelSize: 20
                lineHeight: 1.45
                color: "#2d2925"
            }
        }
    }
}
