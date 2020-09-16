import QtQuick 2.4
import QtQuick.Layouts 1.1
import QmlChess 1.0
import QtGraphicalEffects 1.0

Item {
    id: root
    property color blackColor: "#d18b47"
    property color whiteColor: Qt.lighter(blackColor, 1.55)//"#ffce9e"
    property alias board: board
    property alias __boardContainer: c_brd
    property alias topmost: topmost
    property BoardModel model
    property real cellsize: Math.min(width, height) / 12
    property bool flip: false
    property bool interactive: false

    Image {
        id: bg
        anchors.fill: parent
        source: "qrc:/low_contrast_linen.png"
        fillMode: Image.Tile
    }
    Keys.onSpacePressed: flip ^= true

    Item {
        id: c_brd
        //        clip: true
        Behavior on rotation {
            id: rotBehavior
            NumberAnimation {
                easing.type: Easing.OutQuint
                duration: 300
            }
        }
        //        visible: false
        anchors.fill: parent

        Colorize {
            id: darker_cells
            anchors.fill: board
            source: board_bk
            hue: 0.068
            saturation: 1.3
            lightness: -0.5
            visible: false
        }


        Item {
            id: board
            anchors.centerIn: parent
            width: cells.implicitWidth + 2
            height: cells.implicitWidth + 2
            //            border.width: 1


            Image {
                //                z: -1
                id: board_bk
                anchors.fill: parent
                source: "qrc:/wood.png"
                fillMode: Image.PreserveAspectFit
                anchors.margins: -cellsize


                //                onWidthChanged: console.log(width, height, implicitWidth, implicitHeight, c_brd.width, c_brd.height)
            }


            Grid {
                id: cells
                anchors.fill: parent
                flow: Grid.LeftToRight
                anchors.margins: 1
                columns: 8
                rows: 8

                Repeater {
                    model: root.model
                    //                    visible: false
                    delegate: FieldSquare {
                        width: cellsize
                        height: cellsize
                        whiteColor: "transparent"
                        blackColor: "#30401e00"
                        onPieceDropped: {
                            if (root.model.turn == Piece.White)
                                root.model.makeMove(source, destination, whiteTimer.elapsed)
                            else
                                root.model.makeMove(source, destination, blackTimer.elapsed)
                        }
                    }
                }
            }

        }
        OpacityMask {
            id: topmost
            anchors.fill: board
            maskSource: cells
            source: darker_cells
            //            visible: false
        }

        Column {
            anchors.right: board.left
            anchors.top: board.top
            anchors.bottom: board.bottom
            anchors.rightMargin: cellsize/5

            Repeater {
                model: 8
                BoardLabel {
                    size: cellsize
                    vertical: true
                    opposite: false
                }
            }
        }
        Column {
            anchors.left: board.right
            anchors.top: board.top
            anchors.bottom: board.bottom
            anchors.leftMargin: cellsize/5


            Repeater {
                model: 8
                BoardLabel {
                    size: cellsize
                    vertical: true
                    opposite: true
                }
            }
        }
        Row {
            anchors.right: board.right
            anchors.left: board.left
            anchors.top: board.bottom
            //        anchors.bottomMargin: cellsize/5

            Repeater {
                model: 8
                BoardLabel {
                    size: cellsize
                    vertical: false
                    opposite: false
                }
            }

        }
        Row {
            anchors.right: board.right
            anchors.bottom: board.top
            anchors.left: board.left
            //        anchors.bottomMargin: cellsize/5

            Repeater {
                model: 8
                BoardLabel {
                    size: cellsize
                    vertical: false
                    opposite: true
                }
            }

        }

    }

    states: [
        State {
            name: "whiteSide"
            when: flip === false
            PropertyChanges {
                target: c_brd
                rotation: 0
            }

        },
        State {
            name: "blackSide"
            when: flip === true
            PropertyChanges {
                target: c_brd
                rotation: 180
            }
        }
    ]


    DropShadow {
        //        id: topmost
        anchors.fill: root
        source: c_brd
        rotation: c_brd.rotation
        radius: 30
        samples: 32
        //        verticalOffset: 10;
    }




}



