import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ColumnLayout {
    spacing: 3

    Label {
        text: "Find new contact"
        font.pixelSize: 24
        Layout.margins: 24
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
    }

    TextField {
        id: email_input
        placeholderText: qsTr("Enter email")
        Layout.margins: 3
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        focus: true
    }

    RowLayout {
        BackButton {
            Layout.fillWidth: true
            Layout.margins: 5
        }

        Button {
            id: add_contact_btn
            text: qsTr("Add contact")
            Layout.fillWidth: true
            Layout.margins: 5

            // callback for recieving success status response
            function addContactReaction(isAdded) {
                console.log("addContactReaction(isAdded) called with isAdded = " + isAdded)

                if (isAdded) {
                    notificator.showMessage("Contact added succesfully")
                    email_input.clear()

                    if (backend.availible()) {
                        console.log("Will reload chats now")
                        backend.loadContacts()
                    }

                    // todo all automatic calling other screen in stack button
                } else {
                    notificator.showMessage(email_input.text + " not found")
                }
            }

            Connections {
                target: backend
                onDoAddContactReaction: {
                    add_contact_btn.addContactReaction(isAdded)
                }
            }

            onClicked: {
                if (backend.availible()) {
                    if (email_input.text === "") {
                        notificator.showMessage("Empty email")
                    } else {
                        backend.addContact(email_input.text)
                    }
                }
            }
        }
    }
}
