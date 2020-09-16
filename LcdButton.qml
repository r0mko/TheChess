import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Button {
    style: ButtonStyle {
        background: Item {
            implicitHeight: 36
            implicitWidth: 120

            Rectangle {
                radius: 8
                anchors.fill: parent
                anchors.margins: 0
                color: "transparent"
                border.width: 3
                border.color: "#80000e27"
            }

            Rectangle {
                radius: 5
                anchors.fill: parent
                anchors.margins: 3
                color: "transparent"
                border.width: control.hovered ? 3 : 0
                border.color: "#40000e27"
            }
        }

        label: Label {
            color: "#90000e27"
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: control.text
            opacity: control.hovered ? 1.0 : 0.1
            font.family: registerfont.name
        }
    }
}
