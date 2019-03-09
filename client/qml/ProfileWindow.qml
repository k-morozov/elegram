import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ColumnLayout {
    spacing: 3

    Text {
        text: "Profile"
        font.pixelSize: 24
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        Layout.margins: 3
    }

    ColumnLayout {
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth: true
        width: parent.width
        Layout.margins: 10

        Text {
            id: name
            text: '<b>Name:</b> ' + backend.userName
        }
        Text {
            id: email
            text: '<b>Email:</b> ' + backend.userEmail
        }
    }

    BackButton {
        Layout.fillWidth: true
        Layout.margins: 5
    }
}
