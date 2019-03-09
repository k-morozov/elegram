import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Button {
    id: back_btn
    text: qsTr("Back")

    onClicked: {
        onClicked: stackView.pop()
    }
}
