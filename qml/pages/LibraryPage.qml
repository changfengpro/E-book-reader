import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import EbookReader
import EbookReader.Backend

Page {
    id: page

    signal openBook(var book)
    signal openSettings()
    signal importRequested()

    readonly property bool tabletWide: width >= 900
    readonly property int columnCount: width >= 1200 ? 5 : tabletWide ? 4 : 2

    LibraryController {
        id: library
    }

    FileDialog {
        id: importDialog
        title: "导入书籍"
        nameFilters: ["电子书 (*.txt *.pdf *.epub)"]
        onAccepted: library.importLocalFile(selectedFile)
    }

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
                onClicked: {
                    page.importRequested()
                    importDialog.open()
                }
            }

            Button {
                text: "设置"
                onClicked: page.openSettings()
            }
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

        Label {
            visible: library.lastError.length > 0
            text: library.lastError
            color: "#b42318"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        GridView {
            id: grid
            visible: library.books.length > 0
            model: library.books
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
                    bookId: modelData.bookId
                    title: modelData.title
                    format: modelData.format
                    progress: modelData.progress
                    onOpenBook: function() { page.openBook(modelData) }
                }
            }
        }

        EmptyLibraryView {
            visible: library.books.length === 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            onImportRequested: {
                page.importRequested()
                importDialog.open()
            }
        }
    }
}
