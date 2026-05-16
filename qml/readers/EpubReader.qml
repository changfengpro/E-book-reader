import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property var chapters: []
    property int currentIndex: 0
    property string chapterHtml: ""
    property string errorText: ""

    signal chapterSelected(int index)
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
                    clip: true
                    model: root.chapters

                    delegate: ItemDelegate {
                        width: ListView.view.width
                        text: modelData.title
                        highlighted: modelData.index === root.currentIndex
                        onClicked: root.chapterSelected(modelData.index)
                    }
                }
            }
        }

        Flickable {
            id: scroll
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
                text: root.errorText.length > 0
                    ? root.errorText
                    : root.chapterHtml.length > 0
                        ? root.chapterHtml
                        : (root.chapters.length === 0
                            ? "EPUB 章节将在这里显示"
                            : "正在加载章节...")
                textFormat: root.errorText.length > 0 ? Text.PlainText : Text.RichText
                wrapMode: Text.WordWrap
                font.pixelSize: 20
                lineHeight: 1.45
                color: "#2d2925"
                onLinkActivated: function(link) {
                    Qt.openUrlExternally(link)
                }
            }
        }
    }

    onChapterHtmlChanged: scroll.contentY = 0

    onCurrentIndexChanged: {
        if (root.chapters.length === 0) {
            return
        }
        const chapter = root.chapters[root.currentIndex]
        if (!chapter) {
            return
        }
        root.locatorChanged(JSON.stringify({
            type: "epub",
            chapterId: chapter.id,
            chapterIndex: root.currentIndex
        }))
    }
}
