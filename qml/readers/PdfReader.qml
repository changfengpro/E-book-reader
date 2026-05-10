import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property int page: 1
    property int pageCount: 0
    property real zoom: 1.0
    property string statusText: ""

    signal locatorChanged(string locatorJson)

    function previousPage() {
        if (root.page > 1) {
            root.page -= 1
            root.locatorChanged(JSON.stringify({ type: "pdf", page: root.page, zoom: root.zoom }))
        }
    }

    function nextPage() {
        if (root.page < root.pageCount) {
            root.page += 1
            root.locatorChanged(JSON.stringify({ type: "pdf", page: root.page, zoom: root.zoom }))
        }
    }

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
                onClicked: root.previousPage()
            }

            Label {
                text: root.pageCount > 0 ? root.page + " / " + root.pageCount : "0 / 0"
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Button {
                text: "下一页"
                enabled: root.page < root.pageCount
                onClicked: root.nextPage()
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

            ColumnLayout {
                anchors.centerIn: parent
                width: Math.min(parent.width - 48, 520)
                spacing: 10

                Label {
                    text: root.statusText
                    color: "#4b5563"
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Label {
                    visible: root.pageCount > 0
                    text: "已识别 PDF 页数，渲染后端接入后这里会显示页面内容。"
                    color: "#64748b"
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }
    }
}
