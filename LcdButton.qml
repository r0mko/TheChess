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
//                color: "#309f1b00"
//                border.width: 2
                border.width: 3
                border.color: "#80000e27"
//                visible: control.hovered
            }
            Rectangle {
                radius: 5
                anchors.fill: parent
                anchors.margins: 3
//                anchors.margins: control.hovered ? 0 : 3
//                color: "#a2c4e2"
                color: "transparent"
//                color: "#9e7676"
                border.width: control.hovered ? 3 : 0
                border.color: "#40000e27"
//                border.color: control.hovered ? "#80000e27" : "#40ffffff"
            }
        }

        label: Label {
            color: "#90000e27"
//            color: control.pressed ? "#fff6da" : control.hovered ? "#e0000e27" : "#627358"
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: control.text
            opacity: control.hovered ? 1.0 : 0.1
            font.family: registerfont.name
//            renderType: Text.NativeRendering
//            style: Text.Outline
//            styleColor: "#a2c4e2"
        }
    }
}
