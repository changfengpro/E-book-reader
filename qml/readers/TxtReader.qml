import QtQuick

Item {
    id: root

    property string content: ""
    property string pageTurnMode: "vertical"
    property real dragStartX: 0
    property real dragStartY: 0

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

    function finishDrag(endX, endY) {
        const dx = endX - dragStartX
        const dy = endY - dragStartY
        const minDistance = 64

        if (root.pageTurnMode === "horizontal") {
            if (Math.abs(dx) >= minDistance && Math.abs(dx) > Math.abs(dy)) {
                if (dx < 0) {
                    root.nextPage()
                } else {
                    root.previousPage()
                }
            }
            return
        }

        const maxY = Math.max(0, flick.contentHeight - flick.height)
        if (Math.abs(dy) >= minDistance && Math.abs(dy) > Math.abs(dx)) {
            if (dy < 0 && flick.contentY >= maxY - 1) {
                root.nextBoundaryRequested()
            } else if (dy > 0 && flick.contentY <= 1) {
                root.previousBoundaryRequested()
            }
        }
    }

    onContentChanged: flick.contentY = 0

    Flickable {
        id: flick
        anchors.fill: parent
        clip: true
        interactive: root.pageTurnMode === "vertical"
        contentWidth: width
        contentHeight: textItem.implicitHeight + 72
        boundsBehavior: root.pageTurnMode === "vertical" ? Flickable.DragOverBounds : Flickable.StopAtBounds

        onContentYChanged: {
            const scrollable = Math.max(1, contentHeight - height)
            root.positionChanged(Math.max(0, Math.min(1, contentY / scrollable)))
        }

        Text {
            id: textItem
            width: Math.min(flick.width - 56, 860)
            x: Math.max(28, (flick.width - width) / 2)
            y: 36
            text: root.content.length > 0 ? root.content : "请选择一本 TXT 书籍"
            wrapMode: Text.WordWrap
            font.pixelSize: 21
            lineHeight: 1.55
            color: "#26211c"
        }
    }

    DragHandler {
        id: dragHandler
        target: null
        acceptedButtons: Qt.LeftButton

        onActiveChanged: {
            if (active) {
                root.dragStartX = centroid.position.x
                root.dragStartY = centroid.position.y
            } else {
                root.finishDrag(centroid.position.x, centroid.position.y)
            }
        }
    }
}
