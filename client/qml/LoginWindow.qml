import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ColumnLayout {
    spacing: 3

    Label {
        text: "Login"
        font.pixelSize: 24
        Layout.margins: 24
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
    }

    ColumnLayout {
        Layout.alignment: Qt.AlignHCenter

        TextField {
            id: email_input
            placeholderText: qsTr("Enter email")
            Layout.margins: 3
            Layout.alignment: Qt.AlignHCenter
            focus: true
        }

        TextField {
            id: passwd_input
            placeholderText: qsTr("Enter password")
            echoMode: TextInput.Password
            Layout.alignment: Qt.AlignHCenter
            Layout.margins: 3
        }

        //a button in the middle of the content area
        RowLayout {
            Layout.bottomMargin: 16
            Layout.alignment: Qt.AlignHCenter

            Button {
                id: signin_btn
                text: qsTr("Sign in")
                Layout.margins: 3

                // callback for recieving success status response
                function loginReaction(isLogged) {
                    console.log("loginReaction called")

                    if (isLogged) {
                        // load chats here
                        notificator.showMessage(
                                    "Hello, " + backend.userName + "!")
                        stackView.push("ChatsWindow.qml")
                    } else {
                        notificator.showMessage("Wrong name or password")
                    }
                }

                Connections {
                    target: backend
                    onDoLoginReaction: {
                        signin_btn.loginReaction(isLogged)
                    }
                }

                onClicked: {
                    onClicked: {
                        if (backend.availible()) {
                            if (email_input.text === "") {
                                notificator.showMessage("Empty email")
                            } else if (passwd_input.text === "") {
                                notificator.showMessage("Empty password")
                            } else {
                                backend.signin(email_input.text,
                                               passwd_input.text)
                            }
                        } else {
                            console.log("Backend is busy")
                        }
                    }
                }
            }

            Button {
                id: signup_btn
                text: qsTr("Sign up")
                Layout.margins: 3
                onClicked: {
                    if (backend.availible()) {
                        stackView.push("RegisterWindow.qml")
                    } else {
                        console.log("Hello from backend's logger")
                    }
                }
            }
        }

        Button {
            id: settings_btn
            text: qsTr("Settings")
            Layout.margins: 3
            Layout.preferredWidth: parent.width
            onClicked: {
                stackView.push("SettingsWindow.qml")
            }
        }
    }
}
