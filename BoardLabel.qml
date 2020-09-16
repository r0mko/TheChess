import QtQuick 2.4
import QtQuick.Layouts 1.1


Text {

    property real size
    property bool vertical: true
    property bool opposite: false
//    Behavior on rotation {
//        NumberAnimation {
//            easing.type: Easing.InOutCubic
//            duration: 1000
//        }
//    }
    width: vertical ? implicitWidth: size
    height: vertical ? size : implicitHeight
//    height: size
//    width: size
//    rotation: turn ? root.rotation - 180 : root.rotation
    rotation: -c_brd.rotation
    fontSizeMode: Text.Fit
    minimumPointSize: 4
    font.pointSize: cellsize ? cellsize * 0.4 : 1
    color: "#333"
    font.family: "Roboto"
    font.weight: Font.Light
    text: vertical ? 8 - index : String.fromCharCode(index + 97)
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
}
