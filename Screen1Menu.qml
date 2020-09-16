import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2


ColumnLayout {
    id: buttons
    Layout.fillWidth: true
    LcdButton {
        Layout.fillWidth: true
        action: newGameAction
    }
    LcdButton {
            Layout.fillWidth: true
        action: loadGameAction
    }
}
