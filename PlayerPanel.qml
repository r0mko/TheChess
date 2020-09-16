import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QmlChess 1.0

Rectangle {
    property alias prison: prison
    color: "#888"
    radius: 0
    Layout.minimumHeight: 40 + prison.spacing * 2
//    Layout.margins: 1
    implicitHeight: prison.implicitHeight
    height: implicitHeight
    Row {
        anchors.left: parent.left
        anchors.right: parent.right
        id: prison
    }
}
