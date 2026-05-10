import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property int page: 1
    property int pageCount: 1
    property real zoom: 1.0
    property string errorText: ""

    signal locatorChanged(string locatorJson)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                text: "上一页"
                enabled: root.page > 1
                onClicked: {
                    root.page -= 1
                    root.locatorChanged(JSON.stringify({ type: "pdf", page: root.page, zoom: root.zoom }))
                }
            }

            Label {
                text: root.page + " / " + root.pageCount
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Button {
                text: "下一页"
                enabled: root.page < root.pageCount
                onClicked: {
                    root.page += 1
                    root.locatorChanged(JSON.stringify({ type: "pdf", page: root.page, zoom: root.zoom }))
                }
            }
        }

        Slider {
            from: 0.75
            to: 2.5
            value: root.zoom
            Layout.fillWidth: true
            onMoved: {
                root.zoom = value
                root.locatorChanged(JSON.stringify({ type: "pdf", page: root.page, zoom: root.zoom }))
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#ffffff"
            border.color: "#d7dee8"
            radius: 8

            Label {
                anchors.centerIn: parent
                width: parent.width - 48
                text: root.errorText.length > 0 ? root.errorText : "PDF 页面将在这里显示"
                color: "#4b5563"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }
    }
}
