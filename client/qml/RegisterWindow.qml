////import related modules
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ColumnLayout {
    spacing: 3

    Label {
        text: "Registration"
        font.pixelSize: 24
        Layout.margins: 24
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
    }

    TextField {
        id: name_input
        placeholderText: qsTr("Enter name")
        Layout.margins: 3
        Layout.alignment: Qt.AlignHCenter
        focus: true
    }

    TextField {
        id: email_input
        placeholderText: qsTr("Enter email")
        Layout.margins: 3
        Layout.alignment: Qt.AlignHCenter
    }

    TextField {
        id: password_input
        placeholderText: qsTr("Enter password")
        echoMode: TextInput.Password
        Layout.alignment: Qt.AlignHCenter
        Layout.margins: 3
    }

    ColumnLayout {
        spacing: 10
        Layout.margins: 10

        Button {
            id: signup_btn
            text: qsTr("Sign up")
            Layout.fillWidth: true
            Layout.margins: 5

            function registerReaction(isRegistered) {
                if (isRegistered) {
                    notificator.showMessage("Succesfully registred")
                    name_input.text = ""
                    email_input.text = ""
                    password_input.text = ""
                } else {
                    notificator.showMessage("Email already in use")
                }
            }

            Connections {
                target: backend
                onDoRegisterReaction: {
                    signup_btn.registerReaction(isRegistered)
                }
            }

            onClicked: {
                if (backend.availible()) {
                    if (name_input.text === "") {
                        notificator.showMessage("Empty name ")
                    } else if (email_input.text === "") {
                        notificator.showMessage("Empty email")
                    } else if (password_input.text === "") {
                        notificator.showMessage("Empty password ")
                    } else {
                        backend.signup(name_input.text, email_input.text,
                                       password_input.text)
                    }
                }
            }
        }

        RowLayout {
            Layout.margins: 5
            spacing: 5

            Button {
                text: qsTr("Settings")
                Layout.fillWidth: true

                onClicked: {
                    onClicked: stackView.push("SettingsWindow.qml")
                }
            }

            BackButton {
                Layout.fillWidth: true
            }
        }
    }
}
