import QtQuick
import QtQuick.Controls

Flickable {
    id: root

    property string content: ""

    signal positionChanged(real progress)

    onContentChanged: contentY = 0

    clip: true
    contentWidth: width
    contentHeight: textItem.implicitHeight + 48

    onContentYChanged: {
        const scrollable = Math.max(1, contentHeight - height)
        root.positionChanged(Math.max(0, Math.min(1, contentY / scrollable)))
    }

    Text {
        id: textItem
        width: root.width - 48
        x: 24
        y: 24
        text: root.content.length > 0 ? root.content : "请选择一本 TXT 书籍"
        wrapMode: Text.WordWrap
        font.pixelSize: 20
        lineHeight: 1.45
        color: "#2d2925"
    }
}
