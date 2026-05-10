import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property int page: 1
    property int pageCount: 0
    property real zoom: 1.0
    property string imageUrl: ""
    property string statusText: ""

    signal pageRequested(int page, real zoom)
    signal locatorChanged(string locatorJson)

    function previousPage() {
        if (root.page > 1) {
            root.page -= 1
            root.pageRequested(root.page, root.zoom)
            root.locatorChanged(JSON.stringify({ type: "pdf", page: root.page, zoom: root.zoom }))
        }
    }

    function nextPage() {
        if (root.page < root.pageCount) {
            root.page += 1
            root.pageRequested(root.page, root.zoom)
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
                root.pageRequested(root.page, root.zoom)
                root.locatorChanged(JSON.stringify({ type: "pdf", page: root.page, zoom: root.zoom }))
            }
        }

        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: Math.max(width, pageImage.implicitWidth)
            contentHeight: Math.max(height, pageImage.implicitHeight)
            boundsBehavior: Flickable.StopAtBounds

            Image {
                id: pageImage
                anchors.centerIn: parent
                source: root.imageUrl
                fillMode: Image.PreserveAspectFit
                cache: false
                asynchronous: true
                width: Math.min(implicitWidth, parent.width)
            }

            Label {
                anchors.centerIn: parent
                width: Math.min(parent.width - 48, 520)
                visible: root.imageUrl.length === 0
                text: root.statusText
                color: "#4b5563"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }
    }
}
