import QtQuick
import QtQuick.Controls

Item {
    id: root

    property var pageCount: 0
    property real zoom: 1.0
    property var statusText: ""
    readonly property int safePageCount: Number(pageCount || 0)
    readonly property string safeStatusText: String(statusText || "")
    readonly property int currentPage: Math.max(1, Number(pageAtY(scroll.contentY + scroll.height * 0.35) || 1))

    signal pageRequested(int page, real zoom)
    signal locatorChanged(string locatorJson)

    ListModel {
        id: pageModel
    }

    function resetPages() {
        pageModel.clear()
        const initialCount = Math.min(root.safePageCount, 3)
        for (let i = 1; i <= initialCount; ++i) {
            pageModel.append({ pageNumber: i, imageUrl: "" })
            root.pageRequested(i, root.zoom)
        }
    }

    function appendNextPage() {
        if (pageModel.count >= root.safePageCount) {
            return
        }

        const nextPage = pageModel.count + 1
        pageModel.append({ pageNumber: nextPage, imageUrl: "" })
        root.pageRequested(nextPage, root.zoom)
    }

    function updatePageImage(pageNumber, url) {
        for (let i = 0; i < pageModel.count; ++i) {
            if (pageModel.get(i).pageNumber === pageNumber) {
                pageModel.setProperty(i, "imageUrl", String(url || ""))
                return
            }
        }
    }

    function pageAtY(y) {
        if (pageModel.count === 0) {
            return 1
        }

        let bestPage = Number(pageModel.get(0).pageNumber || 1)
        for (let i = 0; i < pageColumn.children.length; ++i) {
            const item = pageColumn.children[i]
            const candidatePage = Number(item.pageNumber || 0)
            if (candidatePage > 0 && item.y <= y) {
                bestPage = candidatePage
            }
        }
        return bestPage
    }

    function maybeAppendNearBottom() {
        if (root.safePageCount <= 0) {
            return
        }

        const remaining = scroll.contentHeight - scroll.contentY - scroll.height
        if (remaining < scroll.height * 1.5) {
            appendNextPage()
        }
    }

    onSafePageCountChanged: resetPages()
    onZoomChanged: resetPages()

    onCurrentPageChanged: {
        root.locatorChanged(JSON.stringify({ type: "pdf", page: root.currentPage, zoom: root.zoom }))
    }

    Flickable {
        id: scroll
        anchors.fill: parent
        anchors.margins: 24
        clip: true
        contentWidth: width
        contentHeight: pageColumn.height
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick
        onContentYChanged: root.maybeAppendNearBottom()
        Component.onCompleted: root.maybeAppendNearBottom()

        Column {
            id: pageColumn
            width: scroll.width
            spacing: 20

            Repeater {
                model: pageModel

                Item {
                    property int pageNumber: model.pageNumber

                    width: pageColumn.width
                    height: pageImage.status === Image.Ready
                        ? pageImage.paintedHeight + pageNumberLabel.height + 16
                        : Math.max(360, scroll.height * 0.8)

                    Image {
                        id: pageImage
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        source: model.imageUrl
                        fillMode: Image.PreserveAspectFit
                        cache: true
                        asynchronous: true
                        width: parent.width
                        height: status === Image.Ready
                            ? Math.min(implicitHeight, implicitHeight * (parent.width / Math.max(1, implicitWidth)))
                            : parent.height - pageNumberLabel.height - 16
                    }

                    BusyIndicator {
                        anchors.centerIn: parent
                        running: pageImage.status !== Image.Ready
                        visible: running
                    }

                    Label {
                        id: pageNumberLabel
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: pageImage.bottom
                        anchors.topMargin: 8
                        text: pageNumber + " / " + root.safePageCount
                        color: "#4b5563"
                    }
                }
            }
        }
    }

    Label {
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 520)
        visible: root.safePageCount === 0 || (pageModel.count === 0 && root.safeStatusText.length > 0)
        text: root.safeStatusText
        color: "#4b5563"
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
    }
}
