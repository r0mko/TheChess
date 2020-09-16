import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

ColumnLayout {
    id: buttons

    LcdButton {
        Layout.fillWidth: true
        action: newGameAction
    }

    LcdButton {
            Layout.fillWidth: true
        action: loadGameAction
    }

    RowLayout {
        Layout.fillWidth: true

        LcdButton {
            Layout.fillWidth: true
            action: prevMoveAction
        }

        LcdButton {
            Layout.fillWidth: true
            action: nextMoveAction
        }
    }
}
