import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import EbookReader

Page {
    id: page

    signal openBook(string bookId)
    signal openSettings()
    signal importRequested()

    readonly property bool tabletWide: width >= 900
    readonly property int columnCount: width >= 1200 ? 5 : tabletWide ? 4 : 2

    background: Rectangle {
        color: "#eef2f6"
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 10

            Label {
                text: "书架"
                font.pixelSize: 22
                font.bold: true
                Layout.fillWidth: true
            }

            ImportButton {
                onClicked: page.importRequested()
            }

            Button {
                text: "设置"
                onClicked: page.openSettings()
            }
        }
    }

    ListModel {
        id: libraryModel

        ListElement {
            bookId: "sample-txt"
            title: "长篇小说"
            format: "txt"
            progress: 0.37
        }
        ListElement {
            bookId: "sample-pdf"
            title: "技术手册"
            format: "pdf"
            progress: 0.18
        }
        ListElement {
            bookId: "sample-epub"
            title: "文学选集"
            format: "epub"
            progress: 0.64
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: page.tabletWide ? 28 : 18
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            TextField {
                placeholderText: "搜索书名、作者、格式"
                Layout.fillWidth: true
            }

            ComboBox {
                model: ["全部", "TXT", "PDF", "EPUB"]
                Layout.preferredWidth: page.tabletWide ? 140 : 112
            }
        }

        GridView {
            id: grid
            visible: libraryModel.count > 0
            model: libraryModel
            cellWidth: Math.max(156, Math.floor(width / page.columnCount))
            cellHeight: 286
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            Layout.fillWidth: true
            Layout.fillHeight: true

            delegate: Item {
                width: grid.cellWidth
                height: grid.cellHeight

                BookCard {
                    anchors.centerIn: parent
                    width: Math.min(parent.width - 16, 196)
                    bookId: model.bookId
                    title: model.title
                    format: model.format
                    progress: model.progress
                    onOpenBook: function(id) { page.openBook(id) }
                }
            }
        }

        EmptyLibraryView {
            visible: libraryModel.count === 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            onImportRequested: page.importRequested()
        }
    }
}
