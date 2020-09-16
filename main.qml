import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QmlChess 1.0
import QtGraphicalEffects 1.0

ApplicationWindow {
    title: qsTr("The Chess")
    width: 960
    height: 720
    visible: true
    color: "#333"

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {

            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
    }



    SplitView {
        id: main
        handleDelegate: Rectangle {
            color: "#80000000"
            width: 1
        }

        state: "mainmenu"
        states: [
            State {
                name: "mainmenu"
                StateChangeScript {
                    script: {
                        boardModel.clear()
                    }
                }
                PropertyChanges {
                    target: menuLoader
                    source: "Screen1Menu.qml"
                }
                PropertyChanges {
                    target: boardModel
                }
            },
            State {
                name: "game"
                PropertyChanges {
                    target: menuLoader
                    source: "Screen2Menu.qml"
                }
                StateChangeScript {
                    script: {
                        boardModel.clear()
                        boardModel.pieces.game = {}
                        boardModel.initFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR")
                    }
                }
                PropertyChanges {
                    target: checkerboard
                    interactive: true
                }
                PropertyChanges {
                    target: blackTimer
                    running: checkerboard.model.turn == Piece.Black
                }
                PropertyChanges {
                    target: blackClock
                    text: "♟" + Qt.formatTime(blackTimer.elapsed, "h:mm:ss")
                }
                PropertyChanges {
                    target: whiteTimer
                    running: checkerboard.model.turn == Piece.White
                }
                PropertyChanges {
                    target: whiteClock
                    text: "♙" + Qt.formatTime(whiteTimer.elapsed, "h:mm:ss")
                }
            },
            State {
                name: "replay"
                PropertyChanges {
                    target: menuLoader
                    source: "Screen3Menu.qml"
                }

                StateChangeScript {
                    script: boardModel.pieces.toFirstMove()
                }

                PropertyChanges {
                    target: historyView
                    log.currentIndex: boardModel.pieces.currentMove
                }

            }

        ]
        anchors.fill: parent
        anchors.margins: 0

        ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
            anchors.leftMargin: -sidebar.width
//            anchors.fill: parent
            spacing: 0
            PlayerPanel {
                id: blackPanel
                color: "#555"
                Layout.fillWidth: true
    //            Layout.leftMargin: sidebar.width
            }
            ChessBoard {
                id: checkerboard
                model: BoardModel {
                    id: boardModel
                    pieces.onGameLoaded: { main.state = "replay" }
                }
                Layout.fillHeight: true
                Layout.fillWidth: true
                focus: true
                Instantiator {
                    model: boardModel.pieces
                    delegate: ChessPiece {
                        parent: checkerboard
                        enabled: checkerboard.interactive
                    }
                }

            }
            PlayerPanel {
                id: whitePanel
                color: "#ddd"
                Layout.fillWidth: true
    //            Layout.leftMargin: sidebar.width
            }
        }
        Image {
            id: sidebar
            Layout.fillHeight: true
            Layout.minimumWidth: 240

            width: 240
            source: "qrc:/metal.png"

            Rectangle {
                id: lcd_bg
                ElapsedTimer { id: blackTimer }
                ElapsedTimer { id: whiteTimer }
                height: t_lay.implicitHeight + t_lay.anchors.margins * 2
                color: "#8fa780"
                radius: 10
                anchors.fill: parent
                anchors.margins: 8
                border.color: "#80000000"
                border.width: 1
                Item {
                    id: lcd_text
                    anchors.fill: parent
                    ColumnLayout {
                        id: t_lay
                        anchors.margins: 8
                        anchors.fill: parent


                        Text {
                            id: whiteClock
                            font.family: lcdfont.name
                            color: "#e0000e27"
                            font.pixelSize: 34
                            horizontalAlignment: Text.AlignHCenter
                            text: "♙ -:--:--"
                        }

                        Text {
                            id: blackClock
                            font.family: lcdfont.name
                            color: "#e0000e27"
                            font.pixelSize: 34
                            horizontalAlignment: Text.AlignHCenter
                            text: "♟ -:--:--"
                        }

                        LogView {
                            id: historyView
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            model: boardModel.pieces.history
                        }

                        Loader {
                            id: menuLoader
                            Layout.fillWidth: true
                            property Action newGameAction: newGame
                            property Action loadGameAction: importGame
                            property Action saveGameAction: exportGame
                            property Action quitGameAction: quitGame
                        }


                    }
                }

                DropShadow {
                    source: lcd_text
                    anchors.fill: lcd_text
                    samples: 32
                    radius: 8
                    verticalOffset: 5
                    color: "#80000000"
                }

            }

            InnerShadow {
                anchors.fill: lcd_bg
                radius: 9
                verticalOffset: 9
                color: "#80000000"
                samples: 16
                source: lcd_bg
            }
        }
    }

    FileDialog {
        id: filePicker
        nameFilters: ["Chess JSON files (*.cjson)"]
        function run(callback) {
            if (typeof callback !== "function") {
                console.warn("callback must be a function!", callback, typeof callback)
                return
            }
            func = callback
            open()
        }
        property var func
        onAccepted: {
            if (selectMultiple)
                func(fileUrls)
            else
                func(fileUrl)
            func = undefined
        }
    }

    Action {
        id: newGame
        text: qsTr("New game")
        shortcut: "Ctrl+Shift+N"
        onTriggered:  main.state = "game"

    }
    Action {
        id: exportGame
        text: qsTr("Save game")
        shortcut: "Ctrl+Shift+S"
        onTriggered: {
            filePicker.selectExisting = false
            filePicker.run(boardModel.pieces.exportGame)
        }
    }

    Action {
        id: importGame
        text: qsTr("Load game")
        onTriggered: {
            filePicker.selectExisting = true
            filePicker.run(boardModel.pieces.importGame)
        }
    }

    Action {
        id: prevMoveAction
        text: qsTr("◀ Prev")
        onTriggered: {
            boardModel.pieces.toPrevMove()
        }
    }


    Action {
        id: nextMoveAction
        text: qsTr("Next ▶")
        onTriggered: {
            boardModel.pieces.toNextMove()
        }
    }

    Action {
        id: quitGame
        text: qsTr("Quit game")
        onTriggered: {
            boardModel.clear()
            boardModel.pieces.game = {}
            main.state = "mainmenu"
        }
    }

    FontLoader {
        id: registerfont
        source: "qrc:/register.ttf"
    }
    FontLoader {
        id: lcdfont
        source: "qrc:/digital7mi.ttf"
    }
}
