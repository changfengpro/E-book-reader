import QtQuick
import QtQuick.Controls
import EbookReader

ApplicationWindow {
    id: window
    width: 1280
    height: 800
    visible: true
    title: "E-Book Reader"
    property string pageTurnMode: "vertical"

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: LibraryPage {
            onOpenSettings: stack.push(settingsPage)
            onOpenBook: function(book) {
                stack.push(readerPage, {
                    bookId: book.bookId,
                    filePath: book.filePath,
                    formatOverride: book.format,
                    titleOverride: book.title
                })
            }
        }
    }

    Component {
        id: settingsPage
        SettingsPage {
            pageTurnMode: window.pageTurnMode
            onBackRequested: stack.pop()
            onPageTurnModeSelected: function(mode) {
                window.pageTurnMode = mode
            }
        }
    }

    Component {
        id: readerPage
        ReaderPage {
            pageTurnMode: window.pageTurnMode
            onBackRequested: stack.pop()
            onSettingsRequested: stack.push(settingsPage)
        }
    }
}
