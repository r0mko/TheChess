import QtQuick 2.4

Rectangle {
    property color whiteColor
    property color blackColor
    color: isDark ? whiteColor : blackColor
    border.color: hl.color
    border.width: droparea.containsDrag ? 2 : 0

    signal pieceDropped(int source, int destination)

    Rectangle {
        id: hl
        anchors.fill: parent
        anchors.margins: 4
        opacity: 0.2
        color: move ? "#00ff40" : "#ff0000"
        radius: width / 2
        visible: move || capture
    }

    DropArea {
        id: droparea
        enabled: move || capture
        anchors.fill: parent
        onDropped: pieceDropped(drop.source.index, boardIndex)
        keys: "piece"
    }
}

