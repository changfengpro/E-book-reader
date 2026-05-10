import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    property string pageTurnMode: "vertical"

    signal backRequested()
    signal pageTurnModeSelected(string mode)

    background: Rectangle {
        color: "#f8fafc"
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 10

            ToolButton {
                text: "返回"
                onClicked: page.backRequested()
            }

            Label {
                text: "设置"
                font.pixelSize: 18
                font.bold: true
                Layout.fillWidth: true
            }
        }
    }

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: settingsColumn.implicitHeight + 48
        clip: true

        ColumnLayout {
            id: settingsColumn
            width: Math.min(page.width - 48, 720)
            x: (page.width - width) / 2
            y: 24
            spacing: 18

            Label {
                text: "阅读设置"
                font.pixelSize: 28
                font.bold: true
                Layout.fillWidth: true
            }

            Frame {
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 14

                    Label {
                        text: "默认字体大小"
                        font.bold: true
                    }

                    Slider {
                        from: 14
                        to: 30
                        value: 20
                        stepSize: 1
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "默认行距"
                        font.bold: true
                    }

                    Slider {
                        from: 1.1
                        to: 1.8
                        value: 1.45
                        stepSize: 0.05
                        Layout.fillWidth: true
                    }
                }
            }

            Frame {
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Label {
                        text: "翻页方式"
                        font.bold: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Button {
                            text: "上下"
                            checkable: true
                            checked: page.pageTurnMode === "vertical"
                            Layout.fillWidth: true
                            onClicked: page.pageTurnModeSelected("vertical")
                        }

                        Button {
                            text: "左右"
                            checkable: true
                            checked: page.pageTurnMode === "horizontal"
                            Layout.fillWidth: true
                            onClicked: page.pageTurnModeSelected("horizontal")
                        }
                    }
                }
            }

            Frame {
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Label {
                        text: "主题"
                        font.bold: true
                    }

                    ComboBox {
                        model: ["浅色", "护眼", "深色"]
                        Layout.fillWidth: true
                    }
                }
            }

            Frame {
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Label {
                        text: "本地缓存"
                        font.bold: true
                    }

                    Label {
                        text: "缓存用于保存封面、PDF 缩略图和 EPUB 解析结果。"
                        color: "#64748b"
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    Button {
                        text: "清理缓存"
                    }
                }
            }
        }
    }
}
