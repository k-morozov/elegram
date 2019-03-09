import QtQuick 2.12
import QtQuick.Controls 2.12

Popup {
    id: popup
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    width: parent.width * 2 / 3
    height: 100
    modal: true
    focus: true

    contentItem: Text {
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        wrapMode: Text.WordWrap

        text: "Simple Text"
    }
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
}
