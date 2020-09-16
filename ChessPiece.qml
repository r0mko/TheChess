import QtQuick 2.4
import QmlChess 1.0
import QtGraphicalEffects 1.0
Item {
    id: root
    property int index: boardIndex


    Behavior on x {
        NumberAnimation {
            easing.type: Easing.InOutCubic
            duration: 300
        }
    }
    Behavior on y {
        NumberAnimation {
            easing.type: Easing.InOutCubic
            duration: 300
        }
    }

    Image {
        id: img
//        visible: false
        anchors.fill: parent
        sourceSize: Qt.size(parent.width, parent.height)
        source: "qrc:/Pieces/" + iconSrc + ".svg"
        Component.onCompleted: {
//            console.log("source", source, "status", model.status, Piece.InGame, "turn", boardModel.turn, Piece.White, root.state)
        }
        opacity: marea.enabled ? 1.0 : 0.5

    }

    DropShadow {
        anchors.fill: img
        source: img
        samples: 16
        color: "#c0000000"
        radius: 3
        verticalOffset: 2
    }

    MouseArea {
        id: marea
        anchors.fill: parent
        drag.threshold: 0
        enabled: false
        drag.onActiveChanged: {
            piece.active = drag.active
            if (drag.active) {
                root.Drag.start(Qt.MoveAction)
            } else {
                if (root.Drag.target)
                    root.Drag.drop()
                root.x = column * width
                root.y = row * height
            }
        }
        onPressed: {
            console.log("pressed:", model.piece, model.inGame, boardIndex, position)
        }
    }

    Drag.active: marea.drag.active
    Drag.dragType: Drag.Internal
    Drag.keys: [ "piece" ]
    Drag.hotSpot: Qt.point(marea.mouseX, marea.mouseY)

    states: [
        State {
            name: "readyToMove"
            extend: 'onboard'
            when: boardModel.turn == color && inGame
            PropertyChanges {
                target: marea
                drag.target: root
                enabled: true
            }
        },
        State {
            name: "onboard"
            when: inGame
            ParentChange {
                target: root
                parent: checkerboard.topmost

            }

            PropertyChanges {
                target: root
                width: checkerboard.cellsize
                height: width
                rotation: -checkerboard.__boardContainer.rotation
                x: column * width
                y: row * height
            }
        },

        State {
            name: "capturedByWhite"
            when: color == Piece.Black && !inGame
            ParentChange {
                target: root
                parent: whitePanel.prison
                rotation: 360
                width: 40
                height: 40
                x: 0
                y: 0
            }
        },

        State {
            name: "capturedByBlack"
            when: color == Piece.White && !inGame

            ParentChange {
                target: root
                parent: blackPanel.prison
                rotation: 360
                width: 40
                height: 40
                x: 0
                y: 0
            }
        }
    ]
    transitions: Transition {
        ParentAnimation {
            NumberAnimation { properties: "x,y,rotation,width,heigth"; duration: 300; easing.type: Easing.OutCubic }
        }
    }
}

