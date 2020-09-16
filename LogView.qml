import QtQuick 2.4
import QtQuick.Layouts 1.1
import QmlChess 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    property alias model: view.model
    property alias log: view

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: header
            Layout.fillWidth: true
            height: 22
            color: "#347"
            radius: 2

            Text {
                text: "Move history"
                anchors.centerIn: parent
                color: "#ddd"
                font.bold: true
                font.pixelSize: 14
            }
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            style: ScrollViewStyle {

                handle: Rectangle {
                    color: "#e0000e27"
                    radius: 4
                    implicitHeight: 8
                    implicitWidth: 8
                }

                scrollBarBackground: Item {}
                incrementControl: Item {}
                decrementControl: Item {}
                transientScrollBars: true

            }

            ListView {
                id: view

                delegate: Item {
                    height: 24
                    width: parent.width
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 4
                        anchors.rightMargin: 4
                        spacing: 0
                        Text {
                            verticalAlignment: Text.AlignVCenter
                            text: "%1. %2%3%4%5".arg(String("   " + String(index + 1)).slice(-3)).arg(modelData.sign).arg(modelData.source).arg(modelData.captured ? "x" : "-").arg(modelData.destination)
                            color: "#e0000e27"
                            Layout.fillWidth: true
                            font.family: registerfont.name
                            font.pixelSize: 25
                        }
                    }
                }

                highlight: Rectangle {
                    color: "#40005cac"
                    width: view.width
                    height: 26
                    radius: 3
                }
            }
        }
    }
}

