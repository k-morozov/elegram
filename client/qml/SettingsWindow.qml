import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ColumnLayout {
    //    anchors.fill: parent
    spacing: 3

    Label {
        text: "Settings"
        font.pixelSize: 24
        Layout.margins: 24
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
    }

    GridLayout {
        Layout.alignment: Qt.AlignHCenter
        columns: 2

        Label {
            text: qsTr("Server ip")
            Layout.margins: 3
        }

        TextField {
            id: ip_field
            placeholderText: qsTr("Enter server ip")
            Layout.margins: 3
            text: backend.serverIp
        }

        Label {
            text: qsTr("Server port")
            Layout.margins: 3
        }

        TextField {
            id: port_field
            placeholderText: qsTr("Enter server port")
            Layout.margins: 3
            text: backend.serverPort
        }
    }

    RowLayout {
        BackButton {
            Layout.fillWidth: true
            Layout.margins: 10
        }
        Button {
            id: set_settings_btn
            text: qsTr("Connect")
            Layout.fillWidth: true
            Layout.margins: 10

            function updateSettingsReaction(errorMesg) {
                console.log("updateSettingsReaction called with status: " + errorMesg)

                if (errorMesg !== "")
                    notificator.showMessage(errorMesg)
            }

            Connections {
                target: backend
                onDoUpdateConnectionSettingsReaction: {
                    set_settings_btn.updateSettingsReaction(errorMesg)
                }
            }

            onClicked: {
                if (ip_field.text === "") {
                    notificator.showMessage("Empty server ip ")
                } else if (port_field.text === "") {
                    notificator.showMessage("Empty server port ")
                } else {
                    backend.updateConnectionSettings(ip_field.text,
                                                     port_field.text)
                }
            }
        }
    }
}
