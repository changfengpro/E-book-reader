import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property string content: ""
    property string pageTurnMode: "vertical"

    signal positionChanged(real progress)
    signal previousBoundaryRequested()
    signal nextBoundaryRequested()

    function previousPage() {
        const step = Math.max(80, flick.height - 56)
        if (flick.contentY <= 1) {
            root.previousBoundaryRequested()
            return
        }
        flick.contentY = Math.max(0, flick.contentY - step)
    }

    function nextPage() {
        const step = Math.max(80, flick.height - 56)
        const maxY = Math.max(0, flick.contentHeight - flick.height)
        if (flick.contentY >= maxY - 1) {
            root.nextBoundaryRequested()
            return
        }
        flick.contentY = Math.min(maxY, flick.contentY + step)
    }

    onContentChanged: flick.contentY = 0

    Flickable {
        id: flick
        anchors.fill: parent
        anchors.bottomMargin: 56
        clip: true
        contentWidth: width
        contentHeight: textItem.implicitHeight + 48
        boundsBehavior: Flickable.StopAtBounds

        onContentYChanged: {
            const scrollable = Math.max(1, contentHeight - height)
            root.positionChanged(Math.max(0, Math.min(1, contentY / scrollable)))
        }

        Text {
            id: textItem
            width: flick.width - 48
            x: 24
            y: 24
            text: root.content.length > 0 ? root.content : "请选择一本 TXT 书籍"
            wrapMode: Text.WordWrap
            font.pixelSize: 20
            lineHeight: 1.45
            color: "#2d2925"
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 56
        color: "#f7f1e8"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            spacing: 12

            Button {
                text: root.pageTurnMode === "horizontal" ? "上一页" : "上一屏"
                Layout.fillWidth: true
                onClicked: root.previousPage()
            }

            Label {
                text: root.pageTurnMode === "horizontal" ? "左右翻页" : "上下翻页"
                horizontalAlignment: Text.AlignHCenter
                color: "#64748b"
                Layout.preferredWidth: 112
            }

            Button {
                text: root.pageTurnMode === "horizontal" ? "下一页" : "下一屏"
                Layout.fillWidth: true
                onClicked: root.nextPage()
            }
        }
    }
}
