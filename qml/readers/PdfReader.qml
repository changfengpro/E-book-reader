import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property int currentPage: 1
    property var pageCount: 0
    property real zoom: 1.0
    property var imageUrl: ""
    property var statusText: ""
    readonly property int safePageCount: Number(pageCount || 0)
    readonly property string safeImageUrl: String(imageUrl || "")
    readonly property string safeStatusText: String(statusText || "")

    signal pageRequested(int page, real zoom)
    signal locatorChanged(string locatorJson)

    function previousPage() {
        if (root.currentPage > 1) {
            root.currentPage -= 1
            root.pageRequested(root.currentPage, root.zoom)
            root.locatorChanged(JSON.stringify({ type: "pdf", page: root.currentPage, zoom: root.zoom }))
        }
    }

    function nextPage() {
        if (root.currentPage < root.safePageCount) {
            root.currentPage += 1
            root.pageRequested(root.currentPage, root.zoom)
            root.locatorChanged(JSON.stringify({ type: "pdf", page: root.currentPage, zoom: root.zoom }))
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
                enabled: root.currentPage > 1
                onClicked: root.previousPage()
            }

            Label {
                text: root.safePageCount > 0 ? root.currentPage + " / " + root.safePageCount : "0 / 0"
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Button {
                text: "下一页"
                enabled: root.currentPage < root.safePageCount
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
                root.pageRequested(root.currentPage, root.zoom)
                root.locatorChanged(JSON.stringify({ type: "pdf", page: root.currentPage, zoom: root.zoom }))
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
                source: root.safeImageUrl
                fillMode: Image.PreserveAspectFit
                cache: false
                asynchronous: true
                width: Math.min(implicitWidth, parent.width)
            }

            Label {
                anchors.centerIn: parent
                width: Math.min(parent.width - 48, 520)
                visible: root.safeImageUrl.length === 0
                text: root.safeStatusText
                color: "#4b5563"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }
    }
}
